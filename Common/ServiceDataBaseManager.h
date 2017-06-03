#ifndef SERVICEDATABASEMANAGER_H
#define SERVICEDATABASEMANAGER_H

#include <string>
#include <fstream>
#include "utils.h"

class ServiceDataBaseManager
{
public:
	ServiceDataBaseManager (std::string filePath);
	~ServiceDataBaseManager ();
	
	bool OpenFileStream();
	bool CloseFileStream(); 
	std::string GetNextWord();
	
	bool IsServiceInDataBase(int serverName, int serviceName);
	
private:
	std::string filePath;
	std::fstream fileStream;
	char GetNextChar();
	
};

#endif // SERVICEDATABASEMANAGER_H