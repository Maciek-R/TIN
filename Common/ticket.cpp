#include "ticket.h"
#include <string>

Ticket::Ticket()
{

}
const char* Ticket::ToCharArray()
{
	std::string ticket= "TICKET\0";
	return ticket.c_str();
}
