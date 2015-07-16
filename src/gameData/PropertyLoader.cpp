#include "PropertyLoader.h"
#include <iostream>
#include <utility>
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

// END Helpers //////////////////////////////////////////////////////////////////////

PropertyLoader::PropertyLoader(void){
	loaded = false;
}

PropertyLoader::~PropertyLoader(void){}

string PropertyLoader::getXmlName(string name) const {
	replace(name.begin(), name.end(), ' ', '_');
	return name;
}

void PropertyLoader::load(const string& filePath){
    for(int i=0; i<Nation::NAT_COUNT; i++){
        NationName2Idx.insert(make_pair(getXmlName(NationNames[i]), Nation(i)));
    }
	if(loaded){
		cerr << "Tried to load nations multiple times, ignoring";
		return;
	}
	const string nationsXml = filePath + NATIONS_DATA_FILE;
    XmlSerializer doc(nationsXml, XML_LOAD);
	for(XmlNode* node = doc.getNode("nations")->getChild(); node; node = node->getNextSibling()){
		string name = node->getName();
        if(name.empty()){
            error("Nations file is invalid as the name for a nations is missing: %s", nationsXml.c_str());
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
			loadNation(nat, filePath+node->getValue("path")+"/", "buildings.xml");
		}catch (exception e)
		{
			error("Error loading nation %s: %s", name, e.what());
			continue;
		}
		loadedNations.insert(nat);
	}
}

void PropertyLoader::save(const string& filePath){
    for(int i=0; i<Nation::NAT_COUNT; i++){
        NationName2Idx.insert(make_pair(getXmlName(NationNames[i]), Nation(i)));
    }
	const string nationsXml = filePath + NATIONS_DATA_FILE;
	mkdir_p(filePath);
	XmlSerializer doc(nationsXml, XML_CREATE);
	XmlNode* nations = doc.getRoot()->addNode("nations");
	for(std::map<string, Nation>::const_iterator it = NationName2Idx.cbegin(); it!=NationName2Idx.cend(); ++it){
		XmlNode* nation = nations->addNode(it->first);
		string nationPath = filePath + it->first;
		nation->addNode("path", nationPath);

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
	XmlSerializer doc(nationXml, XML_LOAD);
	for(XmlNode* node = doc.getNode("buildings")->getChild(); node; node = node->getNextSibling()){
		string name = node->getName();
		BuildingType type = getBuildingType(name);
		if(contains(regBaseBuildingProb, type)){
			load(node, baseBuildingProbs[type].props[nation]);
		}
	}
}

void PropertyLoader::saveNation(Nation nation, const string& filePath){
	mkdir_p(filePath);
	string nationXml = filePath + "buildings.xml";
	XmlSerializer doc(nationXml, XML_CREATE);
	XmlNode* buildings = doc.addNode(doc.getRoot(), "buildings");
	for(int i=0; i<BuildingType::BLD_COUNT; i++){
		BuildingType type = BuildingType(i);
		const std::string name = BUILDING_NAMES[i];
		if(name.empty())
			continue;
		XmlNode* b = buildings->addNode(getXmlName(name));
		if(contains(regBaseBuildingProb, type)){
			save(b, nation, type, BaseBuildingProp());
		}
	}
	doc.write();
}

void PropertyLoader::load(XmlNode* node, BaseBuildingProp& prop){
	XmlNode* costs = node->getChild("Costs");
	XmlNode* door = node->getChild("Door");
	prop.costs.boards = node->getIntValue<char>("Boards");
	prop.costs.stones = node->getIntValue<char>("Stones");
	prop.door.y = node->getIntValue<char>("Y");
}

void PropertyLoader::save(XmlNode* node, Nation nation, BuildingType type, const BaseBuildingProp& prop){
	XmlNode* costs = node->addNode("Costs");
	costs->addNode("Boards", BUILDING_COSTS[nation][type].boards);
	costs->addNode("Stones", BUILDING_COSTS[nation][type].stones);
	XmlNode* door = node->addNode("Door");
	door->addNode("Y", DOOR_CONSTS[nation][type]);
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