#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <istream>

namespace Utils
{

	std::string ToString(unsigned char*, int, int);
	int ToInt(unsigned char*, int, int);
	void loadAddress(unsigned char *, std::string, int);
	std::string ToStr(unsigned char*, int, int);
};

#endif
