#ifndef HISTORY_H
#define HISTORY_H
#include <string>
#include "strfun.h"

class History
{
	public:
		History();
		~History();
	public:
		void add(std::string s );
		std::string next();
		std::string prev();
		std::string current();
		size_t size() { return m_history.size(); };
	private:
		size_t pos;
		StringVector m_history;
};

#endif
