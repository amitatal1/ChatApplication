#include "Server.h"
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include "Helper.h"


using std::string;

Server::Server()
{

	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the server use UDP we will use: SOCK_DGRAM & IPPROTO_UDP
	_serverSocket = socket(AF_INET,  SOCK_STREAM,  IPPROTO_TCP); 

	if (_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");
}

Server::~Server()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(_serverSocket);
	}
	catch (...) {}
}

void Server::serve(int port)
{
	
	struct sockaddr_in sa = { 0 };
	
	sa.sin_port = htons(port); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	
	// Start listening for incoming requests of clients
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	std::cout << "Listening on port " << port << std::endl;
	std::cout << "Waiting for client connection request" << std::endl;

	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		try
		{ 
			acceptClient();
		}
		catch (...)
		{
		}
	}
}


void Server::acceptClient()
{

	// this accepts the client and create a specific socket from server to this client
	// the process will not continue until a client connects to the server
	SOCKET client_socket = accept(_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);
	
	std::cout << "Client accepted." << std::endl;
	
	// the function that handle the conversation with the client
	std::thread clientTh(&Server::clientHandler,this, client_socket);
	clientTh.detach();
}


void Server::clientHandler(const SOCKET clientSocket)
{
	printf("client met");
	try
	{
		int code=Helper::getMessageTypeCode(clientSocket);

		string userName = loginUser(clientSocket);

	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
		closesocket(clientSocket);
	}


}

string Server::loginUser(const SOCKET clientSocket)
{
	int nameLen = Helper::getIntPartFromSocket(clientSocket, 2); 
	string name = Helper::getStringPartFromSocket(clientSocket,nameLen);// retrieving name from socket

	_users[name] = clientSocket; // inserts user connection to the list of users;

	string namesList = "";
	for (auto& user : _users)
	{
		namesList += user.first;
		namesList += "&";
	}
	namesList.erase(namesList.size() - 1); // remove "&"


	std::cout<<name <<"\n";
	Helper::send_update_message_to_client(
		clientSocket,
		"",
		"",
		namesList
	);
	
	return name;
}

