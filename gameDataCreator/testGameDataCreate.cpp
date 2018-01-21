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
#include "gameData/BuildingDesc.h"
#include "gameData/NatBuildingDesc.h"
#include "gameData/NationDesc.h"
#include "gameData/WorldDescription.h"
#include <boost/test/unit_test.hpp>

std::ostream& operator<<(std::ostream& os, const WaresNeeded& wares)
{
    return os << wares.getNum() << "[" << unsigned(wares[0]) << "," << unsigned(wares[1]) << "," << unsigned(wares[2]) << "]";
}

struct GameDataFixture
{
    WorldDescription worldDesc, newWorldDesc;
    GameDataFixture()
    {
        GameDataLoader gdLoader(worldDesc);
        BOOST_REQUIRE(gdLoader.Load());
        addNewData(newWorldDesc);
    }
};

BOOST_FIXTURE_TEST_SUITE(GameData, GameDataFixture)

BOOST_AUTO_TEST_CASE(NationTest)
{
    BOOST_REQUIRE_LE(newWorldDesc.nations.size(), worldDesc.nations.size());
    for(DescIdx<NationDesc> i(0); i.value < newWorldDesc.nations.size(); i.value++)
    {
        BOOST_REQUIRE_EQUAL(newWorldDesc.get(i).s2Id, worldDesc.get(i).s2Id);
    }
}

BOOST_AUTO_TEST_CASE(BuildingTest)
{
    BOOST_REQUIRE_LE(newWorldDesc.buildings.size(), worldDesc.buildings.size());
    for(DescIdx<BuildingDesc> i(0); i.value < newWorldDesc.buildings.size(); i.value++)
    {
        const BuildingDesc& newBld = newWorldDesc.get(i);
        const BuildingDesc& bld = worldDesc.get(i);
        BOOST_REQUIRE_EQUAL(newBld.help, bld.help);
        BOOST_REQUIRE_EQUAL(newBld.costs.boards, bld.costs.boards);
        BOOST_REQUIRE_EQUAL(newBld.costs.stones, bld.costs.stones);
        BOOST_REQUIRE_EQUAL(newBld.requiredSpace, bld.requiredSpace);
        BOOST_REQUIRE_EQUAL(newBld.workDescr.job, bld.workDescr.job);
        BOOST_REQUIRE_EQUAL(newBld.workDescr.producedWare, bld.workDescr.producedWare);
        BOOST_REQUIRE_EQUAL(newBld.workDescr.waresNeeded, bld.workDescr.waresNeeded);
        BOOST_REQUIRE_EQUAL(newBld.workDescr.numSpacesPerWare, bld.workDescr.numSpacesPerWare);
        BOOST_REQUIRE_EQUAL(newBld.workDescr.useOneWareEach, bld.workDescr.useOneWareEach);
    }
}

BOOST_AUTO_TEST_CASE(NatBuildingTest)
{
    BOOST_REQUIRE_LE(newWorldDesc.nations.size(), worldDesc.nations.size());
    for(DescIdx<NationDesc> i(0); i.value < newWorldDesc.nations.size(); i.value++)
    {
        const NationDesc& newNat = newWorldDesc.get(i);
        const NationDesc& nat = worldDesc.get(i);
        BOOST_REQUIRE_LE(newNat.buildings.size(), nat.buildings.size());
        for(DescIdx<NatBuildingDesc> j(0); j.value < newNat.buildings.size(); j.value++)
        {
            const NatBuildingDesc& newBld = newNat.get(j);
            const NatBuildingDesc& bld = nat.get(j);
            BOOST_REQUIRE_EQUAL(newBld.smoke.type, bld.smoke.type);
            if(bld.smoke.type)
                BOOST_REQUIRE_EQUAL(newBld.smoke.offset, bld.smoke.offset);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
