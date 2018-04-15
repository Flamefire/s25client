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
#include "simpleLuaParser.h"
#include "GameDataFile.h"
#include <boost/test/unit_test.hpp>

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
    gd.setContent(
      "rttr:AddBld{\n\tname = 'foo',\n\t--Comment\n\tbar = {\n\t\tfoo1 = 1,\n\t\tfoo2 = 2\n\t},\n\t--Comment2\n\tbar2 = {1, 2}\n}");
    BOOST_REQUIRE(gd.findMainTable("foo"));
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), gd.getContent());
}

BOOST_AUTO_TEST_CASE(FindTable)
{
    GameDataFile gd;
    gd.setContent(" name = \"foo\"");
    BOOST_REQUIRE(!gd.findMainTable("foo"));
    gd.setContent(" name = __\"foo\"");
    BOOST_REQUIRE(!gd.findMainTable("foo"));
    gd.setContent("rttr:AddBld{name = \"foo\"}");
    boost::optional<const LuaTable&> pos = gd.findMainTable("foo");
    BOOST_REQUIRE(pos);
    BOOST_REQUIRE_EQUAL(pos->data.start, 11);
    BOOST_REQUIRE_EQUAL(pos->data.len, gd.getContent().size() - 11);
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContent().substr(11));

    // Test with different whitespace
    gd.setContent("rttr:AddBld{ name = \"foo\"}");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContent().substr(11));
    gd.setContent("rttr:AddBld{name = \"foo\" }");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContent().substr(11));
    gd.setContent("rttr:AddBld{ name = \"foo\" }");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE_EQUAL(gd.getData(pos), gd.getContent().substr(11));

    gd.setContent("rttr:AddBld{ name = \"foo\",1,2}");
    pos = gd.findMainTable("foo");
    BOOST_REQUIRE(pos);

    gd.setContent("rttr:AddBld{ name = \"foo\" }\nrttr:AddBld{ name = \"foo2\" }\nrttr:AddBld{ name = \"foo3\" }\n");
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
    gd.setContent("rttr:AddBld{ name = __\"foo\"}");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), gd.getContent().substr(11));
    gd.setContent("rttr:AddBld{ name = __\"foo\" }\nrttr:AddBld{ name = __\"foo2\" }\nrttr:AddBld{ name = __\"foo3\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), "{ name = __\"foo\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo2")), "{ name = __\"foo2\" }");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo3")), "{ name = __\"foo3\" }");

    // Find first
    gd.setContent("rttr:AddBld{ name = \"foo\" }\nrttr:AddBld{ name = \"foo\", bar = \"foo\" }\nrttr:AddBld{ name = \"foo3\" }\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), "{ name = \"foo\" }");

    // Skip comments
    gd.setContent("--rttr:AddBld{\n name = \"foo\" --}\nrttr:AddBld{ --name = \"foo\" \n}\nrttr:AddBld{\n--name = \"foo\" "
                   "\n}\nrttr:AddBld{name = \"foo\"}\n");
    BOOST_REQUIRE_EQUAL(gd.getData(gd.findMainTable("foo")), "{name = \"foo\"}");
}

BOOST_AUTO_TEST_CASE(FindComment)
{
    GameDataFile gd;

    gd.setContent("rttr:AddBld{--Foo\nname = \"table\"}\n"); // Regular comment
    boost::optional<const LuaTable&> pos = gd.findMainTable("table");
    boost::optional<const LuaTableEntry&> optEntry = gd.findTableEntry(*pos, "name");
    BOOST_REQUIRE(optEntry);
    BOOST_REQUIRE_EQUAL(gd.getComment(optEntry), "--Foo");

    gd.setContent("rttr:AddBld{ \t --Foo  \t\nname = \"table\"}\n"); // whitespace around
    const LuaTableEntry* entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Foo");

    gd.setContent("rttr:AddBld{--Foo\n--Bar3\nname = \"table\"}\n"); // 2 lines
    entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Foo\n--Bar3");

    gd.setContent("rttr:AddBld{bar=2,--Bar32\n--Foo\nname = \"table\"}\n"); // 2nd belongs to other
    entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(gd.getComment(*entry), "--Foo");

    gd.setContent("rttr:AddBld{name = \"table\"}\n"); // no comment
    entry = &gd["table"]["name"];
    BOOST_REQUIRE(gd.getComment(*entry).empty());

    gd.setContent("rttr:AddBld{--Foo\nbar=2,\nname = \"table\"}\n"); // belongs to other
    entry = &gd["table"]["name"];
    BOOST_REQUIRE(gd.getComment(*entry).empty());

    gd.setContent("rttr:AddBld{name = \"table\"--Foo\n}"); // trailing comment
    entry = &gd["table"]["name"];
    BOOST_REQUIRE(entry->preComment.empty());
    BOOST_REQUIRE_EQUAL(entry->postComment, "--Foo");

    gd.setContent("rttr:AddBld{bar=2, --Foo\nname = \"table\"}\n"); // trailing comment to other
    entry = &gd["table"]["name"];
    BOOST_REQUIRE(gd.getComment(*entry).empty());

    gd.setContent("rttr:AddBld{--Foo\nname = \"table\" --Bar\n}\n"); // trailing and leading
    entry = &gd["table"]["name"];
    BOOST_REQUIRE_EQUAL(entry->preComment, "--Foo");
    BOOST_REQUIRE_EQUAL(entry->postComment, "--Bar");
}

BOOST_AUTO_TEST_CASE(InsertField)
{
    GameDataFile gd;

    std::string expected = "rttr:AddBld{\n\tname = 'Foo',\n\tbar1 = 1\n}";
    std::string contents = "rttr:AddBld{\n\tname='Foo'\n}";
    gd.setContent(contents);
    gd.insertFieldAfter("Foo", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expected);
    gd.setContent("rttr:AddBld{\n\tname='Foo',\n}");
    gd.insertFieldAfter("Foo", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expected);
    gd.setContent(contents);
    gd.insertFieldAfter("Foo:name", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expected);
    gd.setContent(contents);
    gd.insertFieldAfter("Foo:notExisting", "bar1=1");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expected);
    gd.setContent(contents);
    gd.insertFieldAfter("Foo", "--Bar\nbar1=1");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), "rttr:AddBld{\n\tname = 'Foo',\n\t--Bar\n\tbar1 = 1\n}");

    contents = expected;
    std::string expAfterName = "rttr:AddBld{\n\tname = 'Foo',\n\tbar2 = 2,\n\tbar1 = 1\n}";
    std::string expAtEnd = "rttr:AddBld{\n\tname = 'Foo',\n\tbar1 = 1,\n\tbar2 = 2\n}";
    gd.setContent(contents);
    gd.insertFieldAfter("Foo", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expAtEnd);
    gd.setContent(contents);
    gd.insertFieldAfter("Foo:bar1", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expAtEnd);
    gd.setContent(contents);
    gd.insertFieldAfter("Foo:notExisting", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expAtEnd);
    gd.setContent(contents);
    gd.insertFieldAfter("Foo:name", "bar2=2");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), expAfterName);
}

BOOST_AUTO_TEST_CASE(ChangeValues)
{
    GameDataFile gd;
    gd.setContent("foo=bar\nfoo2 = bar2\nrttr:AddBld{}");
    BOOST_REQUIRE(gd.findNamedValue("foo"));
    BOOST_REQUIRE(gd.findNamedValue("foo2"));
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), "foo = bar\nfoo2 = bar2\nrttr:AddBld{\n}");
    gd.setNameValue("foz", "baz");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), "foo = bar\nfoo2 = bar2\nfoz = baz\nrttr:AddBld{\n}");
    gd.setNameValue("foo", "baz2");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), "foo = baz2\nfoo2 = bar2\nfoz = baz\nrttr:AddBld{\n}");
    gd.setContent("rttr:AddBld{}");
    gd.setNameValue("foo", "bar");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), "foo = bar\nrttr:AddBld{\n}");
    gd.setContent("");
    gd.setNameValue("foo", "bar");
    BOOST_REQUIRE_EQUAL(gd.getUnparsedData(), "foo = bar\n");
}

BOOST_AUTO_TEST_CASE(LoadRealFiles)
{
    const std::string nationPath = RTTRCONFIG.ExpandPath("<RTTR_RTTR>/gameData/nations");
    GameDataFile gd;
    BOOST_REQUIRE(gd.load(nationPath + "/buildings.lua"));
    BOOST_REQUIRE(gd.load(nationPath + "/africans/buildings.lua"));
    BOOST_REQUIRE(gd.load(nationPath + "/romans/buildings.lua"));
    BOOST_REQUIRE(gd.load(nationPath + "/babylonians/buildings.lua"));
    BOOST_REQUIRE(gd.load(nationPath + "/japanese/buildings.lua"));
    BOOST_REQUIRE(gd.load(nationPath + "/vikings/buildings.lua"));
}

BOOST_AUTO_TEST_SUITE_END()
