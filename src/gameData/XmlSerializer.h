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

#ifndef XmlSerializer_h__
#define XmlSerializer_h__

#include <rapidxml.hpp>
#include <string>
#include <sstream>

typedef rapidxml::xml_document<> xmlDoc;
typedef rapidxml::xml_node<>* node_t;
typedef rapidxml::xml_attribute<>* att_t;

class XmlSerializer;

class XmlNode{
private:
	XmlSerializer& doc;
	node_t node;

	template <typename T>
	std::string ToString(T const &data){
		std::stringstream Io;
		Io << data;
		return Io.str();
	}
	std::string ToString(signed char const& data){
		return ToString<int>(data);
	}
	std::string ToString(unsigned char const& data){
		return ToString<unsigned>(data);
	}
public:
	XmlNode(XmlSerializer& doc, node_t& node): doc(doc), node(node){}
	inline XmlNode addNode(const std::string& name);
	inline XmlNode addNode(const std::string& name, const std::string& value);
	template<typename T>
	inline XmlNode addNode(const std::string& name, const T& value){
		return addNode(name, ToString(value));
	}

	inline att_t addAttribute(const std::string& name, const std::string& value);

	operator node_t(){ return node; }
};

class XmlSerializer{
public:
	XmlSerializer(const std::string& filePath, const std::string& rootName = "root");
	~XmlSerializer();

	XmlNode getRoot(){ return XmlNode(*this, root); }

	XmlNode addNode(node_t parent, const char* name = NULL, const char* value = NULL);
	XmlNode addNode(node_t parent, const std::string& name);
	XmlNode addNode(node_t parent, const std::string& name, const std::string& value);

	att_t addAttribute(node_t parent, const char* name = NULL, const char* value = NULL);
	att_t addAttribute(node_t parent, const std::string& name, const std::string& value);

	void write();
private:
	// Disallow copy...
	XmlSerializer(XmlSerializer&);
	XmlSerializer& operator=(XmlSerializer);

	std::string filePath;
	xmlDoc doc;
	node_t root;
};

XmlNode XmlNode::addNode(const std::string& name){ return doc.addNode(*this, name); }
XmlNode XmlNode::addNode(const std::string& name, const std::string& value){ return doc.addNode(*this, name, value); }

att_t XmlNode::addAttribute(const std::string& name, const std::string& value){ return doc.addAttribute(*this, name, value); }

#endif // XmlSerializer_h__