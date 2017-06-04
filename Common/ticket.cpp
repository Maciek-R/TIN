#include "ticket.h"
#include <iostream>

Ticket::Ticket()
{}

Ticket::Ticket(unsigned char* buffer) ///WAŻNE - UWZGLEDNIAM JAKIS 1 BAJT WIEC PRZESUNIETE
{
	SetClientAddress(Utils::ToString(buffer, 0, 4));
	SetServiceAddress(Utils::ToString(buffer, 4, 8));
	SetServicePort(Utils::ToInt(buffer, 8, 12));
	SetServiceId(buffer[12]);
	SetValidTime(Utils::ToIntLimitedWithZero(buffer, 13, 29));

	
	/*
	//ticket.SetValidateTime(Utils::ToStr(buffer,14,30));
		*/
		
	for(unsigned int i = 0; i < 16; ++i)
	{
		checkSum.push_back((int)buffer[29 + i]);
	}
	//ticket.SetCheckSum(checkSum);
	
	std::cout << "ClientAddress: " << clientAddress <<'\n';
	std::cout << "ServiceAddress: " << serviceAddress <<'\n';
	std::cout << "ServicePort: " << servicePort <<'\n';
	std::cout << "ServiceID: " << serviceId <<'\n';
	std::cout << "ValidTime: " << validTime <<'\n';
	
	std::string sum;
	
	for(unsigned int i = 0; i < 16; ++i)
	{
		sum += std::to_string(checkSum[i]);
		sum += " ";
	}
	
	std::cout << "CheckSum: " << sum <<'\n';
	


}


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
	size = 45;
	unsigned char * mess = new unsigned char[size];	//rozmiar sie zmieni potem
	Utils::LoadAddress(mess, clientAddress, 0);
	Utils::LoadAddress(mess, serviceAddress, 4);

	mess[8] = 8;	//tu zmienic
	mess[9] = 8;	//na metode ktora zamienia inta na 4 bajty
	mess[10] = 8;
	mess[11] = 9;

	mess[12] = serviceId;
	
	std::string validateTime = std::to_string(10);
	Utils::InsertStringToCharTable(mess, validateTime, 14, 29);

	for(unsigned int i = 0; i < 16 ; ++i)
	{
		mess[30+i] = checkSum[i];
		//std::cout << "chuj: " << checkSum[i] << "\n";
	}

	return mess;
}

void Ticket::SetValidateTime(std::string time)
{
	validateTime = time;
	std::cout << "Time set: " << time << "\n";
}

void Ticket::SetCheckSum(std::vector<int> checkSum)
{
	this->checkSum = checkSum;
}

unsigned char* Ticket::Serialize()
{
	unsigned char * result = new unsigned char[45];
	
	Utils::LoadAddress(result, clientAddress, 0);
	Utils::LoadAddress(result, serviceAddress, 4);
	Utils::InsertNumberToCharTable(result, servicePort, 8, 11); // check it
	result[12] = serviceId;
	Utils::InsertNumberToCharTableWithTerm(result, validTime, 13, 28);
	
	for(unsigned int i = 0; i < 16 ; ++i)
	{
		result[29+i] = checkSum[i];
	}
	
	return result;
}

void Ticket::SetValidTime(int validTime)
{
	this->validTime = validTime;
}

void Ticket::GenerateCheckSum() //TODO: seqfault !!!!!
{

	std::string ticketAsString = GenerateTicketInString();//MessageAsString = Utils::TicketMessageToString(mess);
		std::cout << "Generating checksum1: \n";
	unsigned char hash[16];
	unsigned char* newCheckSum = SHA1((unsigned char*)(ticketAsString.c_str()), ticketAsString.size(), hash);
	std::cout << "Generating checksum: \n";
	for(unsigned int i = 0; i < 16 ; ++i)
	{
		checkSum.push_back((int)newCheckSum[i]);
		//checkSum.push_back(1);
	}
		
}

std::string Ticket::GenerateTicketInString()
{
	std::string result;
	result += clientAddress;
	result += serviceAddress;
	result += std::to_string(servicePort);
	result += serviceId;
	result += std::to_string(validTime);
	return result;
}
