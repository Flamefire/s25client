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

#ifndef WorldDescription_h__
#define WorldDescription_h__

#include "DescriptionContainer.h"
#include <boost/core/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <stdexcept>

struct LandscapeDesc;
struct EdgeDesc;
struct TerrainDesc;
struct NationDesc;
struct BuildingDesc;
struct BuildingBPDesc;

struct GameDataError : public std::runtime_error
{
    GameDataError(const std::string& desc) : std::runtime_error("Invalid game data: " + desc) {}
};

struct GameDataLoadError : public std::runtime_error
{
    GameDataLoadError(const std::string& desc) : std::runtime_error("Failed to load game data: " + desc) {}
};

struct WorldDescription
{
    WorldDescription();
    ~WorldDescription();
    WorldDescription& operator=(const WorldDescription& other);
    DescriptionContainer<LandscapeDesc> landscapes;
    DescriptionContainer<EdgeDesc> edges;
    DescriptionContainer<TerrainDesc> terrain;
    DescriptionContainer<NationDesc> nations;
    DescriptionContainer<BuildingBPDesc> buildingBlueprints;

    // Convenience accessors
    // Allows access to buildingBlueprints with DescIdx<BuildingDesc>
    template<class T>
    typename boost::disable_if<boost::is_same<T, BuildingDesc>, const T&>::type get(DescIdx<T> idx) const
    {
        return getContainer<T>().get(idx);
    }
    template<class T>
    typename boost::enable_if<boost::is_same<T, BuildingDesc>, const BuildingBPDesc&>::type get(DescIdx<T> idx) const
    {
        return getContainer<T>().get(DescIdx<BuildingBPDesc>(idx.value));
    }

    template<class T>
    const DescriptionContainer<T>& getContainer() const;
};

template<>
inline const DescriptionContainer<LandscapeDesc>& WorldDescription::getContainer() const
{
    return landscapes;
}

template<>
inline const DescriptionContainer<EdgeDesc>& WorldDescription::getContainer() const
{
    return edges;
}

template<>
inline const DescriptionContainer<TerrainDesc>& WorldDescription::getContainer() const
{
    return terrain;
}

template<>
inline const DescriptionContainer<NationDesc>& WorldDescription::getContainer() const
{
    return nations;
}

template<>
inline const DescriptionContainer<BuildingBPDesc>& WorldDescription::getContainer() const
{
    return buildingBlueprints;
}

#endif // WorldDescription_h__
