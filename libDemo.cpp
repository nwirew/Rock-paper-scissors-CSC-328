/* ---------------------------------------------------------------------- *\
 *                                                                        *
 *     File: libDemo.cpp                                                  *
 *   Author: Nathan Wilson Rew                                            *
 *  Purpose: Demonstrate usage of several library functions for the       *
 *             CSC328 RPS group project.                                  *
 *  Compile: g++ -o libDemo libDemo.o RPSLib.o                            *
 *           OR                                                           *
 *           make demo                                                    *
 *      Run: ./libDemo                                                    *
 *                                                                        *
\* ---------------------------------------------------------------------- */



#include "RPSLib.h"
#include <string>
#include <typeinfo>
#include <iostream>
#include <unistd.h>



int main(int argc, char** argv)
{
	std::string hostName = argc >= 2 ? argv[1] : "acad.kutztown.edu";
	
	//RPSLib::sysCall(RPSLib::testFunc, 5);
	
	// RPSLib::getSock sets up and returns a file descriptor for the
	// provided hostname and port number.
	int desc = RPSLib::getSock(hostName, 17);
	
	// ptrCall performs error checking for a function that returns a
	// pointer, returning said pointer if successful and terminating
	// execution if nullptr was returned. Check errors manually if you
	// expect nullptr as a valid return value!
	RPSLib::ptrCall(gethostbyname, hostName.c_str());
	
	// sysCall performs error checking for functions that set errno; it
	// terminates program execution if errno is set during the call to
	// its first argument, or returns that function's return value if
	// errno remains unset.
	/*RPSLib::sysCall(connect,
		desc,
		(struct sockaddr*)&RPSLib::sin,
		sizeof RPSLib::sin
	);*/
	connect(desc, (struct sockaddr*)&RPSLib::sin, sizeof RPSLib::sin);
	
	// Send a message of type NICK out from the socket.
	struct RPSLib::msgData msg =
	{
		.type = RPSLib::NICK,
		.text = "test name"
	};
	RPSLib::sendMessage(desc, msg);
	
	// recvMessage returns a string read from the file descriptor
	// specified by its argument. By default it reads 256 characters
	// from the buffer; this number can be changed temporarily by
	// passing a second argument to recvMessage or permanently by
	// assigning a different value to RPSLib::MESSAGE_SIZE. The current
	// demo has slightly garbled output here since QotD isn't designed
	// to be interpreted this way.
	struct RPSLib::msgData data = RPSLib::recvMessage(desc);
	std::cout << data.type << data.text << std::endl;
	
	exit(0);
}