#include "ticketserver.h"
#include <stdlib.h>
#include <time.h>
 
int main(int argc , char *argv[])
{

	srand(time(nullptr));
	TicketServer ticketServer; 
	ticketServer.Run();
	return 0;
} 
