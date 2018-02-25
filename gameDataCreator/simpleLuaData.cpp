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
#include "simpleLuaData.h"
#include "helpers/converters.h"
#include "libutil/Tokenizer.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <boost/nowide/fstream.hpp>
#include <algorithm>
#include <stdexcept>

namespace simpleLuaData {
struct GameDataFile::LuaDataError : public std::runtime_error
{
    LuaDataError(const std::string& msg) : std::runtime_error(msg) {}
};

bool startsWith(const std::string& txt, size_t offset, const std::string& toSearch)
{
    return string_view(txt).substr(offset).starts_with(toSearch);
}

GameDataFile::LuaDataError GameDataFile::createError(const std::string& msg) const
{
    return LuaDataError(msg + " at position " + helpers::toString(curPos) + "\n" + contents.substr(curPos, 40));
}

StringRef GameDataFile::parseComment(StringRef lastComment)
{
    size_t endl = contents.find('\n', curPos);
    if(endl == std::string::npos)
        endl = contents.size();
    size_t start = curPos;
    curPos = endl;
    // Ignore trailing whitespace
    endl = skipWhitespaces(contents, endl, false) + 1u;
    // If the last comment was not a trailing one AND the 2 are only separated by whitespace -> combine
    if(!lastComment.empty() && !isTrailingComment(lastComment) && isAdjacent(lastComment.end(), start))
        start = lastComment.start;
    return StringRef(start, endl - start);
}

bool GameDataFile::isAdjacent(size_t pos1, size_t pos2)
{
    if(pos2 < pos1)
        std::swap(pos2, pos1);
    return skipWhitespaces(contents, pos1) >= pos2;
}

bool GameDataFile::isTrailingComment(StringRef comment)
{
    // Just whitespace between newline and comment -> not trailing
    if(skipWhitespaces(contents, getStartOfLine(contents, comment.start)) == comment.start)
        return false;
    // Comment after start of table or function -> not trailing
    size_t pos = skipWhitespaces(contents, comment.start - 1u, false);
    RTTR_Assert(pos < contents.size());
    if(contents[pos] == '{' || contents[pos] == '(')
        return false;
    return true;
}

void GameDataFile::parseString()
{
    char startChar = contents[curPos];
    bool isEscaped = false;
    while(++curPos < contents.size())
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

const LuaTable& GameDataFile::operator[](const std::string& tableName) const
{
    return *findMainTable(tableName);
}

void GameDataFile::skipList()
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
    while(curPos < contents.size())
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

LuaTable GameDataFile::parseTable()
{
    RTTR_Assert(getNext() == '{');
    LuaTable result;
    result.data.start = curPos;
    curPos++;
    StringRef lastComment;
    boost::regex nameValueRE("^(\\w+) *= *");
    while(curPos < contents.size())
    {
        curPos = skipWhitespaces(contents, curPos);
        if(curPos == std::string::npos)
            break;
        if(isNext('}'))
        {
            result.data.len = curPos - result.data.start + 1u;
            curPos++;
            return result;
        }
        if(isNext("--"))
        {
            lastComment = parseComment(lastComment);
            if(isTrailingComment(lastComment))
            {
                if(!result.values.empty())
                    result.values.back().postComment = lastComment.getStr(contents);
                lastComment = StringRef();
            }
            continue;
        }
        LuaTableEntry entry;
        if(isAdjacent(lastComment.end(), curPos))
            entry.preComment = lastComment.getStr(contents);
        boost::smatch nameValue;
        if(matchesNext(nameValue, nameValueRE))
        {
            entry.name = contents.substr(curPos, nameValue.length(1));
            curPos += nameValue.length();
        }
        entry.value = parseValue('}');
        result.values.push_back(entry);
    }
    throw createError("Invalid table");
}

void GameDataFile::skipWhitespaceAndComments()
{
    size_t oldPos = curPos;
    while(curPos < contents.size())
    {
        curPos = skipWhitespaces(contents, curPos);
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

size_t GameDataFile::skipWhitespaceAndCommentsBackwards(size_t pos) const
{
    while(pos > 0u)
    {
        pos = skipWhitespaces(contents, pos, false);
        if(pos == std::string::npos)
            throw createError("Did not find previous text");
        if(!isInComment(pos))
            break;
        pos = getStartOfComment(pos) - 1u;
    }
    return pos;
}

void GameDataFile::skipExpression()
{
    static boost::regex identifierRE("\\w+");
    static boost::regex binOpsRE("(\\.\\.)|\\+|-|\\*|/|\\^|%|<|(<=)|>|(>=)|(==)|(~=)|(and)|(or)");
    while(curPos < contents.size())
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

boost::variant<std::string, simpleLuaData::LuaTable> GameDataFile::parseValue(char endChar)
{
    curPos = skipWhitespaces(contents, curPos);
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
    curPos = skipWhitespaces(contents, curPos);
    if(isNext("--"))
        throw createError("Comments in values are not supported");
    if(isNext(endChar) || isNext(','))
    {
        size_t endPos = skipWhitespaceAndCommentsBackwards(curPos - 1u);
        if(isNext(','))
            curPos++;
        else
            curPos = endPos + 1u; // Go to next after value which might be a comment
        if(endPos < startPos)
            return "";
        else if(table.data.empty())
            return contents.substr(startPos, endPos - startPos + 1u);
        else
            return table;
    }
    throw createError("Invalid value");
}

bool GameDataFile::isNext(const std::string& toSearch) const
{
    return startsWith(contents, curPos, toSearch);
}

bool GameDataFile::isNext(char toSearch) const
{
    return getNext() == toSearch;
}

bool GameDataFile::matchesNext(boost::smatch& match, boost::regex& re) const
{
    return boost::regex_search(contents.begin() + curPos, contents.end(), match, re, boost::regex_constants::match_continuous);
}

char GameDataFile::getNext() const
{
    return contents[curPos];
}

void GameDataFile::setContents(const std::string& src)
{
    contents = src;
    parse();
}

std::string GameDataFile::getUnparsedData() const
{
    std::stringstream result;
    size_t curPos = 0;
    BOOST_FOREACH(const LuaTable& table, tables)
    {
        RTTR_Assert(!table.data.empty());
        result << contents.substr(curPos, table.data.start - curPos) << table.toString();
        curPos = table.data.end();
    }
    result << contents.substr(curPos);
    return result.str();
}

void GameDataFile::parse()
{
    tables.clear();
    boost::regex re("\\w+:Add\\w+(\\{)");
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    curPos = 0;
    std::string::const_iterator curIt = contents.begin();
    std::string::const_iterator endPos = contents.end();
    while(boost::regex_search(curIt, endPos, what, re, flags))
    {
        curPos += what.position(1);
        if(!isInComment(curPos))
        {
            tables.push_back(parseTable());
            // Main tables are always multi-line
            tables.back().isSingleLine_ = LuaTable::SL_NO;
        }
        curIt = contents.begin() + curPos;
    }
}

bool GameDataFile::load(const std::string& filepath)
{
    bnw::ifstream fs(filepath);
    if(!fs)
        return false;
    contents.assign(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
    parse();
    return true;
}

bool GameDataFile::save(const std::string& filepath) const
{
    bnw::ofstream fs(filepath);
    if(!fs)
        return false;
    std::copy(contents.begin(), contents.end(), std::ostreambuf_iterator<char>(fs));
    return true;
}

template<class T_Str, class T_Str2>
size_t find(const T_Str& str, const T_Str2& strToFind, size_t offset = 0)
{
    string_view tmp = string_view(str).substr(offset);
    size_t result = tmp.find(strToFind);
    return (result != std::string::npos) ? result + offset : result;
}

boost::optional<const LuaTable&> GameDataFile::findMainTable(const std::string& name) const
{
    BOOST_FOREACH(const LuaTable& table, tables)
    {
        OptTableEntry entry = findNamedValue(table, "name");
        if(!entry)
            continue;
        if(entry->value.type() != typeid(std::string))
            continue;
        std::string value = boost::get<std::string>(entry->value);
        if(value.substr(0, 2) == "__")
            value.erase(0, 2);
        if(value.front() == value.back())
            value = value.substr(1, value.size() - 2u);
        if(value == name)
            return table;
    }
    return boost::none;
}

GameDataFile::OptLuaTableC GameDataFile::findTable(const std::string& name) const
{
    std::vector<std::string> parts = Tokenizer(name, ":").explode();
    RTTR_Assert(!parts.empty());
    std::reverse(parts.begin(), parts.end());
    OptLuaTableC table = findMainTable(parts.back());
    if(!table)
        return boost::none;
    parts.pop_back();
    while(!parts.empty())
    {
        OptTableEntry entry = findNamedValue(*table, parts.back());
        if(!entry || entry->value.type() != typeid(LuaTable))
            return boost::none;
        parts.pop_back();
        table = boost::get<LuaTable>(entry->value);
    }
    return table;
}

GameDataFile::OptLuaTable GameDataFile::findTable(const std::string& name)
{
    OptLuaTableC result = const_cast<const GameDataFile&>(*this).findTable(name);
    if(result)
        return const_cast<LuaTable&>(*result);
    else
        return boost::none;
}

boost::optional<const LuaTableEntry&> GameDataFile::findNamedValue(const LuaTable& table, const std::string& name) const
{
    int index = table.indexOf(name);
    if(index < 0)
        return boost::none;
    else
        return table.values[index];
}

std::string GameDataFile::getValue(const std::string& elName) const
{
    size_t pos = elName.find_last_of(':');
    if(pos == std::string::npos)
        throw std::runtime_error("Invalid value name " + elName);
    std::string tableName = elName.substr(0, pos);
    OptLuaTableC table = findTable(tableName);
    if(!table)
        return "";
    OptTableEntry entry = findNamedValue(*table, elName.substr(pos + 1u));
    if(!entry)
        return "";
    if(entry->value.type() == typeid(LuaTable))
        throw std::runtime_error(elName + " is a table, not a value");
    else
        return boost::get<std::string>(entry->value);
}

void GameDataFile::insertFieldAfter(const std::string& elName, const std::string& name, const std::string& value,
                                    const std::string& comment)
{
    LuaTableEntry entry;
    entry.name = name;
    entry.value = value;
    entry.preComment = comment;
    insertFieldAfter(elName, entry);
}

void GameDataFile::insertFieldAfter(const std::string& elName, const LuaTableEntry& entry)
{
    LuaTable* outerTable;
    size_t pos = findPosition(elName, &outerTable);
    if(outerTable->values.size() < pos)
        outerTable->values.push_back(entry);
    else
        outerTable->values.insert(outerTable->values.begin() + pos, entry);
}

void GameDataFile::insertFieldAfter(const std::string& elName, const std::string& entry)
{
    static boost::regex elRE("((--.*?\\n)*)\\s*((\\w+)\\s*=\\s*)?(.+)");
    boost::smatch match;
    if(!boost::regex_match(entry, match, elRE))
        throw std::runtime_error("Invalid name-value-pair" + entry);
    insertFieldAfter(elName, match[4], match[5], match[1]);
}

size_t GameDataFile::findPosition(const std::string& elName, LuaTable** outerTable)
{
    // Check if this is a table
    OptLuaTable table = findTable(elName);
    if(table)
    {
        if(outerTable)
            *outerTable = table.get_ptr();
        return table->values.size();
    }
    size_t pos = elName.find_last_of(':');
    if(pos == std::string::npos)
        throw std::runtime_error("Table with name " + elName + " not found");
    std::string parentName = elName.substr(0, pos);
    table = findTable(parentName);
    if(!table)
        throw std::runtime_error("Subtable with name " + parentName + " not found");
    if(outerTable)
        *outerTable = table.get_ptr();
    int index = table->indexOf(elName.substr(pos + 1));
    return (index >= 0) ? static_cast<size_t>(index + 1) : table->values.size();
}

size_t GameDataFile::getStartOfComment(size_t pos) const
{
    if(pos == std::string::npos)
        return std::string::npos;
    // Now go to the start of this line or of the text
    size_t startLine = getStartOfLine(contents, pos);
    // Cut out line
    string_view curLine = string_view(contents).substr(startLine, pos - startLine);
    // Just look for a start of a comment.
    // NOTE: Ignores that "--" could be in a string, hence could have false positives
    size_t cmtStart = curLine.find("--");
    if(cmtStart == std::string::npos)
        return std::string::npos;
    return startLine + cmtStart;
}

bool GameDataFile::isNextString()
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
        result << "{ ";
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
            result << " ";
        else
            result << "\n";
    }
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
        std::string tmp(preComment);
        if(tmp.back() != '\n')
            tmp += '\n';
        boost::algorithm::replace_all(tmp, "\n", "\n" + indent);
        result << tmp;
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
        result << " " << postComment;
    return result.str();
}

} // namespace simpleLuaData
