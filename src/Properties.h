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

/// Base class for all property types that can be loaded
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
// Do NOT use instances of any of them directly. Inherit from Property<Prop> to use them!
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
// To use a property in a unit, add an instance of Property<ActualProperty> to it
// and call PROPERTY_LOADER.register*<ActualProperty>(UnitType) in static context (e.g. via anonymous namespace)
// All other classes can then use the property simply by creating an instance of Property<ActualProperty>
//////////////////////////////////////////////////////////////////////////

class IProperty{
protected:
	static const IPropertyLoader* loader;
};

template<class T>
class Property: public IProperty {
    const T* const prop;
public:
	Property(Nation nation, BuildingType type): prop(loader->getProperty<T>(nation, type)){}
    const T* operator->() const{ return prop; }
    const T& operator*() const{ return *prob; }
};
#endif // Properties_h__