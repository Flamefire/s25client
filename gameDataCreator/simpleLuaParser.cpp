// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "commonDefines.h" // IWYU pragma: keep
#include "simpleLuaParser.h"
#include "helpers/converters.h"
#include "libutil/Tokenizer.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <stdexcept>

namespace simpleLuaData {
struct SimpleLuaParser::LuaDataError : public std::runtime_error
{
    LuaDataError(const std::string& msg) : std::runtime_error(msg) {}
};

SimpleLuaParser::LuaDataError SimpleLuaParser::createError(const std::string& msg) const
{
    return LuaDataError(msg + " at position " + helpers::toString(curPos) + "\n" + content_.substr(curPos, 40));
}

StringRef SimpleLuaParser::parseComment(StringRef lastComment)
{
    size_t endl = content_.find('\n', curPos);
    if(endl == std::string::npos)
        endl = content_.size();
    size_t start = curPos;
    curPos = endl;
    // Ignore trailing whitespace
    endl = skipWhitespaces(content_, endl, false) + 1u;
    // If the last comment was not a trailing one AND the 2 are only separated by whitespace -> combine
    if(!lastComment.empty() && !isTrailingComment(lastComment) && isAdjacent(lastComment.end(), start))
        start = lastComment.start;
    return StringRef(start, endl - start);
}

bool SimpleLuaParser::isAdjacent(size_t pos1, size_t pos2)
{
    if(pos2 < pos1)
        std::swap(pos2, pos1);
    return skipWhitespaces(content_, pos1) >= pos2;
}

bool SimpleLuaParser::isTrailingComment(StringRef comment)
{
    // Just whitespace between newline and comment -> not trailing
    if(skipWhitespaces(content_, getStartOfLine(content_, comment.start)) == comment.start)
        return false;
    // Comment after start of table or function -> not trailing
    size_t pos = skipWhitespaces(content_, comment.start - 1u, false);
    RTTR_Assert(pos < content_.size());
    if(content_[pos] == '{' || content_[pos] == '(')
        return false;
    return true;
}

void SimpleLuaParser::parseString()
{
    char startChar = content_[curPos];
    bool isEscaped = false;
    while(++curPos < content_.size())
    {
        char c = getNext();
        if(c == '\\')
            isEscaped = !isEscaped;
        else if(!isEscaped && c == startChar)
        {
            curPos++;
            break;
        } else if(c == '\n')
            throw createError("New line in string");
    }
}

void SimpleLuaParser::skipList()
{
    char openChar = getNext();
    char closeChar;
    if(openChar == '(')
        closeChar = ')';
    else if(openChar == '{')
        closeChar = '}';
    else if(openChar == '[')
        closeChar = ']';
    else
        throw createError("Open list expected");
    curPos++;
    while(curPos < content_.size())
    {
        if(isNext("--"))
            parseComment();
        else if(isNext(closeChar))
        {
            curPos++;
            return;
        } else if(isNextString())
            parseString();
        else if(isNext('{') || isNext('(') || isNext('['))
            skipList();
        else if(std::string("})[]").find(getNext()) != std::string::npos)
            throw createError("Invalid char");
        else
            curPos++;
    }
}

LuaTable SimpleLuaParser::parseTable()
{
    RTTR_Assert(getNext() == '{');
    LuaTable result;
    result.data.start = curPos;
    curPos++;
    StringRef lastComment;
    boost::regex nameValueRE("^(\\w+) *= *");
    while(curPos < content_.size())
    {
        curPos = skipWhitespaces(content_, curPos);
        if(curPos == std::string::npos)
            break;
        if(isNext('}'))
        {
            result.data.len = curPos - result.data.start + 1u;
            curPos++;
            if(result.data.get(content_).find('\n') != std::string::npos)
                result.isSingleLine_ = LuaTable::SL_NO;
            return result;
        }
        if(isNext("--"))
        {
            lastComment = parseComment(lastComment);
            if(isTrailingComment(lastComment))
            {
                if(!result.values.empty())
                    result.values.back().postComment = lastComment.getStr(content_);
                lastComment = StringRef();
            }
            continue;
        }
        LuaTableEntry entry;
        if(isAdjacent(lastComment.end(), curPos))
            entry.preComment = lastComment.getStr(content_);
        boost::smatch nameValue;
        if(matchesNext(nameValue, nameValueRE))
        {
            entry.name = content_.substr(curPos, nameValue.length(1));
            curPos += nameValue.length();
        }
        entry.value = parseValue('}');
        result.values.push_back(entry);
    }
    throw createError("Invalid table");
}

void SimpleLuaParser::skipWhitespaceAndComments()
{
    size_t oldPos = curPos;
    while(curPos < content_.size())
    {
        curPos = skipWhitespaces(content_, curPos);
        if(curPos == std::string::npos)
        {
            curPos = oldPos;
            throw createError("Did not find next text");
        }
        if(!isNext("--"))
            break;
        parseComment();
    }
}

size_t SimpleLuaParser::skipWhitespaceAndCommentsBackwards(size_t pos) const
{
    while(pos > 0u)
    {
        pos = skipWhitespaces(content_, pos, false);
        if(pos == std::string::npos)
            throw createError("Did not find previous text");
        if(!isInComment(pos))
            break;
        pos = getStartOfComment(pos) - 1u;
    }
    return pos;
}

void SimpleLuaParser::skipExpression()
{
    static boost::regex identifierRE("\\w+");
    static boost::regex binOpsRE("(\\.\\.)|\\+|-|\\*|/|\\^|%|<|(<=)|>|(>=)|(==)|(~=)|(and)|(or)");
    while(curPos < content_.size())
    {
        skipWhitespaceAndComments();
        // Table? -> Just parse it and go
        if(isNext('{'))
        {
            skipList();
            return;
        }
        // Translated string? Skip "__"
        if(isNext("__"))
        {
            curPos += 2;
            if(!isNextString())
                throw createError("String expected after translation function");
        }
        // String, variable or literal
        boost::smatch match;
        if(isNextString())
            parseString();
        else if(matchesNext(match, identifierRE))
        {
            curPos += match.length();
            // Function call or table subscript?
            skipWhitespaceAndComments();
            if(isNext('{') || isNext('(') || isNext('['))
                skipList();
        }
        skipWhitespaceAndComments();
        // Possibly concatenated with a binary op?
        if(matchesNext(match, binOpsRE))
            curPos += match.length();
        else if(isNext('.') || isNext(':')) // member access
            curPos++;
        else
            return; // End of expression
    }
}

boost::variant<std::string, simpleLuaData::LuaTable> SimpleLuaParser::parseValue(char endChar)
{
    curPos = skipWhitespaces(content_, curPos);
    if(isNext("--"))
        throw createError("Value expected but comment found");
    if(isNext(','))
        throw createError("Value expected but ',' found");
    // Table end
    if(isNext(endChar))
        return "";
    size_t startPos = curPos;
    // Parse subtable
    LuaTable table;
    if(isNext('{'))
        table = parseTable();
    else
        skipExpression();
    if(!isWhitespace(endChar))
    {
        curPos = skipWhitespaces(content_, curPos);
        if(isNext("--"))
            throw createError("Comments in values are not supported");
    } else
        endChar = getNext();
    if(curPos >= content_.size() || isNext(endChar) || isNext(','))
    {
        size_t endPos = skipWhitespaceAndCommentsBackwards(curPos - 1u);
        if(isNext(','))
            curPos++;
        else
            curPos = endPos + 1u; // Go to next after value which might be a comment
        if(endPos < startPos)
            return "";
        else if(table.data.empty())
            return content_.substr(startPos, endPos - startPos + 1u);
        else
            return table;
    }
    throw createError("Invalid value");
}

bool SimpleLuaParser::isNext(const std::string& toSearch) const
{
    return startsWith(content_, curPos, toSearch);
}

bool SimpleLuaParser::isNext(char toSearch) const
{
    return getNext() == toSearch;
}

bool SimpleLuaParser::matchesNext(boost::smatch& match, boost::regex& re) const
{
    return boost::regex_search(content_.begin() + curPos, content_.end(), match, re, boost::regex_constants::match_continuous);
}

char SimpleLuaParser::getNext() const
{
    return content_[curPos];
}

template<class T_Str, class T_Str2>
size_t find(const T_Str& str, const T_Str2& strToFind, size_t offset = 0)
{
    string_view tmp = string_view(str).substr(offset);
    size_t result = tmp.find(strToFind);
    return (result != std::string::npos) ? result + offset : result;
}

SimpleLuaParser::SimpleLuaParser(const std::string& content)
{
    setContent(content);
}

void SimpleLuaParser::setContent(const std::string& content)
{
    content_ = content;
    curPos = 0;
}

size_t SimpleLuaParser::getStartOfComment(size_t pos) const
{
    if(pos == std::string::npos)
        return std::string::npos;
    // Now go to the start of this line or of the text
    size_t startLine = getStartOfLine(content_, pos);
    // Cut out line
    string_view curLine = string_view(content_).substr(startLine, pos - startLine);
    // Just look for a start of a comment.
    // NOTE: Ignores that "--" could be in a string, hence could have false positives
    size_t cmtStart = curLine.find("--");
    if(cmtStart == std::string::npos)
        return std::string::npos;
    return startLine + cmtStart;
}

bool SimpleLuaParser::isNextString()
{
    return isNext('\'') || isNext('"');
}

const LuaTableEntry& LuaTable::operator[](const std::string& entryName) const
{
    int index = indexOf(entryName);
    if(index < 0)
        throw std::runtime_error(entryName + " not found");
    return values[index];
}

LuaTableEntry& LuaTable::operator[](const std::string& entryName)
{
    int index = indexOf(entryName);
    if(index < 0)
        throw std::runtime_error(entryName + " not found");
    return values[index];
}

bool LuaTable::isSingleLine() const
{
    if(isSingleLine_ == SL_NO)
        return false;
    BOOST_FOREACH(const LuaTableEntry& entry, values)
    {
        // Any comment -> not single line
        if(!entry.postComment.empty() || !entry.preComment.empty())
            return false;
        if(entry.value.type() == typeid(LuaTable) && !boost::get<LuaTable>(entry.value).isSingleLine())
            return false;
    }
    return true;
}

bool LuaTable::needsSpaceInside() const
{
    BOOST_FOREACH(const LuaTableEntry& entry, values)
    {
        if(!entry.name.empty())
            return true;
    }
    return false;
}

int LuaTable::indexOf(const std::string& name) const
{
    int i = 0;
    BOOST_FOREACH(const LuaTableEntry& entry, values)
    {
        if(entry.name == name)
            return i;
        ++i;
    }
    return -1;
}

std::string LuaTable::toString(int indentAmount) const
{
    std::stringstream result;
    bool singleLine = isSingleLine();
    if(singleLine)
        indentAmount = 0;
    std::string indent(indentAmount, '\t');
    if(values.empty())
        return singleLine ? "{}" : "{\n" + indent + "}";
    if(singleLine)
        result << (needsSpaceInside() ? "{ " : "{");
    else
    {
        result << "{\n";
        indentAmount++;
    }
    const LuaTableEntry* lastEntry = &values.back();
    BOOST_FOREACH(const LuaTableEntry& entry, values)
    {
        result << entry.toString(indentAmount, &entry != lastEntry && values.size() > 1u);
        if(singleLine)
        {
            if(&entry != lastEntry || needsSpaceInside())
                result << " ";
        } else
            result << "\n";
    }
    if(!singleLine)
        result << indent;
    result << "}";
    return result.str();
}

const LuaTableEntry& LuaTableEntry::operator[](const std::string& entryName) const
{
    return boost::get<LuaTable>(value)[entryName];
}

std::string LuaTableEntry::toString(int indentAmount, bool includeComma) const
{
    std::string indent(indentAmount, '\t');
    std::stringstream result;
    result << indent;
    if(!preComment.empty())
    {
        // Make sure indentation is ok and all lines start with "-- "
        Tokenizer tokenizer(preComment, "\n");
        while(tokenizer)
        {
            std::string cur = tokenizer.next();
            if(cur.empty())
                continue;
            boost::algorithm::trim(cur);
            if(!startsWith(cur, 0, "--"))
                cur.insert(0, "-- ");
            result << cur << "\n" << indent;
        }
    }
    if(!name.empty())
        result << name << " = ";
    if(value.type() == typeid(LuaTable))
        result << boost::get<LuaTable>(value).toString(indentAmount);
    else
        result << boost::get<std::string>(value);
    if(includeComma)
        result << ",";
    if(!postComment.empty())
    {
        if(postComment.find('\n') != std::string::npos)
            throw std::runtime_error("Post-Comment must not contain newlines");
        std::string cur = boost::algorithm::trim_copy(postComment);
        if(!startsWith(cur, 0, "--"))
            cur.insert(0, "-- ");
        result << cur;
    }
    return result.str();
}

} // namespace simpleLuaData
