#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <map>
#include "Message.h"
#include <queue>



class Server
{
public:
	Server();
	~Server();
	void serve(int port);
private:

	void acceptClient();
	void clientHandler(SOCKET clientSocket);
	string loginUser(SOCKET clientSocket);

	SOCKET _serverSocket;
	std::queue<Message> _msgQueue;
	std::map<string, SOCKET>  _users;
};

