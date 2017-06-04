#include "utils.h"
#include <ifaddrs.h>
#include <assert.h>
#include <arpa/inet.h>
#include <iostream>

namespace Utils
{

	void LoadAddress(unsigned char * buffer, std::string adres, int from)
	{
		std::istringstream iss(adres);
		std::stringstream ss;
		std::string str;
		while(std::getline(iss, str, '.'))
			if(!str.empty())
			{
				ss.clear();
				ss << str;
				int a;
				ss >> a;
				buffer[from++] = a;
			}
	}


	std::string ToString(unsigned char * buff, int from, int to)
	{
		std::stringstream ss;

		for(size_t i=from; i<to-1; ++i)
		{
			ss << (int) buff[i];
			ss << ".";
		}
		ss << (int) buff[to-1];
		
		return ss.str();
	}

	//ucina reszte po napotkaniu pierwszego bajtu==0
	std::string ToStr(unsigned char * buff, int from, int to)
	{
		std::stringstream ss;

		for(size_t i=from; i<to; ++i)
		{
			if(buff[i] == 0)
				break;
			ss << (unsigned char) buff[i];
		}
		
		return ss.str();
	}

	int ToInt(unsigned char * buff, int from, int to)
	{
		std::stringstream ss;

		for(size_t i=from; i<to; ++i)
			ss << (int) buff[i];
		
		int a;
		ss >> a;

		return a;
	}

	void InsertStringToCharTable(unsigned char* charTable, std::string newString, int start, int limit)
	{
		unsigned char* currentChar = charTable + start;
		int difference = limit - start;
		
		unsigned int i;
		for(i = 0; (i < newString.size() && (i <= difference)); ++i)
		{
			*currentChar = newString[i];
			++currentChar;
		}
		
		*currentChar = '\0';
	}

	void InsertNumberToCharTable(unsigned char* charTable ,long number, int start, int limit)
	{
		std::string numberAsAString = std::to_string(number);
		
		unsigned char* currentChar = charTable + start;
		int difference = limit - start;
		
		unsigned int i;
		for(i = 0; (i < numberAsAString.size() && (i <= difference)); ++i)
		{
			*currentChar = numberAsAString[i] - '0';
			++currentChar;
		}
	}

	std::string DetectIP(InterfaceType interfaceType)
	{
		ifaddrs* ifAddrStruct = nullptr;
		void* tmpAddrPtr = nullptr;

		getifaddrs(&ifAddrStruct);

		std::string ip = "";
		for (ifaddrs* ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
		{
			if (ifa ->ifa_addr->sa_family==AF_INET)
			{
				tmpAddrPtr = &(/*(struct sockaddr_in *)*/reinterpret_cast<sockaddr_in*>(ifa->ifa_addr))->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
				std::string interfaceName{ifa->ifa_name};
				if(interfaceType == InterfaceType::ETH)
				{
					if(interfaceName == "enp9s0" || interfaceName == "eth0")
						ip = std::string{addressBuffer};
				}
				else if(interfaceType == InterfaceType::WLAN)
				{
					if(interfaceName == "wlp8s0" || interfaceName == "wlan0")
						ip = std::string{addressBuffer};
				}
			}
		}

		if (ifAddrStruct != nullptr)
			freeifaddrs(ifAddrStruct);

		assert(ip != "");
		return ip;
	}


	std::string TicketMessageToString(unsigned char* charTable)
	{
		std::string result;

		unsigned int i;
		for(i = 1; i<14; ++i)
		{
			result += std::to_string((int)charTable[i]);
		}
		
		while(charTable[i])
		{
			result += std::to_string(charTable[i]);
			++i;
		}
		
		return result;
	}
	
	std::string CalculateBroadCast(std::string ip, std::string subnetMask)
	{
		const char *host_ip = ip.c_str();
		const char *netmask = subnetMask.c_str();
		struct in_addr host, mask, broadcast;
		char broadcast_address[INET_ADDRSTRLEN];
		if (inet_pton(AF_INET, host_ip, &host) == 1 &&
			inet_pton(AF_INET, netmask, &mask) == 1)
		{
			broadcast.s_addr = host.s_addr | ~mask.s_addr;
		}
		else
		{
			std::cerr << "Failed converting strings to numbers\n";
			exit(1);
		}

		if (inet_ntop(AF_INET, &broadcast, broadcast_address, INET_ADDRSTRLEN) != NULL)
		{
			return std::string{broadcast_address};
		}
		else
		{
			std::cerr << "Failed converting number to string\n";
			exit(1);
		}

	}

	int ToIntLimitedWithZero(unsigned char* buff, int from, int to)
	{
		std::stringstream ss;

		for(size_t i=from; i<to; ++i)
			{
				if((int)buff[i]!=0)
				ss << (int) buff[i];
			}
		
		int a;
		ss >> a;

		return a;
	}
}
