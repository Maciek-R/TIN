#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <istream>

class Utils
{
public:

	static std::string ToString(unsigned char*, int, int);
	static int ToInt(unsigned char*, int, int);
	static void loadAddress(unsigned char *, std::string, int);

};

#endif
