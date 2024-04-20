#ifndef TEMPLATES_H
#define TEMPLATES_H
#include <map>
#include "strfun.h"


void initTemplates();
StringVector& getTemplates( bool statement=false );
std::string getTemplate(std::string n, bool statement=false);

bool createNewPou(std::string name);

#endif
