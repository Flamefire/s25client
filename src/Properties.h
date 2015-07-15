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

#ifndef Properties_h__
#define Properties_h__

#include "BuildingConsts.h"

//////////////////////////////////////////////////////////////////////////
// Interfaces
//////////////////////////////////////////////////////////////////////////

/// Base class for all properties that are used inside the units
class PropertyBase{
public:
	virtual ~PropertyBase(){} // Required to do dynamic_casts
};

/// Basic interface for the property loader to avoid cyclic references
class IPropertyLoader{
	/// Checks the properties real type and assigns a pointer to the data
	virtual void getProperty(Nation nation, BuildingType type, const PropertyBase*& prop) const = 0;
public:
	virtual ~IPropertyLoader(){}
	/// Returns a pointer to a property of the templated type
	template<typename T>
	const T* getProperty(Nation nation, BuildingType type) const {
		const PropertyBase* prop = nullptr;
		getProperty(nation, type, prop);
		return static_cast<const T*>(prop);
	}
};

//////////////////////////////////////////////////////////////////////////
// Properties
// Do NOT use instances of any of them directly. Inherit from ContainsProb<Prop> to use them!
//////////////////////////////////////////////////////////////////////////

struct DoorProb{
	signed char y; // Where does the worker disappear
};

struct BaseBuildingProp: PropertyBase {
	BuildingCost costs;
	DoorProb door;
};

//////////////////////////////////////////////////////////////////////////
// Property container
// To use a property in a unit, make it inherit from ContainsProp<Prop>
// and call PROPERTY_LOADER.register*<UnitClass>(UnitType) in static context (e.g. via anonymous namespace)
//////////////////////////////////////////////////////////////////////////

class ContainsPropBase{
protected:
	static const IPropertyLoader* loader;
};

template<class T>
class ContainsProp: public ContainsPropBase {
public:
	const T* const prop;
	ContainsProp(Nation nation, BuildingType type): prop(loader->getProperty<T>(nation, type)){}
};
#endif // Properties_h__