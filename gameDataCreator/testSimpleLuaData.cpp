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
#include "simpleLuaData.h"
#include <boost/test/unit_test.hpp>

using namespace simpleLuaData;

BOOST_AUTO_TEST_SUITE(SimpleLuaData)

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

BOOST_AUTO_TEST_CASE(FindTable)
{
    GameDataFile gd;
    gd.setContents(" name = \"foo\"");
    BOOST_REQUIRE(!gd.findTableByName("foo"));
    gd.setContents(" name = __\"foo\"");
    BOOST_REQUIRE(!gd.findTableByName("foo"));
    gd.setContents("rttr:AddBld{name = \"foo\"}");
    boost::optional<const LuaTable&> pos = gd.findTableByName("foo");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(pos->data.start, 11);
    BOOST_REQUIRE_EQUAL(pos->data.len, gd.getContents().size() - 11);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));

    // Test with different whitespace
    gd.setContents("rttr:AddBld{ name = \"foo\"}");
    pos = gd.findTableByName("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));
    gd.setContents("rttr:AddBld{name = \"foo\" }");
    pos = gd.findTableByName("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));
    gd.setContents("rttr:AddBld{ name = \"foo\" }");
    pos = gd.findTableByName("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents().substr(11));

    gd.setContents("rttr:AddBld{ name = \"foo\",1,2}");
    pos = gd.findTableByName("foo");
    BOOST_REQUIRE(pos);

    gd.setContents("rttr:AddBld{ name = \"foo\" }\nrttr:AddBld{ name = \"foo2\" }\nrttr:AddBld{ name = \"foo3\" }\n");
    pos = gd.findTableByName("foo");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), "{ name = \"foo\" }");
    pos = gd.findTableByName("foo2");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), "{ name = \"foo2\" }");
    pos = gd.findTableByName("foo3");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), "{ name = \"foo3\" }");
    BOOST_REQUIRE(!gd.findTableByName("foo4"));

    // Same with translations
    gd.setContents("rttr:AddBld{ name = __\"foo\"}");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), gd.getContents().substr(11));
    gd.setContents("rttr:AddBld{ name = __\"foo\" }\nrttr:AddBld{ name = __\"foo2\" }\nrttr:AddBld{ name = __\"foo3\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), "{ name = __\"foo\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo2")), "{ name = __\"foo2\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo3")), "{ name = __\"foo3\" }");

    // Find first
    gd.setContents("rttr:AddBld{ name = \"foo\" }\nrttr:AddBld{ name = \"foo\", bar = \"foo\" }\nrttr:AddBld{ name = \"foo3\" }\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), "{ name = \"foo\" }");

    // Skip comments
    gd.setContents("--rttr:AddBld{\n name = \"foo\" --}\nrttr:AddBld{ --name = \"foo\" \n}\nrttr:AddBld{\n--name = \"foo\" "
                   "\n}\nrttr:AddBld{name = \"foo\"}\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), "{name = \"foo\"}");
}

BOOST_AUTO_TEST_CASE(FindComment)
{
    GameDataFile gd;

    gd.setContents("rttr:AddBld{--Foo\nname = \"table\"}\n"); // Regular comment
    boost::optional<const LuaTable&> pos = gd.findTableByName("table");
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

BOOST_AUTO_TEST_SUITE_END()
