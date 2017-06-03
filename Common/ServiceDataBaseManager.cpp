#include "ServiceDataBaseManager.h"
#include <iostream>

ServiceDataBaseManager::ServiceDataBaseManager(std::string filePath)
{
	this->filePath = filePath;
}

ServiceDataBaseManager::~ServiceDataBaseManager()
{
}

bool ServiceDataBaseManager::OpenFileStream()
{
	fileStream.open(filePath.c_str(), std::fstream::in);
	return fileStream.is_open();
}

bool ServiceDataBaseManager::CloseFileStream()
{
	fileStream.close();
	return !fileStream.is_open();
}

char ServiceDataBaseManager::GetNextChar()
{
	char result;
	fileStream.get(result);
	return result;
}

std::string ServiceDataBaseManager::GetNextWord()
{
	std::string result;
	char currentChar;
	bool isString = false;
	
	do
	{
		currentChar = fileStream.get();
	}
	while(!fileStream.eof() && std::isspace(currentChar));
	
	while(!fileStream.eof() && (std::isgraph(currentChar) ))
	{
		result.push_back(currentChar);
		fileStream.get(currentChar);
	}
	
	return result;
}

bool ServiceDataBaseManager::IsServiceInDataBase(int serverName, int serviceName)
{
	if(!OpenFileStream())
	{
		printf("Error: No such file directory!");
	}
	
	std::string currentServerName;
	std::string currentServiceName;
	
	do
	{
		currentServerName = GetNextWord();
		currentServiceName = GetNextWord();
		
		std::cout << "Current server and service: " << currentServerName << " " <<
						currentServiceName << "\n";
		
		if(std::to_string(serverName) == currentServerName && std::to_string(serviceName) == currentServiceName)
		{
			CloseFileStream();
			return true;
		}
	}
	while(!currentServerName.empty() && !currentServiceName.empty());

	CloseFileStream();
	return false;
}
