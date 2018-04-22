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

#ifndef BuildingDesc_h__
#define BuildingDesc_h__

#include "AnimationDesc.h"
#include "ArchiveEntryRef.h"
#include "BuildingBPDesc.h"
#include "gameTypes/BuildingTypes.h"
#include <map>
#include <stdexcept>
#include <vector>

struct WorldDescription;
class CheckedLuaTable;

/// Nation dependent building data
struct BuildingDesc : public BuildingBPDesc
{
    struct SummerWinterTex
    {
        ArchiveEntryRef summer, winter;
    };
    struct Textures
    {
        SummerWinterTex main, skeleton, door;
        SummerWinterTex shadow, skeletonShadow;
    };

    std::string name;
    ArchiveEntryRef icon;
    Textures textures;
    std::map<std::string, AnimationDesc> animations;
    std::map<std::string, Point<int8_t> > workOffsets;
    int8_t doorPosY;
    Point<int8_t> signPos;
    SmokeConst smoke;

    BuildingDesc() {}
    BuildingDesc(CheckedLuaTable luaData, const WorldDescription& worldDesc);

    const AnimationDesc* getAnimation(const std::string& name) const
    {
        std::map<std::string, AnimationDesc>::const_iterator it = animations.find(name);
        if(it != animations.end())
            return &it->second;
        else
            return NULL;
    }
    Point<int8_t> getWorkOffset(const std::string& name) const
    {
        std::map<std::string, Point<int8_t> >::const_iterator it = workOffsets.find(name);
        if(it == workOffsets.end())
            throw std::runtime_error("Missing work offset '" + name + "'");
        return it->second;
    }
};

#endif // BuildingDesc_h__
