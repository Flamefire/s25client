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
#include "files.h"
#include "helpers/containerUtils.h"
#include "simpleLuaData.h"
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

unsigned GetOldBuildingIdx(const std::string& name)
{
    for(unsigned i = 0; i < OldBuildingType::count_; i++)
    {
        std::string oldName = capitalize(OldBuildingType::names_[i]);
        boost::algorithm::replace_all(oldName, "_", " ");
        if(oldName == name)
            return i;
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

        for(int i = 0; i < gd.getTables().size(); i++)
        {
            std::string bldName = gd.getTableName(i);
            unsigned bld = GetOldBuildingIdx(bldName);
            std::string comment = (i) ? "" : "Texture for the icon";
            if(bld == OldBuildingType::CHARBURNER)
                gd.insertFieldAfter(bldName + ":name", "icon",
                                    "{ filepath = \"charburner\", idx = " + s25util::toStringClassic((natIdx + 1) * 8) + " }", comment);
            else
                gd.insertFieldAfter(bldName + ":name", "icon",
                                    "{ filepath = \"" + NATION_ICON_FILES[natIdx] + "\", idx = " + s25util::toStringClassic(bld) + " }",
                                    comment);
            comment = (i) ? "" : "Y-Position of the door (X will be calculated by extending the path from the flag)";
            gd.insertFieldAfter(bldName + ":icon", "doorPosY", DOOR_CONSTS[natIdx][bld], comment);
        }
        gd.save(gd.getFilepath());
    }
    gdBuildings.save(gdBuildings.getFilepath());
    for(int i = 0; i < NUM_NATS; i++)
        gdNations[i].save(gdNations[i].getFilepath());
}
