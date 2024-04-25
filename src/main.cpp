#include <curses.h>
#include <vector>
#include <dlfcn.h>
#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include "ScintillaCurses.h"
#include "Scintilla.h"
#include "SciLexer.h"
#include "Lexilla.h"
#include "teditor.h"
#include "templates.h"
#include "app.h"
#include "file.h"

// app is singleton
static App a;

App& getApp()
{
	return a;
}


void usignal( int no )
{
	a.setStatus("use :q to quit");
	usleep(1000*500);
	a.setStatus("");
}


int main(int argc,char**argv )
{
	initTemplates();
	if( argc == 2 ) {
		if( !a.fileExists(argv[1] )) {
			if( !createNewPou(argv[1]) ) {
				printf("Failed to create new POU\n");
				exit(1);
			}
		}
		a.setExecutablePath(argv[0]);
		a.New( argv[1] );
		signal(SIGINT, usignal );
		a.run();
	} else {
		printf("specify filename\n");
	}
}

/* vim: set foldmethod=syntax: */
