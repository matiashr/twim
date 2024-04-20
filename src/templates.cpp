#include "xmlfile.h"
#include "templates.h"


static std::string fbtemplate=\
"<TcPlcObject Version=\"1.1.0.1\" ProductVersion=\"3.1.4022.18\">\n"\
"<POU Name=\"FB_NAME\" Id=\"UUID\" SpecialFunc=\"None\">"\
"<Declaration>\n"\
"<![CDATA[\n"\
"FUNCTION_BLOCK FB_NAME\n"\
"VAR_INPUT\n"\
"END_VAR\n"\
"VAR_OUTPUT\n"\
"END_VAR\n"\
"VAR\n"\
"END_VAR\n"\
"]]>\n"\
"</Declaration>\n"\
"    <Implementation>\n"\
"	<ST>\n"\
"		<![CDATA[(* New fb *)]]>\n"\
"	</ST>\n"\
"	</Implementation>\n"\
"</POU>\n"\
"</TcPlcObject>\n";


static StringVector g_statementNames;
static StringVector g_declarationNames;

static std::map<std::string, std::string>  g_declarations;
static std::map<std::string, std::string>  g_statements;

typedef struct list {
	std::string name;
	std::string templ;
	bool statement;		//statement or declaration
}list_t;

static list_t declarationList[] = {
	{"declare input",  " in  AT %I* : BOOL;", false },
	{"declare output", " out AT %Q* : BOOL;", false },
	{"declare OPC call attribute rpc", 
		"{attribute 'TcRpcEnable' := '1'}",
		false
	},
	{"declare OPC RW attribute", 
		"{attribute 'OPC.UA.DA.Access' := '1'}\n"\
		"{attribute 'OPC.UA.DA' := '1'}\n",
		false
	},
	{"method",
		" : BOOL\n"\
		"END_METHOD\n"\
		"VAR_INPUT\n"\
		"END_VAR\n"\
		"VAR_OUTPUT\n"\
		"END_VAR\n",
		false
	}
};

static list_t statementList[] = {
	{"if", 		   
		"IF X THEN\n"\
		"END_IF\n",
		true
	},
	{"for", 	   
		"FOR i:=0 TO X BY 1\n"\
		"END_FOR",
		true
	},
	{"do", 
		"DO X\n"\
		"WHILE Y;\n",
		true
	},
	{"state machine", 
		"CASE State OF\n"\
		"END_CASE\n",
		true
	},
	{"loop array", 
		"FOR i:=0 TO SIZEOF(arr)/SIZEOF(arr[0]) BY 1\n"\
		"END_FOR",
		true
	}

};

StringVector& getTemplates(bool statement)
{
	if( statement ) {
		return g_statementNames;
	}
	return g_declarationNames;
}

std::string getTemplate(std::string n, bool statement)
{
	if( statement ) {
		return g_statements[n];
	}
	return g_declarations[n];
}

void initTemplates()
{
	for( auto t: statementList ) {
		g_statementNames.push_back(t.name);	
		g_statements[t.name] = t.templ;
	}
	for( auto t: declarationList) {
		g_declarationNames.push_back(t.name);	
		g_declarations[t.name] = t.templ;
	}

}

static std::string replace( std::string str, std::string old_substr, std::string new_substr )
{
	// Find the first occurrence of the substring to be replaced
	size_t pos = str.find(old_substr);

	// Replace all occurrences of the substring
	while (pos != std::string::npos) {
		// Replace the substring with the new one
		str.replace(pos, old_substr.length(), new_substr);

		// Find the next occurrence of the substring
		pos = str.find(old_substr, pos + new_substr.length());
	}
	return str;
}

static std::string file2entity( std::string filename )
{
	size_t dotPos = filename.find_last_of('.');
	if (dotPos != std::string::npos) {
		return filename.substr(0, dotPos);
	}
	return filename; 
}

bool createNewPou(std::string name)
{
	FILE* fd = fopen(name.c_str(), "w");
	if( fd == NULL ) {
		return false;
	}
	std::string data = replace(fbtemplate, "FB_NAME", file2entity(name));
	data = replace(data, "UUID", newUUID() );
	fprintf(fd,"%s\n",data.c_str() );
	fclose(fd);
	return true;
}


/* vim: set foldmethod=syntax: */
