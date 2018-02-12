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
    gd.setContents("{ name = \"foo\"}");
    boost::optional<LuaValueRef> pos = gd.findTableByName("foo");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(pos->data.start, 0);
    BOOST_REQUIRE_EQUAL(pos->data.len, gd.getContents().size());
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContents());

    gd.setContents("{ name = \"foo\" }\n{ name = \"foo2\" }\n{ name = \"foo3\" }\n");
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
    gd.setContents("{ name = __\"foo\"}");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), gd.getContents());
    gd.setContents("{ name = __\"foo\" }\n{ name = __\"foo2\" }\n{ name = __\"foo3\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), "{ name = __\"foo\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo2")), "{ name = __\"foo2\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo3")), "{ name = __\"foo3\" }");

    // Find first
    gd.setContents("{ name = \"foo\" }\n{ name = \"foo\", bar = \"foo\" }\n{ name = \"foo3\" }\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), "{ name = \"foo\" }");

    // Skip comments
    gd.setContents("--{\n name = \"foo\" --}\n{ --name = \"foo\" \n}\n{\n--name = \"foo\" \n}\n{name = \"foo\"}\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findTableByName("foo")), "{name = \"foo\"}");
}

BOOST_AUTO_TEST_CASE(FindComment)
{
    GameDataFile gd;
    gd.setContents("--Foo\n{name = \"regular\"}\n"                // Regular comment
                   " \t --Foo2  \t\n{name = \"whitespace\"}\n"    // whitespace around
                   "--Foo3\n--Bar3\n{name = \"2line\"}\n"         // 2 lines
                   "bar=2--Bar32\n--Foo32\n{name = \"2line2\"}\n" // 2nd belongs to other
                   "{name = \"none\"}\n"                          // no comment
                   "--Foo4\nbar=2\n{name = \"none2\"}\n"          // belongs to other
                   "{name = \"trailing\"}--Foo5\n"                // trailing comment
                   "bar=2 --Foo6\n{name = \"trailing2\"}\n"       // trailing comment to other
                   "--Foo7\n{name = \"trailLead\"} --Foo8\n"      // trailing and leading
    );
    boost::optional<LuaValueRef> pos = gd.findTableByName("regular");
    BOOST_REQUIRE(gd.findComment(*pos));
    BOOST_REQUIRE_EQUAL(gd.getComment(pos), "--Foo");

    pos = gd.findTableByName("whitespace");
    BOOST_REQUIRE(gd.findComment(*pos));
    BOOST_REQUIRE_EQUAL(gd.getComment(pos), "--Foo2");

    pos = gd.findTableByName("2line");
    BOOST_REQUIRE(gd.findComment(*pos));
    BOOST_REQUIRE_EQUAL(gd.getComment(pos), "--Foo3\n--Bar3");

    pos = gd.findTableByName("2line2");
    BOOST_REQUIRE(gd.findComment(*pos));
    BOOST_REQUIRE_EQUAL(gd.getComment(pos), "--Foo32");

    pos = gd.findTableByName("none");
    BOOST_REQUIRE(!gd.findComment(*pos));
    BOOST_REQUIRE(gd.getComment(pos).empty());

    pos = gd.findTableByName("none2");
    BOOST_REQUIRE(!gd.findComment(*pos));

    pos = gd.findTableByName("trailing");
    BOOST_REQUIRE(gd.findComment(*pos));
    BOOST_REQUIRE_EQUAL(gd.getComment(pos), "--Foo5");

    pos = gd.findTableByName("trailing2");
    BOOST_REQUIRE(!gd.findComment(*pos));

    pos = gd.findTableByName("trailLead");
    BOOST_REQUIRE(gd.findComment(*pos));
    BOOST_REQUIRE_EQUAL(gd.getComment(pos), "--Foo7");
}

BOOST_AUTO_TEST_SUITE_END()
