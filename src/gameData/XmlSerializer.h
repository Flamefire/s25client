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

#include "XmlNode.h"
#include <vector>
#include <rapidxml.hpp>
typedef rapidxml::xml_document<> xmlDoc;
typedef rapidxml::xml_attribute<>* att_t;

enum XmlOpenAction{
    XML_LOAD,
    XML_CREATE
};

class XmlSerializer{
public:
	XmlSerializer(const std::string& filePath, XmlOpenAction loadOrCreate, const std::string& rootName = "root");
	~XmlSerializer();

	XmlNode* getRoot(){ return root; }
    XmlNode* getNode(const string& name);

    XmlNode* addNode(XmlNode* parent, const char* name, const char* value = NULL);
	XmlNode* addNode(XmlNode* parent, const std::string& name);
	XmlNode* addNode(XmlNode* parent, const std::string& name, const std::string& value);

	void addAttribute(XmlNode* parent, const std::string& name, const std::string& value);

	void write();
private:
	// Disallow copy...
	XmlSerializer(XmlSerializer&);
	XmlSerializer& operator=(XmlSerializer);

    XmlNode* newNode(node_t node);
    friend class XmlNode;

	std::string filePath;
	xmlDoc doc;
	XmlNode* root;

    struct NodeChunk{
        static const unsigned chunkSize = 64;
        unsigned usedCt;
        XmlNode nodes[chunkSize];
    };
    std::vector<NodeChunk*> chunks_;
};

#endif // XmlSerializer_h__