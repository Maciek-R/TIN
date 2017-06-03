#ifndef SERVICEDATABASEMANAGER_H
#define SERVICEDATABASEMANAGER_H

#include <string>
#include <fstream>

class ServiceDataBaseManager
{
public:
	ServiceDataBaseManager (std::string filePath);
	~ServiceDataBaseManager ();
	std::string GetNextWord();
	
private:
	std::string filePath;
	std::fstream fileStream;
	
	bool OpenFileStream();
	bool CloseFileStream(); 
	
	char GetNextChar();
};

#endif // SERVICEDATABASEMANAGER_H