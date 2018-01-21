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
#include "NatBuildingDesc.h"
#include "lua/CheckedLuaTable.h"
#include "lua/LuaHelpers.h"
#include <kaguya/kaguya.hpp>

NatBuildingDesc::NatBuildingDesc(CheckedLuaTable luaData, const WorldDescription& worldDesc)
{
    luaData.getOrThrow(name, "name");
    lua::assertTrue(!!worldDesc.buildings.getIndex(name), "Building with name '" + name + "' not found");
    CheckedLuaTable smokeData =
      luaData.getOrDefault("smoke", CheckedLuaTable(kaguya::LuaTable(luaData.getBaseTable().state(), kaguya::NewTable())));
    smoke.type = smokeData.getOrDefault("type", 0);
    std::vector<int> smokePos = smokeData.getOrDefault("offset", std::vector<int>());
    if(smokePos.size() < 2)
        smokePos.resize(2);
    smoke.offset = Point<int8_t>(smokePos[0], smokePos[1]);
    smokeData.checkUnused();
    luaData.checkUnused();
}
