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
#include "addNewData.h"
#include "files.h"
#include "gameTypes/BuildingType.h"
#include "gameData/BuildingConsts.h"
#include "gameData/BuildingDesc.h"
#include "gameData/BuildingProperties.h"
#include "gameData/NatBuildingDesc.h"
#include "gameData/NationConsts.h"
#include "gameData/NationDesc.h"
#include "gameData/WorldDescription.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <stdexcept>

unsigned GetNationIdx(const std::string& name)
{
    unsigned i = 0;
    for(; i < NUM_NATIONS; i++)
    {
        if(NationNames[i] == name)
            return i;
    }
    throw std::runtime_error("Nat Not found");
}

unsigned GetBuildingIdx(const std::string& name)
{
    unsigned i = 0;
    for(; i < NUM_BUILDING_TYPES; i++)
    {
        if(BUILDING_NAMES[i] == name)
            return i;
    }
    throw std::runtime_error("Bld Not found");
}

void AddNations(WorldDescription& worldDesc)
{
    for(unsigned n = 0; n < NUM_NATIONS; n++)
    {
        NationDesc nat;
        nat.name = NationNames[n];
        nat.s2Id = NATION_RTTR_TO_S2[n];
        if(!worldDesc.nations.getIndex(nat.name))
            worldDesc.nations.add(nat);
    }
}

void AddBuildings(WorldDescription& worldDesc)
{
    for(unsigned b = 0; b < NUM_BUILDING_TYPES; b++)
    {
        BuildingType bld = BuildingType(b);
        if(!BuildingProperties::IsValid(bld))
            continue;
        BuildingDesc dsc;
        dsc.name = BUILDING_NAMES[b];
        if(!worldDesc.buildings.getIndex(dsc.name))
            worldDesc.buildings.add(dsc);
    }

    for(DescIdx<NationDesc> i(0); i.value < worldDesc.nations.size(); i.value++)
    {
        NationDesc& nat = worldDesc.nations.getMutable(i);

        for(unsigned b = 0; b < NUM_BUILDING_TYPES; b++)
        {
            BuildingType bld = BuildingType(b);
            if(!BuildingProperties::IsValid(bld))
                continue;
            NatBuildingDesc dsc;
            dsc.name = BUILDING_NAMES[b];
            if(!nat.buildings.getIndex(dsc.name))
                nat.buildings.add(dsc);
        }
    }
}

void addNewData(WorldDescription& worldDesc)
{
    AddNations(worldDesc);
    AddBuildings(worldDesc);
    for(DescIdx<BuildingDesc> b(0); b.value < worldDesc.buildings.size(); b.value++)
    {
        BuildingDesc& dsc = worldDesc.buildings.getMutable(b);
        unsigned bld = GetBuildingIdx(dsc.name);
        dsc.help = BUILDING_HELP_STRINGS[bld];
        dsc.costs = BUILDING_COSTS[0][bld];
        dsc.requiredSpace = BUILDING_SIZE[bld];
        dsc.workDescr = BLD_WORK_DESC[bld];
    }
    for(DescIdx<NationDesc> i(0); i.value < worldDesc.nations.size(); i.value++)
    {
        NationDesc& nat = worldDesc.nations.getMutable(i);
        unsigned natIdx = GetNationIdx(nat.name);

        for(DescIdx<NatBuildingDesc> b(0); b.value < nat.buildings.size(); b.value++)
        {
            NatBuildingDesc& dsc = nat.buildings.getMutable(b);
            unsigned bld = GetBuildingIdx(dsc.name);
            dsc.smoke = BUILDING_SMOKE_CONSTS[natIdx][bld];
        }
    }
}
