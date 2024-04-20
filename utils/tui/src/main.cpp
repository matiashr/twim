#define Uses_TScrollBar
#define Uses_TLabel
#define Uses_TEventQueue
#define Uses_TCheckBoxes
#define Uses_THistory
#define Uses_TSItem
#define Uses_TInputLine
#define Uses_TSortedListBox
#define Uses_TDataCollection

#define Uses_TApplication
#define Uses_TEditWindow
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TEditor
#define Uses_TFileEditor
#define Uses_TFileDialog
#define Uses_TChDirDialog
#define Uses_TMenuBar
#define Uses_TRect
#define Uses_TSubMenu
#define Uses_TKeys
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TPoint
#define Uses_TEditor
#define Uses_MsgBox
#define Uses_TFileDialog
#define Uses_TDeskTop
#define Uses_TScrollBar

#include <iostream>
#include <tvision/tv.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sstream>
#include <iomanip.h>
#include <signal.h>
#include "main.h"
#include "window.h"
#include "editorwindow.h"
#include "icons.h"
#include "stringfun.h"

typedef char *_charPtr;
typedef TPoint *PPoint;

#pragma warn -rvl

static bool g_verbose;
const int cmAbout = 100;
const int cmPlugin= 200;
const int cmTest  = 300;

static TuiApp* app = NULL;
TuiApp* getApp() { return app;}
TDeskTop* getMainDesktop() { return app->getDesktop(); }
LogWindow* getLogWin() { return app->getLogWindow(); }


TMenuBar *TuiApp::initMenuBar( TRect r )
{
    TSubMenu& sub1 = *new TSubMenu( "~F~ile", kbAltF ) +
        *new TMenuItem( getIcon(ABOUT)+"About", cmAbout, kbF1, hcNoContext, "F1" ) +
        *new TMenuItem( getIcon(FOLDER_OPEN)+"~N~ew", cmNew, kbF3, hcNoContext, "F3" ) +
        *new TMenuItem( getIcon(FOLDER_OPEN)+"~C~lose", cmClose, kbF11, hcNoContext, "F11" ) +
        *new TMenuItem( "~C~hange dir...", cmChangeDrct, kbNoKey ) +
        *new TMenuItem( "O~S~ shell", cmDosShell, kbNoKey ) +
        *new TMenuItem( getIcon(EXIT)+"E~x~it", cmQuit, kbCtrlQ, hcNoContext, "Ctrl-Q" );

    TSubMenu& sub3 = *new TSubMenu( "~S~earch", kbAltS ) +
        *new TMenuItem( getIcon(SEARCH)+"~F~ind...", cmFind, kbNoKey ) +
        *new TMenuItem( "~S~earch again", cmSearchAgain, kbNoKey );
    TSubMenu* puginsmenu =NULL;


    TSubMenu& sub4 = *new TSubMenu( "~W~indows", kbAltW ) +
        *new TMenuItem( "~S~ize/move",cmResize, kbCtrlF5, hcNoContext, "Ctrl-F5" ) +
        *new TMenuItem( "~Z~oom", cmZoom, kbF5, hcNoContext, "F5" ) +
        *new TMenuItem( "~T~ile", cmTile, kbNoKey ) +
        *new TMenuItem( "C~a~scade", cmCascade, kbNoKey ) +
        *new TMenuItem( "~N~ext", cmNext, kbF6, hcNoContext, "F6" ) +
        *new TMenuItem( "~P~revious", cmPrev, kbShiftF6, hcNoContext, "Shift-F6" ) +
        *new TMenuItem( "~C~lose", cmClose, kbCtrlW, hcNoContext, "Ctrl+W" );
    TSubMenu& sub5 = *new TSubMenu( "~?~", kbAltH ) +
        *new TMenuItem( "~H~elp",cmHelp, kbCtrlF5, hcNoContext, "Ctrl-F1" ) ;

    TSubMenu& sub6 = *new TSubMenu( "~T~est", kbAltT ) +
        *new TMenuItem( "Sheet",cmTest, kbCtrlF9, hcNoContext, "Ctrl-F9" ) ;

    r.b.y = r.a.y+1;
    if( puginsmenu != NULL ) {
        return new TMenuBar( r, sub1 + sub3 + sub4 + *puginsmenu + sub5 );
    }
    return new TMenuBar( r, sub1 + sub3 + sub4 + sub5 + sub6);
}

TStatusLine *TuiApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y-1;
    return new TStatusLine( r,
            *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( 0, kbAltX, cmQuit) +
            *new TStatusItem( "~F3~ Open", kbF3, cmNew) +
            *new TStatusItem( "~Ctrl-W~ Close", kbAltF3, cmClose ) +
            *new TStatusItem( "~F5~ Zoom", kbF5, cmZoom ) +
            *new TStatusItem( "~F6~ Next", kbF6, cmNext ) +
            *new TStatusItem( "~F10~ Menu", kbF10, cmMenu ) +
            *new TStatusItem( "~F8~ Clear log", kbF8, cmClearLog) +
            *new TStatusItem( "~F11~ Close", kbF11, cmClose) +
            *new TStatusItem( 0, kbCtrlF5, cmResize )
            );

}

void TuiApp::outOfMemory()
{
    messageBox("Not enough memory for this operation.", mfError | mfOKButton );
}



TuiApp::TuiApp( int argc, char **argv ) :
    TProgInit( TuiApp::initStatusLine,
    TuiApp::initMenuBar,
    TuiApp::initDeskTop),
    TApplication()
{
    //some theming..
    TPalette &palette = getPalette();       /*  foregr.   backgr.  */
    palette[cpGrayDialog[3  - 1]] = TColorAttr(0xbf0303,    '\x7'); // Frame icon
    palette[cpGrayDialog[10 - 1]] = TColorAttr(0xffffff, 0xb218b2); // Button normal
    palette[cpGrayDialog[12 - 1]] = TColorAttr(0x00007f, 0xb218b2); // Button selected

    cascade();
    TRect r = deskTop->getExtent();
    TRect lg(1,r.b.y-10,r.b.x,r.b.y);
    m_logwin = new LogWindow(lg,NULL,NULL);
    deskTop->insert(m_logwin);
}


bool cliSearch(std::string con, std::string text, StringVector& value )
{
    messageBox("Failed request search\nEnsure that backend is running on ", mfError | mfOKButton );
    return true;
}

/*
    split TEST=123 => TEST, 123
*/
static inline void valuePairSplit( std::string pair, std::string& var, std::string& value)
{/*{{{*/
    bool isobj=true;
    for( auto c : pair ) {
        if( c != '=' && isobj ) {    
            var.push_back(c);            
        } else if( !isobj) {
            value.push_back(c);
        } else {
            isobj = false;
        }
    }
}/*}}}*/


void TuiApp::fileOpen(std::string connection)
{
    TCommandSet ts;
    ts.enableCmd( cmFind );
    ts.enableCmd( cmSearchAgain );
    enableCommands( ts );
    TRect bounds= TRect( 0, 0, 80, 34 );
    getLogWin()->message("Browsing..\n");

}

void TuiApp::fileNew()
{
    EditorWindow* editor = new EditorWindow(TRect(0, 0, 167, 43), NULL, NULL);
    if( validView( editor) ) {
        deskTop->insert(editor);
    }
}

void TuiApp::about()
{
    TDialog *aboutDlgBox = new TDialog(TRect(0, 0, 47, 13), "About");
    if( validView( aboutDlgBox ) )
    {
        aboutDlgBox->insert(
            new TStaticText(
                TRect(2,1,45,9),
                "\n"+getIcon(ABOUT)+"EditTUI \n \n"
                "\003A generic Editor\n \n"
                "\003written by\n \n"
                "\003Matias Henttunen\n"
            ));
        aboutDlgBox->insert( new TButton(TRect(18,10,29,12), "OK", cmOK, bfDefault));
        aboutDlgBox->options |= ofCentered;     // Centered on
        execView( aboutDlgBox );                // Bring up the
        destroy( aboutDlgBox );                 // Destroy the
    }
}

void TuiApp::help()
{
    TDialog *aboutDlgBox = new TDialog(TRect(0, 0, 47, 13), "Keyboard shortcuts");
    if( validView( aboutDlgBox ) )
    {
        aboutDlgBox->insert(
            new TStaticText(
                TRect(2,1,45,9),
                "\n"+getIcon(ABOUT)+"PTT NavTui\n \n"
                "Enter - Show object information / browse object\n"
                "'+' - Expand object (browse)\n"
                "'-' - Unexpand object \n"
                "'c' - Call method (selected object must be a method)\n"
                "'w' - Write object\n"
                "'s' - Subscribe to object\n"
                "'u' - Unsubscribe object\n"
                "'f' - up\n"
                "'b' - down\n"
            ));
        aboutDlgBox->insert( new TButton(TRect(18,10,29,12), "OK", cmOK, bfDefault));
        aboutDlgBox->options |= ofCentered;     // Centered on
        execView( aboutDlgBox );                // Bring up the
        destroy( aboutDlgBox );                 // Destroy the box
    }
}



void TuiApp::changeDir()
{
}

void TuiApp::showClip()
{
}

void TuiApp::findDialog()
{
}




void TuiApp::handleEvent( TEvent& event )
{
    TApplication::handleEvent( event );
    if( event.what == evCommand ) {
        switch( event.message.command )
        {
            case cmAbout:
                about();
                break;
           case cmClose:
                getLogWin()->message("Close backend\n");
            break;
            case cmNew: 		
                fileNew(); 
                break;
            case cmChangeDrct: 	
                changeDir(); 
                break;
            case cmShowClip:
                showClip(); 
                break;
            case cmFind:
                findDialog();
                break;
            case cmTest:
                {

                }break;
            case cmHelp:
                {
                    help();
                }break;
            case cmClearLog:
                {
                    getLogWin()->clearAll();
                }break;
            default:
                {
                        getLogWin()->message("Unhandled command " + std::to_string(event.message.command)+"\n");
                }break;
        }
    }
    clearEvent( event );
}

void help()
{
    printf("Arguments:\n"\
        " -g         ; enable graphics\n"
    );
    exit(0);
}

void useCustomSkin();

int main( int argc, char **argv )
{
    useCustomSkin();
    for(int i=0; i < argc; i++ ) {
        if( std::string(argv[i]) == "-g") {
            enablegraphics(true);
        }else if( std::string(argv[i]) == "-b") {
            i++;
        }else if(std::string(argv[i]) == "-v" ) {
            g_verbose=true;
        }else if(std::string(argv[i]) == "-h" ) {
            help();
        }

    }
    app = new TuiApp(argc,argv);
    app->run();
    app->shutDown();
    return 0;
}

/* vim: set ts=4 sw=4 tw=0 foldmethod=marker noet expandtab: */
