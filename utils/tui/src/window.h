#ifndef A_WIN_H
#define A_WIN_H
#define Uses_TScrollBar
#define Uses_TLabel
#define Uses_TEventQueue
#define Uses_TCheckBoxes
#define Uses_THistory
#define Uses_TSItem
#define Uses_TForm
#define Uses_TInputLine
#define Uses_TSortedListBox
#define Uses_TDataCollection
#define Uses_TFileDialog
#define Uses_TStreamable
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TView
#define Uses_TButton
#define Uses_TKeys
#define Uses_TDeskTop
#define Uses_TApplication
#define Uses_MsgBox

#include <tvision/tv.h>
#include <string>

class Window  : public TWindow
{
		public:
				Window( TRect&pos, std::string title, short no );
				virtual ~Window();
				virtual void handleEvent( TEvent& );
				void addData( TCollection* collection );
				TPalette& getPalette() const;
		private:
				TListBox* m_list;
};

#endif
