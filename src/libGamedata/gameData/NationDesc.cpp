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
#include "NationDesc.h"
#include "BuildingDesc.h"
#include "lua/CheckedLuaTable.h"
#include "lua/PointTraits.h"

NationDesc::NationDesc() {}

NationDesc::NationDesc(CheckedLuaTable luaData, const WorldDescription&)
{
    luaData.getOrThrow(name, "name");
    s2Id = luaData.getOrDefault("s2Id", 0);
    texOverideFolder = luaData.getOrDefault<std::string>("texOverideFolder", "");
    luaData.getOrThrow(summerTexFile, "summerTexFile");
    luaData.getOrThrow(winterTexFile, "winterTexFile");
    luaData.getOrThrow(defaultAvatar, "defaultAvatar");
    luaData.checkUnused();
}

NationDesc::~NationDesc() {}

NationDesc& NationDesc::operator=(const NationDesc& other)
{
    name = other.name;
    buildings = other.buildings;
    return *this;
}
