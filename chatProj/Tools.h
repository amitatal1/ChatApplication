#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>

static bool sendMessage(const SOCKET socket, const std::string& message)
{
	int msgBytes = message.size();
	const char* cMessage = message.c_str();
	int bytesSent = 0;
	while (bytesSent < msgBytes) 
	{
		int res = send(
			socket,
			cMessage + bytesSent, 
			msgBytes - bytesSent,
			0
			);
		if (res == SOCKET_ERROR) 
		{
			printf("Send Error");
			return false;
		}
		bytesSent += res;
	}
		return true;
}

static bool recvMessage(const SOCKET socket, const std::string& message)
{
	int msgBytes = message.size();
	const char* cMessage = message.c_str();
	int bytesSent = 0;
	while (bytesSent < msgBytes)
	{
		int res = send(
			socket,
			cMessage + bytesSent,
			msgBytes - bytesSent,
			0
		);
		if (res == SOCKET_ERROR)
		{
			printf("Send Error");
			return false;
		}
		bytesSent += res;
	}
	return true;
}