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

#ifndef addNewData_h__
#define addNewData_h__

#include <boost/optional.hpp>
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
    std::string get(const std::string& src) const { return empty() ? "" : src.substr(start, len); }
};

struct LuaValueRef
{
    StringRef data;
    StringRef comment;
    explicit LuaValueRef(StringRef data, StringRef comment = StringRef()) : data(data), comment(comment) {}
};

struct LuaTableEntry;
class GameDataFile;

struct LuaTable
{
    StringRef data;
    std::vector<LuaTableEntry> values;

    explicit LuaTable(const GameDataFile& gd) : gd(&gd) {}
    const LuaTableEntry& operator[](const std::string& entryName) const;

private:
    const GameDataFile* gd;
};

struct LuaTableEntry
{
    StringRef data;
    StringRef comment;
    StringRef name;
    boost::variant<StringRef, LuaTable> value;
};

class GameDataFile
{
    std::string contents;
    std::vector<LuaTable> tables;
    size_t curPos;
    bool isAdjacent(size_t pos1, size_t pos2);
    bool isTrailingComment(StringRef comment);
    StringRef parseComment(StringRef lastComment = StringRef());
    void parseString();
    bool isNext(const std::string& toSearch) const;
    bool isNext(char toSearch) const;
    bool matchesNext(boost::smatch& match, boost::regex& re) const;
    char getNext() const;

public:
    struct LuaDataError;

    void setContents(const std::string& src);
    const std::string& getContents() const { return contents; }
    const std::vector<LuaTable>& getTables() const { return tables; }
    bool load(const std::string& filepath);
    bool save(const std::string& filepath) const;
    void parse();
    /// Find the first table that has the given name. Range includes { and }
    boost::optional<const LuaTable&> findTableByName(const std::string& name) const;
    boost::optional<const LuaTableEntry&> findNamedValue(const LuaTable& table, const std::string& name) const;
    /// Return true, if the position is part of a comment (has "--" before a new line)
    bool isInComment(size_t pos) const { return getStartOfComment(pos) != std::string::npos; }
    /// Get the start of the comment ("--" before a new line) or npos if not found
    size_t getStartOfComment(size_t pos) const;
    std::string getComment(const LuaTableEntry& valRef) const { return valRef.comment.get(contents); }
    template<class T>
    std::string getComment(const boost::optional<T>& ref) const
    {
        return ref ? getComment(*ref) : "";
    }
    std::string getData(const LuaValueRef& valRef) const { return valRef.data.get(contents); }
    std::string getData(const LuaTable& tableRef) const { return tableRef.data.get(contents); }
    template<class T>
    std::string getData(const boost::optional<T>& ref) const
    {
        return ref ? getData(*ref) : "";
    }

    const LuaTable& operator[](const std::string& tableName);

private:
    void skipList();
    bool isNextString();
    LuaTable parseTable();
    boost::variant<StringRef, LuaTable> parseValue(char endChar);
    void skipExpression();
    void skipWhitespaceAndComments();
    size_t skipWhitespaceAndCommentsBackwards(size_t pos) const;
    LuaDataError createError(const std::string& msg) const;
};

//////////////////////////////////////////////////////////////////////////

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
    }
    return std::string::npos;
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

#endif // addNewData_h__
