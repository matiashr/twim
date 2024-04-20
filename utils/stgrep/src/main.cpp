/*
	ST "grep"

*/
#include <iostream>
#include <regex.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"

struct cliopt{
	std::string regexpr;
	std::string file;
	bool printPath;
	bool printMethod;
	bool printLine;
	bool printColors;
	bool recursive;
}g_options;



bool lineByLine(char* buffer, regex_t regex, char* method, const char*fullpath )
{
	const char *start = buffer;
	const char *end = buffer;
	size_t buffer_size = strlen(buffer);
	unsigned int no=1;
	while (end < buffer + buffer_size) {
		// Find the end of the current line
		while (*end != '\n' && end < buffer + buffer_size) 
			end++;
		// Calculate the length of the line
		size_t line_length = end - start;

		// Allocate memory for the line
		char *line = (char *)malloc(line_length + 1);
		if (line == NULL) {
			fprintf(stderr, "Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
		// Copy the line from buffer to line
		strncpy(line, start, line_length);
		line[line_length] = '\0'; // Null-terminate the line

		// Print or process the line as needed
		regmatch_t match;
		if (regexec(&regex, line, 1, &match, 0) == 0) { // Print the line if it matches
			if( g_options.printPath ) {
				printf("%s:\n", fullpath);
			}
			if( g_options.printMethod ) {
				printf("%18s ", method );
			}

			if( g_options.printLine ) {
				printf("%4d: ",no );
			}
			if( g_options.printColors ) {
				printf("%.*s" COLOR_RED "%.*s" COLOR_RESET "%s\n",
						(int)match.rm_so, line,
						(int)(match.rm_eo - match.rm_so), line + match.rm_so,
						line + match.rm_eo);
			} else {
				printf("%s\n", line);
			}
		}
		// Move to the next line
		start = end + 1;
		end = start;
		free(line);
		no++;
	}
}

int attributeExists(xmlNodePtr node, const char *attrName) {
    xmlAttrPtr attr = node->properties;
    while (attr != NULL) {
        if (attr->type == XML_ATTRIBUTE_NODE) {
            if (xmlStrcmp(attr->name, (const xmlChar *)attrName) == 0) {
                return 1; // Attribute found
            }
        }
        attr = attr->next;
    }
    return 0; // Attribute not found
}


void printElements(xmlNode *node, regex_t regex, char* objectAttrName, const char* fullpath ) 
{
	xmlNode *cur_node = NULL;
	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if( g_options.printMethod ) {
			if( attributeExists(cur_node, "Name")  ) {
				objectAttrName = (char*)xmlGetProp(cur_node, (xmlChar*)"Name");
			}
		}
		if (cur_node->type == XML_ELEMENT_NODE) {
			std::string elementName=std::string((char*)cur_node->name);
			if( elementName == "Declaration" || elementName == "Implementation" ) {
				lineByLine( (char*)xmlNodeGetContent(cur_node), regex, objectAttrName, fullpath );
			}
		}
		printElements(cur_node->children, regex, objectAttrName, fullpath);
	}
}

int grepInFile( std::string file, regex_t regex )
{
	xmlDocPtr doc;
	xmlNodePtr root;
	doc = xmlReadFile(file.c_str(), NULL, 0);
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

	char* current=NULL;
	printElements(root, regex,current, file.c_str() );
	// Free the document
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 0;
}


void forEachFile( const char* path, regex_t regex )
{
	DIR *dir;
	struct dirent *entry;
	struct stat statbuf;

	if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	while ((entry = readdir(dir)) != NULL) {
		char fullpath[1024]; // Adjust the buffer size as needed
		// Skip "." and ".." directories
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
		if (stat(fullpath, &statbuf) == -1) {
			perror("stat");
			exit(EXIT_FAILURE);
		}

		if (S_ISDIR(statbuf.st_mode)) {
			forEachFile(fullpath, regex );
		} else if (S_ISREG(statbuf.st_mode)) {
			// Check if the file has a .Tc* extension
			const char* pt = strrchr(entry->d_name, '.');
			if( pt != NULL) {
				std::string extension = pt;
				if (extension != "" && extension.find(".Tc") != std::string::npos ) {				
					std::string file = fullpath;
					grepInFile(file, regex);
				}
			}
		}
	}
	closedir(dir);
}

int main(int argc, char*argv[]) 
{
	regex_t regex;

	g_options.regexpr = "";	
	g_options.file    = "";	
	g_options.printPath   = false;
	g_options.printMethod = false;
	g_options.printLine   = false;
	g_options.recursive   = false;

	for( int i=1;i < argc; i++ ) {
		if( !strcmp(argv[i],"-n") ) {
			g_options.printLine = true;
		} else if( !strcmp(argv[i],"-m") ) {
			g_options.printMethod = true;
		} else if( !strcmp(argv[i],"-f") ) {
			if( i+1 <= argc ) {
				g_options.file = argv[i+1];
			} 
		} else if( !strcmp(argv[i],"-e") ) {
			if( i+1 <= argc ) {
				g_options.regexpr= argv[i+1];
			} else{
				printf("Missing expression\n");
			}
		} else if( !strcmp(argv[i],"-r") ) {
			g_options.recursive=true;
			if( i+1 <= argc ) {
				g_options.file = argv[i+1];
			} else{
				printf("Missing directory\n");
			}
		} else if( !strcmp(argv[i],"-p") ) {
			g_options.printPath = true;
		} else if( !strcmp(argv[i],"-c") ) {
			g_options.printColors = true;
		} else {
		//	printf("Unexpected argument '%s'\n",argv[i]);
		//	exit(1);
		}

	}
	if( g_options.regexpr == "" || (g_options.file == "" && !g_options.recursive ) ) {
		printf("Missing argument(s)\n");
		printf( "Usage:\n"\
			" $ stgrep <args>\n"\
			"Args:\n"\
			" -f <filename>\n"\
			" -r <directory>\n"\
			" -e <expression>\n"\
			" -p ; print filename \n"\
			" -m ; print method \n"\
			" -c ; print using colors \n"\
			" -n ; print line within method\n");
		exit(1);
	}

	// Load XML file
	//if (regcomp(&regex, g_options.regexpr.c_str(), REG_EXTENDED | REG_NOSUB) != 0) {
	if (regcomp(&regex, g_options.regexpr.c_str(),  REG_EXTENDED|REG_ICASE) != 0) {
		fprintf(stderr, "Invalid regular expression: %s\n", g_options.regexpr.c_str() );
		return EXIT_FAILURE;
	}

	if( g_options.recursive ) {
		forEachFile( g_options.file.c_str(), regex  );
	} else {
		grepInFile(g_options.file, regex);
	}
	return 0;
}



