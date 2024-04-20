#include <vector>
#include <thread>
#include <string>
#include "logwindow.h"
#if !defined( __TVEDIT_H )
#define __TVEDIT_H
class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

#define Uses_TApplication
#include <tvision/tv.h>
#include <map>
const int
cmChangeDrct = 102,
	     cmCalculator = 104,
	     cmShowClip   = 105,
	     cmClearLog   = 106;

class TuiApp : public TApplication
{
	public:
		TuiApp( int argc, char **argv );
		virtual void handleEvent( TEvent& event );
		static TMenuBar *initMenuBar( TRect );
		static TStatusLine *initStatusLine( TRect );
		virtual void outOfMemory();
		TDeskTop * getDesktop() { return deskTop; };
		LogWindow* getLogWindow() { return m_logwin; };
		void fileOpen(std::string connection);
		void showCallDialog(std::string con, std::string ns, std::string nodeid);
		void about();
		static void subscriptionThread(TuiApp*);
		void updateSubscription(std::string data);
	private:
		void fileConnections();
		void help();
		void fileNew();
		void changeDir();
		void showClip();
		void findDialog();
	private:
		LogWindow* m_logwin;
};

extern TEditWindow *clipWindow;

ushort execDialog( TDialog *d, void *data );
TDialog *createFindDialog();
TDialog *createReplaceDialog();
ushort doEditDialog( int dialog, ... );

TuiApp* getApp();
TDeskTop* getMainDesktop();
LogWindow* getLogWin();

#endif // __TVEDIT_H
