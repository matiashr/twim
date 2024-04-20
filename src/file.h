#ifndef FILE_FUN_H
#define FILE_FUN_H

namespace FileMgt
{
	bool fileExists(std::string f);
	std::string getExtension(const std::string& filename);
	static std::string getName( std::string filename );

}

#endif
