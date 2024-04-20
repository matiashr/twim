#ifndef METHWIDGET_H
#define METHWIDGET_H

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
#define Uses_TScrollBar
#include <tvision/tv.h>
#include <string>
#include <chrono>
#include "genwindow.h"
#include <tvision/tv.h>

class EditorWindow;

class MethodWidget: public TWindow
{
	public:
		MethodWidget(EditorWindow* parent, const TRect& bounds);
		virtual void handleEvent(TEvent& event);
		TPalette& getPalette() const;
		virtual void draw();
	private:
		TOutline* m_tree;
		EditorWindow* m_parent;
};


#endif
