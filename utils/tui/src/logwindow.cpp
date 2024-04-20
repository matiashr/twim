#include "logwindow.h"
#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <chrono>
#include <iomanip>

LogWindow::LogWindow( const TRect &bounds, TScrollBar *hsb, TScrollBar *vsb ):
	GenWindow( bounds,"Log",11 ),
	TWindowInit( LogWindow::initFrame )
{
	flags =wfZoom;
	growMode =  gfGrowHiX |gfGrowHiY  ;
	control = new TEditor( getExtent().grow(-2,-2),NULL, standardScrollBar(sbVertical), NULL,8000 );
	control->options |= ofCentered;
	control->overwrite=True;
	insert(control);
	selectNext( False );
}

LogWindow::~LogWindow() {
//		clearAll();
}

TPalette& LogWindow::getPalette() const
{
		static TPalette paletteGray( cpGrayDialog, sizeof( cpGrayDialog )-1 );
		return paletteGray;
}



void LogWindow::clearAll() 
{
		control->deleteRange(0,control->curPtr,true);
		//control->deleteRange(control->curPtr-10,control->curPtr,true);
}


static std::string time_in_HH_MM_SS_MMM()
{
    using namespace std::chrono;

    // get current time
    auto now = system_clock::now();

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(now);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

void LogWindow::message(std::string msg )
{
	std::stringstream  ss;
	ss <<  time_in_HH_MM_SS_MMM() << "|"<< msg;
	std::string out = ss.str();
	if(!control->insertText(  out.c_str(), out.length() ,false ) ) {
		clearAll();
		control->insertText(  out.c_str(), out.length() ,false ) ;
	}
	control->scrollTo(-1,control->curPtr);
}

