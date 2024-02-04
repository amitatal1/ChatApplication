#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <map>
#include "Message.h"
#include <queue>
#include <mutex>
#include <condition_variable>

class Server
{
public:
	Server();
	~Server();
	void serve(int port);

private:

	void acceptClient();
	void clientHandler(const SOCKET clientSocket);
	string loginUser(const SOCKET clientSocket);
	void parseNewMsgReq(const SOCKET clientSocket, const string& userName);

	string connectedUserList() ;

	void writeToFile(Message& msg)
	void messagesHandling();

	//properties
	SOCKET _serverSocket;
	std::queue<Message> _msgQueue;
	std::map<string, SOCKET>  _users;


	//threads handling
	std::mutex _usersMx;
	std::mutex _msgsMx;
	std::condition_variable _msgcond;
};

