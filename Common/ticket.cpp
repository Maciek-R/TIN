#include "ticket.h"

Ticket::Ticket()
{

}
std::string Ticket::ToString()
{
	std::string ticket= "TICKET\0";
	return ticket;
}
