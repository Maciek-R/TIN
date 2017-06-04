#include "serviceserver.h"
#include <thread>
#include <vector>

int main(int argc , char *argv[])
{
	ServiceServer service{1, 8889};
	ServiceServer service1{2, 8890};
	ServiceServer service2{3, 8891};
	ServiceServer service3{4, 8892};
	std::thread t{&ServiceServer::Run, &service};
	std::thread t1{&ServiceServer::Run, &service1};
	std::thread t2{&ServiceServer::Run, &service2};
	std::thread t3{&ServiceServer::Run, &service3};
	t.join();
	t1.join();
	t2.join();
	t3.join();
	return 0;
} 
