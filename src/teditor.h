#ifndef TVEDITOR_H
#define TVEDITOR_H
#include "ScintillaCurses.h"
#include "strfun.h"
#include "history.h"

class App;
class SplittedView;
class TEditor 
{
	public:
		TEditor( App* app, SplittedView* file, bool isTop=false);
		virtual ~TEditor();
		void handleNormal( int c );
		void handleInsert( int c );
		void handleResize( int c );
		void insert( const char*data);		//inserts text to current
		void setBuffer( const char*data, std::string name);	//replace all test with new
		void Refresh();
		void run();
		void activate();
		void suspend();
		void open();
		void setStatus(std::string msg );
		std::string inputCommand(std::string msg );
		void inputSearch(std::string msg );
		bool inputDialog(std::string title, std::string& r_str);
		void execCommand(std::string cmd);
		void getWindowSize( int&x ,int &y );
		void getWindowPos( int&x ,int &y );
		void moveWindow( int x, int y );
		void resizeWindow(int x, int y );
		void setStatusLine( WINDOW* w ) { statusWin = w;};
		void setBoxed(bool b) { m_boxed =b;};
		std::string getBuffer();
	public:
		void setViewName( std::string n );
		std::string getViewName() { return m_viewName; };
		std::string getName();
	private:
		size_t lineLength(int line);
		int getCurrentLine();
	private:
		void Initialise();
		static void scinotification(void *view, int msg, SCNotification *n, void *userdata) 
		{
			// printw("SCNotification received: %i", n->nmhdr.code);
		}

		bool popUp(std::string title, StringVector& options, std::string& r_selection );
		void ClearStyle();
		void StyleSetForeground(int style, const long color );
		void StyleSetBackground(int style, const long color );
		void StyleSetCaret( uptr_t c);
		void SetLexer( std::string s);
		void StyleEnableFolding();
		void SetKeyWordsList();
		bool inputWindow( std::string p, WINDOW* w, std::string& r_str);
	private:
		History m_hist;
		App* m_app;
		SplittedView* m_xfile;
		bool visual;
		bool yank;
		bool quit;
		std::string keywords;
		WINDOW* statusWin;
		Scintilla::Internal::ScintillaCurses sci {scinotification, NULL};
		enum {COMMAND, INSERT, SUSPEND, RESIZE }mode;
		bool m_boxed;
		std::string m_viewName;
};

#endif
