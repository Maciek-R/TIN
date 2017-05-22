#include "ticket.h"

Ticket::Ticket()
{

}
char* Ticket::ToCharArray()
{
    //char[10] buffer;
    char * ticket= "TICKET\0";

    //strncpy(buffer, ticket, 7);

    return ticket;
}