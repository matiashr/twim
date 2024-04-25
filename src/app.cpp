#include <curses.h>
#include <unistd.h>
#include <vector>
#include <dlfcn.h>
#include <locale.h>
#include "ScintillaCurses.h"
#include "Scintilla.h"
#include "SciLexer.h"
#include "Lexilla.h"
#include "teditor.h"
#include "app.h"
#include "file.h"
#include "config.h"

App::App():
	m_quit(false),
	ci(0),
	m_buffers{}
{

	setup();
}

App::~App()
{
	endwin();
}

void App::quit()
{
	m_quit=true;
}

void App::setStatusWindow( SplittedView* fn  ) 
{
	TEditor* decl = fn->getView(DECLARATION);
	TEditor* impl = fn->getView(IMPLEMENTATION);
	// create status line
	int rows, cols;
	getTerminalSize(rows, cols );		//default size for win is fullscreen

	statusWin = newwin(1, cols, rows-1, STATUS_WIN_X_OFFS);
	scrollok(statusWin, FALSE); // Disable scrolling for the status window
	decl->setStatusLine( statusWin);
	impl->setStatusLine( statusWin);
	std::string statusMsg = "Status: Ready";
	mvwprintw(statusWin, 0, STATUS_WIN_X_OFFS, "%s", statusMsg.c_str());
	wrefresh(statusWin);
}

void App::setStatus(std::string msg )
{
	werase(statusWin);
	mvwprintw(statusWin, 0, STATUS_WIN_X_OFFS, "%s",msg.c_str() );
	wrefresh(statusWin);
}

void App::showDialog(std::string msg )
{
	// Calculate window dimensions
	int winHeight = 50;
	int winWidth = 40;
	int winY = (LINES - winHeight) / 2;
	int winX = (COLS - winWidth) / 2;

	// Create a window for the text input dialog
	WINDOW* inputWin = newwin(winHeight, winWidth, winY, winX);
	keypad(inputWin, TRUE);
	mvwprintw(inputWin, 1, 1, "%s", msg.c_str());
	wrefresh(inputWin);
	box(inputWin, 0, 0);

	int ch;
	while ((ch= wgetch(stdscr)) != 'q') 
		;
	
	getViewMgr()->refreshScreen();
	// Clean up ncurses
	delwin(inputWin);
	endwin();
}

void App::getTerminalSize(int&rows, int&cols)
{
	getmaxyx(stdscr, rows, cols);
}

// create new buffer
SplittedView* App::New(std::string name)
{
	XmlFile::OuType ouType = XmlFile::getOuType( name );
	if( ouType == XmlFile::OuType::NOT_POU ) {
		endwin();
		printf("This is not a valid PLC ou :%s\n", toUpperCase( FileMgt::getExtension(name) ).c_str() );
		exit(1);
	}
	struct SplittedView* fn = new SplittedView;
	fn->file = name;
	fn->setEditor(view_e::DECLARATION, new TEditor(this, fn, true) );
	fn->setEditor(view_e::IMPLEMENTATION, new TEditor(this, fn, false) );

	int rows, cols;
	TEditor* decl = fn->getView( view_e::DECLARATION);
	TEditor* impl = fn->getView( view_e::IMPLEMENTATION);
	getTerminalSize(rows, cols );		//default size for win is fullscreen
	
	decl->resizeWindow( (rows/10 ), cols);
	decl->moveWindow(0,0);
	decl->setBoxed(false);
	//hline(0, rows-(rows/10)+1 );
	impl->resizeWindow( rows-(rows/10)-2, cols);
	impl->moveWindow(0, (rows/10)+1);
	impl->setBoxed(false);

	setStatusWindow( fn );
	m_buffers.push_back(fn);
	impl->Refresh();
	decl->Refresh();
	current = fn;
	return fn;
}

void App::run()
{
	current->open();
	while( !m_quit )
	{
		current->run();
	}
}

void App::next() 
{
	if( ci >= m_buffers.size()-1) {
		ci = m_buffers.size()-1;
	} else {
		ci++;
	}
	current = m_buffers.at(ci);
	current->activate();
}

void App::prev() 
{
	if( ci <= 0) {
		ci = 0;
	} else {
		ci--;
	}
	current = m_buffers.at(ci);
	current->activate();
}

void App::setup()
{
	printf("\033[?1000h"); // enable mouse press and release events
			       // printf("\033[?1002h"); // enable mouse press, drag, and release events
			       // printf("\033[?1003h"); // enable mouse move, press, drag, and release events
	mousemask(ALL_MOUSE_EVENTS, NULL);
	mouseinterval(0);
	setlocale(LC_CTYPE, ""); // for displaying UTF-8 characters properly
	initscr(), raw(), cbreak(), noecho(), start_color();
}

bool App::fileExists(std::string f)
{
	return FileMgt::fileExists(f);
}
/* vim: set foldmethod=syntax: */

