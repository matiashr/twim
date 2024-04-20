#ifndef ICONS_H
#define ICONS_H

#include <string>
typedef enum symbol {
	NONE,
	FOLDER_OPEN,
	FOLDER_CLOSED,
	FILE_NORMAL,
	FILE_CABINET,
	CARDS,
	HAPPY,
	FLOPPY,
	PLUG,
	PC,
	SAT,
	DNA,
	SEARCH,
	EXIT,
	ABOUT,
	LAST_ENTRY
}symbol_t;

void enablegraphics(bool s);
std::string getIcon( symbol_t t );


#endif
