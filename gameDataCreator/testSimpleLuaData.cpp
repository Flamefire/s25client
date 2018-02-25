// Copyright (c) 2017 - 2017 Settlers Freaks (sf-team at siedler25.org)
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
#include "RttrConfig.h"
#include "simpleLuaData.h"
#include <boost/test/unit_test.hpp>
#ifndef BOOST_NO_CXX11_HDR_CHRONO
#include <chrono>
#endif

using namespace simpleLuaData;

BOOST_AUTO_TEST_SUITE(SimpleLuaData)

static const size_t tblOff = strlen("rttr:AddBld");

BOOST_AUTO_TEST_CASE(SkipWhitespaces)
{
    std::string str = " \t\r\na \t\r\n";
    for(unsigned i = 0; i <= 4; i++)
        BOOST_REQUIRE_EQUAL(skipWhitespaces(str, i), 4u);
    for(unsigned i = 5; i <= str.size(); i++)
        BOOST_REQUIRE_EQUAL(skipWhitespaces(str, i), std::string::npos);
    BOOST_REQUIRE_EQUAL(skipWhitespaces(str, std::string::npos), std::string::npos);
    for(unsigned i = 0; i < 4; i++)
        BOOST_REQUIRE_EQUAL(skipWhitespaces(str, i, false), std::string::npos);
    for(unsigned i = 4; i < str.size(); i++)
        BOOST_REQUIRE_EQUAL(skipWhitespaces(str, i, false), 4u);
    BOOST_REQUIRE_EQUAL(skipWhitespaces(str, std::string::npos, false), std::string::npos);
}

BOOST_AUTO_TEST_CASE(GetStartOfLine)
{
    std::string str = "test\r\ntest2\n";
    for(unsigned i = 0; i <= 5; i++)
        BOOST_REQUIRE_EQUAL(getStartOfLine(str, i), 0u);
    for(unsigned i = 6; i < str.size(); i++)
        BOOST_REQUIRE_EQUAL(getStartOfLine(str, i), 6u);
}

BOOST_AUTO_TEST_CASE(ParseSubTable)
{
    GameDataFile gd;
    gd.setContents("rttr:AddBld{ name = 'foo',\n--Comment\nbar={foo1=1, foo2=2},\n--Comment2\nbar2={1,2}}");
    BOOST_REQUIRE(gd.findMainTable("foo"));
}

BOOST_AUTO_TEST_CASE(FindTable)
{
    GameDataFile gd;
    gd.setContents(" name = \"foo\"");
    BOOST_REQUIRE(!gd.findMainTable("foo"));
    gd.setContents(" name = __\"foo\"");
    BOOST_REQUIRE(!gd.findMainTable("foo"));
    gd.setContents("rttr:AddBld{name = \"foo\"}");
    boost::optional<const LuaTable&> pos = gd.findMainTable("foo");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(pos->data.start, 11);
    BOOST_REQUIRE_EQUAL(pos->data.len, gd.getContents().size() - 11);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));

    // Test with different whitespace
    gd.setContents("rttr:AddBld{ name = \"foo\"}");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));
    gd.setContents("rttr:AddBld{name = \"foo\" }");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));
    gd.setContents("rttr:AddBld{ name = \"foo\" }");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));

    gd.setContents("rttr:AddBld{ name = \"foo\",1,2}");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE(pos);

    gd.setContents("rttr:AddBld{ name = \"foo\" }\nrttr:AddBld{ name = \"foo2\" }\nrttr:AddBld{ name = \"foo3\" }\n");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), "{ name = \"foo\" }");
    pos = gd.findMainTable("foo2");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), "{ name = \"foo2\" }");
    pos = gd.findMainTable("foo3");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), "{ name = \"foo3\" }");
    BOOST_REQUIRE(!gd.findMainTable("foo4"));

    // Same with translations
    gd.setContents("rttr:AddBld{ name = __\"foo\"}");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), gd.getContents().substr(11));
    gd.setContents("rttr:AddBld{ name = __\"foo\" }\nrttr:AddBld{ name = __\"foo2\" }\nrttr:AddBld{ name = __\"foo3\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), "{ name = __\"foo\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo2")), "{ name = __\"foo2\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo3")), "{ name = __\"foo3\" }");

    // Find first
    gd.setContents("rttr:AddBld{ name = \"foo\" }\nrttr:AddBld{ name = \"foo\", bar = \"foo\" }\nrttr:AddBld{ name = \"foo3\" }\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), "{ name = \"foo\" }");

    // Skip comments
    gd.setContents("--rttr:AddBld{\n name = \"foo\" --}\nrttr:AddBld{ --name = \"foo\" \n}\nrttr:AddBld{\n--name = \"foo\" "
                   "\n}\nrttr:AddBld{name = \"foo\"}\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), "{name = \"foo\"}");
}

BOOST_AUTO_TEST_CASE(FindComment)
{
    GameDataFile gd;

    gd.setContents("rttr:AddBld{--Foo\nname = \"table\"}\n"); // Regular comment
    boost::optional<const LuaTable&> pos = gd.findMainTable("table");
    boost::optional<const LuaTableEntry&> optEntry = gd.findNamedValue(*pos, "name");
    BOOST_REQUIRE(optEntry);
    BOOST_REQUIRE_EQUAL(gd.getComment(optEntry), "--Foo");

    gd.setContents("rttr:AddBld{ \t --Foo  \t\nname = \"table\"}\n"); // whitespace around
    const LuaTableEntry* entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Foo");

    gd.setContents("rttr:AddBld{--Foo\n--Bar3\nname = \"table\"}\n"); // 2 lines
    entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Foo\n--Bar3");

    gd.setContents("rttr:AddBld{bar=2,--Bar32\n--Foo\nname = \"table\"}\n"); // 2nd belongs to other
    entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Foo");

    gd.setContents("rttr:AddBld{name = \"table\"}\n"); // no comment
    entry = &gd["table"]["name"];
    BOOST_REQUIRE(gd.getComment(*entry).empty());

    gd.setContents("rttr:AddBld{--Foo\nbar=2,\nname = \"table\"}\n"); // belongs to other
    entry = &gd["table"]["name"];
    BOOST_REQUIRE(entry->comment.empty());

    gd.setContents("rttr:AddBld{name = \"table\"--Foo\n}"); // trailing comment
    entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Foo");

    gd.setContents("rttr:AddBld{bar=2, --Foo\nname = \"table\"}\n"); // trailing comment to other
    entry = &gd["table"]["name"];
    BOOST_REQUIRE(entry->comment.empty());

    gd.setContents("rttr:AddBld{--Foo\nname = \"table\" --Bar\n}\n"); // trailing and leading
    entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Bar");
}

struct ExpectedLuaTableEntry
{
    std::string comment, name, data;
};

void checkEntries(const GameDataFile& gd, const LuaTable& mainTable, const std::vector<ExpectedLuaTableEntry>& expectedEntries,
                  const std::vector<const LuaTableEntry*>& entries)
{
    BOOST_REQUIRE_EQUAL(gd.getData(mainTable), gd.getContents().substr(tblOff));
    for(int i = 0; i < expectedEntries.size(); i++)
    {
        BOOST_REQUIRE_EQUAL(gd.getComment(*entries[i]), expectedEntries[i].comment);
        BOOST_REQUIRE_EQUAL(entries[i]->name.get(gd.getContents()), expectedEntries[i].name);
        BOOST_REQUIRE_EQUAL(gd.getData(*entries[i]), expectedEntries[i].data);
    }
}

BOOST_AUTO_TEST_CASE(InsertAt)
{
    GameDataFile gd;

    std::string luaDef = "rttr:AddBld{ --Foo\nname=\"table\", foo={bar = 42}\n}";
    gd.setContents(luaDef);
    const LuaTable& mainTable = gd["table"];
    const LuaTableEntry& name = mainTable["name"];
    const LuaTableEntry& foo = mainTable["foo"];
    const LuaTableEntry& bar = foo["bar"];
    std::vector<ExpectedLuaTableEntry> expectedEntries;
    expectedEntries.push_back(ExpectedLuaTableEntry{"--Foo", "name", "name=\"table\","});
    expectedEntries.push_back(ExpectedLuaTableEntry{"", "foo", "foo={bar = 42}"});
    expectedEntries.push_back(ExpectedLuaTableEntry{"", "bar", "bar = 42"});
    std::vector<const LuaTableEntry*> entries;
    entries.push_back(&name);
    entries.push_back(&foo);
    entries.push_back(&bar);

    // Add after last value
    gd.insertData("--bar", luaDef.find_last_of('}'));
    checkEntries(gd, mainTable, expectedEntries, entries);
    // Add into last value
    gd.insertData(",foo2=1", luaDef.find_last_of('}') - 1u);
    expectedEntries[1].data += ",foo2=1";
    checkEntries(gd, mainTable, expectedEntries, entries);
    // Add onto last value
    gd.insertData("42", luaDef.find("42"));
    expectedEntries[1].data.insert(expectedEntries[1].data.find("42"), "42");
    expectedEntries[2].data.insert(expectedEntries[2].data.find("42"), "42");
    checkEntries(gd, mainTable, expectedEntries, entries);
    // Add into last name
    gd.insertData("2", luaDef.find("bar") + 1u);
    expectedEntries[1].data.insert(expectedEntries[1].data.find("bar") + 1u, "2");
    expectedEntries[2].data.insert(expectedEntries[2].data.find("bar") + 1u, "2");
    expectedEntries[2].name.insert(1u, "2");
    checkEntries(gd, mainTable, expectedEntries, entries);
    // Add into name
    gd.insertData("2", luaDef.find("foo") + 2u);
    expectedEntries[1].data.insert(expectedEntries[1].data.find("foo") + 2u, "2");
    expectedEntries[1].name.insert(2u, "2");
    checkEntries(gd, mainTable, expectedEntries, entries);
    // Add into name
    gd.insertData("2", luaDef.find("name") + 3u);
    expectedEntries[0].data.insert(expectedEntries[0].data.find("name") + 3u, "2");
    expectedEntries[0].name.insert(3u, "2");
    checkEntries(gd, mainTable, expectedEntries, entries);
    // Add into comment
    gd.insertData("2", luaDef.find("--Foo") + 3u);
    expectedEntries[0].comment.insert(3u, "2");
    checkEntries(gd, mainTable, expectedEntries, entries);
}

BOOST_AUTO_TEST_CASE(InsertField)
{
    GameDataFile gd;

    std::string expected = "rttr:AddBld{\n\tname='Foo',\n\tbar1=1\n}";
    std::string contents = "rttr:AddBld{\n\tname='Foo'\n}";
    gd.setContents(contents);
    gd.insertFieldAfter("Foo", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expected);
    gd.setContents("rttr:AddBld{\n\tname='Foo',\n}");
    gd.insertFieldAfter("Foo", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expected);
    gd.setContents(contents);
    gd.insertFieldAfter("Foo:name", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expected);
    gd.setContents(contents);
    gd.insertFieldAfter("Foo:notExisting", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expected);
    gd.setContents(contents);
    gd.insertFieldAfter("Foo", "--Bar\nbar1=1");
    BOOST_REQUIRE_EQUAL(gd.getContents(), "rttr:AddBld{\n\tname='Foo',\n\t--Bar\n\tbar1=1\n}");

    contents = expected;
    std::string expAfterName = "rttr:AddBld{\n\tname='Foo',\n\tbar2=2,\n\tbar1=1\n}";
    std::string expAtEnd = "rttr:AddBld{\n\tname='Foo',\n\tbar1=1,\n\tbar2=2\n}";
    gd.setContents(contents);
    gd.insertFieldAfter("Foo", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expAtEnd);
    gd.setContents(contents);
    gd.insertFieldAfter("Foo:bar1", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expAtEnd);
    gd.setContents(contents);
    gd.insertFieldAfter("Foo:notExisting", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expAtEnd);
    gd.setContents(contents);
    gd.insertFieldAfter("Foo:name", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getContents(), expAfterName);
}

BOOST_AUTO_TEST_CASE(LoadRealFiles)
{
    const std::string nationPath = RTTRCONFIG.ExpandPath("<RTTR_RTTR>/gameData/nations");
    GameDataFile gd;
#ifndef BOOST_NO_CXX11_HDR_CHRONO
    auto start = std::chrono::high_resolution_clock::now();
#endif
    for(int i = 0; i < 50; i++)
    {
        BOOST_REQUIRE(gd.load(nationPath + "/buildings.lua"));
        BOOST_REQUIRE(gd.load(nationPath + "/africans/buildings.lua"));
        BOOST_REQUIRE(gd.load(nationPath + "/romans/buildings.lua"));
        BOOST_REQUIRE(gd.load(nationPath + "/babylonians/buildings.lua"));
        BOOST_REQUIRE(gd.load(nationPath + "/japanese/buildings.lua"));
        BOOST_REQUIRE(gd.load(nationPath + "/vikings/buildings.lua"));
    }
#ifndef BOOST_NO_CXX11_HDR_CHRONO
    auto diff = std::chrono::high_resolution_clock::now() - start;
    BOOST_TEST_MESSAGE("Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "ms");
#endif
}

BOOST_AUTO_TEST_SUITE_END()
