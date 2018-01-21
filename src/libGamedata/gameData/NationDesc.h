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

#ifndef NationDesc_h__
#define NationDesc_h__

#include "DescriptionContainer.h"
#include "NatBuildingDesc.h" // TODO: How to get rid of this?
#include "Rect.h"
#include <string>

struct WorldDescription;
class CheckedLuaTable;
struct NatBuildingDesc;

struct NationDesc
{
    NationDesc();
    NationDesc(CheckedLuaTable luaData, const WorldDescription& worldDesc);
    ~NationDesc();
    NationDesc& operator=(const NationDesc& other);

    std::string name;
    uint8_t s2Id;
    DescriptionContainer<NatBuildingDesc> buildings;

    // Convenience accessors
    template<class T>
    const T& get(DescIdx<T> idx) const
    {
        return getContainer<T>().get(idx);
    }
    template<class T>
    const DescriptionContainer<T>& getContainer() const;
};

template<>
inline const DescriptionContainer<NatBuildingDesc>& NationDesc::getContainer() const
{
    return buildings;
}

#endif // NationDesc_h__
