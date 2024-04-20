#include "main.h"
#include "exec.h"

void Execute::setEditor(TEditor* ed )
{
	m_editor = ed;
}

bool Execute::execute( std::string& command )
{
	getApp().setStatus("execute command:'"+command +"'");
	return true;	
}

bool Execute::execute( std::string& command, std::string& result)
{
	getApp().setStatus("execute command with redirect:'"+command +"'");
	std::array<char, 128> buffer;
	std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
	}
	return true;	
}
