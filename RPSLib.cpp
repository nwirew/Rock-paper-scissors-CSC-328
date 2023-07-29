/* ---------------------------------------------------------------------- *\
 *                                                                        *
 *         File: RPSLib.cpp                                               *
 *       Author: Nathan Wilson Rew                                        *
 *  Description: Implementation of non-templated functions in the RPSLib  *
 *                 static class library. See RPSLib.h for more            *
 *                 information.                                           *
 *                                                                        *
\* ---------------------------------------------------------------------- */



#include "RPSLib.h"
#include <iostream>
#include <unistd.h>
#include <regex>

#include <sys/socket.h>	// For socket definitions
#include <netinet/in.h>	// For protocol definitions
#include <netdb.h>		// For base addrinfo-related definitions



struct sockaddr_in RPSLib::sin;
int RPSLib::MESSAGE_SIZE = 256;
int RPSLib::defaultPort = 44444;

int RPSLib::getSock(string hostName, unsigned short port)
{
	// 1: Safely get host and TCP protocol information.
	struct hostent*  hinf = ptrCall(gethostbyname, hostName.c_str());
	struct protoent* pinf = ptrCall(getprotobyname, "tcp");
	
	// 2: Refresh the library's socket.
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	memcpy(&sin.sin_addr, hinf->h_addr, hinf->h_length);
	
	// 3: Safely get a descriptor for the socket.
	return sysCall(socket, AF_INET, SOCK_STREAM, pinf->p_proto);
}



struct RPSLib::msgData RPSLib::recvMessage(int desc, int numChars)
{
	// 1: Create a char buffer to store the message, then safely read
	// the message from the socket descriptor.
	char bucket[numChars];
	memset(bucket, 0, numChars*sizeof(char));
	int numread = sysCall(read, desc, bucket, numChars);
	
	// 2: Convert to string and split into type and text.
	string buckStr = string(bucket);
	struct msgData result = {
		.type = Signal(buckStr[0] - '0'),
		.text = buckStr.substr(1)
	};
	
	// 3: Return the result struct.
	return result;
}



void RPSLib::sendMessage(int desc, struct msgData msg)
{
	// 1: Safely write to the socket descriptor. The single-digit
	// message type first, the message text second.
	string data = to_string(msg.type) + msg.text;
	sysCall(write, desc, data.c_str(), data.length());
}
