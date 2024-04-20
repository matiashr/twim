/*
	convert TC to ST

*/
#include <string>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <algorithm>


void printDut( xmlNode* node )
{
	for ( xmlNode* cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			std::string elementName=std::string((char*)cur_node->name);
			if( elementName == "Declaration"  ) {
				std::cout << xmlNodeGetContent(cur_node) << std::endl;
			} 
		}
	}
}

/*
	FB or program
*/
void printPou( xmlNode* node )
{
	bool fb;
	xmlNode* declaration;
	xmlNode* implementation;
	for ( xmlNode* cur_node = node->children; cur_node; cur_node = cur_node->next) {
		std::string elementName=std::string((char*)cur_node->name);
		if( elementName == "Declaration" ) {
			declaration = cur_node;
		} else if( elementName == "Implementation") {
			implementation = cur_node;
		}
	}
/*
 			std::string elementName=std::string((char*)cur_node->name);
			if( elementName == "Declaration" || elementName == "Implementation" ) {
				std::string data = (char*)xmlNodeGetContent(cur_node) ;
				if( elementName == "Declaration") {
					std::string up = data;
					transform(up.begin(), up.end(), up.begin(), ::toupper); 
					if( up.find("PROGRAM") != std::string::npos ) {
						fb=false;
					} else {
						fb=true;
					}
				}
				std::cout << data << std::endl;
			}
*/
	std::string decl = (char*)xmlNodeGetContent(declaration) ;
	std::string impl = (char*)xmlNodeGetContent(implementation) ;

	std::cout << decl << std::endl;

	for ( xmlNode* cur_node = node->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			std::string elementName=std::string((char*)cur_node->name);
			if( elementName == "Method") {
				std::cout << xmlNodeGetContent(cur_node) << std::endl;
			}
		}
	}
	std::cout << impl << std::endl;
	std::string up = decl;
		transform(up.begin(), up.end(), up.begin(), ::toupper); 
	if( up.find("PROGRAM") != std::string::npos ) {
		fb=false;
	} else {
		fb=true;
	}

	if( fb ) {
		std::cout << "END_FUNCTION_BLOCK" << std::endl;
	} else {
		std::cout << "END_PROGRAM" << std::endl;
	}
}

void printItf( xmlNode* node )
{
	for ( xmlNode* cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			std::string elementName=std::string((char*)cur_node->name);
			if( elementName == "Declaration" || elementName == "Implementation" ) {
				std::cout << xmlNodeGetContent(cur_node) << std::endl;
			} else if( elementName == "Method") {
				std::cout << xmlNodeGetContent(cur_node) << std::endl;
			}
		}
	}
	std::cout << "END_INTERFACE" << std::endl;
}

void printElements(xmlNode *node) 
{
	xmlNode *cur_node = NULL;
	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			std::string elementName=std::string((char*)cur_node->name);
			if( elementName == "POU") {
				printPou( cur_node);
			} else if( elementName == "DUT") {
				printDut( cur_node->children );
			} else if( elementName == "Itf") {
				printItf( cur_node->children );
			}  else if( elementName == "GVL") {
				printDut( cur_node->children );
			} 
		}
		printElements(cur_node->children);
	}
}

int main(int argc, char*argv[]) 
{
	xmlDocPtr doc;
	xmlNodePtr root;
	if( argc < 2 ) {
		printf("No file given\n");
		exit(1);
	}
	// Load XML file
	doc = xmlReadFile(argv[1], NULL, 0);
	if (doc == NULL) {
		std::cerr << "Error: Failed to parse document." << std::endl;
		return 1;
	}
	root = xmlDocGetRootElement(doc);
	if (root == NULL) {
		std::cerr << "Error: Document is empty." << std::endl;
		xmlFreeDoc(doc);
		return 1;
	}
	// Print all elements
	printElements(root);
	// Free the document
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 0;
}

