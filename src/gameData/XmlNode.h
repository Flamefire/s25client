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

#ifndef XmlNode_h__
#define XmlNode_h__

#include "IDataNode.h"

namespace rapidxml{
    template<class Ch>
    class xml_node;
}
class XmlSerializer;

typedef rapidxml::xml_node<char>* node_t;

class XmlNode: public IDataNode{
    node_t node;
    XmlSerializer* doc;
    friend class XmlSerializer;
    XmlNode(){}
public:
    XmlNode(XmlSerializer* doc, node_t node): doc(doc), node(node){}

    using IDataNode::addNode;
    using IDataNode::getValue;

    virtual XmlNode* addNode(const string& name);
    virtual XmlNode* addNode(const string& name, const string& value);
    virtual void addAttribute(const string& name, const string& value);

    virtual XmlNode* getChild(const string& name = string(), bool throwOnError = true) const;
    virtual XmlNode* getNextSibling(const string& name = string()) const;

    virtual string getName() const;
    virtual string getValue() const;
    virtual string getAttribute(const string& name) const;
};

#endif // XmlNode_h__