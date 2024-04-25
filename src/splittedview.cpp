#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

#include "splittedview.h"
#include "main.h"
#include "xmlfile.h"
//#define DEBUG_XML



SplittedView::SplittedView(): 
	m_xml(NULL)
{
}

bool SplittedView::open() 
{
	m_xml = new XmlFile(file);
	if( m_xml->open() ) {
		impl->insert( m_xml->getImplementation().c_str() );
		decl->insert( m_xml->getDeclaraion().c_str() );
		this->decl->setViewName( m_xml->getPrimaryName() );
		this->impl->setViewName( m_xml->getPrimaryName()  );
#ifdef DEBUG_XML
		printf("DATA:%s\n",m_xml->getImplementation().c_str() );
		exit(0);
#endif
	} else {
		decl->open();
		impl->open();
	}
	currentView = decl;
	currentViewNo = view_e::DECLARATION;
	return true;
}

void SplittedView::saveObject()
{
	auto currentName = this->impl->getViewName();
	auto currentImpl = this->impl->getBuffer();
	auto currentDecl = this->decl->getBuffer();
	m_xml->saveToMem(currentName, currentDecl, currentImpl);
	//getApp().showDialog(currentDecl);
}

void SplittedView::showObject( std::string& name)
{
	std::string decl,impl;
	auto currentName = this->impl->getViewName();
	auto currentImpl = this->impl->getBuffer();
	auto currentDecl = this->decl->getBuffer();
	m_xml->saveToMem(currentName, currentDecl, currentImpl);

	if( m_xml->getMethodObject( name, &decl, &impl ) ) {
		getApp().setStatus("switched to : "+name+ " from "+currentName );
		this->impl->setBuffer( impl.c_str(),name );
		this->decl->setBuffer( decl.c_str(),name );
	}
}

void SplittedView::run() 
{
	currentView->run();
}

void SplittedView::setView( int viewNo ) 
{
	switch( viewNo) {
		case view_e::DECLARATION: 
			impl->suspend();
			decl->activate(); 
			currentView = decl; 
			currentViewNo = view_e::DECLARATION;
		break;
		case view_e::IMPLEMENTATION: 
			decl->suspend();
			impl->activate(); 
			currentView = impl; 
			currentViewNo = view_e::IMPLEMENTATION;
		break;
	}
}

TEditor* SplittedView::getView( int viewNo )
{
	switch( viewNo) {
		case view_e::DECLARATION: return decl; break;
		case view_e::IMPLEMENTATION: return impl; break;
	}
	return NULL;
}

void SplittedView::setEditor( int viewNo, TEditor* ed )
{
	switch( viewNo) {
		case view_e::DECLARATION: decl = ed;; break;
		case view_e::IMPLEMENTATION: impl = ed; break;
	}
}

void SplittedView::activate() 
{
	impl->activate();
	decl->activate();
	refreshScreen();
}

void SplittedView::refreshScreen()
{
	impl->Refresh();
	decl->Refresh();
}

void SplittedView::endSash()
{
	decl->setBoxed(false);
}

void SplittedView::setSash( bool up )
{
	int x,y;
	int sizey, sizex ;
	decl->getWindowSize(sizex,sizey);

	decl->setBoxed(true);
	if( !up && (sizey >10 )) {
		decl->resizeWindow(sizey-10, sizex);
		impl->resizeWindow(sizey+10, sizex);
		decl->getWindowPos(x,y);
		decl->getWindowSize(sizex,sizey);
		impl->moveWindow(x, y+sizey);
	} else {
		decl->resizeWindow(sizey+10, sizex);
		impl->resizeWindow(sizey-10, sizex);
		decl->getWindowPos(x,y);
		decl->getWindowSize(sizex,sizey);
		impl->moveWindow(x, y+sizey);
	}
	refreshScreen();
}

std::string SplittedView::getPrimaryName()
{
	return m_xml->getPrimaryName();
}

bool SplittedView::deleteObject( std::string name )
{
	return m_xml->deleteObject( name );
}

/* vim: set foldmethod=syntax: */
