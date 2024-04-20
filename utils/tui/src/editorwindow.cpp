#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "editorwindow.h"

EditorWindow::EditorWindow( const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb ):
	GenWindow( bounds,"Editor",11 ),
	TWindowInit( EditorWindow::initFrame )
{
	TRect topRect = getExtent();
	topRect.a.y = topRect.a.y + 1;
	topRect.a.x = topRect.a.x + 30;
	topRect.b.y = 9; //topRect.b.y - 14;
	topRect.b.x = topRect.b.x - 2 ;

	topPanel = new TWindow(topRect, "Declaration", wnNoNumber);
	topPanel->flags =wfZoom;
	topPanel->growMode =  gfGrowHiX; // |gfGrowHiY  ;
	insert(topPanel);

	implementation = new EditorWidget(this, topPanel->getExtent().grow(-1,-1)  );
	topPanel->insert( implementation );


	TRect bottomRect = getExtent();
	bottomRect.a.y = bottomRect.a.y + 9;
	bottomRect.a.x = bottomRect.a.x + 30;
	bottomRect.b.x = bottomRect.b.x - 2 ;
	bottomRect.b.y = bottomRect.b.y - 1 ;

	bottomPanel = new TWindow(bottomRect, "Implementation", wnNoNumber);
	bottomPanel->flags =wfZoom;
	bottomPanel->growMode =  gfGrowHiX |gfGrowHiY  ;
	bottomPanel->growMode =  gfGrowHiX |gfGrowHiY  ;

	declaration    = new EditorWidget(this, bottomPanel->getExtent().grow(-1,-1)  );
	bottomPanel->insert( declaration);
	insert(bottomPanel);
	showObjectMethods();
	topPanel->focus();
}


EditorWindow::~EditorWindow() 
{
}



TPalette& EditorWindow::getPalette() const
{
	static TPalette paletteGray( cpGrayDialog, sizeof( cpGrayDialog )-1 );
	return paletteGray;
}



void EditorWindow::showObjectMethods()
{
	TRect leftRect = getExtent();
	leftRect.a.x = 1;
	leftRect.a.y = 1;
	leftRect.b.x = 30;
	leftRect.b.y -= 1;
	methodsPanel = new MethodWidget(this, leftRect);
	methodsPanel->flags = 0; //wfZoom;
	methodsPanel->growMode =  gfGrowHiY  ;
	insert(methodsPanel);
}

void EditorWindow::clearAll() 
{
	declaration->deleteRange(0,declaration->curPtr,true);
	//control->deleteRange(control->curPtr-10,control->curPtr,true);
}


void EditorWindow::message(std::string msg )
{
	declaration->insertText(  msg.c_str(), msg.length() ,false ) ;
	declaration->scrollTo(-1,declaration->curPtr);
}



void EditorWindow::focus(int n)
{
	switch(n)
	{
		case 0: methodsPanel->focus(); break;
		case 1: topPanel->focus(); break;
		case 2: bottomPanel->focus(); break;
	}
}
