#include "utils.h"

void Utils::loadAddress(unsigned char * buffer, std::string adres, int from)
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
//zmienie potem nazwe
std::string Utils::ToString(unsigned char * buff, int from, int to)
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
std::string Utils::ToStr(unsigned char * buff, int from, int to)
{
	std::stringstream ss;

	for(size_t i=from; i<to; ++i)
	{	if(buff[i] == 0)
			break;
		ss << (char) buff[i];
	}
	
	return ss.str();
}
int Utils::ToInt(unsigned char * buff, int from, int to)
{
	std::stringstream ss;

	for(size_t i=from; i<to; ++i)
		ss << (int) buff[i];
	
	int a;
	ss >> a;

	return a;
}