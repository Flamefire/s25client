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
#include <boost/foreach.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/regex.hpp>
#include <stdexcept>

namespace simpleLuaData {
    bool startsWith(const std::string& txt, size_t offset, const std::string& toSearch)
    {
        return string_view(txt).substr(offset).starts_with(toSearch);
    }

    simpleLuaData::StringRef GameDataFile::parseComment(StringRef lastComment)
{
        size_t endl = contents.find('\n', curPos);
        if(endl == std::string::npos)
            endl = contents.size();
        curPos = endl;
        // If the last comment was not a trailing one AND the 2 are only separated by whitespace -> combine
        if(!lastComment.empty() && !isTrailingComment(lastComment) && isAdjacent(lastComment.end(), curPos))
        {
            return StringRef(lastComment.start, endl - lastComment.start);
        }else
        return StringRef(curPos, endl - curPos);
    }

    bool GameDataFile::isAdjacent(size_t pos1, size_t pos2)
    {
        if(pos2 < pos1)
            std::swap(pos2, pos1);
        return skipWhitespaces(contents, pos1) >= pos2;
    }

    bool GameDataFile::isTrailingComment(StringRef comment)
    {
        return skipWhitespaces(contents, getStartOfLine(contents, comment.start)) != comment.start;
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
                throw std::runtime_error("New line in string");
        }
    }

    void GameDataFile::skipList()
{
        char openChar = getNext();
        RTTR_Assert(openChar == '(' || openChar == '{');
        char closeChar = (openChar == '(') ? ')' : '}';
        curPos++;
        while(curPos < contents.size())
        {
            if(isNext("--"))
                parseComment();
            else if(isNext(closeChar))
            {
                curPos++;
                return;
            }
            else if(isNext("'") || isNext("\""))
                parseString();
            else if(isNext('{') || isNext('('))
                skipList();
            else if(std::string("})[]").find(getNext()) != std::string::npos)
                throw std::runtime_error("Invalid char");
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
        boost::regex nameValueRE("$(\w+) += +");
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
                // Ignore for now
                if(isTrailingComment(lastComment))
                    lastComment = StringRef();
                continue;
            }
            LuaTableEntry entry;
            entry.data.start = curPos;
            if(isAdjacent(lastComment.end(), curPos))
                entry.comment = lastComment;
            boost::smatch nameValue;
            if(matchesNext(nameValue, nameValueRE))
            {
                entry.name = StringRef(nameValue.position(1), nameValue.length(1));
                curPos += nameValue.position() + nameValue.length();
            }
            entry.value = parseValue('}');
            entry.data.len = curPos - entry.data.start + 1u;
            result.values.push_back(entry);
        }
    }

    boost::variant<simpleLuaData::StringRef, simpleLuaData::LuaTable> GameDataFile::parseValue(char endChar)
{
        curPos = skipWhitespaces(contents, curPos);
        RTTR_Assert(!isNext("--"));
        // Table end
        if(isNext('}'))
            return StringRef();
        if(isNext('{'))
            return parseTable();
        // Ignore translations
        if(isNext("__"))
            curPos += 2;
        size_t startPos = curPos;
        while(curPos < contents.size())
        {
            curPos = skipWhitespaces(contents, curPos);
            if(curPos == std::string::npos)
                break;
            // End of table or value
            if(isNext(endChar) || isNext(','))
            {
                size_t endPos = skipWhitespaces(contents, curPos - 1u, false);
                if(endPos < startPos)
                    return StringRef();
                else
                    return StringRef(startPos, endPos - startPos + 1u);
            }
            // Skip over
            skipList();
        }
    }

    bool GameDataFile::isNext(const std::string & toSearch) const
    {
        return startsWith(contents, curPos, toSearch);
    }

    bool GameDataFile::isNext(char toSearch) const
    {
        return getNext() == toSearch;
    }

    bool GameDataFile::matchesNext(boost::smatch& match, boost::regex& re) const
    {
        return boost::regex_match(contents.begin() + curPos, contents.end(), match, re);
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

    void GameDataFile::parse()
    {
        boost::regex re("\w+:Add\w+(\{)");
            boost::match_results<std::string::const_iterator> what;
            boost::match_flag_type flags = boost::match_default;
            std::string::const_iterator curIt = contents.begin();
            std::string::const_iterator endPos = contents.end();
            while(boost::regex_search(curIt, endPos, what, re, flags))
            {
                curPos = what.position(1);
                if(!isInComment(curPos))
                {
                    tables.push_back(parseTable());
                    curIt = contents.begin() + curPos;
                }else
                    curIt += what.position() + what.length();
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

boost::optional<LuaValueRef> GameDataFile::findTableByName(const std::string& name) const
{
    // Find the first occurrence of a name declaration (possibly translated)
    size_t namePos = 0;
    while(true)
    {
        size_t namePos2 = contents.find(" name = __\"" + name + "\"", namePos);
        namePos = contents.find(" name = \"" + name + "\"", namePos);
        if(namePos == std::string::npos)
        {
            if(namePos2 == std::string::npos)
                return boost::none;
            namePos = namePos2;
        } else if(namePos2 < namePos && !isInComment(namePos2))
            namePos = namePos2;
        // If it is not in a comment -> OK
        if(!isInComment(namePos))
            break;
        // Skip rest of line
        namePos = contents.find('\n', namePos);
        // Nothing more -> Go
        if(namePos == std::string::npos)
            return boost::none;
    }
    // Now go backwards to find the start of the table
    boost::optional<size_t> tableStart;
    int tableDepth = 0;
    for(size_t i = namePos; i > 0; i--)
    {
        size_t cmtStart = getStartOfComment(i);
        if(cmtStart != std::string::npos)
        {
            i = cmtStart;
            if(i == 0)
                break;
        }
        char cur = contents[i - 1];
        if(cur == '{')
        {
            if(tableDepth == 0)
            {
                tableStart = i - 1;
                break;
            } else
                tableDepth--;
        } else if(cur == '}')
            tableDepth++;
    }
    if(!tableStart)
        return boost::none;
    // And forwards to find the end
    boost::optional<size_t> tableEnd;
    tableDepth = 0;
    for(size_t i = namePos; i < contents.length(); i++)
    {
        if(isInComment(i))
        {
            // Skip till line end. npos check omitted as continue is used which checks against len
            i = contents.find('\n', i);
            continue;
        }
        char cur = contents[i];
        if(cur == '}')
        {
            if(tableDepth == 0)
            {
                tableEnd = i;
                break;
            } else
                tableDepth--;
        } else if(cur == '{')
            tableDepth++;
    }
    if(!tableEnd)
        return boost::none;
    return LuaValueRef(StringRef(*tableStart, *tableEnd - *tableStart + 1));
}

bool GameDataFile::findComment(LuaValueRef& valRef) const
{
    // Comment starts at data if no comment
    size_t startCmt = valRef.data.start;
    size_t endCmt = startCmt;
    // Search previous lines for comments and add all found ones
    while(startCmt > 0)
    {
        // Skip backwards to end of previous line
        size_t endl = skipWhitespaces(contents, contents.rfind('\n', startCmt), false);
        // If no previous (non-whitespace) line found -> stop
        if(endl == std::string::npos)
            break;
        // Try to find a comment
        size_t curCmtStart = getStartOfComment(endl);
        if(curCmtStart == std::string::npos)
            break;
        // Check if is a trailing comment (anything except whitespace is in front of it)
        size_t startLine = getStartOfLine(contents, curCmtStart);
        if(skipWhitespaces(string_view(contents).substr(startLine, curCmtStart - startLine)) !=std::string::npos)
            break;
        // If first comment line, update end position
        if(startCmt == endCmt)
            endCmt = endl;
        // Extent comment
        startCmt = startLine;
    }
    if(startCmt == endCmt)
    {
        // We might have a trailing comment
        string_view strEndLine = string_view(contents).substr(valRef.data.end());
        strEndLine = strEndLine.substr(0, strEndLine.find('\n'));
        size_t endVal = skipWhitespaces(strEndLine, 0);
        if(endVal == std::string::npos || !strEndLine.substr(endVal).starts_with("--"))
            return false;
        startCmt = endVal + valRef.data.end();
        endCmt = skipWhitespaces(strEndLine, strEndLine.size() - 1, false) + valRef.data.end();
    }
    valRef.comment = StringRef(startCmt, endCmt - startCmt + 1);
    return true;
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

} // namespace simpleLuaData
