#ifndef FILEIF_H
#define FILEIF_H
/*
	File interface
	class xmlfile implements this
*/

class FileIf
{
	public:
		virtual bool open( std::string name ) = 0;			//open file
		virtual bool save() = 0;					//save changes
		virtual bool saveAs( std::string name ) = 0;			//save file as
		virtual bool isModified() = 0;					//any changes
		virtual bool reload() = 0;					//reload file

};

#endif
