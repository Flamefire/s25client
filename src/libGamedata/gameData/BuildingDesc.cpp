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
#include "BuildingDesc.h"
#include "lua/CheckedLuaTable.h"
#include "lua/LuaHelpers.h"
#include "gameTypes/GoodTypes.h"

BuildingQuality strToBq(const std::string& bq)
{
    if(bq == "hut")
        return BQ_HUT;
    if(bq == "house")
        return BQ_HOUSE;
    if(bq == "castle")
        return BQ_CASTLE;
    if(bq == "mine")
        return BQ_MINE;
    if(bq == "harbor")
        return BQ_HARBOR;
    throw GameDataError("Invalid space size: " + bq);
}

Job strToJob(const std::string& job)
{
    if(job == "none")
        return JOB_NOTHING;
    for(unsigned i = 0; i < NUM_JOB_TYPES; i++)
    {
        if(JOB_NAMES[i] == job)
            return Job(i);
    }
    throw GameDataError("Invalid job: " + job);
}

GoodType strToWare(const std::string& gd)
{
    if(gd == "none")
        return GD_NOTHING;
    if(gd == "invalid")
        return GD_INVALID;
    for(unsigned i = 0; i < NUM_WARE_TYPES; i++)
    {
        if(WARE_NAMES[i] == gd)
            return GoodType(i);
    }
    throw GameDataError("Invalid ware: " + gd);
}

BuildingDesc::BuildingDesc(CheckedLuaTable luaData, const WorldDescription& worldDesc)
{
    luaData.getOrThrow(name, "name");
    luaData.getOrThrow(help, "help");
    CheckedLuaTable costsData = luaData.getOrThrow<CheckedLuaTable>("costs");
    costs.boards = costsData.getOrDefault("boards", 0);
    costs.stones = costsData.getOrDefault("stones", 0);
    requiredSpace = strToBq(luaData.getOrThrow<std::string>("size"));
    CheckedLuaTable workData = luaData.getOrThrow<CheckedLuaTable>("work");
    workDescr.job = strToJob(workData.getOrDefault<std::string>("worker", "none"));
    workDescr.producedWare = strToWare(workData.getOrDefault<std::string>("producedWare", "none"));
    std::vector<std::string> waresNeededData = workData.getOrDefault("waresNeeded", std::vector<std::string>());
    for(unsigned i = 0; i < waresNeededData.size(); i++)
        workDescr.waresNeeded[i] = strToWare(waresNeededData[i]);
    workDescr.numSpacesPerWare = workData.getOrDefault("numSpacesPerWare", workDescr.numSpacesPerWare);
    workDescr.useOneWareEach = workData.getOrDefault("useOneWareEach", workDescr.useOneWareEach);

    costsData.checkUnused();
    workData.checkUnused();
    luaData.checkUnused();
}
