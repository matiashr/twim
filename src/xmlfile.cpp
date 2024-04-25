#include <string>
#include <unistd.h>
#include <fstream>
#include <ncurses.h>
#include <uuid/uuid.h>
#include "xmlfile.h"
#include "main.h"
#include "templates.h"
#include "file.h"

inline bool fexists(const std::string& name) 
{
	try {
		std::ifstream f(name.c_str());
		return f.good();
	}catch( ... ) {
	}
	return false;
}

static xmlChar* getCDATAContent(xmlNodePtr node) 
{
        xmlNodePtr child = node->children;
        while (child != NULL) {
                if (child->type == XML_CDATA_SECTION_NODE) {
                        xmlChar *content = xmlNodeGetContent(child);
                        return content;
                }
                child = child->next;
        }
        return NULL;
}

static char* nodeToString(xmlNodePtr node) 
{
    xmlBufferPtr buffer = xmlBufferCreate();
    xmlNodeDump(buffer, node->doc, node, 0, 1);

    char* str = (char*)xmlBufferContent(buffer);
    char* result = strdup(str); // Make a copy of the string

    xmlBufferFree(buffer);
    return result;
}

static xmlNodePtr findElementByName(xmlNodePtr node, const char* name) 
{
	for (xmlNodePtr curr = node; curr != NULL; curr = curr->next) {
		if (curr->type == XML_ELEMENT_NODE && xmlStrcmp(curr->name, (const xmlChar *)name) == 0) {
			return curr;
		}
		xmlNodePtr found = findElementByName(curr->children, name);
		if (found != NULL) {
			return found;
		}
	}
	return NULL;
}

std::string newUUID()
{
        uuid_t uuid;
        char uuid_str[37]; // UUID string representation needs 36 characters plus '\0'
        // Generate a UUID
        uuid_generate(uuid);
        // Convert UUID to string
        uuid_unparse(uuid, uuid_str);
        return std::string(uuid_str);
}

static xmlNodePtr getChild(xmlNodePtr node, std::string name ) 
{
        xmlNodePtr child = node->children;
        while (child != NULL) {
		std::string elementName=std::string((char*)child->name);
		if( elementName == name ) {
			return child;
		}
                child = child->next;
        }
        return NULL;
}

XmlFile::XmlFile( std::string f ):
	m_file(f)
{
}

XmlFile::~XmlFile()
{
	// Free the document
	xmlFreeDoc(doc);
	xmlCleanupParser();
}

bool XmlFile::parseDut(xmlNode* node)
{
	m_fileType = DUT;
	return true;
}

bool XmlFile::parsePou(std::string entity, xmlNode* node)
{
	Object body;
	m_fileType = POU;
	for ( xmlNode* currentNode = node->children; currentNode; currentNode = currentNode->next) {
		if( currentNode->name == NULL) {
			return false;
		}
		std::string elementName=std::string((char*)currentNode->name);
		if( elementName == "Declaration" ) {
			body.declNode    = currentNode;
			body.declaration =  std::string( (char*)xmlNodeGetContent(body.declNode) );
		} else if( elementName == "Implementation") {
			xmlNodePtr ST = getChild( currentNode, "ST");
			//printf("ST=%x\n", ST);
			body.implNode  = currentNode;
			body.implementation =  std::string( (char*)xmlNodeGetContent(body.implNode) ); //getCDATAContent(currentNode);
		}
	}
	
	m_primary  = entity;
	body.name  = entity;
	body.parent= node;
	m_methodNames.push_back(entity);
	m_objects[ entity ] =  body;

	for ( xmlNode* currentNode = node->children; currentNode; currentNode = currentNode->next) {
		if (currentNode->type == XML_ELEMENT_NODE) {
			xmlAttrPtr attribute = currentNode->properties;
			std::string elementName=std::string((char*)currentNode->name);
			if( elementName == "Method") {
				Object data;
				std::string attrName= (char*) xmlGetProp(currentNode, (xmlChar*)"Name");
				data.declNode       = getChild( currentNode, "Declaration");
				data.implNode       = getChild( currentNode, "Implementation");
				data.parent         = currentNode;
				data.declaration    = std::string( (char*)xmlNodeGetContent(data.declNode) );
				data.implementation = std::string( (char*)xmlNodeGetContent(data.implNode) );
				m_objects[attrName] =  data;
				m_methodNames.push_back(attrName);
#if 0
				printf("Method:%s %s\n", elementName.c_str(), attrName.c_str() );
				printf("Decl\n%s\n", data.declaration.c_str() );
				printf("impl\n%s\n", data.implementation.c_str() );
#endif
			}
		}
	}
	return true;
}

std::string XmlFile::getDeclaraion()
{
	return m_objects[ getPrimaryName() ].declaration;
}

std::string XmlFile::getImplementation()
{
	return m_objects[ getPrimaryName() ].implementation;
}

bool XmlFile::getMethodObject( std::string name, std::string* r_decl, std::string* r_impl ) 
{ 
	if( m_objects.find(name) != m_objects.end() ) {
		*r_decl = m_objects[name].declaration;
		*r_impl = m_objects[name].implementation;
		return true;
	}
	return false;
};

bool XmlFile::parseItf(xmlNode* node)
{
	m_fileType = ITF;
	return true;
}

std::string XmlFile::getPrimaryName()
{
	return m_primary;
}

bool XmlFile::parse(xmlNode*node )
{
	// file seems valid
	xmlNode *currentNode = NULL;
	for (currentNode = node; currentNode; currentNode = currentNode->next) {
		if (currentNode->type == XML_ELEMENT_NODE) {
			std::string elementName=std::string((char*)currentNode->name);
			//printf("EL: %s\n", elementName.c_str() );
			if( elementName == "POU") {
				std::string attrName= (char*) xmlGetProp(currentNode, (xmlChar*)"Name");
				parsePou( attrName, currentNode);
			} else if( elementName == "DUT") {
				parseDut( currentNode->children );
			} else if( elementName == "Itf") {
				parseItf( currentNode->children );
			}  else if( elementName == "GVL") {
				parseDut( currentNode->children );
			} 
		}
		parse(currentNode->children);
	}
	return true;
}

bool XmlFile::open()
{
	if( fexists(m_file)) {
		doc  = xmlReadFile(m_file.c_str(), NULL, 0);
		root = xmlDocGetRootElement(doc);
		if( root == NULL ) {
			return false;
		}
		return parse(root);
	}
	endwin();
	printf("Unable to parse xml\n");
	exit(0);
	return false;
}

void XmlFile::saveBody()
{
	std::string ou = getPrimaryName();
	//sync changes to xml doc
	std::string decl=m_objects[ou].declaration;
	std::string impl=m_objects[ou].implementation;

	if( m_objects.find(ou) == m_objects.end() ) {
		getApp().setStatus("Object not found?");
		return;
	}

	xmlNodeSetContent( m_objects[ou].declNode, (const xmlChar *)"" );
	xmlNodeSetContent( m_objects[ou].implNode, (const xmlChar *)"" );
	xmlNodePtr cdataDecl = xmlNewCDataBlock( doc, (xmlChar*)decl.c_str(), decl.size() );
	auto parent = m_objects[ou].declNode;
	xmlAddChild( parent, cdataDecl );

	xmlNodePtr ST = xmlNewNode(NULL, (xmlChar*) "ST");
	xmlNodePtr cdataImpl = xmlNewCDataBlock( doc, (xmlChar*)impl.c_str(), impl.size() );
	xmlAddChild( ST, cdataImpl);
	xmlAddChild( m_objects[ou].implNode, ST);
}

void XmlFile::saveMembers()
{
	std::string ou = getPrimaryName();
	for( auto [name, obj] : m_objects ) {
		if( name != ou ) {
			//printf("Store: %s node:%p\n", name.c_str(), obj.parent);
			auto decl=m_objects[name].declaration;
			auto impl=m_objects[name].implementation;

			xmlNodeSetContent( m_objects[name].declNode, (const xmlChar *)"" );
			xmlNodeSetContent( m_objects[name].implNode, (const xmlChar *)"" );
			xmlNodePtr cdataDecl = xmlNewCDataBlock( doc, (xmlChar*)decl.c_str(), decl.size() );
			xmlAddChild( m_objects[name].declNode, cdataDecl );

			xmlNodePtr ST        = xmlNewNode(NULL, (xmlChar*) "ST");
			xmlNodePtr cdataImpl = xmlNewCDataBlock( doc, (xmlChar*)impl.c_str(), impl.size() );
			xmlAddChild( ST, cdataImpl);
			xmlAddChild( m_objects[name].implNode, ST);
		}
	}
}

bool XmlFile::save()
{	
	saveBody();
	saveMembers();
	//save doc to disk
	if (xmlSaveFormatFile(m_file.c_str(), doc, 1) == -1) {
		getApp().setStatus("Error: write '"+ m_file+"' failed!");
		fprintf(stderr, "Failed to save modified XML file.\n");
		return false;
	}
	getApp().setStatus("wrote "+ m_file);
	return true;
}

bool XmlFile::saveToMem(std::string obj, std::string decl, std::string impl)
{
	if( (m_objects[obj].declaration != decl) || (m_objects[obj].implementation != impl) ) {
		getApp().setStatus("saved '"+obj+"' changes to mem");
		m_objects[obj].declaration = decl;
		m_objects[obj].implementation = impl;
	} else {
		getApp().setStatus("No changes of '"+obj+"'");
		usleep(200*1000);
	}
}

bool XmlFile::saveAs( std::string name )
{
	getApp().setStatus("save as "+name);
	return false;
}

static inline void vectorErase(StringVector& v, std::string str )
{
	auto itr = std::find(v.begin(), v.end(), str);
	if (itr != v.end()) v.erase(itr);
}

bool XmlFile::deleteObject( std::string name )
{
	if( name == getPrimaryName() ) {
		getApp().setStatus("Cannot delete primary object - delete file to remove");
		return false;
	}
	if( name == getApp().getViewMgr()->getCurrentView()->getViewName() ) {
		getApp().setStatus("Cannot delete object in view - switch view to delete object");
		return false;
	}

	if( m_objects.find(name) != m_objects.end() ) {
		xmlUnlinkNode( m_objects[name].parent );
		xmlFree( m_objects[name].parent );
		m_objects.erase(name);
		vectorErase( m_methodNames, name );
		getApp().setStatus("deleted object "+name);
		// erase object from xml
	} else {
		getApp().setStatus("object "+name+" not found - cannot delete");
		return false;
	}
	return true;
}

bool XmlFile::XmlFile::reload()
{
	getApp().setStatus("reloaded");
	return false;
}

bool XmlFile::createObject( std::string name, enum XmlFile::ObjectType type )
{
	if( std::find(m_methodNames.begin(),m_methodNames.end(),name) != m_methodNames.end() ) {
		getApp().setStatus("object '"+name+"' already exists");
		return false;
	}
	getApp().setStatus("created object "+name);
	m_methodNames.push_back(name);
	std::string decl = "METHOD "+name+getTemplate("method",false);
	std::string impl ="(*\n"+name+"\n*)";
	m_objects[name].declaration    = decl;
	m_objects[name].implementation = impl;

	// xml doc update
	xmlNodePtr pou = findElementByName( root, "POU");
	xmlNodePtr  method= xmlNewChild(pou, NULL, (xmlChar*)"Method", NULL);
	xmlNewProp(method, (xmlChar*)"Name", (xmlChar*) name.c_str() );
	xmlNewProp(method, (xmlChar*)"Id", (xmlChar*)  newUUID().c_str() );

	xmlNodePtr Declaration= xmlNewChild(method, NULL, (xmlChar*)"Declaration", NULL);
	xmlNodePtr DeclarationCdata = xmlNewCDataBlock( doc, (xmlChar*)decl.c_str(), decl.size() );
	xmlAddChild( Declaration, DeclarationCdata);

	xmlNodePtr Implementation= xmlNewChild(method, NULL, (xmlChar*)"Implementation", NULL);
	xmlNodeSetContent( Implementation, (xmlChar*)impl.c_str() );

	return true;
}

bool XmlFile::isModified()
{
	return false;
}

XmlFile::OuType XmlFile::getOuType( std::string name )
{
	std::string ext = toUpperCase( FileMgt::getExtension(name) );
	if( ext == ".TCPOU" ) {
		return XmlFile::OuType::POU;
	} else if( ext ==".TCDUT") {
		return XmlFile::OuType::DUT;
	} else if( ext ==".TCIO") {
		return XmlFile::OuType::ITF;
	}

	return XmlFile::OuType::NOT_POU;
}


/* vim: set foldmethod=syntax: */
