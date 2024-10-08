// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "PointOutput.h"
#include "RttrForeachPt.h"
#include "ai/AIPlayer.h"
#include "ai/aijh/AIPlayerJH.h"
#include "buildings/noBuilding.h"
#include "buildings/noBuildingSite.h"
#include "buildings/nobBaseWarehouse.h"
#include "buildings/nobMilitary.h"
#include "factories/AIFactory.h"
#include "factories/BuildingFactory.h"
#include "helpers/containerUtils.h"
#include "network/GameMessage_Chat.h"
#include "notifications/NodeNote.h"
#include "worldFixtures/WorldWithGCExecution.h"
#include "nodeObjs/noFlag.h"
#include "nodeObjs/noTree.h"
#include "gameTypes/GameTypesOutput.h"
#include "gameData/BuildingProperties.h"
#include "gameData/MilitaryConsts.h"
#include "rttr/test/random.hpp"
#include <boost/test/unit_test.hpp>
#include <memory>
#include <set>

namespace {
// We need border land
using BiggerWorldWithGCExecution = WorldWithGCExecution<1, 24, 22>;
using EmptyWorldFixture1P = WorldFixture<CreateEmptyWorld, 1>;
using EmptyWorldFixture2P = WorldFixture<CreateEmptyWorld, 2>;

template<class T_Col>
inline bool containsBldType(const T_Col& collection, BuildingType type)
{
    return helpers::contains_if(collection,
                                [type](const noBaseBuilding* bld) { return bld->GetBuildingType() == type; });
}

inline bool playerHasBld(const GamePlayer& player, BuildingType type)
{
    const BuildingRegister& blds = player.GetBuildingRegister();
    if(containsBldType(blds.GetBuildingSites(), type))
        return true;
    if(BuildingProperties::IsMilitary(type))
        return containsBldType(blds.GetMilitaryBuildings(), type);
    if(BuildingProperties::IsWareHouse(type)) // Includes harbors
        return containsBldType(blds.GetStorehouses(), type);
    return !blds.GetBuildings(type).empty();
}

struct MockAI final : public AIPlayer
{
    MockAI(unsigned char playerId, const GameWorldBase& gwb, const AI::Level level) : AIPlayer(playerId, gwb, level) {}
    // LCOV_EXCL_START
    void RunGF(unsigned /*gf*/, bool /*gfisnwf*/) override {}
    void OnChatMessage(unsigned /*sendPlayerId*/, ChatDestination, const std::string& /*msg*/) override {}
    // LCOV_EXCL_STOP
};
} // namespace

// Note game command execution is emulated to be like the ones send via network:
// Run "Network Frame" then execute GCs from last NWF
// Also use "HARD" AI for faster execution
BOOST_AUTO_TEST_SUITE(AI)

BOOST_FIXTURE_TEST_CASE(PlayerHasBld_IsCorrect, EmptyWorldFixture1P)
{
    const GamePlayer& player = world.GetPlayer(0);
    BOOST_TEST(playerHasBld(player, BuildingType::Headquarters));
    MapPoint pos = player.GetHQPos();
    for(const auto bld : {BuildingType::Woodcutter, BuildingType::Barracks, BuildingType::Storehouse})
    {
        pos = world.MakeMapPoint(pos + Position(2, 0));
        BOOST_TEST_INFO(bld);
        BOOST_TEST(!playerHasBld(player, bld));
        BuildingFactory::CreateBuilding(world, bld, pos, player.GetPlayerId(), Nation::Romans);
        BOOST_TEST_INFO(bld);
        BOOST_TEST(playerHasBld(player, bld));
    }
}

BOOST_FIXTURE_TEST_CASE(AIChat, EmptyWorldFixture2P)
{
    MockAI ai(1, world, AI::Level::Easy);
    ai.getAIInterface().Chat("Hello players!");
    ai.getAIInterface().Chat("2nd Message!", ChatDestination::Allies);
    const auto msgs = ai.getAIInterface().FetchChatMessages();
    BOOST_TEST_REQUIRE(msgs.size() == 2u);
    BOOST_TEST(msgs[0]->player == 1u);
    BOOST_TEST(msgs[0]->destination == ChatDestination::All);
    BOOST_TEST(msgs[0]->text == "Hello players!");
    BOOST_TEST(msgs[1]->player == 1u);
    BOOST_TEST(msgs[1]->destination == ChatDestination::Allies);
    BOOST_TEST(msgs[1]->text == "2nd Message!");
    // Messages cleared by first call
    BOOST_TEST(ai.getAIInterface().FetchChatMessages().empty());
    // Can readd
    const auto dest = rttr::test::randomEnum<ChatDestination>();
    ai.getAIInterface().Chat("Hello again!", dest);
    // Iterate just like in ExecuteNWF function
    for(auto& msg : ai.getAIInterface().FetchChatMessages())
    {
        BOOST_TEST(msg->player == 1u);
        BOOST_TEST(msg->destination == dest);
        BOOST_TEST(msg->text == "Hello again!");
    }
}

BOOST_FIXTURE_TEST_CASE(KeepBQUpdated, BiggerWorldWithGCExecution)
{
    // Place some trees to reduce BQ at some points
    RTTR_FOREACH_PT(MapPoint, world.GetSize())
    {
        if(pt.x % 4 == 0 && pt.y % 2 == 0 && world.GetNode(pt).bq == BuildingQuality::Castle
           && world.CalcDistance(pt, hqPos) > 6)
            world.SetNO(pt, new noTree(pt, 0, 3));
    }
    world.InitAfterLoad();

    auto ai = AIFactory::Create(AI::Info(AI::Type::Default, AI::Level::Hard), curPlayer, world);
    const AIJH::AIPlayerJH& aijh = static_cast<AIJH::AIPlayerJH&>(*ai);

    const auto assertBqEqualOnWholeMap = [this, &aijh](const unsigned lineNr) {
        BOOST_TEST_CONTEXT("Line #" << lineNr)
        RTTR_FOREACH_PT(MapPoint, world.GetSize())
        {
            BOOST_TEST_INFO(pt);
            BOOST_TEST(this->world.GetBQ(pt, curPlayer) == aijh.GetAINode(pt).bq);
        }
    };
    const auto assertBqEqualAround = [this, &aijh](const unsigned lineNr, MapPoint pt, unsigned radius) {
        BOOST_TEST_CONTEXT("Line #" << lineNr)
        world.CheckPointsInRadius(
          pt, radius,
          [&](const MapPoint curPt, unsigned) {
              BOOST_TEST_INFO(curPt);
              BOOST_TEST(this->world.GetBQ(curPt, curPlayer) == aijh.GetAINode(curPt).bq);
              return false;
          },
          true);
    };

    // 100GFs for initialization
    for(unsigned gf = 0; gf < 100; ++gf)
    {
        em.ExecuteNextGF();
        ai->RunGF(em.GetCurrentGF(), true);
    }
    assertBqEqualOnWholeMap(__LINE__);

    // Set and destroy flag everywhere possible
    std::vector<MapPoint> possibleFlagNodes;
    RTTR_FOREACH_PT(MapPoint, world.GetSize())
    {
        if(world.GetBQ(pt, curPlayer) != BuildingQuality::Nothing && !world.IsFlagAround(pt))
            possibleFlagNodes.push_back(pt);
    }
    for(const MapPoint flagPos : possibleFlagNodes)
    {
        this->SetFlag(flagPos);
        BOOST_TEST_REQUIRE(world.GetSpecObj<noFlag>(flagPos));
        em.ExecuteNextGF();
        ai->RunGF(em.GetCurrentGF(), true);
        assertBqEqualAround(__LINE__, flagPos, 3);

        this->DestroyFlag(flagPos);
        BOOST_TEST_REQUIRE(!world.GetSpecObj<noFlag>(flagPos));
        em.ExecuteNextGF();
        ai->RunGF(em.GetCurrentGF(), true);
        assertBqEqualAround(__LINE__, flagPos, 3);
    }

    // Build road
    const MapPoint flagPos = world.MakeMapPoint(world.GetNeighbour(hqPos, Direction::SouthEast) + Position(4, 0));
    this->BuildRoad(world.GetNeighbour(hqPos, Direction::SouthEast), false, std::vector<Direction>(4, Direction::East));
    BOOST_TEST_REQUIRE(world.GetSpecObj<noFlag>(flagPos)->GetRoute(Direction::West));
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), true);
    assertBqEqualAround(__LINE__, flagPos, 6);

    // Destroy road and flag
    this->DestroyFlag(flagPos);
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), true);
    assertBqEqualAround(__LINE__, flagPos, 6);

    // Build building
    const MapPoint bldPos = world.MakeMapPoint(hqPos + Position(5, 0));
    this->SetBuildingSite(bldPos, BuildingType::Barracks);
    BOOST_TEST_REQUIRE(world.GetSpecObj<noBuildingSite>(bldPos));
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), true);
    assertBqEqualAround(__LINE__, bldPos, 6);

    this->BuildRoad(world.GetNeighbour(bldPos, Direction::SouthEast), false,
                    std::vector<Direction>(5, Direction::West));
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), true);
    RTTR_EXEC_TILL(2000, world.GetSpecObj<noBuilding>(bldPos));
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), false);
    assertBqEqualOnWholeMap(__LINE__);

    // Gain land
    const nobMilitary* bld = world.GetSpecObj<nobMilitary>(bldPos);
    for(unsigned i = 0; i < 500; i++)
    {
        em.ExecuteNextGF();
        ai->RunGF(em.GetCurrentGF(), false);
        if(bld->GetNumTroops() > 0u)
            break;
    }
    BOOST_TEST_REQUIRE(bld->GetNumTroops() > 0u);
    assertBqEqualOnWholeMap(__LINE__);

    // Move the boundary by one node
    std::set<MapPoint, MapPointLess> outerBoundaryNodes;
    std::vector<MapPoint> borderNodes;
    RTTR_FOREACH_PT(MapPoint, world.GetSize())
    {
        if(world.IsBorderNode(pt, curPlayer + 1))
        {
            borderNodes.push_back(pt);
            world.CheckPointsInRadius(
              pt, 1,
              [&outerBoundaryNodes, &world = this->world, curPlayer = this->curPlayer](const MapPoint curPt, unsigned) {
                  if(world.GetNode(curPt).owner != curPlayer + 1)
                      outerBoundaryNodes.insert(curPt);
                  return false;
              },
              false);
        }
    }
    // Once to outside
    for(const MapPoint pt : outerBoundaryNodes)
        world.SetOwner(pt, curPlayer + 1);
    world.RecalcBorderStones(Position(0, 0), Extent(world.GetSize()));
    for(const MapPoint pt : outerBoundaryNodes)
        world.GetNotifications().publish(NodeNote(NodeNote::Owner, pt));
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), false);
    assertBqEqualOnWholeMap(__LINE__);

    // And back
    for(const MapPoint pt : outerBoundaryNodes)
        world.SetOwner(pt, 0);
    world.RecalcBorderStones(Position(0, 0), Extent(world.GetSize()));
    for(const MapPoint pt : outerBoundaryNodes)
        world.GetNotifications().publish(NodeNote(NodeNote::Owner, pt));
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), false);
    assertBqEqualOnWholeMap(__LINE__);

    // And once to inside
    for(const MapPoint pt : borderNodes)
        world.SetOwner(pt, 0);
    world.RecalcBorderStones(Position(0, 0), Extent(world.GetSize()));
    for(const MapPoint pt : borderNodes)
        world.GetNotifications().publish(NodeNote(NodeNote::Owner, pt));
    em.ExecuteNextGF();
    ai->RunGF(em.GetCurrentGF(), false);
    assertBqEqualOnWholeMap(__LINE__);
}

BOOST_FIXTURE_TEST_CASE(BuildWoodIndustry, WorldWithGCExecution<1>)
{
    // Place a few trees
    for(const MapPoint& pt : world.GetPointsInRadius(hqPos + MapPoint(4, 0), 2))
    {
        if(!world.GetNode(pt).obj)
            world.SetNO(pt, new noTree(pt, 0, 3));
    }
    world.InitAfterLoad();

    const GamePlayer& player = world.GetPlayer(curPlayer);
    auto ai = AIFactory::Create(AI::Info(AI::Type::Default, AI::Level::Hard), curPlayer, world);
    // Build a woodcutter, sawmill and forester at some point
    for(unsigned gf = 0; gf < 2000;)
    {
        std::vector<gc::GameCommandPtr> aiGcs = ai->FetchGameCommands();
        for(unsigned i = 0; i < 5; i++, gf++)
        {
            em.ExecuteNextGF();
            ai->RunGF(em.GetCurrentGF(), i == 0);
        }
        for(gc::GameCommandPtr& gc : aiGcs)
        {
            gc->Execute(world, curPlayer);
        }
        if(playerHasBld(player, BuildingType::Sawmill) && playerHasBld(player, BuildingType::Woodcutter)
           && playerHasBld(player, BuildingType::Forester))
            break;
    }
    BOOST_TEST(playerHasBld(player, BuildingType::Sawmill));
    BOOST_TEST(playerHasBld(player, BuildingType::Woodcutter));
    BOOST_TEST(playerHasBld(player, BuildingType::Forester));
}

namespace {
void forceExpansion(const GamePlayer& player, GameWorld& world)
{
    // No space for saw mill due to altitude diff of 3 in range 2 -> Huts only
    for(unsigned y = 0; y < world.GetHeight(); y += 2)
    {
        for(unsigned x = 0; x < world.GetWidth(); x += 2)
            world.ChangeAltitude(MapPoint(x, y), 13);
    }
    RTTR_FOREACH_PT(MapPoint, world.GetSize())
    {
        BOOST_TEST_REQUIRE(world.GetBQ(pt, player.GetPlayerId()) <= BuildingQuality::Hut);
    }
}

void runUntilMilitaryBuildingSiteFound(TestEventManager& em, unsigned curPlayer, GameWorld& world,
                                       const std::list<noBuildingSite*>& bldSites)
{
    auto ai = AIFactory::Create(AI::Info(AI::Type::Default, AI::Level::Hard), curPlayer, world);
    for(unsigned gf = 0; gf < 2000;)
    {
        std::vector<gc::GameCommandPtr> aiGcs = ai->FetchGameCommands();
        for(unsigned i = 0; i < 5; i++, gf++)
        {
            em.ExecuteNextGF();
            ai->RunGF(em.GetCurrentGF(), i == 0);
        }
        for(gc::GameCommandPtr& gc : aiGcs)
        {
            gc->Execute(world, curPlayer);
        }
        if(containsBldType(bldSites, BuildingType::Barracks) || containsBldType(bldSites, BuildingType::Guardhouse))
            break;
    }
}
} // namespace

BOOST_FIXTURE_TEST_CASE(ExpandWhenNoSpace, BiggerWorldWithGCExecution)
{
    const auto& player = world.GetPlayer(curPlayer);
    const auto& bldSites = player.GetBuildingRegister().GetBuildingSites();

    forceExpansion(player, world);
    runUntilMilitaryBuildingSiteFound(em, curPlayer, world, bldSites);

    BOOST_TEST_REQUIRE(
      (containsBldType(bldSites, BuildingType::Barracks) || containsBldType(bldSites, BuildingType::Guardhouse)));
}

BOOST_FIXTURE_TEST_CASE(DoNotBuildMilitaryBuildingsWithinComputerBarrier, BiggerWorldWithGCExecution)
{
    const auto& player = world.GetPlayer(curPlayer);
    const auto& bldSites = player.GetBuildingRegister().GetBuildingSites();

    const auto& barrierPt = player.GetHQPos();
    constexpr auto radius = HQ_RADIUS;

    world.SetComputerBarrier(barrierPt, radius);
    forceExpansion(player, world);
    runUntilMilitaryBuildingSiteFound(em, curPlayer, world, bldSites);

    BOOST_TEST_REQUIRE(
      !(containsBldType(bldSites, BuildingType::Barracks) || containsBldType(bldSites, BuildingType::Guardhouse)));
}

BOOST_FIXTURE_TEST_CASE(DoBuildMilitaryBuildingsOutsideComputerBarrier, BiggerWorldWithGCExecution)
{
    const auto& player = world.GetPlayer(curPlayer);
    const auto& bldSites = player.GetBuildingRegister().GetBuildingSites();

    auto barrierPt = player.GetHQPos();
    // move barrier 2 tiles west of HQ, now military buildings should be buildable to the east
    barrierPt.x -= 2;
    constexpr auto radius = HQ_RADIUS;

    world.SetComputerBarrier(barrierPt, radius);
    forceExpansion(player, world);
    runUntilMilitaryBuildingSiteFound(em, curPlayer, world, bldSites);

    BOOST_TEST_REQUIRE(
      (containsBldType(bldSites, BuildingType::Barracks) || containsBldType(bldSites, BuildingType::Guardhouse)));

    for(noBuildingSite* bldSite : bldSites)
        BOOST_TEST_REQUIRE(!world.CheckPointsInRadius(
          barrierPt, radius,
          [bldSite](const MapPoint& pt, unsigned) {
              const auto type = bldSite->GetBuildingType();
              return (type == BuildingType::Barracks || type == BuildingType::Guardhouse) && pt == bldSite->GetPos();
          },
          true));
}

BOOST_AUTO_TEST_SUITE_END()
