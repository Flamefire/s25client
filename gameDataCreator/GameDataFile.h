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

#ifndef GameDataFile_h__
#define GameDataFile_h__

#include "simpleLuaParser.h"
#include <boost/container/vector.hpp>
#include <boost/optional.hpp>
#include <string>

namespace simpleLuaData {

class GameDataFile
{
    std::string content, filepath_, lastInsertedField;
    boost::container::vector<LuaTable> tables;
    boost::container::vector<LuaNamedValue> namedValues;

    size_t findPosition(const std::string& elName, LuaTable** outerTable);

public:
    typedef boost::optional<const LuaTable&> OptLuaTableC;
    typedef boost::optional<LuaTable&> OptLuaTable;
    typedef boost::optional<const LuaTableEntry&> OptTableEntry;
    typedef boost::optional<LuaNamedValue&> OptNamedValue;

    /// Replace the whole buffer invalidating all references
    void setContent(const std::string& src);
    const std::string& getContent() { return content; }
    const std::string& getFilepath() { return filepath_; }
    std::string getUnparsedData() const;
    /// Reparse contents invalidating all references
    void parse();
    /// Get all main tables
    boost::container::vector<LuaTable>& getTables() { return tables; }
    const boost::container::vector<LuaTable>& getTables() const { return tables; }
    /// Load from file
    bool load(const std::string& filepath);
    /// Save from file
    bool save(const std::string& filepath) const;
    /// Reset everything
    void clear();
    /// Get the comment of a given value
    std::string getComment(const LuaTableEntry& valRef) const { return valRef.preComment; }
    template<class T>
    std::string getComment(const boost::optional<T>& ref) const
    {
        return ref ? getComment(*ref) : "";
    }
    /// Get the data for a given value
    template<class T>
    std::string getData(const T& ref) const
    {
        return ref.data.getStr(content);
    }
    template<class T>
    std::string getData(const boost::optional<T>& ref) const
    {
        return ref ? getData(*ref) : "";
    }

    /// Find the first table that has the given name. Range includes { and }
    const LuaTable& operator[](const std::string& tableName) const;
    /// Find the first table that has the given name. Range includes { and }
    OptLuaTableC findMainTable(const std::string& name) const;
    /// Find the table with the given qualified name (e.g. "foo:bar:baz")
    OptLuaTable findTable(const std::string& name);
    OptLuaTableC findTable(const std::string& name) const;
    /// Find the entry with the given name (table.name)
    OptTableEntry findTableEntry(const LuaTable& table, const std::string& name) const;
    /// Get the value with the qualified name
    std::string getValue(const std::string& elName) const;
    OptNamedValue findNamedValue(const std::string& name);

    /// Insert the given field (e.g. "foo = bar") after the named field which may be nested (e.g. "foo:bar") or at the end of the parent
    /// table if field does not exist Will add a comma to the previous field if required and the same indentation as the previous field
    void insertFieldAfter(const std::string& elName, const std::string& name, const std::string& value, const std::string& comment = "");
    void insertFieldAfter(const std::string& elName, const std::string& name, int value, const std::string& comment = "");
    void insertFieldAfter(const std::string& elName, const LuaTableEntry& entry);
    void insertFieldAfter(const std::string& elName, const std::string& entry);
    /// Adds a field after the last inserted field
    void insertField(const std::string& name, const std::string& value, const std::string& comment = "");
    void insertField(const std::string& name, int value, const std::string& comment = "");
    /// Add a new table after the last inserted field (if it doesn't exist yet)
    void insertTable(const std::string& name, const std::string& comment = "");

    void setNameValue(const std::string& name, const std::string& value);

    std::string getTableName(unsigned idx) const;
};

} // namespace simpleLuaData

#endif // GameDataFile_h__
