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
#include "GameDataLoader.h"
#include "CheckedLuaTable.h"
#include "NationDataLoader.h"
#include "RttrConfig.h"
#include "files.h"
#include "helpers/containerUtils.h"
#include "gameData/BuildingBPDesc.h"
#include "gameData/EdgeDesc.h"
#include "gameData/LandscapeDesc.h"
#include "gameData/NationDesc.h"
#include "gameData/TerrainDesc.h"
#include "gameData/WorldDescription.h"
#include "libutil/Log.h"
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

GameDataLoader::GameDataLoader(WorldDescription& worldDesc, const std::string& basePath)
    : worldDesc_(worldDesc), basePath_(bfs::canonical(basePath).make_preferred().string()), curIncludeDepth_(0), errorInIncludeFile_(false)
{
    Register(lua);

    lua["rttr"] = this;
    lua["include"] = kaguya::function<void(const std::string&)>(boost::bind(&GameDataLoader::Include, this, _1));
}

GameDataLoader::GameDataLoader(WorldDescription& worldDesc)
    : worldDesc_(worldDesc), basePath_(bfs::canonical(RTTRCONFIG.ExpandPath(FILE_PATHS[1])).make_preferred().string()), curIncludeDepth_(0),
      errorInIncludeFile_(false)
{
    Register(lua);

    lua["rttr"] = this;
    lua["include"] = kaguya::function<void(const std::string&)>(boost::bind(&GameDataLoader::Include, this, _1));
}

GameDataLoader::~GameDataLoader() {}

bool GameDataLoader::Load()
{
    curFile_ = (bfs::path(basePath_) / "default.lua").string();
    curIncludeDepth_ = 0;
    errorInIncludeFile_ = false;
    nationLoaders.clear();
    try
    {
        if(!LoadScript(curFile_))
            return false;
        BOOST_FOREACH(NationDataLoader& nationLoader, nationLoaders)
            nationLoader.CopyBuildings();
    } catch(std::exception& e)
    {
        LOG.write("Failed to load game data!\nReason: %1%\nCurrent file being processed: %2%\n") % e.what() % curFile_;
        return false;
    }
    return !errorInIncludeFile_;
}

void GameDataLoader::Register(kaguya::State& state)
{
    state["RTTRGameData"].setClass(kaguya::UserdataMetatable<GameDataLoader, LuaInterfaceBase>()
                                     .addFunction("AddLandscape", &GameDataLoader::AddLandscape)
                                     .addFunction("AddTerrainEdge", &GameDataLoader::AddTerrainEdge)
                                     .addFunction("AddTerrain", &GameDataLoader::AddTerrain)
                                     .addFunction("AddBuilding", &GameDataLoader::AddBuilding)
                                     .addFunction("AddNation", &GameDataLoader::AddNation)
                                     .addFunction("GetWorldDesc", &GameDataLoader::GetWorldDesc));
    NationDataLoader::Register(state);
}

void GameDataLoader::Include(const std::string& filepath)
{
    BOOST_CONSTEXPR_OR_CONST int maxIncludeDepth = 10;
    // Protect against cycles and stack overflows
    if(++curIncludeDepth_ >= maxIncludeDepth)
        throw std::runtime_error("Include file '" + filepath + "' cannot be included as the maximum include depth of 10 is reached!");
    bfs::path absFilePath = bfs::absolute(filepath, bfs::path(curFile_).parent_path());
    if(!bfs::is_regular_file(absFilePath))
        throw std::runtime_error("Cannot find include file '" + filepath + "'!");
    absFilePath = bfs::canonical(absFilePath).make_preferred();
    std::string cleanedFilepath = absFilePath.string();
    if(cleanedFilepath.find(basePath_) == std::string::npos)
        throw std::runtime_error("Cannot load include file '" + filepath + "' outside the lua data directory!");
    std::string oldCurFile = curFile_;
    curFile_ = cleanedFilepath;
    errorInIncludeFile_ |= !LoadScript(cleanedFilepath);
    curFile_ = oldCurFile;
    RTTR_Assert(curIncludeDepth_ > 0);
    --curIncludeDepth_;
}

void GameDataLoader::AddLandscape(const kaguya::LuaTable& data)
{
    worldDesc_.landscapes.add(LandscapeDesc(data, worldDesc_));
}

void GameDataLoader::AddTerrainEdge(const kaguya::LuaTable& data)
{
    worldDesc_.edges.add(EdgeDesc(data, worldDesc_));
}

void GameDataLoader::AddTerrain(const kaguya::LuaTable& data)
{
    worldDesc_.terrain.add(TerrainDesc(data, worldDesc_));
}

void GameDataLoader::AddBuilding(const kaguya::LuaTable& data)
{
    worldDesc_.buildingBlueprints.add(BuildingBPDesc(data, worldDesc_));
}

NationDataLoader& GameDataLoader::AddNation(const kaguya::LuaTable& data)
{
    DescIdx<NationDesc> idx = worldDesc_.nations.add(NationDesc(data, worldDesc_));
    nationLoaders.push_back(NationDataLoader(worldDesc_, worldDesc_.nations.getMutable(idx)));
    return nationLoaders.back();
}

WorldDescription& GameDataLoader::GetWorldDesc()
{
    return worldDesc_;
}
