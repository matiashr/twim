#include "genwindow.h"
#include <sstream>

GenWindow:: GenWindow( const TRect&bounds, std::string title, short no ):
	TWindow( bounds, title, no),
	TWindowInit( TWindow::initFrame )
{
}

GenWindow::~GenWindow()
{

}

TPalette& GenWindow::getPalette() const
{
	static TPalette paletteGray( cpGrayDialog, sizeof( cpGrayDialog )-1 );
	return paletteGray;
}


void GenWindow::handleEvent( TEvent& event )
{
	TWindow::handleEvent( event );
	if ( (event.what == evCommand) ) {
		switch( event.message.command)
		{
			case cmOK:
				{
					close();
					clearEvent( event );

				}break;
			case cmCancel:
				{
					close();
					clearEvent( event );
				}break;
			default:
				break;
		}
	}
}



