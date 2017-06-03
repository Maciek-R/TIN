#include "ticket.h"

Ticket::Ticket()
{}

void Ticket::SetClientAddress(std::string address)
{
	clientAddress = address;
}
void Ticket::SetServiceAddress(std::string address)
{
	serviceAddress = address;
}
void Ticket::SetServicePort(int port)
{
	servicePort = port;
}
void Ticket::SetServiceId(unsigned char id)
{
	serviceId = id;
}
unsigned char * Ticket::GetAsBuffor(int &size)
{
	size = 13;
	unsigned char * mess = new unsigned char[size];	//rozmiar sie zmieni potem
	Utils::LoadAddress(mess, clientAddress, 0);
	Utils::LoadAddress(mess, serviceAddress, 4);

	mess[8] = 8;	//tu zmienic
	mess[9] = 8;	//na metode ktora zamienia inta na 4 bajty
	mess[10] = 8;
	mess[11] = 9;

	mess[12] = serviceId;

	return mess;
}

void Ticket::SetValidateTime(int time)
{
	validateTime = time;
}
