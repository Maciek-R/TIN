#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <istream>

enum class InterfaceType
{
	ETH,
	WLAN,
};

namespace Utils
{

	std::string ToString(unsigned char*, int, int);
	int ToInt(unsigned char*, int, int);
	void LoadAddress(unsigned char *, std::string, int);
	std::string ToStr(unsigned char*, int, int);
	void InsertStringToCharTable(unsigned char* charTable, std::string newString, int start, int limit);
	std::string DetectIP(InterfaceType interfaceType);
	void InsertNumberToCharTable(unsigned char* charTable ,long number, int star, int limit);
	std::string TicketMessageToString(unsigned char* charTable);
	
};

#endif
