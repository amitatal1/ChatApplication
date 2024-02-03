#pragma comment (lib, "ws2_32.lib")

#include "WSAInitializer.h"
#include "Server.h"
#include <iostream>
#include <exception>

#define PORT 8826

int main()
{
	try
	{
		WSAInitializer wsaInit;
		Server myServer;

		myServer.serve(PORT);
	}
	catch (std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}
	system("PAUSE");
	return 0;
}