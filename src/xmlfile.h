#ifndef TC_XML_FILE_PARSER_H
#define TC_XML_FILE_PARSER_H
/*
	This class "unfolds" a .TcPOU/.TcXXX file 
	it's contents are stored in a hashmap, with a pointer to the actual
	node in the xml doc.
	notes:
		saveToMem() store will changes to the xml tree, not disk
		save() store changes to disk

*/
#include <string>
#include <map>
#include <vector>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <uuid/uuid.h>
#include "fileif.h"
#include "strfun.h"

struct Object {
	std::string name;
	std::string declaration;
	std::string implementation;
	xmlNode* parent;
	xmlNode* declNode;
	xmlNode* implNode;
};


class XmlFile
{
	public:
		XmlFile( std::string f );
		virtual ~XmlFile();
		bool open();

		typedef enum OuType{NOT_POU, POU, ITF, DUT};
		OuType m_fileType;
		std::string getDeclaraion();
		std::string getImplementation();
		bool getMethodObject( std::string name, std::string* r_decl, std::string* r_impl );
		StringVector* getMethodList() { return &m_methodNames; };
		std::string getPrimaryName();		

		static OuType getOuType( std::string name );
	public:
		bool open( std::string name );
		bool save();
		bool saveAs( std::string name );
		bool reload();					//reload file
		bool isModified();
		bool saveToMem(std::string obj, std::string decl, std::string impl);		//just save in xmltree, but not synced to disk
	public:
		enum ObjectType { METHOD };
		bool deleteObject( std::string name );		//delete method
		bool createObject( std::string name, enum ObjectType t );		//create new method/xx
	private:
		bool parse(xmlNode* node );
		bool parseDut(xmlNode* node);
		bool parsePou(std::string name, xmlNode* node);
		bool parseItf(xmlNode* node);
	private:
		void saveBody();
		void saveMembers();
	private:
		std::string m_file;
		xmlDocPtr doc;
		xmlNodePtr root;
		std::string m_primary;
		std::map<std::string, Object> m_objects;
		StringVector m_methodNames;
};

std::string newUUID();


#endif
