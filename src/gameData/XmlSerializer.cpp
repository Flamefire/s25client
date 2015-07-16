#include "XmlSerializer.h"
#include <fstream>
#include <rapidxml_print.hpp>

using namespace rapidxml;

XmlSerializer::XmlSerializer(const std::string& filePath, const std::string& rootName): filePath(filePath){
	root = doc.allocate_node(node_element, doc.allocate_string(rootName.c_str()));
	doc.append_node(root);
}


XmlSerializer::~XmlSerializer(void){}

void XmlSerializer::write(){
	std::ofstream file(filePath.c_str());
	rapidxml::print(std::ostream_iterator<char>(file), doc);
}

XmlNode XmlSerializer::addNode(node_t parent, const char* name, const char* value){
	node_t node = doc.allocate_node(rapidxml::node_element, name, value);
	parent->append_node(node);
	return XmlNode(*this, node);
}

XmlNode XmlSerializer::addNode(node_t parent, const std::string& name){
	return addNode(parent, doc.allocate_string(name.c_str()));
}

XmlNode XmlSerializer::addNode(node_t parent, const std::string& name, const std::string& value){
	return addNode(parent, doc.allocate_string(name.c_str()), doc.allocate_string(value.c_str()));
}

att_t XmlSerializer::addAttribute(node_t parent, const char* name, const char* value){
	att_t att = doc.allocate_attribute(name, value);
	parent->append_attribute(att);
	return att;
}

att_t XmlSerializer::addAttribute(node_t parent, const std::string& name, const std::string& value){
	return addAttribute(parent, doc.allocate_string(name.c_str()), doc.allocate_string(value.c_str()));
}