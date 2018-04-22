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

#ifndef simpleLuaParser_h__
#define simpleLuaParser_h__

#include <boost/container/vector.hpp>
#include <boost/regex.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>
#include <string>

namespace simpleLuaData {

/// View into a string. Can be replaced by future std::string_view or boost::string_view
typedef boost::string_ref string_view;

bool isWhitespace(char c);

/// Skip whitespace (space, tab, newline) in the given direction and return position to first non-whitespace char or std::string::npos
template<typename T_String>
size_t skipWhitespaces(const T_String& str, size_t pos = 0, bool forward = true);
/// Return the position of the first char on the current line. This is the char after the previous \n or 0 if no \n found
template<typename T_String>
size_t getStartOfLine(const T_String& str, size_t pos);

struct StringRef
{
    size_t start, len;
    explicit StringRef(size_t start = 0, size_t len = 0) : start(start), len(len) {}
    bool empty() const { return len == 0; }
    size_t end() const { return start + len; }
    string_view get(const std::string& src) const { return string_view(src).substr(start, len); }
    std::string getStr(const std::string& src) const { return get(src).to_string(); }
};

struct LuaTableEntry;

struct LuaTable
{
    enum ESingleLine
    {
        SL_YES,
        SL_NO
    };
    ESingleLine isSingleLine_;
    boost::container::vector<LuaTableEntry> values;
    /// Data range in original data. Only required for main tables
    StringRef data;

    LuaTable() : isSingleLine_(SL_YES) {}
    const LuaTableEntry& operator[](const std::string& entryName) const;
    LuaTableEntry& operator[](const std::string& entryName);
    bool isSingleLine() const;
    bool needsSpaceInside() const;
    int indexOf(const std::string& name) const;
    std::string toString(int indentAmount = 0) const;
};

struct LuaTableEntry
{
    std::string preComment, postComment;
    std::string name;
    boost::variant<std::string, LuaTable> value;

    const LuaTableEntry& operator[](const std::string& entryName) const;
    std::string toString(int indentAmount = 0, bool includeComma = false) const;
};

struct LuaNamedValue : LuaTableEntry
{
    /// Data range in original data.
    StringRef data;
};

class SimpleLuaParser
{
    struct LuaDataError;

    std::string content_;
    bool isAdjacent(size_t pos1, size_t pos2);
    bool isTrailingComment(StringRef comment);
    StringRef parseComment(StringRef lastComment = StringRef());
    void parseString();
    bool isNext(const std::string& toSearch) const;
    bool isNext(char toSearch) const;
    bool matchesNext(boost::smatch& match, boost::regex& re) const;
    char getNext() const;
    void skipList();
    bool isNextString();
    void skipExpression();
    void skipWhitespaceAndComments();
    size_t skipWhitespaceAndCommentsBackwards(size_t pos) const;
    LuaDataError createError(const std::string& msg) const;

public:
    size_t curPos;

    SimpleLuaParser(const std::string& content);
    void setContent(const std::string& content);
    const std::string& getContent() { return content_; }
    /// Return true, if the position is part of a comment (has "--" before a new line)
    bool isInComment(size_t pos) const { return getStartOfComment(pos) != std::string::npos; }
    bool isInComment() const { return isInComment(curPos); }
    /// Get the start of the comment ("--" before a new line) or npos if not found
    size_t getStartOfComment(size_t pos) const;

    LuaTable parseTable();
    boost::variant<std::string, LuaTable> parseValue(char endChar);
};

//////////////////////////////////////////////////////////////////////////

inline bool startsWith(const std::string& txt, size_t offset, const std::string& toSearch)
{
    return string_view(txt).substr(offset).starts_with(toSearch);
}

inline bool isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

template<typename T_String>
inline size_t skipWhitespaces(const T_String& str, size_t pos, bool forward)
{
    if(forward)
    {
        while(pos < str.size())
        {
            if(!isWhitespace(str[pos]))
                return pos;
            pos++;
        }
    } else if(pos < str.size())
    {
        while(pos > 0)
        {
            if(!isWhitespace(str[pos]))
                return pos;
            pos--;
        }
        return std::string::npos;
    }
    return pos;
}

template<typename T_String>
inline size_t getStartOfLine(const T_String& str, size_t pos)
{
    string_view tmpStr = string_view(str).substr(0, pos);
    size_t startLine = tmpStr.rfind('\n');
    if(startLine == std::string::npos)
        return 0;
    return startLine + 1u;
}

} // namespace simpleLuaData

#endif // simpleLuaParser_h__
