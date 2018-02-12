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

#ifndef BuildingType_h__
#define BuildingType_h__

enum BuildingType
{
    BLD_HEADQUARTERS,
    BLD_BARRACKS,
    BLD_GUARDHOUSE,
    BLD_WATCHTOWER,
    BLD_FORTRESS,
    BLD_GRANITEMINE,
    BLD_COALMINE,
    BLD_IRONMINE,
    BLD_GOLDMINE,
    BLD_LOOKOUTTOWER,
    BLD_CATAPULT,
    BLD_WOODCUTTER,
    BLD_FISHERY,
    BLD_QUARRY,
    BLD_FORESTER,
    BLD_SLAUGHTERHOUSE,
    BLD_HUNTER,
    BLD_BREWERY,
    BLD_ARMORY,
    BLD_METALWORKS,
    BLD_IRONSMELTER,
    BLD_PIGFARM,
    BLD_STOREHOUSE,
    BLD_MILL,
    BLD_BAKERY,
    BLD_SAWMILL,
    BLD_MINT,
    BLD_WELL,
    BLD_SHIPYARD,
    BLD_FARM,
    BLD_DONKEYBREEDER,
    BLD_HARBORBUILDING,
    BLD_CHARBURNER,
    /// Number of building types. Don't use as a BuildingType
    NUM_BUILDING_TYPES,
    BLD_NOTHING = 255 // Value chosen to not change with new buildings added and in range of byte
};

#endif // BuildingType_h__
