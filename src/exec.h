#ifndef IF_EXEC_H
#define IF_EXEC_H
#include "teditor.h"

class Execute
{
	public:
		void setEditor(TEditor* ed );	
		bool execute( std::string& command );
		bool execute( std::string& command, std::string& result );
	private:
		TEditor* m_editor;
};

#endif
