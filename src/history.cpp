#include "history.h"

History::History():
	pos(0)
{
	m_history.push_back("");
}

History::~History()
{
}


void History::add(std::string s )
{
	if( m_history.size() < 30 ) {
		pos++;
		m_history.push_back(s);
	}
}

std::string History::next()
{
	if( m_history.size() > pos+1 ) {
		pos++;
		return m_history.at(pos);
	}
	return m_history.at(0);
}

std::string History::prev()
{
	if( m_history.size()-1 > 1 ) {
		if( pos > 1) {
			pos--;
		}
		return m_history.at(pos);
	}
	return m_history.at(0);
}

std::string History::current()
{
	return m_history.at(pos);
}

