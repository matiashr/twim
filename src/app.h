#ifndef APP_H
#define APP_H
#include "teditor.h"
#include "splittedview.h"



class App
{
	public:
		App();
		~App();
		SplittedView* New(std::string name);
		void run();
		void next(); 
		void prev(); 
		void setup();
		void quit();
		SplittedView* getViewMgr() { return current; };	//class containing 2 views
		void setStatus(std::string msg );
		void showDialog(std::string msg );
		bool fileExists(std::string f);
	private:
		void setStatusWindow( SplittedView* fn, int rows, int cols );
	private:
		bool m_quit;
		SplittedView* current;
		uint32_t ci;
		std::vector<SplittedView*> m_buffers;
	private:
		WINDOW* statusWin;
};

#endif
