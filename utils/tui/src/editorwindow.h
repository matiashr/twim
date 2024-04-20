#ifndef EDITWIN_H
#define EDITWIN_H
#define Uses_TKeys
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TEditor
#define Uses_TOutline
#define Uses_TScrollBar
#define Uses_TParamText
#define Uses_TScreen
#define Uses_TText
#define Uses_TSplitter
#include <tvision/tv.h>
#include <string>
#include <chrono>
#include "genwindow.h"
#include "editorwidget.h"
#include "methodswidget.h"

class EditorWindow: public GenWindow 
{
	public:
		EditorWindow( const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb );
		~EditorWindow();
		void clearAll();
		void message(std::string msg);
		TPalette& getPalette() const;
		void focus(int n);
	private:
		void showObjectMethods();
	private:
		TWindow* topPanel;
		TWindow* bottomPanel;
		MethodWidget* methodsPanel;
		EditorWidget* declaration;
		EditorWidget* implementation;
};

#endif
