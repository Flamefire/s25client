// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef BuildingConstsLegacy_h__
#define BuildingConstsLegacy_h__

#include "BuildingTypes.h"
#include "NationConsts.h"

// Konstanten für die Baukosten der Gebäude von allen 4 Völkern
const BuildingCost BUILDING_COSTS[NAT_COUNT][40] =
{
    // Nubier
    {
        {0, 0}, {2, 0}, {2, 3}, {0, 0}, {3, 5}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4, 7},
        {4, 0}, {4, 0}, {4, 0}, {4, 0}, {4, 0}, {0, 0}, {4, 2}, {2, 0}, {2, 0}, {2, 0},
        {2, 0}, {2, 2}, {2, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {4, 3}, {3, 3}, {4, 3},
        {0, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 0}, {2, 3}, {3, 3}, {3, 3}, {4, 6}
    },
        // Japaner
    {
        {0, 0}, {2, 0}, {2, 3}, {0, 0}, {3, 5}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4, 7},
        {4, 0}, {4, 0}, {4, 0}, {4, 0}, {4, 0}, {0, 0}, {4, 2}, {2, 0}, {2, 0}, {2, 0},
        {2, 0}, {2, 2}, {2, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {4, 3}, {3, 3}, {4, 3},
        {0, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 0}, {2, 3}, {3, 3}, {3, 3}, {4, 6}
    },
        // Römer
    {
        {0, 0}, {2, 0}, {2, 3}, {0, 0}, {3, 5}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4, 7},
        {4, 0}, {4, 0}, {4, 0}, {4, 0}, {4, 0}, {0, 0}, {4, 2}, {2, 0}, {2, 0}, {2, 0},
        {2, 0}, {2, 2}, {2, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {4, 3}, {3, 3}, {4, 3},
        {0, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 0}, {2, 3}, {3, 3}, {3, 3}, {4, 6}
    },
        // Wikinger
    {
        {0, 0}, {2, 0}, {2, 3}, {0, 0}, {3, 5}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4, 7},
        {4, 0}, {4, 0}, {4, 0}, {4, 0}, {4, 0}, {0, 0}, {4, 2}, {2, 0}, {2, 0}, {2, 0},
        {2, 0}, {2, 2}, {2, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {4, 3}, {3, 3}, {4, 3},
        {0, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 0}, {2, 3}, {3, 3}, {3, 3}, {4, 6}
    },
        // Babylonier
    {
        {0, 0}, {2, 0}, {2, 3}, {0, 0}, {3, 5}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4, 7},
        {4, 0}, {4, 0}, {4, 0}, {4, 0}, {4, 0}, {0, 0}, {4, 2}, {2, 0}, {2, 0}, {2, 0},
        {2, 0}, {2, 2}, {2, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {4, 3}, {3, 3}, {4, 3},
        {0, 0}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 0}, {2, 3}, {3, 3}, {3, 3}, {4, 6}
    }
};

#endif // BuildingConstsLegacy_h__
