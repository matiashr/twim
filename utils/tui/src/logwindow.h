#ifndef LOGWIN_H
#define LOGWIN_H
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
#include <tvision/tv.h>
#include <string>
#include <chrono>
#include "genwindow.h"

class LogWindow: public GenWindow {
		public:
				LogWindow( const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb );
				~LogWindow();
				void clearAll();
				void message(std::string msg);
				TPalette& getPalette() const;
		private:
				TEditor *control;
};

#endif
