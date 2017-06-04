#include "client.h"
#include <iostream>



int main(int argc , char** argv)
{
	Client client; 

	int serviceID;
	while(true)
	{
		std::cout << "\t\tMENU\n"
				  << "Enter number to execute corresponding request\n\n"
				  << "\t1) TCP echo\n"
				  << "\t2) TCP time\n"
				  << "\t3) UDP echo\n"
				  << "\t4) UDP time\n"
				  << "\t0) Quit\n";
		std::cin >> serviceID;

		if(serviceID == 0)
		{
			std::cout << "Exitting\n";
			break;
		}
		else if(serviceID < 0 || serviceID > 4)
		{
			std::cout << "Invalid choice\n";
			continue;
		}

		client.RunService(serviceID);
	}
	
	return 0;
} 
