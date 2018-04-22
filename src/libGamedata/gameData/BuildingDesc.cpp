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
#include "WorldDescription.h"
#include "lua/CheckedLuaTable.h"
#include "lua/LuaHelpers.h"
#include "lua/LuaTraits.h"
#include <kaguya/kaguya.hpp>

namespace kaguya {
template<>
struct lua_type_traits<BuildingDesc::SummerWinterTex, void>
{
    typedef BuildingDesc::SummerWinterTex get_type;

    static bool checkType(lua_State* l, int index)
    {
        const LuaStackRef table(l, index);
        // We can have 1 value or a table with 1 or 2 values
        if(table.isConvertible<ArchiveEntryRef>())
            return true;
        if(table.type() != LUA_TTABLE || table.size() == 0u || table.size() > 2u)
            return false;
        if(!table[1].isConvertible<ArchiveEntryRef>())
            return false;
        if(table.size() == 2u && !table[2].isConvertible<ArchiveEntryRef>())
            return false;
        return true;
    }
    static bool strictCheckType(lua_State* l, int index)
    {
        // Don't care about exact type, convertible is enough
        checkType(l, index);
    }
    static get_type get(lua_State* l, int index)
    {
        const LuaStackRef table(l, index);
        get_type result;
        if(table.isConvertible<ArchiveEntryRef>())
            result.summer = result.winter = table;
        else
        {
            result.summer = table[1];
            if(table.size() == 2u)
                result.winter = table[2];
            else
                result.winter = result.summer;
        }
        return result;
    }
};
} // namespace kaguya

BuildingDesc::BuildingDesc(CheckedLuaTable luaData, const WorldDescription& worldDesc)
{
    luaData.getOrThrow(name, "name");
    DescIdx<BuildingBPDesc> idx = worldDesc.buildingBlueprints.getIndex(name);
    lua::assertTrue(!!idx, "Building blueprint with name '" + name + "' not found in world");
    // Assign blueprint
    static_cast<BuildingBPDesc&>(*this) = worldDesc.get(idx);

    luaData.getOrThrow(icon, "icon");

    CheckedLuaTable textureData = luaData.getOrThrow<CheckedLuaTable>("texture");
    textures.main = textureData.getOrThrow<SummerWinterTex>("main");
    textures.skeleton = textureData.getOrThrow<SummerWinterTex>("skeleton");
    textures.door = textureData.getOrThrow<SummerWinterTex>("door");
    textures.shadow = textureData.getOrDefault("shadow", textures.shadow);
    textures.skeletonShadow = textureData.getOrDefault("skeletonShadow", textures.skeletonShadow);

    luaData.getOrThrow(doorPosY, "doorPosY");

    CheckedLuaTable smokeData =
      luaData.getOrDefault("smoke", CheckedLuaTable(kaguya::LuaTable(luaData.getBaseTable().state(), kaguya::NewTable())));
    smoke.type = smokeData.getOrDefault("type", 0);
    smoke.offset = smokeData.getOrDefault("offset", Point<int8_t>(0, 0));

    textureData.checkUnused();
    smokeData.checkUnused();
    luaData.checkUnused();
}
