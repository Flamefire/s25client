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
#include "GameDataFile.h"
#include "libutil/StringConversion.h"
#include "libutil/Tokenizer.h"
#include <boost/nowide/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace simpleLuaData {

const LuaTable& GameDataFile::operator[](const std::string& tableName) const
{
    return *findMainTable(tableName);
}

void GameDataFile::setContent(const std::string& src)
{
    content = src;
    parse();
}

std::string GameDataFile::getUnparsedData() const
{
    std::stringstream result;
    size_t curPos = 0;
    BOOST_FOREACH(const LuaNamedValue& namedValue, namedValues)
    {
        if(namedValue.data.empty())
        {
            // Insert new element before next non-whitespace char
            if(curPos != std::string::npos)
            {
                size_t endPos = skipWhitespaces(content, curPos);
                result << content.substr(curPos, endPos - curPos);
                curPos = endPos;
            }
        } else
            result << content.substr(curPos, namedValue.data.start - curPos);
        result << namedValue.toString();
        if(namedValue.data.empty())
            result << '\n';
        else
            curPos = namedValue.data.end();
    }
    BOOST_FOREACH(const LuaTable& table, tables)
    {
        if(table.data.empty())
        {
            // Insert new element before next non-whitespace char
            if(curPos != std::string::npos)
            {
                size_t endPos = skipWhitespaces(content, curPos);
                result << content.substr(curPos, endPos - curPos);
                curPos = endPos;
            }
        } else
            result << content.substr(curPos, table.data.start - curPos);
        result << table.toString();
        if(table.data.empty())
            result << '\n';
        else
            curPos = table.data.end();
    }
    if(curPos != std::string::npos)
        result << content.substr(curPos);
    return result.str();
}

void GameDataFile::parse()
{
    clear();
    boost::regex re("\\w+:Add\\w+(\\{)");
    boost::match_results<std::string::const_iterator> match;
    std::string::const_iterator curIt = content.begin();
    std::string::const_iterator endPos = content.end();
    if(boost::regex_search(curIt, endPos, match, re))
        endPos = curIt + match.position();
    boost::regex reValue("^(\\w+)\\s*=\\s*");
    SimpleLuaParser parser(content);
    while(boost::regex_search(curIt, endPos, match, reValue))
    {
        parser.curPos += match.position();
        LuaNamedValue namedValue;
        namedValue.data.start = parser.curPos;
        namedValue.name = match.str(1);

        parser.curPos += match.length();
        namedValue.value = parser.parseValue(' ');
        namedValue.data.len = parser.curPos - namedValue.data.start;

        curIt = content.begin() + parser.curPos;
        // Catch e.g. "nation = rttr:AddNation..."
        if(curIt > endPos)
            break;
        namedValues.push_back(namedValue);
    }

    curIt = content.begin();
    endPos = content.end();
    parser.curPos = 0;
    while(boost::regex_search(curIt, endPos, match, re))
    {
        parser.curPos += match.position(1);
        if(!parser.isInComment())
        {
            tables.push_back(parser.parseTable());
            // Main tables are always multi-line
            tables.back().isSingleLine_ = LuaTable::SL_NO;
        }
        curIt = content.begin() + parser.curPos;
    }
}

bool GameDataFile::load(const std::string& filepath)
{
    bnw::ifstream fs(filepath);
    if(!fs)
        return false;
    content.assign(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
    parse();
    filepath_ = filepath;
    return true;
}

bool GameDataFile::save(const std::string& filepath) const
{
    bnw::ofstream fs(filepath);
    if(!fs)
        return false;
    std::string result = getUnparsedData();
    std::copy(result.begin(), result.end(), std::ostreambuf_iterator<char>(fs));
    return true;
}

void GameDataFile::clear()
{
    namedValues.clear();
    tables.clear();
    lastInsertedField.clear();
    filepath_.clear();
}

std::string GameDataFile::getTableName(unsigned idx) const
{
    if(idx >= tables.size())
        return "";
    OptTableEntry entry = findTableEntry(tables[idx], "name");
    if(!entry)
        return "";
    if(entry->value.type() != typeid(std::string))
        return "";
    std::string value = boost::get<std::string>(entry->value);
    if(value.substr(0, 2) == "__")
        value.erase(0, 2);
    if(value.front() == value.back())
        value = value.substr(1, value.size() - 2u);
    return value;
}

boost::optional<const LuaTable&> GameDataFile::findMainTable(const std::string& name) const
{
    for(unsigned i = 0; i < tables.size(); i++)
    {
        std::string value = getTableName(i);
        if(value.empty())
            continue;
        // Empty name => get first non empty
        if(name.empty() || value == name)
            return tables[i];
    }
    return boost::none;
}

GameDataFile::OptLuaTableC GameDataFile::findTable(const std::string& name) const
{
    if(name.empty())
        return findMainTable(name);
    std::vector<std::string> parts = Tokenizer(name, ":").explode();
    RTTR_Assert(!parts.empty());
    std::reverse(parts.begin(), parts.end());
    OptLuaTableC table = findMainTable(parts.back());
    if(!table)
        return boost::none;
    parts.pop_back();
    while(!parts.empty())
    {
        OptTableEntry entry = findTableEntry(*table, parts.back());
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

boost::optional<const LuaTableEntry&> GameDataFile::findTableEntry(const LuaTable& table, const std::string& name) const
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
    OptTableEntry entry = findTableEntry(*table, elName.substr(pos + 1u));
    if(!entry)
        return "";
    if(entry->value.type() == typeid(LuaTable))
        throw std::runtime_error(elName + " is a table, not a value");
    else
        return boost::get<std::string>(entry->value);
}

GameDataFile::OptNamedValue GameDataFile::findNamedValue(const std::string& name)
{
    BOOST_FOREACH(LuaNamedValue& namedValue, namedValues)
    {
        if(namedValue.name == name)
            return namedValue;
    }
    return boost::none;
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
    bool inserted = false;
    if(!entry.name.empty())
    {
        int index = outerTable->indexOf(entry.name);
        if(index >= 0)
        {
            outerTable->values[index] = entry;
            inserted = true;
        }
    }
    if(!inserted)
    {
        if(outerTable->values.size() < pos)
            outerTable->values.push_back(entry);
        else
            outerTable->values.insert(outerTable->values.begin() + pos, entry);
    }
    if(entry.name.empty())
        lastInsertedField.clear();
    else
    {
        size_t pos = elName.find_last_of(':');
        lastInsertedField = elName.substr(0, pos) + ":" + entry.name;
    }
}

void GameDataFile::insertFieldAfter(const std::string& elName, const std::string& entry)
{
    static boost::regex elRE("((--.*?\\n)*)\\s*((\\w+)\\s*=\\s*)?(.+)");
    boost::smatch match;
    if(!boost::regex_match(entry, match, elRE))
        throw std::runtime_error("Invalid name-value-pair" + entry);
    insertFieldAfter(elName, match[4], match[5], match[1]);
}

void GameDataFile::insertField(const std::string& name, const std::string& value, const std::string& comment)
{
    if(lastInsertedField.empty())
        throw std::runtime_error("Did not find previously inserted field. Insert one first!");
    insertFieldAfter(lastInsertedField, name, value, comment);
}

void GameDataFile::insertField(const std::string& name, int value, const std::string& comment)
{
    insertField(name, s25util::toStringClassic(value), comment);
}

void GameDataFile::insertTable(const std::string& name, const std::string& comment)
{
    if(findTable(name))
        return;
    if(lastInsertedField.empty())
        throw std::runtime_error("Did not find previously inserted field. Insert one first!");
    LuaTableEntry entry;
    entry.name = name;
    entry.value = LuaTable();
    entry.preComment = comment;
    insertFieldAfter(lastInsertedField, entry);
}

void GameDataFile::setNameValue(const std::string& name, const std::string& value)
{
    OptNamedValue old = findNamedValue(name);
    if(old)
        old->value = value;
    else
    {
        LuaNamedValue newValue;
        newValue.name = name;
        newValue.value = value;
        namedValues.push_back(newValue);
    }
}

void GameDataFile::insertFieldAfter(const std::string& elName, const std::string& name, int value, const std::string& comment)
{
    insertFieldAfter(elName, name, s25util::toStringClassic(value), comment);
}

size_t GameDataFile::findPosition(const std::string& elName, LuaTable** outerTable)
{
    size_t pos = elName.find_last_of(':');
    std::string parentName = elName.substr(0, pos);
    std::string entryName = (pos == std::string::npos) ? "" : elName.substr(pos + 1u);

    OptLuaTable table = findTable(parentName);
    if(!table)
        throw std::runtime_error("Table with name " + parentName + " not found");
    if(outerTable)
        *outerTable = table.get_ptr();
    if(entryName.empty())
        return table->values.size();
    int index = table->indexOf(entryName);
    if(index < 0)
        return table->values.size();
    return static_cast<size_t>(index + 1);
}

} // namespace simpleLuaData
