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

#include "XmlNode.h"
#include "XmlSerializer.h"
#include <rapidxml.hpp>

string XmlNode::getName() const{ return node->name(); }
string XmlNode::getValue() const{ return node->value(); }


XmlNode* XmlNode::addNode(const string& name){ return doc->addNode(this, name); }
XmlNode* XmlNode::addNode(const string& name, const string& value){ return doc->addNode(this, name, value); }
void XmlNode::addAttribute(const string& name, const string& value){ doc->addAttribute(this, name, value); }


XmlNode* XmlNode::getChild(const string& name, bool throwOnError) const{
    node_t child = node->first_node(name.empty() ? NULL : name.c_str());
    if(!child){
        if(throwOnError && !name.empty())
            throw std::runtime_error("Node " + string(name) +" in " + string(getName()) + " not found!");
        return NULL;
    }
    return doc->newNode(child);
}

XmlNode* XmlNode::getNextSibling(const string& name) const{
    node_t child = node->next_sibling(name.empty() ? NULL : name.c_str());
    return (child) ?  doc->newNode(child) : NULL;
}

string XmlNode::getAttribute(const string& name) const{
    att_t att = node->first_attribute(name.c_str());
    if(!att)
        throw std::runtime_error("Attribute " + string(name) +" in " + string(getName()) + " not found!");
    return att->value();
}