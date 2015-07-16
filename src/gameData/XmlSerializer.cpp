#include "XmlSerializer.h"
#include <fstream>
#include <rapidxml_print.hpp>
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable:4244)
#endif
    #include <rapidxml_utils.hpp>
#ifdef _MSC_VER
    #pragma warning(pop)
#endif

using namespace rapidxml;

XmlSerializer::XmlSerializer(const std::string& filePath, XmlOpenAction loadOrCreate, const std::string& rootName): filePath(filePath){
    if(loadOrCreate == XmlOpenAction::XML_LOAD){
        rapidxml::file<> xmlFile(filePath.c_str());
        xmlDoc doc;
        doc.parse<0>(xmlFile.data());
    }else{
	    node_t rootNode = doc.allocate_node(node_element, doc.allocate_string(rootName.c_str()));
	    doc.append_node(rootNode);
        root = newNode(rootNode);
    }
}

XmlSerializer::~XmlSerializer(void){
    for(std::vector<NodeChunk*>::iterator it = chunks_.begin(); it!=chunks_.end(); ++it){
        NodeChunk* chunk = *it;
        // Clean up to avoid accidential access
        for(unsigned i=0; i<chunk->usedCt; ++i){
            chunk->nodes[i].doc = NULL;
            chunk->nodes[i].node = NULL;
        }
        delete chunk;
    }
}

XmlNode* XmlSerializer::getNode(const string& name){
    node_t node = doc.first_node(name.empty() ? NULL : name.c_str());
    return (node) ? newNode(node) : NULL;
}

XmlNode* XmlSerializer::newNode(node_t node){
    if(chunks_.size() == 0 || chunks_.back()->usedCt == NodeChunk::chunkSize){
        NodeChunk* chunk = new NodeChunk;
        chunk->usedCt = 0;
        chunks_.push_back(chunk);
    }
    NodeChunk* chunk = chunks_.back();
    XmlNode* result = &chunk->nodes[chunk->usedCt++];
    *result = XmlNode(this, node);
    return result;
}

void XmlSerializer::write(){
	std::ofstream file(filePath.c_str());
	rapidxml::print(std::ostream_iterator<char>(file), doc);
}

XmlNode* XmlSerializer::addNode(XmlNode* parent, const char* name, const char* value){
	node_t node = doc.allocate_node(rapidxml::node_element, name, value);
	parent->node->append_node(node);
	return newNode(node);
}

XmlNode* XmlSerializer::addNode(XmlNode* parent, const std::string& name){
	return addNode(parent, doc.allocate_string(name.c_str()));
}

XmlNode* XmlSerializer::addNode(XmlNode* parent, const std::string& name, const std::string& value){
	return addNode(parent, doc.allocate_string(name.c_str()), doc.allocate_string(value.c_str()));
}

void XmlSerializer::addAttribute(XmlNode* parent, const std::string& name, const std::string& value){
    att_t att = doc.allocate_attribute(doc.allocate_string(name.c_str()), doc.allocate_string(value.c_str()));
	parent->node->append_attribute(att);
}