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

#ifndef PropertyLoader_h__
#define PropertyLoader_h__

#include "Singleton.h"
#include "Properties.h"
#include <set>
#include <map>
#include <string>

namespace rapidxml{
	template<class Ch>
	class xml_node;
	template<class Ch>
	class xml_attribute;
}

class XmlNode;
class noBaseBuilding;

typedef rapidxml::xml_node<char>* node_t;
typedef rapidxml::xml_attribute<char>* att_t;

/// Trait that evaluates to true if type T can be converted to type U
template<class T, class U>
class CanConvert{
	typedef char one;
	struct two{one dummy[2];};
	static one Test(const U&);
	static two Test(...);
	static T MakeT();
public:
	enum{ value = sizeof(Test(MakeT()) == sizeof(one)) };
};

template<class T, class U>
struct is_same{
	enum{value = false };
};

template<class T>
struct is_same<T, T>{
	enum{value = true };
};

/// This will be true if T is a super class of U
#define IS_SUPER_CLASS(T, U) \
	(CanConvert<const U*, const T*>::value && !is_same<const T*, const void*>::value)

template<class T>
class FunctorInterface{
public:
	void virtual operator()(T&) = 0;
};

class PropertyLoader: public Singleton<PropertyLoader>, public IPropertyLoader
{
public:
	PropertyLoader(void);
	~PropertyLoader(void);

	/// Registers a building to have its data loaded from XML
	/// Template parameter has to be the class type
	template<class T>
	void registerBuilding(BuildingType type);
	/// Loads all data from XML
	void load(const std::string& filePath);
	/// Saves current (hard-coded) data to XML
	void save(const std::string& filePath);

	virtual void getProperty(Nation nation, BuildingType type, const PropertyBase*& prop) const;
private:
	std::string getXmlName(std::string name) const;
	void loadNation(Nation nation, const std::string& filePath, const std::string& fileName);
	void saveNation(Nation nation, const std::string& filePath);
	void load(node_t node, BaseBuildingProp& prop);
	void save(XmlNode& node, Nation nation, BuildingType type, const BaseBuildingProp& prop);

	struct BaseBuildingProps{ BaseBuildingProp props[Nation::NAT_COUNT]; };
	std::set<BuildingType> regBaseBuildingProb;
	std::map<BuildingType, BaseBuildingProps> baseBuildingProbs;
	std::map<std::string, Nation> NationName2Idx;
	std::set<Nation> loadedNations;
	bool loaded;
};

#define PROPERTY_LOADER PropertyLoader::inst()

#endif // PropertyLoader_h__