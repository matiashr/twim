/*
   WndProc() defined at
   ../scintilla/src/Editor.cxx

 */
#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

#include "Scintilla.h"
#include "SciLexer.h"
#include "teditor.h"
#include "exec.h"
#include "app.h"
#include "templates.h"
#include "file.h"

#define ASCII_ESC 27

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define wxColour(b,g,r) r | (g << 8) | (b << 16)
/*********************************************************/
// default color scheme
/*********************************************************/
#define DEFAULT_BG 	wxColour(0x00, 0x00, 0x00) 	//dark brown
#define DEFAULT_FG 	wxColour(0xff, 0xff, 0xff)	//white
#define COL_COMMENT	wxColour(0x00, 0xff, 0x00)
#define COL_STRING	wxColour(0xff, 0x00, 0xff)
#define COL_NUMBER	wxColour(0xff, 0xff, 0x00)
#define COL_LINE_NUMBER	wxColour(0x00, 0x00, 0x00)
#define COL_OPERATOR	wxColour(0xff, 0x00, 0xff)
#define COL_IDENTIFIER	wxColour(0xff, 0xff, 0xff)
#define COL_KEYWORDS	wxColour(0x00, 0xff, 0x00)


TEditor::TEditor( App* app, SplittedView* xf):
	m_app(app), 
	m_xfile(xf),
	visual(false),
	yank(false),
	quit(false),
	mode(COMMAND)
{
	sci.Initialise();
	Initialise();
	sci.UpdateCursor();
	sci.NoutRefresh();
	int rows, cols;
	getmaxyx(sci.GetWINDOW(), rows, cols);
	wresize(sci.GetWINDOW(), rows-1, cols);
	wrefresh(sci.GetWINDOW() );
}

TEditor::~TEditor() 
{
}

// x = row, y =col
void TEditor::moveWindow( int x, int y )
{
	mvwin(sci.GetWINDOW(), x, y ); 
}

void TEditor::resizeWindow(int x, int y )
{
	wresize(sci.GetWINDOW(), x, y );
}

void TEditor::getWindowSize( int&x ,int &y )
{
	getmaxyx(sci.GetWINDOW(),x,y);
}

void TEditor::getWindowPos( int&x ,int &y )
{
	getbegyx(sci.GetWINDOW(),x,y);
}

std::string TEditor::getName() { return m_xfile->file;};

void TEditor::Initialise()
{
	// clear default colors
	for(int i=0;i <STYLE_MAX; i++ ) {
		StyleSetBackground(i, DEFAULT_BG);     	
	}

	StyleSetForeground(SCE_STTXT_DEFAULT,     DEFAULT_FG);
	StyleSetBackground(SCE_STTXT_DEFAULT,     DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_NUMBER,      COL_NUMBER );
	StyleSetBackground(SCE_STTXT_NUMBER,      DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_COMMENTLINE, COL_COMMENT);
	StyleSetBackground(SCE_STTXT_COMMENTLINE, DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_COMMENT,     COL_COMMENT);
	StyleSetBackground(SCE_STTXT_COMMENT,     DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_STRING1,     COL_STRING );
	StyleSetBackground(SCE_STTXT_STRING1,     DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_STRING2,     COL_STRING );
	StyleSetBackground(SCE_STTXT_STRING2,     DEFAULT_BG);

	StyleSetForeground(SCE_STTXT_KEYWORD,     COL_KEYWORDS);
	StyleSetBackground(SCE_STTXT_KEYWORD, 	  DEFAULT_BG);

	StyleSetForeground(SCE_STTXT_IDENTIFIER,  COL_IDENTIFIER);
	StyleSetBackground(SCE_STTXT_IDENTIFIER,  DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_PRAGMA,  	  COL_IDENTIFIER);
	StyleSetBackground(SCE_STTXT_PRAGMA, 	  DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_PRAGMAS,  	  COL_IDENTIFIER);
	StyleSetBackground(SCE_STTXT_PRAGMAS, 	  DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_HEXNUMBER,   COL_NUMBER);
	StyleSetBackground(SCE_STTXT_HEXNUMBER,   DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_DATETIME,    COL_NUMBER);
	StyleSetBackground(SCE_STTXT_DATETIME,    DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_FUNCTION, 	  COL_NUMBER);
	StyleSetBackground(SCE_STTXT_FUNCTION, 	  DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_CHARACTER,   COL_IDENTIFIER);
	StyleSetBackground(SCE_STTXT_CHARACTER,   DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_TYPE, 	  COL_IDENTIFIER);
	StyleSetBackground(SCE_STTXT_TYPE, 	  DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_VARS, 	  COL_NUMBER);
	StyleSetBackground(SCE_STTXT_VARS,	  DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_OPERATOR,    COL_OPERATOR);
	StyleSetBackground(SCE_STTXT_OPERATOR,    DEFAULT_BG);
	StyleSetForeground(SCE_STTXT_FB, 	  COL_NUMBER);
	StyleSetBackground(SCE_STTXT_FB,	  DEFAULT_BG);


	// line no column
	StyleSetForeground(STYLE_LINENUMBER, 0);
	StyleSetBackground(STYLE_LINENUMBER, wxColour(60,50,59) );

	// caret style
	StyleSetCaret(CARETSTYLE_OVERSTRIKE_BLOCK);
	sci.SendMsg(SCI_SETCARETFORE, 0xff00ff, 0);

	sci.WndProc( Scintilla::Message::SetMarginWidthN, 0, 2);
	sci.WndProc( Scintilla::Message::SetMarginWidthN, 2, 1);
	sci.WndProc( Scintilla::Message::SetMarginMaskN,  2, SC_MASK_FOLDERS);
	sci.WndProc( Scintilla::Message::SetMarginSensitiveN , 2, 1);
	sci.WndProc( Scintilla::Message::SetAutomaticFold, SC_AUTOMATICFOLD_CLICK, 0);
	sci.WndProc( Scintilla::Message::SetFocus,1, 0);

	sci.WndProc( Scintilla::Message::SetCodePage, SC_CP_UTF8, 0);
	StyleEnableFolding();


	//SetLexer("fcST"); //Should be this??
	SetLexer("cpp");
	sci.SendMsg(SCI_STYLESETBOLD, SCE_STTXT_KEYWORD, 1);		// bold keywords
}

inline bool fexists(const std::string& name) 
{
	try {
		std::ifstream f(name.c_str());
		return f.good();
	}catch( ... ) {
	}

	return false;
}

void TEditor::open()
{
	bool exists=fexists(m_xfile->file);

	if( !exists )  {
		//create a default template
#if 0
		insert("FUNCTION_BLOCK b\n"\
				"{prag}\n"\
				"(* comment\n*)\n"\
				"\tMETHOD test\n"
				"\t    // Start up the gnome\n"
				"\t    call(\'stest\', \'1.0\', argc, argv);\n"\			
				"\tEND_METHOD\n"\
				" a:=1+2;\n"\
				"END_FUNCTION_BLOCK\n"\
				"PROGRAM Prg\n"\
				" foo:=T#1s;\n"\
				"END_PROGRAM"\
		      );
#endif
	} else {
		FILE* fd = fopen(m_xfile->file.c_str(), "r");
		if(!fd ) {
			endwin();
			printf("No such file '%s'\n",m_xfile->file.c_str());
			exit(1);
		}
		std::ifstream t;
		t.open(m_xfile->file);
		std::string buffer;
		std::string line;
		while(t){
			std::getline(t, line);
			buffer+=line +"\n";
		}
		t.close();
		insert( buffer.c_str() );
	}
}

void TEditor::ClearStyle()
{
	sci.WndProc( Scintilla::Message::StyleClearAll, 0, 0);
}
void TEditor::StyleEnableFolding()
{
	sci.WndProc( Scintilla::Message::SetProperty, (uptr_t) "fold", (sptr_t) "1");
	sci.WndProc( Scintilla::Message::SetProperty, (uptr_t) "fold.comment", (sptr_t) "1");
	sci.WndProc( Scintilla::Message::SetProperty, (uptr_t) "fold.compact", (sptr_t) "1");
}

void TEditor:: StyleSetCaret( uptr_t car )
{
	sci.WndProc( Scintilla::Message::SetCaretStyle,  car, 0);
}

void TEditor::SetKeyWordsList()
{
	keywords=  "RETURN FOR WHILE BREAK CONTINUE IF THIS THIS^ METHOD END_METHOD END_PROGRAM END_FUNCTION_BLOCK "\
		    "CASE OF UINT_TO_BOOL BOOL_TO_INT BOOL_TO_UINT BOOL_TO_DINT DINT_TO_INT "\
		    "LINT_TO_STRING LTIME_TO_STRING LWORD_TO_HEXSTRING "\
		    "SIZEOF TRUE FALSE RETURN EXIT TO BY "\
		    "DINT_TO_STRING INT_TO_STRING INT_TO_BOOL BOOL_TO_INT "\
		    "END_CASE END_FOR END_WHILE END_IF DO AND OR ADR THEN ELSE ELSIF <> TRUE FALSE NOT ";
	///	keywords+=keywords.lower();
	keywords +="INT UDINT BYTE WORD INT ARRAY ARRAY POINTER T_MaxString PUBLIC AT %Q* %I* "\
		    "VAR_GLOBAL PROGRAM INTERFACE STRUCT VOID STRING "\
		    "TYPE END_TYPE INT UINT UDINT DINT FLOAT USINT ULINT DATE LWORD SINT TIME WSTRING " \
		    "VOID CHAR BYTE VAR END_VAR NOT TODO LREAL USINT OF TIME_OF_DAY VAR_GLOBAL "\
		    "IMPLEMENTS EXTENDS FUNCTION_BLOCK "\
		    "METHOD VAR_INPUT VAR_OUTPUT VAR_TEMP END_VAR VAR_IN_OUT "\
		    "TRUE FALSE BOOL LWORD TO REFERENCE "\
		    "FUNCTION "\
		    "CONSTANT END_STRUCT UNION END_UNION";
	sci.WndProc( Scintilla::Message::SetKeyWords, 1, (sptr_t)keywords.c_str() );
}

void TEditor::SetLexer( std::string l )
{
	sci.WndProc(  Scintilla::Message::SetILexer, 0, (sptr_t)sci.lexer(l.c_str()) );
	SetKeyWordsList();
}

void TEditor::StyleSetForeground(int style, const long color )
{
	sci.SendMsg(SCI_STYLESETFORE, style, color);
}

void TEditor::StyleSetBackground(int style, const long color )
{
	sci.SendMsg(SCI_STYLESETBACK, style, color);
}

void TEditor::activate()
{
	setStatus("Buffer:"+getName());
	if( mode == SUSPEND ) {
		sci.WndProc( Scintilla::Message::SetFocus,1, 0);
		mode = COMMAND;
	}
	Refresh();
}

void TEditor::setStatus(std::string msg )
{
	werase(statusWin);
	mvwprintw(statusWin, 0, 0, "%s",msg.c_str() );
	wrefresh(statusWin);
}

// input from window, use prompt
bool TEditor::inputWindow(std::string prompt,  WINDOW* win, std::string& m)
{
	int ch;
	keypad(win, TRUE);
	while ((ch = wgetch(win)) != '\n') {
		if (ch == KEY_BACKSPACE || ch == KEY_DC || ch == 127) {
			if( !m.empty() ) {
				m.pop_back();
			}
		} else if( ch == KEY_UP ) {
			if( m_hist.size() > 0 ) {
				m = m_hist.prev();
			} else {
				m = "";	
			}
		} else if( ch == KEY_DOWN) {
			if( m_hist.size() > 0 ) {
				m = m_hist.next();
			} else {
				m = "";	
			}
		} else if( ch == ASCII_ESC ) {
			setStatus("Abort new");
		} else {
			m.push_back( (char)ch);
		}
		werase(win);
		mvwprintw(win, 0, 0, "%s%s",prompt.c_str(), m.c_str() );
		wrefresh(win);
		m_app->getViewMgr()->refreshScreen();
	}
	m_hist.add( m );
	return true;
}

std::string TEditor::inputCommand(std::string msg )
{
	werase(statusWin);
	mvwprintw(statusWin, 0, 0, "%s",msg.c_str() );
	wrefresh(statusWin);
	std::string rv;
	if( inputWindow(":",statusWin, rv) ){
		return rv;
	}
	return "";
}

void TEditor::inputSearch(std::string msg )
{
	std::string m;
	werase(statusWin);
	mvwprintw(statusWin, 0, 0, "%s",msg.c_str() );
	wrefresh(statusWin);
	int ch;
	while ((ch = wgetch(statusWin)) != '\n') {
		if( ch == KEY_BACKSPACE ) {
			if( !m.empty() ) {
				m.pop_back();
			}
		} else {
			m.push_back( (char)ch);
			setStatus("search :"+m);
		}
	}
}

bool TEditor::inputDialog(std::string title, std::string& r_str)
{
	// Calculate window dimensions
	int winHeight = 6;
	int winWidth = 40;
	int winY = (LINES - winHeight) / 4;
	int winX = winY; //(COLS - winWidth) / 2;


	// Create a window for the text input dialog
	WINDOW* inputWin = newwin(winHeight, winWidth, winY, winX);
	keypad(inputWin, TRUE);
	mvwprintw(inputWin, 1, 1, "%s ", title.c_str());
	box(inputWin, 0, 0);
	wrefresh(inputWin);

	int ch;
	std::string m;
	while ((ch= wgetch(inputWin)) != '\n') {
		if (ch == KEY_BACKSPACE || ch == KEY_DC || ch == 127) {
			if( !m.empty() ) {
				m.pop_back();
			}
		} else if( ch == KEY_UP ) {
		} else if( ch == KEY_DOWN) {
		} else if( ch == KEY_LEFT ) {
		} else if( ch == KEY_RIGHT) {
		} else if( ch == ASCII_ESC ) {
			goto leave;
		} else {
			m.push_back( (char)ch);
		}
		werase(inputWin);
		box(inputWin, 0, 0);
		mvwprintw(inputWin, 1,1, "%s%s",title.c_str(), m.c_str() );
		wrefresh(inputWin);
	}
	r_str = m;
leave:
	m_app->getViewMgr()->refreshScreen();
	// Clean up ncurses
	delwin(inputWin);
	endwin();
	if( r_str == "" ) {
		return false;
	}
	// Return user input as a string
	return true;
}

bool TEditor::popUp(std::string title, StringVector& choices, std::string& r_selection )
{
	int winHeight = LINES/4;
	int winWidth = (COLS) / 4;
	int winY = (int)(LINES*0.2);
	int winX = COLS / 4;
	bool status=false;
	// Create items for the menu
	ITEM **my_items = new ITEM *[choices.size() + 1];
	for (size_t i = 0; i < choices.size(); ++i) {
		my_items[i] = new_item(choices[i].c_str(), nullptr);
	}
	my_items[choices.size()] = nullptr;
	// Create menu
	MENU *my_menu = new_menu(my_items);
	// Create window to contain the menu
	WINDOW* my_menu_win = newwin(winHeight, winWidth, winY, winX);
	keypad(my_menu_win, TRUE);
	// Set menu window and subwindow
	set_menu_win(my_menu, my_menu_win);
	int nomenurows=10;
	set_menu_sub(my_menu, derwin(my_menu_win, nomenurows, 38, 3, 1));
	set_menu_format(my_menu, nomenurows, 2);
	box(my_menu_win, 0, 0);
	mvwprintw(my_menu_win, 1, 1, title.c_str() );
	refresh();

	// Post the menu
	post_menu(my_menu);
	wrefresh(my_menu_win);

	// Loop to interact with the menu
	int c;
	while ((c = wgetch(my_menu_win)) != 'q') {
		switch (c) {
			case KEY_RIGHT:
				menu_driver(my_menu, REQ_RIGHT_ITEM);
				break;
			case KEY_LEFT:
				menu_driver(my_menu, REQ_LEFT_ITEM);
				break;
			case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case KEY_NPAGE:
				menu_driver(my_menu, REQ_SCR_DPAGE);
				break;
			case KEY_PPAGE:
				menu_driver(my_menu, REQ_SCR_UPAGE);
				break;

			case 10:  // Enter key
				{
					ITEM *cur;
					cur = current_item(my_menu);
					r_selection = item_name(cur);
					pos_menu_cursor(my_menu);
					status=true;
					goto leave;
					break;
				}break;
		}
		wrefresh(my_menu_win);
	}
leave:
	// Clean up
	unpost_menu(my_menu);
	free_menu(my_menu);
	for (size_t i = 0; i < choices.size(); ++i) {
		free_item(my_items[i]);
	}
	werase(my_menu_win);
	m_app->getViewMgr()->refreshScreen();	
	Refresh();
	refresh();
	return status;
}

void TEditor::execCommand(std::string cmd )
{
	if( cmd == "" ) {
		return;
	}
	setStatus("Command:"+cmd );
	if( cmd == "q" ) {
		setStatus("Quit\n");
		usleep(10000);
		quit=true;
	} else if( cmd == "q!" ) {
		setStatus("Quit - ignore changes\n");
		usleep(10000);
		quit=true;
	} else if( cmd == "n" ) {
		m_app->next();
		mode = SUSPEND;
	} else if( cmd == "p" ) {
		m_app->prev();
		mode = SUSPEND;
	} else if( cmd == "w" ) {
		m_app->getViewMgr()->saveObject();	//save current object
		m_app->getViewMgr()->getXml()->save();	//save all changes to disk
	} else if( cmd == "wa" ) {
		std::string newm;
		if( inputDialog("save buffer as :", newm) ) {
			m_app->getViewMgr()->getXml()->saveAs(newm);
		}
	} else if( cmd == "e" ) {
		setStatus("Reload");
		m_app->getViewMgr()->getXml()->reload();
	} else if( cmd == "ml" ) {
		StringVector* methods = m_app->getViewMgr()->getMethodList();
		std::string selected;
		if( methods->size() > 0 ) {
			if( popUp("Select method to view",*methods, selected) ) {
				m_app->getViewMgr()->showObject( selected );
			}
		} else {
			setStatus(std::to_string(methods->size()) +  " methods defined");
		}
	} else if( cmd == "xe" ) {		//debug
		std::string newm;
		setStatus("Debug view");
		inputDialog("DATA:\n"+getBuffer(), newm);
	} else if( cmd == "mn" ) {
		std::string newm;
		if( inputDialog("New method name:", newm) ) {
			m_app->getViewMgr()->getXml()->createObject(newm, XmlFile::ObjectType::METHOD );
		}
	} else if( cmd == "md" ) {
		StringVector* methods = m_app->getViewMgr()->getMethodList();
		std::string selected;
		if( methods->size() > 0 ) {
			if( popUp("Select method to delete",*methods, selected) ) {
				setStatus("Delete : "+selected);
				m_app->getViewMgr()->deleteObject( selected );
				m_app->getViewMgr()->refreshScreen();
			}
		} else {
			setStatus(std::to_string(methods->size()) +  " methods defined");
		}
	} else if( cmd == "lt" ) {
		StringVector methods = getTemplates();
		setStatus("Has "+ std::to_string( methods.size() )+ " templates");
	} else if( cmd == "id" ) {
		StringVector methods = getTemplates();
		std::string selected;
		if( methods.size() > 0 ) {
			if( popUp("Insert declaration ",methods, selected) ) {
				setStatus("Selected: "+selected);
				sci.insertAtCurrent( getTemplate( selected ).c_str() );
				m_app->getViewMgr()->refreshScreen();
			}
		} else {
			setStatus(std::to_string(methods.size()) +  " methods defined");
		}
	} else if( cmd == "is" ) {
		StringVector methods = getTemplates(true);
		std::string selected;
		if( methods.size() > 0 ) {
			if( popUp("Insert statement ",methods, selected) ) {
				setStatus("Selected: "+selected);
				sci.insertAtCurrent( getTemplate( selected, true ).c_str() );
				m_app->getViewMgr()->refreshScreen();
			}
		} else {
			setStatus(std::to_string(methods.size()) +  " methods defined");
		}
	} else  {
		bool isnum=true;
		for (char c : cmd) {
			if (!std::isdigit(c)) {
				isnum = false;
			}
		}
		if( isnum) {
			int line = std::atoi( cmd.c_str() );
			sci.SendMsg(SCI_GOTOLINE, line, 0);
		} else if( (cmd[0] == '!') && (cmd[1] == '>') ) {
			Execute e;
			e.setEditor(this);
			std::string result;
			auto s = cmd.substr(2, cmd.length()-2 );
			e.execute( s , result);
			sci.insertAtCurrent(result.c_str() );
		} else if( cmd[0] == '!' ) {
			Execute e;
			e.setEditor(this);
			cmd.erase( cmd.begin());
			e.execute( cmd );
		}
	}
}

void TEditor::handleNormal( int c )
{
	if (c != KEY_MOUSE) {
		bool ok=true;
		switch(c) {
			case 23:	//ctrl+w
				{
					setStatus("ctrl+w - resize using +/-");	
					mode = RESIZE;
					ok=false;
				}break;

			case 262:	//home
			case '0':
				{
					ok = false;
					sci.SendMsg(SCI_HOME, 0, 0);
				}break;
			case 360:	//end
				ok=false;
				sci.SendMsg(SCI_LINEEND, 0, 0);
				break;
			case ASCII_ESC:
				{
					visual=false;
					yank = false;
					setStatus("stop visual");
					ok=false;
				}break;
			case KEY_UP:
				{
					if(visual ) {
						int pos = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
						sci.SendMsg(SCI_LINEUPEXTEND, 0, pos);
						setStatus("Up");
						ok=false;
					} else {
						c = SCK_UP;
					}
				}break;
			case KEY_DOWN:
				{
					if(visual ) {
						int pos = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
						sci.SendMsg(SCI_LINEDOWNEXTEND, 0, pos);
						setStatus("Down");
						ok=false;
					} else {
						c = SCK_DOWN;
					}
				}break;

			case KEY_LEFT: c = SCK_LEFT;break;
			case KEY_RIGHT:c = SCK_RIGHT;break;
			case 10:       c = SCK_DOWN;break;
			case 32:       c = SCK_RIGHT;break;
			case KEY_BACKSPACE: 
				       c= SCK_LEFT;
				       break;
			case '/':
				       inputSearch("/");
				       ok=false;
				       break;
			case 'u':
				       sci.undo();
				       ok=false;
				       break;
			case 'p':
				       sci.paste();
				       ok=false;
				       break;
			case 'v':
				       {
					       setStatus("visual select");
					       int pos = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
					       //sci.SendMsg(SCI_SETCURRENTPOS, 0, pos);
					       ok=false;
					       visual=true;
				       }break;
			case 338:
			case 6:	   
				       sci.SendMsg(SCI_PAGEDOWN, 0, 0);
				       ok=false;
				       break;
			case 339:
			case 2:	   
				       sci.SendMsg(SCI_PAGEUP, 0, 0);
				       ok=false;
				       break;
			case 'E':
				       sci.SendMsg(SCI_WORDRIGHT, 0, 0);
				       ok=false;
				       break;
			case 7:
				       {
					       int lines=sci.SendMsg(SCI_GETLINECOUNT, 0, 0);
					       werase(statusWin);
					       int pos = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
					       int x,y;
					       getWindowSize(x,y );
					       mvwprintw(statusWin, 0, 0, "file:%s entity:%s pos:%d lines:%d \t win:(%d,%d)", 
							       m_xfile->file.c_str(), 
							       m_viewName.c_str(), 
							       pos,
							       lines, 
							       x, 
							       y );
					       wrefresh(statusWin);
					       ok=false;
				       }break;
			case 'J':	   
				       setStatus("join");
				       sci.SendMsg(SCI_LINESJOIN, 0, 0);
				       ok=false;
				       break;
			case 'a':
				       sci.SendMsg(SCI_WORDRIGHT, 0, 0);
				       mode = INSERT;
				       ok=false;
				       break;
			case 'A':
				       {
					       sci.SendMsg(SCI_LINEEND, 0, 0);
					       mode = INSERT;
					       ok=false;
				       }break;
			case 'S':
				       sci.selectAll();
				       ok=false;
				       break;
			case 'y':
				       if( visual ) {
					       setStatus("Yanked");
					       sci.copy();
					       ok=false;
					       yank=false;
					       visual = false;
				       } else {
					       ok=false;
					       if( yank ) {
						       setStatus("Yank line");
						       int len = lineLength(getCurrentLine());
						       if (!len) {
							       return;
						       }
						       char buf[len];
						       sci.SendMsg(SCI_GETLINE, getCurrentLine(), (sptr_t)buf);
						       setStatus("Yank: "+std::string(buf));
						       sci.SendMsg(SCI_LINECOPY,getCurrentLine(),0);
						       yank=false;
						       visual = false;
					       } else {
						       yank=true;
					       }
				       }
				       break;
			case 'w': 
				       if( yank ) {
					       setStatus("yank word");
					       int start = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
					       sci.SendMsg(SCI_WORDRIGHT, 0, 0);
					       int stop  = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
					       sci.SendMsg(SCI_COPYRANGE, start, stop );
					       sci.SendMsg(SCI_SETCURRENTPOS, start, 0);
					       visual = false;
					       yank=false;
					       ok=false;
				       }
				       break;
			case 'g':
				       sci.gotoLine(0);
				       ok=false;
				       break;
			case 'G':
				       sci.gotoLine(100000);
				       ok=false;
				       break;
			case '.':
				       sci.redo();
				       ok=false;
				       break;
			case 'e': 
				       ok=false;
				       sci.SendMsg(SCI_WORDRIGHT, 0, 0);
				       break;
			case 'b': 
				       ok=false;
				       sci.SendMsg(SCI_WORDLEFT, 0, 0);
				       break;
			case 'i':
				       mode = INSERT;
				       ok=false;
				       break;
			case '>':
				       {
					       ok=false;
					       int pos = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
					       sci.SendMsg(SCI_HOME, 0, 0);
					       sci.SendMsg(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>("    "));
					       sci.SendMsg(SCI_SETCURRENTPOS, pos, 0);
				       }break;
			case '<':
				       {
					       ok=false;
					       int pos = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
					       sci.SendMsg(SCI_DELETERANGE, pos, 4);

				       }break;
			case 'x': c= SCK_DELETE;break;
			case 'd':
				  {
					  setStatus(":d");
					  int c;
					  while ((c= wgetch(sci.GetWINDOW())) != 10) {
						  if( c == 'd' ) {
							  setStatus("Delete line");
							  sci.SendMsg(SCI_LINEDELETE, 0, 0);
							  break;
						  } else if( c == 'w' ) {
							  setStatus("Delete word");
							  sci.SendMsg(SCI_DELWORDRIGHT, 0, 0);
							  break;
						  } else {
							  setStatus("No such cmd");
						  }
					  }
					  ok=false;
				  }break;
			case ':':
				  {
					  std::string data = inputCommand(":");
					  execCommand( data );
					  ok=false;				
				  }break;
			default:
				  ok=false;
				  setStatus("No such Key: "+ std::to_string(c));
				  break;
		}
		if(ok) {
			sci.KeyPress( c, static_cast<Scintilla::KeyMod>(SCMOD_NORM));
		}
	}
	/* 
	   MEVENT mouse;
	   else if (getmouse(&mouse) == OK) {
	   int event = SCM_DRAG, button = 0;
	   if (mouse.bstate & BUTTON1_PRESSED)
	   event = SCM_PRESS, button = 1;
	   else if (mouse.bstate & BUTTON1_RELEASED)
	   event = SCM_RELEASE, button = 1;
	   bool shift = mouse.bstate & BUTTON_SHIFT;
	   bool ctrl = mouse.bstate & BUTTON_CTRL;
	   bool alt = mouse.bstate & BUTTON_ALT;
	   int modifiers = (shift ? SCMOD_SHIFT : 0) | (ctrl ? SCMOD_CTRL : 0) | (alt ? SCMOD_ALT : 0);
	//scintilla_send_mouse(buffer.sci, event, button, modifiers, mouse.y, mouse.x);
	} */
}

int TEditor::getCurrentLine()
{
	int pos = sci.SendMsg(SCI_GETCURRENTPOS, 0, 0);
	return sci.SendMsg(SCI_LINEFROMPOSITION, pos, 0);
}

size_t TEditor::lineLength(int line)
{
	return sci.SendMsg(SCI_LINELENGTH, line, 0);	
}

void TEditor::handleInsert( int c )
{
	if (c != KEY_MOUSE) 
	{
		bool ok=true;
		switch( c) 
		{
			case KEY_UP:   c = SCK_UP;break;
			case KEY_DOWN: c = SCK_DOWN;break;
			case KEY_LEFT: c = SCK_LEFT;break;
			case KEY_RIGHT:c = SCK_RIGHT;break;
			case ASCII_ESC: ok=false; mode = COMMAND;break;
			case 262:	//home
			case 360:	//end
					ok=false;
					sci.SendMsg(SCI_LINEEND, 0, 0);
					break;
			case KEY_BACKSPACE: 
					ok=false;
					sci.SendMsg(SCI_DELETEBACK, 0, 0);
					break;


		}
		if(ok) {
			sci.KeyPress( c, static_cast<Scintilla::KeyMod>(SCMOD_NORM));
		}
	} 
}

void TEditor::handleResize( int c )
{
	switch( c)
	{
		case KEY_UP:
		case '1':
			{
				setStatus("Switch to declaraion view");
				m_xfile->setView(view_e::DECLARATION);
			}break;
		case KEY_DOWN:
		case '2':
			{
				setStatus("Switch to implementation view");
				m_xfile->setView(view_e::IMPLEMENTATION);
			}break;
		case 'w':
			{
				setStatus("Next view");
				mode = COMMAND;
				int n = m_xfile->getCurrentViewNo();
				if( n == view_e::IMPLEMENTATION) {
					m_xfile->setView(view_e::DECLARATION);
				} else {
					m_xfile->setView(view_e::IMPLEMENTATION);
				}
			}break;
		case '+':
			{
				//m_boxed=true;
				if( mode == RESIZE ) {
					m_app->getViewMgr()->setSash( true );
					//mode = COMMAND;
				}
			}break;
		case '-': 
			{
				//m_boxed=true;
				if( mode == RESIZE ) {
					m_app->getViewMgr()->setSash( false );
					//mode = COMMAND;
				}
			}break;
		case 10:
		case ASCII_ESC:
			{
				setStatus("end resize");
				mode = COMMAND;
				m_app->getViewMgr()->endSash();
				m_app->getViewMgr()->refreshScreen();
			}break;
		default:
			{
				m_boxed =false;
				setStatus("not a windowmgr command");
			}break;

	}
}

void TEditor::insert( const char*data)
{
	sci.insert(data);
	sci.SendMsg(SCI_EMPTYUNDOBUFFER,0,0 );
}

void TEditor::setBuffer( const char*data, std::string name)
{
	m_viewName = name;
	sci.WndProc( Scintilla::Message::SetFocus,1, 0);
	sci.SendMsg(SCI_SETTEXT, 0, (uptr_t)data);
	sci.SendMsg(SCI_EMPTYUNDOBUFFER,0,0 );
	sci.NoutRefresh();
	sci.Refresh();
	wrefresh(sci.GetWINDOW() );
}

void TEditor::Refresh()
{
	sci.NoutRefresh();
	sci.Refresh();
	if( m_boxed) {
		box(sci.GetWINDOW(), 0, 0);
	}
	wrefresh(sci.GetWINDOW() );
}

void TEditor::run()
{
	mode = COMMAND;
	Refresh();
	int c;
	while ((c= wgetch(sci.GetWINDOW())) ) 
	{
		switch( mode )
		{
			case INSERT:  handleInsert(c); break;
			case COMMAND: handleNormal(c); break;
			case RESIZE:  handleResize(c); break;
			case SUSPEND: return;break;
		}
		Refresh();
		if( quit ) {
			break;
		}
	}
	m_app->quit();
}

void TEditor::setViewName( std::string n )
{
	m_viewName = n;
}

std::string TEditor::getBuffer()
{
	return sci.GetText();
}

void TEditor::suspend() { mode = SUSPEND; };



/* vim: set foldmethod=syntax: */
