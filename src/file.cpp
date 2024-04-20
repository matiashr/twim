#include <unistd.h>
#include <string>

namespace FileMgt
{
	bool fileExists(std::string f)
	{
		if (access(f.c_str(), F_OK) != -1) {
			return true;
		}
		return false;
	}

	std::string getExtension(const std::string& filename) {
		size_t dotPos = filename.find_last_of('.');
		if (dotPos != std::string::npos) {
			return filename.substr(dotPos);
		}
		return ""; // No extension found
	}

	static std::string getName( std::string filename )
	{
		size_t dotPos = filename.find_last_of('.');
		if (dotPos != std::string::npos) {
			return filename.substr(0, dotPos);
		}
		return filename; 
	}

};
