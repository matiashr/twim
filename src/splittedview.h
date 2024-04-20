#ifndef SPL_V_H
#define SPL_V_H
#include "teditor.h"
#include "xmlfile.h"
#include "strfun.h"

typedef enum {
	DECLARATION=1,
	IMPLEMENTATION=2
}view_e;

class SplittedView
{
	public:
		SplittedView();
		bool open() ;
		void run(); 
		void setView( int viewNo );
		void setEditor( int viewNo, TEditor* ed );
		int  getCurrentViewNo() {return currentViewNo;};
		TEditor* getView( int viewNo );
		TEditor* getCurrentView() { return currentView;};
		void activate();
		void refreshScreen();
		void setSash( bool up);
		void endSash();
		bool deleteObject( std::string name );
		std::string file;
		StringVector* getMethodList() { return m_xml->getMethodList(); };
		void showObject( std::string& obj );
		void saveObject();	//save changes to current object
		std::string getPrimaryName();
		XmlFile* getXml() { return m_xml; };
	private:
		XmlFile* m_xml;
		TEditor* currentView;
		TEditor* decl;
		TEditor* impl;
		int currentViewNo;

};

#endif
