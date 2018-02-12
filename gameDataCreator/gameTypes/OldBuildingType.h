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

#pragma once

#ifndef OldBuildingType_h__
#define OldBuildingType_h__

#include "helpers/EnumWithString.h"

ENUM_WITH_STRING(OldBuildingType,
                 HEADQUARTERS = 0,    // ----
                 BARRACKS = 1,        // NJR
                 GUARDHOUSE = 2,      // NJR
                 NOTHING2 = 3,        // ----
                 WATCHTOWER = 4,      // NJ
                 NOTHING3 = 5,        // ----
                 NOTHING4 = 6,        // ----
                 NOTHING5 = 7,        // ----
                 NOTHING6 = 8,        // ----
                 FORTRESS = 9,        // NJR
                 GRANITE_MINE = 10,   // NJR
                 COAL_MINE = 11,      // NJR
                 IRON_MINE = 12,      // NJR
                 GOLD_MINE = 13,      // NJR
                 LOOKOUT_TOWER = 14,  //
                 NOTHING7 = 15,       // ----
                 CATAPULT = 16,       //
                 WOODCUTTER = 17,     // NJR
                 FISHERY = 18,        // N
                 QUARRY = 19,         // NJR
                 FORESTER = 20,       // NJR
                 SLAUGHTERHOUSE = 21, // NJR
                 HUNTER = 22,         // NJ
                 BREWERY = 23,        // NJR
                 ARMORY = 24,         // NJR
                 METALWORKS = 25,     // NJR
                 IRON_SMELTER = 26,   // NJR
                 CHARBURNER = 27,     // new
                 PIGFARM = 28,        // NJR
                 STOREHOUSE = 29,     //
                 NOTHING9 = 30,       // ----
                 MILL = 31,           // NJR
                 BAKERY = 32,         // NJR
                 SAWMILL = 33,        // NJR
                 MINT = 34,           // NJR
                 WELL = 35,           // NJR
                 SHIPYARD = 36,       //
                 FARM = 37,           // NJR
                 DONKEY_BREEDER = 38, //
                 HARBOR_BUILDING = 39)

#define OLD_NUM_BUILDING_TYPES (OldBuildingType::HARBOR_BUILDING + 1)

#endif // OldBuildingType_h__
