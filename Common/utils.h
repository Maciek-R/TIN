#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <istream>
#include <arpa/inet.h>

enum class InterfaceType
{
	ETH,
	WLAN,
};

namespace Utils
{

	std::string ToString(unsigned char*, int, int);
	int ToInt(unsigned char*, int, int);
	int ToIntLimitedWithZero(unsigned char*, int, int);
	void LoadAddress(unsigned char *, std::string, int);
	std::string ToStr(unsigned char*, int, int);
	void InsertStringToCharTable(unsigned char* charTable, std::string newString, int start, int limit);
	std::string DetectIP(InterfaceType interfaceType);

	void InsertNumberToCharTable(unsigned char* charTable ,long number, int start, int limit);
	void InsertNumberToCharTableWithTerm(unsigned char* charTable ,long number, int start, int limit);
	
	std::string TicketMessageToString(unsigned char* charTable);

	std::string CalculateBroadCast(std::string ip, std::string subnetMask);

	int udpsock(int port, const char* addr);
	int recvudp(unsigned char* buffer, int sock, const int size, sockaddr_in& SenderAddr, int& SenderAddrSize);
	int sendudp(unsigned char* buffer, int size, sockaddr_in dest, int sock);
};

#endif
