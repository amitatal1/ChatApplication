#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <map>

using std::string;


class Server
{
public:
	Server();
	~Server();
	void serve(int port);
private:

	void acceptClient();
	void clientHandler(SOCKET clientSocket);
	   
	SOCKET _serverSocket;
	std::vector<std::thread> _clientsTh;
	std::map<string, SOCKET>  _users;
};

