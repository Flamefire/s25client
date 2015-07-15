#include "PropertyLoader.h"
#include <iostream>
#include <utility>
#include <rapidxml.hpp>

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable:4244)
#endif
#include <rapidxml_utils.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include <rapidxml_iterators.hpp>
#include <algorithm>
#include <limits>
#include "error.h"
#include "XmlSerializer.h"
#include "files.h"

#include "LegacyConsts.h"
#include "DoorConsts.h"

using namespace std;

// Helpers ////////////////////////////////////////////////////////////////////////

template<class T_Set, typename T_Val>
bool contains(const T_Set& set, const T_Val& val){
	return (set.find(val) != set.end());
}

template<class T>
node_t getNode(T* parent, const char* name){
	node_t res = parent->first_node(name);
	if(!res)
		throw runtime_error("Node " + string(name) +" in " + string(parent->name()) + " not found!");
	return res;
}

node_t getNode(xmlDoc& parent, const char* name){
	return getNode(&parent, name);
}

const char* getValue(node_t parent, const char* name){
	node_t res = getNode(parent, name);
	const char* val = res->value();
	if(!val || !val[0])
		throw runtime_error("Value " + string(name) +" in " + string(parent->name()) + " is empty!");
	return val;
}

inline int string2Int(const string& str, size_t *idx = 0, int base = 10){
	char *endPtr;
	errno = 0;
	long res = strtol(str.c_str(), &endPtr, base);

	if (str.c_str() == endPtr)
		throw std::runtime_error(str + " is not a number");
	if (errno == ERANGE || res < INT_MIN != INT_MAX < res)
		throw std::runtime_error(str + "is out of range");
	if (idx)
		*idx = (size_t)(endPtr - str.c_str());
	else if(str.c_str() + str.size() != endPtr)
		throw std::runtime_error(str + " contains non-numeric values");
	return static_cast<int>(res);
}

template<typename T>
T getIntValue(node_t parent, const char* name, T minVal = numeric_limits<T>::min(), T maxVal = numeric_limits<T>::max()){
	T val = string2Int(getValue(parent, name));
	if(val < minVal || val > maxVal)
		throw runtime_error("Value " + string(name) +" in " + string(parent->name()) + " is out of range!"); 
	return val;
}

// END Helpers //////////////////////////////////////////////////////////////////////

PropertyLoader::PropertyLoader(void){
	loaded = false;
	for(int i=0; i<Nation::NAT_COUNT; i++){
		NationName2Idx.insert(make_pair(getXmlName(NationNames[i]), Nation(i)));
	}
}

PropertyLoader::~PropertyLoader(void){}

string PropertyLoader::getXmlName(string name) const {
	replace(name.begin(), name.end(), ' ', '_');
	return name;
}

void PropertyLoader::load(const string& filePath){
	if(loaded){
		cerr << "Tried to load nations multiple times, ignoring";
		return;
	}
	const string nationsXml = filePath + NATIONS_DATA_FILE;
	rapidxml::file<> xmlFile(nationsXml.c_str());
	xmlDoc doc;
	doc.parse<0>(xmlFile.data());
	for(node_t node = getNode(doc, "nations")->first_node(); node; node = node->next_sibling()){
		string name = node->name();
		node_t pathNode = node->first_node("path");
		if(name.empty() || !pathNode){
			error("Nations file is invalid as either the name or the path for a nations is missing: %s", nationsXml.c_str());
			throw exception("Invalid nations file");
		}
		if(!contains(NationName2Idx, name)){
			error("Invalid or unknown nation name: %s", name);
			throw exception("Invalid nation name");
		}
		Nation nat = NationName2Idx[name];
		if(loadedNations.find(nat) != loadedNations.end()){
			cerr << "Already loaded nation " << name << ". Ignoring...";
			continue;
		}
		try{
			loadNation(nat, filePath+pathNode->value()+"/", "buildings.xml");
		}catch (exception e)
		{
			error("Error loading nation %s: %s", name, e.what());
			continue;
		}
		loadedNations.insert(nat);
	}
}

void PropertyLoader::save(const string& filePath){
	const string nationsXml = filePath + NATIONS_DATA_FILE;
	mkdir_p(filePath);
	XmlSerializer doc(nationsXml);
	XmlNode nations = doc.getRoot().addNode("nations");
	for(std::map<string, Nation>::const_iterator it = NationName2Idx.cbegin(); it!=NationName2Idx.cend(); ++it){
		XmlNode nation = nations.addNode(it->first);
		string nationPath = filePath + it->first;
		nation.addNode("path", nationPath);

		saveNation(it->second, nationPath+"/");
	}
	doc.write();
}

BuildingType getBuildingType(const string& name){
	for(int i=0; i<BuildingType::BLD_COUNT; i++){
		if(BUILDING_NAMES[i] == name)
			return BuildingType(i);
	}
	return BLD_NOTHING;
}

void PropertyLoader::loadNation(Nation nation, const string& filePath, const string& fileName){
	const string nationXml = filePath + fileName;
	rapidxml::file<> xmlFile(nationXml.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	for(node_t node = doc.first_node("buildings")->first_node(); node; node = node->next_sibling()){
		string name = node->name();
		BuildingType type = getBuildingType(name);
		if(contains(regBaseBuildingProb, type)){
			load(node, baseBuildingProbs[type].props[nation]);
		}
	}
}

void PropertyLoader::saveNation(Nation nation, const string& filePath){
	mkdir_p(filePath);
	string nationXml = filePath + "buildings.xml";
	XmlSerializer doc(nationXml);
	XmlNode buildings = doc.addNode(doc.getRoot(), "buildings");
	for(int i=0; i<BuildingType::BLD_COUNT; i++){
		BuildingType type = BuildingType(i);
		const std::string name = BUILDING_NAMES[i];
		if(name.empty())
			continue;
		XmlNode b = buildings.addNode(getXmlName(name));
		if(contains(regBaseBuildingProb, type)){
			save(b, nation, type, BaseBuildingProp());
		}
	}
	doc.write();
}

void PropertyLoader::load(node_t node, BaseBuildingProp& prop){
	node_t costs = getNode(node, "Costs");
	node_t door = getNode(node, "Door");
	prop.costs.boards = getIntValue<char>(costs, "Boards");
	prop.costs.stones = getIntValue<char>(costs, "Stones");
	prop.door.y = getIntValue<char>(door, "Y");
}

void PropertyLoader::save(XmlNode& node, Nation nation, BuildingType type, const BaseBuildingProp& prop){
	XmlNode costs = node.addNode("Costs");
	costs.addNode("Boards", BUILDING_COSTS[nation][type].boards);
	costs.addNode("Stones", BUILDING_COSTS[nation][type].stones);
	XmlNode door = node.addNode("Door");
	door.addNode("Y", DOOR_CONSTS[nation][type]);
}

void PropertyLoader::getProperty(Nation nation, BuildingType type, const PropertyBase*& prop) const{
	if(dynamic_cast<const BaseBuildingProp*>(prop)){
		map<BuildingType, BaseBuildingProps>::const_iterator el = baseBuildingProbs.find(type);
		if(el == baseBuildingProbs.cend())
			throw runtime_error("Tried to init a not loaded building");
		prop = &el->second.props[nation];
	}
}

//////////////////////////////////////////////////////////////////////////
// Register functions
//////////////////////////////////////////////////////////////////////////

template<>
void PropertyLoader::registerBuilding<BaseBuildingProp>(BuildingType type){
    regBaseBuildingProb.insert(type);
}

//////////////////////////////////////////////////////////////////////////
// Implementation of ContainsProp
//////////////////////////////////////////////////////////////////////////

const IPropertyLoader* IProperty::loader = &PROPERTY_LOADER;