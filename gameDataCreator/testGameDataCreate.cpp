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
#include "PointOutput.h"
#include "addNewData.h"
#include "lua/GameDataLoader.h"
#include "gameTypes/OldBuildingType.h"
#include "gameTypes/OldNation.h"
#include "gameData/BuildingBPDesc.h"
#include "gameData/BuildingConsts.h"
#include "gameData/BuildingDesc.h"
#include "gameData/DoorConsts.h"
#include "gameData/NationConsts.h"
#include "gameData/NationDesc.h"
#include "gameData/WorldDescription.h"
#include <boost/test/unit_test.hpp>

std::ostream& operator<<(std::ostream& os, const WaresNeeded& wares)
{
    return os << wares.getNum() << "[" << unsigned(wares[0]) << "," << unsigned(wares[1]) << "," << unsigned(wares[2]) << "]";
}

unsigned GetNationIdx(const std::string& name)
{
    for(unsigned i = 0; i < NUM_NATS; i++)
    {
        if(NationNames[i] == name)
            return i;
    }
    throw std::runtime_error("Nat Not found");
}

struct GameDataFixture
{
    WorldDescription worldDesc;
    GameDataFixture()
    {
        GameDataLoader gdLoader(worldDesc);
        BOOST_REQUIRE(gdLoader.Load());
    }
};

BOOST_FIXTURE_TEST_SUITE(GameData, GameDataFixture)

BOOST_AUTO_TEST_CASE(NationTest)
{
    BOOST_REQUIRE_LE(NUM_NATS, (int)worldDesc.nations.size());
    for(DescIdx<NationDesc> i(0); i.value < NUM_NATS; i.value++)
    {
        const NationDesc& dsc = worldDesc.get(i);
        BOOST_REQUIRE_LT(dsc.s2Id, NUM_NATS);
        unsigned oldId = GetNationIdx(dsc.name);
        BOOST_REQUIRE_EQUAL(dsc.texOverideFolder.empty(), oldId <= NUM_NATIVE_NATS);
    }
}

BOOST_AUTO_TEST_CASE(BuildingTest)
{
    BOOST_REQUIRE_LE(NUM_NATS, (int)worldDesc.nations.size());
    for(DescIdx<NationDesc> i(0); i.value < worldDesc.nations.size(); i.value++)
    {
        const NationDesc& nat = worldDesc.get(i);
        unsigned oldNat = GetNationIdx(nat.name);
        BOOST_REQUIRE_LE(OLD_NUM_BUILDING_TYPES, (int)nat.buildings.size());
        for(unsigned j = 0; j < nat.buildings.size(); j++)
        {
            const BuildingDesc& bld = nat.buildings[j];
            unsigned oldBld = GetOldBuildingIdx(bld.name);
            BOOST_REQUIRE_EQUAL(BUILDING_HELP_STRINGS[oldBld], bld.help);
            BOOST_REQUIRE_EQUAL(BUILDING_COSTS[oldNat][oldBld].boards, bld.costs.boards);
            BOOST_REQUIRE_EQUAL(BUILDING_COSTS[oldNat][oldBld].stones, bld.costs.stones);
            BOOST_REQUIRE_EQUAL(BUILDING_SIZE[oldBld], bld.requiredSpace);
            BOOST_REQUIRE_EQUAL(BLD_WORK_DESC[oldBld].job, bld.workDescr.job);
            BOOST_REQUIRE_EQUAL(BLD_WORK_DESC[oldBld].producedWare, bld.workDescr.producedWare);
            BOOST_REQUIRE_EQUAL(BLD_WORK_DESC[oldBld].waresNeeded, bld.workDescr.waresNeeded);
            BOOST_REQUIRE_EQUAL(BLD_WORK_DESC[oldBld].numSpacesPerWare, bld.workDescr.numSpacesPerWare);
            BOOST_REQUIRE_EQUAL(BLD_WORK_DESC[oldBld].useOneWareEach, bld.workDescr.useOneWareEach);
            BOOST_REQUIRE_EQUAL(DOOR_CONSTS[oldNat][oldBld], bld.doorPosY);
            BOOST_REQUIRE_EQUAL(BUILDING_SMOKE_CONSTS[oldNat][oldBld].type, bld.smoke.type);
            if(bld.smoke.type)
                BOOST_REQUIRE_EQUAL(BUILDING_SMOKE_CONSTS[oldNat][oldBld].offset, bld.smoke.offset);
            BOOST_REQUIRE(!bld.icon.filepath.get().empty());
            BOOST_REQUIRE_GT(bld.icon.index, 0u);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
