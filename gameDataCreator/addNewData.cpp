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

#define DEFINE_OldBuildingType
#include "commonDefines.h" // IWYU pragma: keep
#include "addNewData.h"
#include "GameDataFile.h"
#include "files.h"
#include "helpers/containerUtils.h"
#include "gameTypes/OldBuildingType.h"
#include "gameData/BuildingBPDesc.h"
#include "gameData/BuildingConsts.h"
#include "gameData/BuildingDesc.h"
#include "gameData/DoorConsts.h"
#include "gameData/NationConsts.h"
#include "gameData/NationDesc.h"
#include "gameData/WorldDescription.h"
#include "libutil/StringConversion.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/utility/string_ref.hpp>
#include <set>
#include <stdexcept>

using namespace simpleLuaData;

std::string capitalize(std::string str)
{
    boost::algorithm::to_lower(str);
    if(!str.empty())
        str[0] = toupper(str[0]);
    return str;
}

OldBuildingType GetOldBuildingIdx(const std::string& name)
{
    for(unsigned i = 0; i < OldBuildingType::count_; i++)
    {
        std::string oldName = capitalize(OldBuildingType::names_[i]);
        boost::algorithm::replace_all(oldName, "_", " ");
        if(oldName == name)
            return OldBuildingType::type_(i);
    }
    throw std::runtime_error("Bld Not found");
}

bool IsBldUsed(OldBuildingType b)
{
    return (std::string(OldBuildingType::names_[b]).find("Nothing") == std::string::npos);
}

const boost::array<const std::string, NUM_NATS> NATION_ICON_FILES = {
  {"<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", "<RTTR_GAME>/DATA/MBOB/JAP_ICON.LST.LST", "<RTTR_GAME>/DATA/MBOB/ROM_ICON",
   "<RTTR_GAME>/DATA/MBOB/VIK_ICON.LST", "<RTTR_RTTR>/LSTS/GAME/Babylonier/bab_icon.lst"}};

const helpers::SimpleMultiArray<const std::string, 2, NUM_NATS> NATION_GFXSET_Z = {
  {{"afr_z", "jap_z", "rom_z", "vik_z", "bab_z"}, {"wafr_z", "wjap_z", "wrom_z", "wvik_z", "wbab_z"}}};

namespace harbor {
// Relative Position des Bauarbeiters
const Position BUILDER_POS[NUM_NATS] = {Position(-20, 18), Position(-28, 17), Position(-20, 15), Position(-38, 17), Position(-38, 17)};
/// Relative Position der Brettertürme
const Position BOARDS_POS[NUM_NATS] = {Position(-75, -5), Position(-60, -5), Position(-55, -5), Position(-65, -5), Position(-65, -5)};
/// Relative Position der Steintürme
const Position STONES_POS[NUM_NATS] = {Position(-65, 10), Position(-52, 10), Position(-42, 10), Position(-52, 10), Position(-52, 10)};
/// Relative Postion der inneren Hafenfeuer
const Position FIRE_POS[NUM_NATS] = {Position(), Position(0, 0), Position(0, 0), Position(), Position(0, 0)};
/// Relative Postion der äußeren Hafenfeuer
const Position EXTRAFIRE_POS[NUM_NATS] = {Position(36, -51), Position(), Position(8, -115), Position(5, -80), Position()};
} // namespace harbor

template<typename T>
std::string toString(const Point<T>& pt)
{
    return std::string("{") + s25util::toStringClassic(pt.x) + "," + s25util::toStringClassic(pt.y) + "}";
}

void addNewData(const std::string& baseLuaPath)
{
    GameDataFile gdBuildings;
    gdBuildings.load(baseLuaPath + "/buildings.lua");
    std::vector<GameDataFile> gdNations(NUM_NATS);
    std::vector<std::string> natTableName(NUM_NATS);
    for(int i = 0; i < NUM_NATS; i++)
    {
        gdNations[i].load(baseLuaPath + "/" + boost::algorithm::to_lower_copy(std::string(NationNames[i])) + "/default.lua");
        natTableName[i] = std::string(NationNames[i]) + ":";
    }

    boost::array<unsigned, NUM_NATIVE_NATS> avatarIds = {{257, 253, 252, 256}};
    for(int i = 0; i < NUM_NATIVE_NATS; ++i)
    {
        std::string basePath = "<RTTR_GAME>/DATA/MBOB/";
        std::string comment = "Texture file for summer textures";
        gdNations[i].insertFieldAfter(":s2Id", "summerTexFile",
                                      "\"" + basePath + boost::algorithm::to_upper_copy(NATION_GFXSET_Z[0][i]) + ".LST\"", comment);
        comment = "Texture file for winter textures";
        gdNations[i].insertFieldAfter(":summerTexFile", "winterTexFile",
                                      "\"" + basePath + boost::algorithm::to_upper_copy(NATION_GFXSET_Z[1][i]) + ".LST\"", comment);
        comment = "Default avatar texture";
        gdNations[i].insertFieldAfter(":winterTexFile", "defaultAvatar",
                                      "{ filepath = \"io\", idx = " + s25util::toStringClassic(avatarIds[i]) + " }", comment);
    }
    {
        std::string basePath = "<RTTR_RTTR>/LSTS/GAME/Babylonier";
        std::string comment = "Texture file for summer textures";
        gdNations[NAT_BABYLONIANS].insertFieldAfter(":s2Id", "summerTexFile", "\"" + basePath + "/bab_z.lst\"", comment);
        comment = "Texture file for winter textures";
        gdNations[NAT_BABYLONIANS].insertFieldAfter(":summerTexFile", "winterTexFile", "\"" + basePath + "/wbab_z.lst\"", comment);
        comment = "Default avatar texture";
        gdNations[NAT_BABYLONIANS].insertFieldAfter(":winterTexFile", "defaultAvatar", "{ filepath = \"io_new\", idx = 7 }", comment);
    }
    for(int natIdx = 0; natIdx < NUM_NATS; natIdx++)
    {
        GameDataFile gd;
        gd.load((bfs::path(gdNations[natIdx].getFilepath()).parent_path() / "buildings.lua").string());
        gd.setNameValue("iconFile", "\"" + NATION_ICON_FILES[natIdx] + "\"");
        if(natIdx == NAT_BABYLONIANS)
        {
            std::string basePath = "<RTTR_RTTR>/LSTS/GAME/Babylonier";
            gd.setNameValue("summerTexFile", "\"" + basePath + "/bab_z.lst\"");
            gd.setNameValue("winterTexFile", "\"" + basePath + "/wbab_z.lst\"");
        } else
        {
            std::string basePath = "<RTTR_GAME>/DATA/MBOB/";
            gd.setNameValue("summerTexFile", "\"" + basePath + boost::algorithm::to_upper_copy(NATION_GFXSET_Z[0][natIdx]) + ".LST\"");
            gd.setNameValue("winterTexFile", "\"" + basePath + boost::algorithm::to_upper_copy(NATION_GFXSET_Z[1][natIdx]) + ".LST\"");
        }
        for(int i = 0; i < gd.getTables().size(); i++)
        {
            std::string bldName = gd.getTableName(i);
            OldBuildingType bld = GetOldBuildingIdx(bldName);
            if(bld == OldBuildingType::CHARBURNER)
            {
                gd.insertFieldAfter(bldName + ":name", "icon",
                                    "{ filepath = \"charburner\", idx = " + s25util::toStringClassic(natIdx * 8 + 8) + " }");
                gd.insertTable("texture");
                gd.insertFieldAfter(bldName + ":texture:", "main",
                                    "{{ \"charburner\", " + s25util::toStringClassic(natIdx * 8 + 1) + " }, { \"charburner\", "
                                      + s25util::toStringClassic(natIdx * 8 + 6) + " }}");
                gd.insertField("shadow", "{ \"charburner\", " + s25util::toStringClassic(natIdx * 8 + 2) + " }");
                gd.insertField("skeleton", "{ \"charburner\", " + s25util::toStringClassic(natIdx * 8 + 3) + " }");
                gd.insertField("skeletonShadow", "{ \"charburner\", " + s25util::toStringClassic(natIdx * 8 + 4) + " }");
                gd.insertField("door", "{{ \"charburner\", " + s25util::toStringClassic(natIdx * 8 + 5) + " }, { \"charburner\", "
                                         + s25util::toStringClassic(natIdx * 8 + 7) + " }}");

            } else
            {
                std::string comment = (i) ? "" : "Texture for the icon";
                gd.insertFieldAfter(bldName + ":name", "icon", "{ filepath = iconFile, idx = " + s25util::toStringClassic((int) bld) + " }",
                                    comment);
                comment = (i) ? "" :
                                "Textures for the building. Required: main, skeleton, door. Optional: shadow, skeletonShadow. Multiple "
                                "values: 0 = summer, 1 = winter";
                gd.insertTable("texture", comment);
                gd.insertFieldAfter(bldName + ":texture:", "main", "getSummerAndWinterTex(" + s25util::toStringClassic(250 + 5 * bld) + ")",
                                    comment);
                comment = (i) ? "" : "Shadow texture";
                gd.insertField("shadow", "getSummerAndWinterTex(" + s25util::toStringClassic(250 + 5 * bld + 1) + ")", comment);
                comment = (i) ? "" : "Skeleton (in construction) texture and its shadow";
                if(bld == OldBuildingType::HEADQUARTERS)
                {
                    // HQ has no skeleton, but we have a tent that can act as an HQ
                    gd.insertField("skeleton", "{ filepath = \"mis0bobs\", idx = 6 }", comment);
                    gd.insertField("skeletonShadow", "{ filepath = \"mis0bobs\", idx = 7 }", comment);
                } else
                {
                    gd.insertField("skeleton", "getSummerAndWinterTex(" + s25util::toStringClassic(250 + 5 * bld + 2) + ")", comment);
                    gd.insertField("skeletonShadow", "getSummerAndWinterTex(" + s25util::toStringClassic(250 + 5 * bld + 3) + ")", comment);
                }
                comment = (i) ? "" : "Texture of the (closed) door";
                gd.insertField("door", "getSummerAndWinterTex(" + s25util::toStringClassic(250 + 5 * bld + 4) + ")", comment);
            }
            if(bld == OldBuildingType::HARBOR_BUILDING)
            {
                gd.insertTableAfter(bldName + ":texture", "animations").isSingleLine_ = LuaTable::SL_NO;
                if(harbor::FIRE_POS[natIdx].isValid())
                {
                    gd.insertFieldAfter(bldName + ":animations:", "idle",
                                        "{ filepath = \"<" + std::string(NationNames[natIdx]) + ">\", frames=range(500, 540, 5), msPerFrame = 158}");
                }
                if(harbor::EXTRAFIRE_POS[natIdx].isValid())
                {
                    gd.insertFieldAfter(bldName + ":animations:", "work",
                                        "{ filepath = \"<Map>\", frames=range(740, 748), offset=" + toString(harbor::EXTRAFIRE_POS[natIdx])
                                          + ", msPerFrame = 197}");
                }
                gd.insertTableAfter(bldName + ":animations", "workOffsets").isSingleLine_ = LuaTable::SL_NO;
                gd.insertFieldAfter(bldName + ":workOffsets:", "builder", toString(harbor::BUILDER_POS[natIdx]));
                gd.insertField("boards", toString(harbor::BOARDS_POS[natIdx]));
                gd.insertField("stones", toString(harbor::STONES_POS[natIdx]));
            }

            std::string comment = (i) ? "" : "Y-Position of the door (X will be calculated by extending the path from the flag)";
            gd.insertFieldAfter(bldName + ":texture", "doorPosY", DOOR_CONSTS[natIdx][bld], comment);
        }
        gd.save(gd.getFilepath());
    }
    gdBuildings.save(gdBuildings.getFilepath());
    for(int i = 0; i < NUM_NATS; i++)
        gdNations[i].save(gdNations[i].getFilepath());
}
