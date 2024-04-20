#ifndef SCINTILLA_CURSES_H
#define SCINTILLA_CURSES_H
#include <curses.h>
#include <cassert>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <memory>
#include <chrono>
#include <curses.h>
#include "Scintilla.h"
#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ScintillaStructures.h"
#include "ILoader.h"
#include "ILexer.h"
#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "CharacterCategoryMap.h"
#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"
#include "UniConversion.h"
#include "Selection.h"
#include "PositionCache.h"
#include "EditModel.h"
#include "MarginView.h"
#include "EditView.h"
#include "Editor.h"
#include "AutoComplete.h"
#include "ScintillaBase.h"
#include "PlatCurses.h"
#include "Lexilla.h"
#include <string>

#ifdef __cplusplus
#define EXTERNC extern "C" 
#endif

#define SCM_PRESS 1
#define SCM_DRAG 2
#define SCM_RELEASE 3

class Interface
{
	virtual void insert(const char* data ) = 0;
	virtual void undo()  = 0;
	virtual void redo()  = 0;
	virtual void paste()  = 0;
	virtual void copy() = 0;
	virtual void cut() = 0;
	virtual	void select() =0;
	virtual	void selectAll() =0;
	virtual	void gotoLine(int n) =0;
	virtual int SendMsg(int msg, uptr_t wParam, sptr_t lPara )  = 0;

};


namespace Scintilla::Internal 
{

	class ScintillaCurses : public ScintillaBase, Interface
	{
		std::unique_ptr<Surface> sur; 				// window surface to draw on
		int width = 0, height = 0; 				// window dimensions
		void (*callback)(void *, int, SCNotification *, void *);// SCNotification cb
		void *userdata; 					// userdata for SCNotification callbacks
		int scrollBarVPos, scrollBarHPos; 			// positions of the scroll bars
		int scrollBarHeight = 1, scrollBarWidth = 1; 		// scroll bar height and width
		SelectionText clipboard; 				// current clipboard text
		bool capturedMouse; 					// whether or not the mouse is currently captured
		unsigned int autoCompleteLastClickTime; 		// last click time in the AC box
		bool draggingVScrollBar, draggingHScrollBar; 		// a scrollbar is being dragged
		int dragOffset; 					// the distance to the position of the scrollbar being dragged
		public:
			ScintillaCurses(void (*callback_)(void *, int, SCNotification *, void *), void *userdata_);
			virtual ~ScintillaCurses() ;
			void Initialise() ;
		// Interface
		public:
			Lexilla::CreateLexerFn lexer;
			void insert(const char* data );
			void insertAtCurrent(const char* data );
			void undo();
			void redo();
			void paste();
			void cut();
			void copy();
			void select();
			void selectAll();
			void gotoLine(int n);
			int SendMsg(int msg, uptr_t wParam, sptr_t lPara ) ;
			int GetCurrentPos() ;
			std::string GetText();
		private:
			void StartDrag() ;
			void SetVerticalScrollPos() ;
			void SetHorizontalScrollPos() ;
			bool ModifyScrollBars(Sci::Line nMax, Sci::Line nPage) ;
			void Copy() ;
			void Paste() ;
			void ClaimSelection() ;
			void NotifyChange() ;
			void NotifyParent(NotificationData scn) ;
			int  KeyDefault(Keys key, KeyMod modifiers) ;
			void CopyToClipboard(const SelectionText &selectedText) ;
			bool FineTickerRunning(TickReason reason) ;
			void FineTickerStart(TickReason reason, int millis, int tolerance) ;
			void FineTickerCancel(TickReason reason) ;
			void SetMouseCapture(bool on) ;
			bool HaveMouseCapture() ;
			std::string UTF8FromEncoded(std::string_view encoded) const ;
			std::string EncodedFromUTF8(std::string_view utf8) const ;
			sptr_t DefWndProc(Message iMessage, uptr_t wParam, sptr_t lParam) ;
			void CreateCallTipWindow(PRectangle rc) ;
			void AddToPopUp(const char *label, int cmd = 0, bool enabled = true) ;
		public:
			sptr_t WndProc(Message iMessage, uptr_t wParam, sptr_t lParam) ;
			WINDOW *GetWINDOW();
			void UpdateCursor();
			void NoutRefresh();
			void Refresh();
			void KeyPress(int key, KeyMod modifiers);
			bool MousePress(int y, int x, int button, KeyMod modifiers);
			bool MouseMove(int y, int x, KeyMod modifiers);
			void MouseRelease(int y, int x, KeyMod modifiers);
			char *GetClipboard(int *len);
	};

};
#endif
