#include <stdio.h>
#include <libxml/tree.h>

int main() {
    // Create a new XML document
    xmlDocPtr doc = xmlNewDoc((xmlChar*) "1.0");
    if (doc == NULL) {
        fprintf(stderr, "Failed to create XML document.\n");
        return 1;
    }

    // Create the root element
    xmlNodePtr root = xmlNewNode(NULL, (xmlChar*) "root");
    xmlDocSetRootElement(doc, root);

    xmlNodePtr decl  = xmlNewNode(NULL, (xmlChar*) "Declaration");
    xmlNodePtr cdata = xmlNewCDataBlock(doc, (xmlChar*) "This is a CDATA block", 22);

    xmlAddChild(decl, cdata);

    // Add the declaration element to the root element
    xmlAddChild(root, decl);

    // Serialize the document to stdout
    xmlDocDump(stdout, doc);

    // Free the document
    xmlFreeDoc(doc);

    return 0;
}

