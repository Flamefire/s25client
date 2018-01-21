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
#include "RttrConfig.h"
#include "addNewData.h"
#include "lua/GameDataLoader.h"
#include "gameData/BuildingDesc.h"
#include "gameData/JobConsts.h"
#include "gameData/NationDesc.h"
#include "gameData/WorldDescription.h"
#include "libutil/LocaleHelper.h"
#include "libutil/Log.h"
#include "libutil/NullWriter.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>

void writeNation(const bfs::path& nationPath, const NationDesc& nat)
{
    bfs::create_directories(nationPath);
    bnw::ofstream defLua(nationPath / "default.lua");
    defLua << "nation = rttr:AddNation{" << std::endl;
    defLua << "\t-- Name used to reference this" << std::endl << "\tname = __\"" << nat.name << "\"," << std::endl;
    defLua << "\t-- Index in the original game" << std::endl << "\ts2Id = " << unsigned(nat.s2Id) << "," << std::endl;
    defLua << "}" << std::endl;
    defLua << "include \"buildings.lua\"" << std::endl;
    bnw::ofstream bldLua(nationPath / "buildings.lua");
#define WRITE_DESC(desc) \
    if(!i.value)         \
    bldLua << desc << std::endl
    for(DescIdx<NatBuildingDesc> i(0); i.value < nat.buildings.size(); i.value++)
    {
        const NatBuildingDesc& bld = nat.get(i);
        bldLua << "nation:AddBuilding{" << std::endl;
        WRITE_DESC("\t-- Name used to reference this");
        bldLua << "\tname = \"" << bld.name << "\"," << std::endl;
        if(!i.value || bld.smoke.type)
        {
            WRITE_DESC("\t-- Smoke type (1-4, 0 = disabled) and offset relative to the buildings origin");
            bldLua << "\tsmoke = { type = " << unsigned(bld.smoke.type) << ", offset = {" << int(bld.smoke.offset.x) << ", "
                   << int(bld.smoke.offset.y) << "} }," << std::endl;
        }
        bldLua << "}" << std::endl;
    }
#undef WRITE_DESC
}

std::string bqToStr(BuildingQuality bq)
{
    switch(bq)
    {
        default:
        case BQ_HUT: return "hut";
        case BQ_HOUSE: return "house";
        case BQ_CASTLE: return "castle";
        case BQ_MINE: return "mine";
        case BQ_HARBOR: return "harbor";
    }
}

std::string wareToStr(GoodType gd)
{
    if(gd == GD_NOTHING)
        return "none";
    if(gd == GD_INVALID)
        return "invalid";
    return WARE_NAMES[gd];
}

bool operator!=(const BldWorkDescription& lhs, const BldWorkDescription& rhs)
{
    return std::memcmp(&lhs, &rhs, sizeof(lhs)) != 0;
}

void writeBuildings(const bfs::path& filepath, const WorldDescription& worldDesc)
{
#define WRITE_DESC(desc) \
    if(!i.value)         \
    bldLua << desc << std::endl

    bnw::ofstream bldLua(filepath);
    for(DescIdx<BuildingDesc> i(0); i.value < worldDesc.buildings.size(); i.value++)
    {
        const BuildingDesc& bld = worldDesc.get(i);
        bldLua << "rttr:AddBuilding{" << std::endl;
        WRITE_DESC("\t-- Name used to reference this");
        bldLua << "\tname = __\"" << bld.name << "\"," << std::endl;
        WRITE_DESC("\t-- Text shown in the help window");
        bldLua << "\thelp = __\"" << bld.help << "\"," << std::endl;
        WRITE_DESC("\t-- Costs to build this building. Non set values default to 0");
        bldLua << "\tcosts = { ";
        if((bld.costs.boards && bld.costs.stones) || !i.value)
            bldLua << "boards = " << unsigned(bld.costs.boards) << ", stones = " << unsigned(bld.costs.stones);
        else if(bld.costs.boards)
            bldLua << "boards = " << unsigned(bld.costs.boards);
        else
            bldLua << "stones = " << unsigned(bld.costs.stones);
        bldLua << " }," << std::endl;
        WRITE_DESC("\t-- Required space for the building (hut, house, castle, mine, harbor)");
        bldLua << "\tsize = \"" << bqToStr(bld.requiredSpace) << "\"," << std::endl;
        WRITE_DESC("\t-- Describes the work that this building does (optional)");
        bldLua << "\twork = {" << std::endl;
        if(!i.value || bld.workDescr != BldWorkDescription())
        {
            if(!i.value || bld.workDescr.job != JOB_NOTHING)
            {
                WRITE_DESC("\t\t-- Worker belonging to the building");
                bldLua << "\t\tworker = \"" << (bld.workDescr.job == JOB_NOTHING ? "none" : JOB_NAMES[bld.workDescr.job]) << "\","
                       << std::endl;
            }
            if(!i.value || bld.workDescr.producedWare != GD_NOTHING)
            {
                WRITE_DESC("\t\t-- Produced ware. 'none' for nothing or 'invalid' for special cases");
                bldLua << "\t\tproducedWare = \"" << wareToStr(bld.workDescr.producedWare) << "\"," << std::endl;
            }
            if(!i.value || bld.workDescr.waresNeeded.getNum() != 0)
            {
                WRITE_DESC("\t\t-- List of up to 3 wares required for production");
                std::vector<std::string> wares;
                for(unsigned i = 0; i < bld.workDescr.waresNeeded.getNum(); i++)
                    wares.push_back(std::string("\"") + wareToStr(bld.workDescr.waresNeeded[i]) + '"');
                bldLua << "\t\twaresNeeded = {" << boost::algorithm::join(wares, ", ") << "}," << std::endl;
            }
            if(!i.value || bld.workDescr.numSpacesPerWare != 6)
            {
                WRITE_DESC("\t\t-- How many wares of each type can be stored. Default: 6");
                bldLua << "\t\tnumSpacesPerWare = " << unsigned(bld.workDescr.numSpacesPerWare) << "," << std::endl;
            }
            if(!i.value || !bld.workDescr.useOneWareEach)
            {
                WRITE_DESC("\t\t-- true (default) if one of each waresNeeded is used per production cycle"
                           << std::endl
                           << "\t\t-- false if the ware type is used, that the building has the most of");
                bldLua << "\t\tuseOneWareEach = " << (bld.workDescr.useOneWareEach ? "true" : "false") << "," << std::endl;
            }
            bldLua << "\t}" << std::endl;
        }
        bldLua << "}" << std::endl;
    }
#undef WRITE_DESC
}

void writeNations(const bfs::path& nationsPath, const WorldDescription& worldDesc)
{
    bfs::create_directories(nationsPath);
    bnw::ofstream defLua(nationsPath / "default.lua");
    writeBuildings(nationsPath / "buildings.lua", worldDesc);
    defLua << "include \"buildings.lua\"" << std::endl << std::endl;
    for(DescIdx<NationDesc> i(0); i.value < worldDesc.nations.size(); i.value++)
    {
        const NationDesc& nat = worldDesc.get(i);
        std::string natPathName = boost::algorithm::to_lower_copy(nat.name);
        defLua << "include \"" << natPathName << "/default.lua\"" << std::endl;
        writeNation(nationsPath / natPathName, nat);
    }
}

int main(int argc, char** argv)
{
    if(!LocaleHelper::init())
        return 1;
    if(!RTTRCONFIG.Init())
        return 1;
    LOG.setWriter(new NullWriter, LogTarget::File);
    WorldDescription worldDesc;
    GameDataLoader gdLoader(worldDesc);
    if(!gdLoader.Load())
    {
        bnw::cerr << "Failed to load game data" << std::endl;
        worldDesc = WorldDescription();
    }
    addNewData(worldDesc);
    writeNations("newGameData/nations", worldDesc);
}
