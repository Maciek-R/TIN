#include "client.h"
 #include <iostream>

int main(int argc , char** argv)
{
	Client client; 
	//tu trzeba bedzie dorobic jakies menu wyboru opcji uslugi, poboru biletu, itp. itd.

	//to jest na razie szkielet w duzym uproszczeniu zeby ogarnac co ma sie dziac krok po kroku

	//
	//client.GetTicketServerAddress() // komunikat rozgloszeniowy
	//
	
	//logowanie usera
	
	
	
	if(client.GetTicketServerAddress())
	{
		if(client.GetTicket())
		{
			client.RunService(1);
		}
	}


	
	return 0;
} 
