#include "Helper.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using std::string;

// Function to receive the type code of the message from the socket (3 bytes)
// Returns the code, or 0 if no message found (client disconnected)
int Helper::getMessageTypeCode(const SOCKET sc)
{
    std::string msg = getPartFromSocket(sc, 3, 0);

    if (msg == "")
        return 0;

    int res = std::atoi(msg.c_str());
    return res;
}

// Function to send a message to the socket
bool Helper::sendMessage(const SOCKET socket, const std::string& message)
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

// Function to send an update message to the client
void Helper::send_update_message_to_client(const SOCKET sc, const string& file_content, const string& second_username, const string& all_users)
{
    const string code = std::to_string(MT_SERVER_UPDATE);
    const string current_file_size = getPaddedNumber(file_content.size(), 5);
    const string username_size = getPaddedNumber(second_username.size(), 2);
    const string all_users_size = getPaddedNumber(all_users.size(), 5);
    const string res = code + current_file_size + file_content + username_size + second_username + all_users_size + all_users;
    sendData(sc, res);
}

// Function to receive an integer part from the socket according to byteSize
int Helper::getIntPartFromSocket(const SOCKET sc, const int bytesNum)
{
    return atoi(getPartFromSocket(sc, bytesNum, 0).c_str());
}

// Function to receive a string part from the socket according to byteSize
string Helper::getStringPartFromSocket(const SOCKET sc, const int bytesNum)
{
    return getPartFromSocket(sc, bytesNum, 0);
}

// Function to return a string after padding zeros if necessary
string Helper::getPaddedNumber(const int num, const int digits)
{
    std::ostringstream ostr;
    ostr << std::setw(digits) << std::setfill('0') << num;
    return ostr.str();
}

// Function to get a file name based on two user names
const string Helper::getFileName(const string& name1, const string& name2)
{
    if (name1 > name2)
    {
        return name1 + "&" + name2+ ".txt";
    }
    else
    {
        return name2 + "&" + name1+ ".txt";
    }
}

// Function to read the content of a file into a string
 std::string Helper::readFileToString(const std::string& filename)
{
    std::ifstream file(filename); // Open the file
    if (!file.is_open())
    {
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}

// Function to receive data from the socket according to byteSize
// This is a private function
std::string Helper::getPartFromSocket(const SOCKET sc, const int bytesNum)
{
    return getPartFromSocket(sc, bytesNum, 0);
}

// Function to send data to the socket
// This is a private function
void Helper::sendData(const SOCKET sc, const std::string message)
{
    const char* data = message.c_str();
    if (sendMessage(sc, data) == false)
    {
        throw std::exception("Error while sending message to client");
    }
}

// Function to receive data from the socket according to byteSize
// This is a private function
std::string Helper::getPartFromSocket(const SOCKET sc, const int bytesNum, const int flags)
{
    if (bytesNum == 0)
    {
        return "";
    }

    char* data = new char[bytesNum + 1];
    int res = recv(sc, data, bytesNum, flags);
    if (res == INVALID_SOCKET)
    {
        std::string s = "Error while receiving from socket: ";
        s += std::to_string(sc);
        throw std::exception(s.c_str());
    }
    data[bytesNum] = 0;
    std::string received(data);
    delete[] data;
    return received;
}
