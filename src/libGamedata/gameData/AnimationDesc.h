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

#ifndef AnimationDesc_h__
#define AnimationDesc_h__

#include "ArchiveEntryRef.h"
#include "Point.h"
#include <boost/flyweight.hpp>
#include <string>
#include <vector>

class CheckedLuaTable;

/// Describe an animation
struct AnimationDesc
{
    boost::flyweight<std::string> filepath;
    std::vector<unsigned> frameIdxs;
    Point<int8_t> offset;
    uint8_t msPerFrame;

    AnimationDesc() {}
    AnimationDesc(CheckedLuaTable luaData);
};

#endif // AnimationDesc_h__
