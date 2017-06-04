#include "serviceserver.h"
#include <thread>
#include <vector>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include <openssl/aes.h>

void ufo(int s)
{
	std::cout << "Terminated\n";
	std::terminate();
}

int main(int argc , char *argv[])
{
	unsigned char in[12];
	unsigned char encrypted[12];
	unsigned char out[12];
	
	unsigned char keyText[] = "whateverwhatever\0";
	AES_KEY key;
	
	AES_set_encrypt_key(keyText, 128, &key);
	memcpy((char*)in, "ufo\0", 4);
	std::cout << "Plain text: " << in << "\n";

	AES_encrypt(in, encrypted, &key);
	std::cout << "Encrypted: " << encrypted << "\n";
	
	AES_set_decrypt_key(keyText, 128, &key);
	AES_decrypt(encrypted, out, &key);
	std::cout << "Decrypted: " << out << "\n";

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = ufo;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

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
