#include "Server.h"
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include "Helper.h"
#include <fstream>

using std::string;

#define CREATE_MESSAGE(author, data) ("&MAGSH_MESSAGE&&Author&"  + author + std::string("&DATA&") + data+"\n")


#define ClientUpdateMessage 204
#define NAME_BYTES_NUM 2
#define CONTENT_BYTES_NUM 5

// Constructor
Server::Server()
{
    // Creating a TCP socket
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (_serverSocket == INVALID_SOCKET)
        throw std::exception(__FUNCTION__ " - socket");
}

// Destructor
Server::~Server()
{
    try
    {
        // Closing the server socket in the destructor
        closesocket(_serverSocket);
    }
    catch (...) {}
}

// Method to start the server and listen for incoming connections
void Server::serve(int port)
{
    struct sockaddr_in sa = { 0 };

    sa.sin_port = htons(port); // Port that server will listen for
    sa.sin_family = AF_INET;   // Must be AF_INET
    sa.sin_addr.s_addr = INADDR_ANY; // Listen on any available network interface

    // Binding the socket to the specified configuration (port and address)
    if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
        throw std::exception(__FUNCTION__ " - bind");

    // Start listening for incoming client connections
    if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
        throw std::exception(__FUNCTION__ " - listen");

    std::cout << "Listening on port " << port << std::endl;
    std::cout << "Waiting for client connection request" << std::endl;

    std::thread messagesHandle(&Server::messagesHandling, this);
    while (true)
    {
        // Accepting new clients in the main loop and handling them in a separate thread
        try
        {
            acceptClient();
        }
        catch (...)
        {
      
        }
    }
}

// Method to accept a client connection
void Server::acceptClient()
{
    // Accepting a client and creating a specific socket from the server to the client
    SOCKET client_socket = accept(_serverSocket, NULL, NULL);
    if (client_socket == INVALID_SOCKET)
        throw std::exception(__FUNCTION__);

    std::cout << "Client accepted." << std::endl;

    // Handling the client in a separate thread
    std::thread clientTh(&Server::clientHandler, this, client_socket);
    clientTh.detach();
}

// Method to handle communication with a specific client
void Server::clientHandler(const SOCKET clientSocket)
{
    printf("Client has been met\n");
    try
    {
        int code = Helper::getMessageTypeCode(clientSocket);

        {
            // Logging in the user and handling messages
            string userName = loginUser(clientSocket);
            try
            {

                while (true)
                {
                    code = Helper::getMessageTypeCode(clientSocket);
                    switch (code)
                    {
                    case ClientUpdateMessage:
                        parseNewMsgReq(clientSocket, userName);
                        break;

                    default:
                        break;
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cout << e.what();
                _users.erase(userName);
                closesocket(clientSocket);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }
}



// Method to log in a user and send updates to other clients
string Server::loginUser(const SOCKET clientSocket)
{
    int nameLen = Helper::getIntPartFromSocket(clientSocket, 2);
    string name = Helper::getStringPartFromSocket(clientSocket, nameLen); // Retrieving name from socket

    std::unique_lock<std::mutex> userMapLock(_usersMx);
    _users[name] = clientSocket; // Inserting user connection into the list of users;

    userMapLock.unlock();
    string namesList = connectedUserList();



    std::cout << name << "\n";
    Helper::send_update_message_to_client(
        clientSocket,
        "",
        "",
        namesList
    );

    return name;
}

// Method to parse a new message request and add it to the message queue
void Server::parseNewMsgReq(const SOCKET clientSocket, const string& userName)
{
    try
    {
        Message msg =
        {
            userName, // Sender name
            Helper::getStringPartFromSocket(clientSocket, Helper::getIntPartFromSocket(clientSocket, NAME_BYTES_NUM)), // Address
            Helper::getStringPartFromSocket(clientSocket, Helper::getIntPartFromSocket(clientSocket, CONTENT_BYTES_NUM)) // Content
        };

        string fileContent = "";

        //update request 
        if (msg.sender != "")
        {
            const string fileName = Helper::getFileName(userName, msg.address);
            fileContent = Helper::readFileToString(fileName);
        }

      


        Helper::send_update_message_to_client
        (
            clientSocket, 
            fileContent,
            msg.address,
            connectedUserList()
        );
    
        // new message 
        if (msg.content != "")
        {
            std::unique_lock<std::mutex> msgLock(_msgsMx);

                _msgQueue.push(msg);

            msgLock.unlock();

            _msgcond.notify_one(); // notfiy messages' handling 
        }
        
    }
    catch (const std::exception&)
    {
        return;
    }
}

string Server::connectedUserList()  // writing const is prevented by the mutex 
{
    std::unique_lock<std::mutex> userMapLock(_usersMx);

    string namesList = "";
    for (auto& user : _users)
    {
        namesList += user.first;
        namesList += "&";
    }
    userMapLock.unlock();

    namesList.erase(namesList.size() - 1); // Remove the trailing "&"

    return namesList;
}

void Server::writeToFile(Message& msg)
{
    //openning file 
    string fileName = Helper::getFileName(msg.address,msg.sender);
    std::ofstream chatFile(fileName, std::ios::app);

    string printintMsg = CREATE_MESSAGE(msg.sender, msg.content);

    // wrtie to file
    chatFile << printintMsg;

    
}

void Server::messagesHandling()
{
    while (true)
    {
        std::unique_lock<std::mutex> queueLock(_msgsMx);
        _msgcond.wait(queueLock, [this]() { return !_msgQueue.empty(); }); // wait untill new msg is entered 
        
        writeToFile(_msgQueue.back());

        _msgQueue.pop(); // remove message;

        queueLock.unlock();
    }


}
