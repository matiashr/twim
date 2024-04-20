#include "window.h"
#include <sstream>
/*
	temp disable some commands:
	    TCommandSet ts;
		ts.enableCmd( cmOK );
		ts.enableCmd( cmCancel);
		disableCommands( ts );
*/

Window:: Window( TRect&bounds, std::string title, short no ):
	TWindow( bounds, title, no),
	TWindowInit( Window::initFrame )
{
	//	TRect r = TRect( bounds.b.x-1, bounds.a.y+1, bounds.b.x, bounds.b.y-1 );
		TScrollBar *vScrollBar = NULL; //new TScrollBar ( r );
		m_list = new TListBox( TRect( 3, 5, 35, 7 ) , 1, vScrollBar);
		short buttonX=10;
		short buttonY=10;
		short buttonWd= 24;
		short buttonHeight= 12;

		insert( new TStaticText( TRect(2,1,45,9), "Select host"));
		insert( m_list );
		insert( new TButton( TRect( buttonX, buttonY, buttonX+10, buttonHeight ), "O~K~", cmOK, bfDefault ) );
		insert( new TButton( TRect( 28, buttonY, 38, buttonHeight ), "Cancel", cmCancel, bfNormal ) );
		selectNext( True );


	//	  options |= ofFirstClick;
	//	options |= ofCentered;
}

Window::~Window()
{

}

TPalette& Window::getPalette() const
{
		static TPalette paletteGray( cpGrayDialog, sizeof( cpGrayDialog )-1 );
		return paletteGray;
}


void Window::handleEvent( TEvent& event )
{
		TWindow::handleEvent( event );
		if ( (event.what == evCommand) ) {
				switch( event.message.command)
				{
						case cmOK:
								{
										//get selected from m_list
										char selected[80];
										m_list->getText(selected,m_list->focused, sizeof(selected)-1 );
										char buf[256] = {0};
										std::ostringstream os( buf  );
										os << "selected :" << selected << std::ends;
										messageBox( buf, mfError | mfOKButton );
										destroy(this);
										clearEvent(event);
								}break;
						case cmCancel:
								destroy(this);
							//	messageBox( "Nothing selected", mfError | mfOKButton );
								clearEvent(event);
								break;
						default:
								break;
				}
		}
}

void Window::addData( TCollection* collection )
{
        m_list->newList( collection );
}


