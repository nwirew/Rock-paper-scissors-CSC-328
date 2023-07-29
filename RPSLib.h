/* ---------------------------------------------------------------------- *\
 *                                                                        *
 *         File: RPSLib.h                                                 *
 *       Author: Nathan Wilson Rew                                        *
 *  Description: Declaration of the RPSLib static class library and       *
 *                 implementation of its templated functions.
 *      Compile: g++ -c RPSLib.cpp -std=c++17                             *
 *               OR                                                       *
 *               make RPSLib.o                                            *
 *                                                                        *
\* ---------------------------------------------------------------------- */



#ifndef RPSLIB_H
#define RPSLIB_H

#include <string>
#include <iostream>
#include <cstring>

#include <sys/socket.h>	// For socket definitions
#include <netinet/in.h>	// For protocol definitions
#include <netdb.h>		// For base addrinfo-related definitions



namespace std
{
	class RPSLib
	{
	public:
		// Network message types
		enum Signal
		{
			CHOICE = 0,    GO     = 1,    NICK   = 2,
			READY  = 3,    RETRY  = 4,    SCORE  = 5,
			STOP   = 6
		};
		
		// Message data structure
		struct msgData
		{
			Signal type;
			string text;
		};
		
		static struct sockaddr_in sin;
		static int MESSAGE_SIZE;
		static int defaultPort;
		
		// Create a socket for the provided string URL and integer port
		static int getSock(string, unsigned short);
		
		// Receive a message from the network
		static struct msgData recvMessage(int, int = MESSAGE_SIZE);
		
		// Send a message across the network
		static void sendMessage(int, struct msgData);
		
		// Handles errors for functions that return a non-zero value on
		// error
		template<typename Func, typename... Args>
		static auto sysCall(Func, Args...);
		
		// Handles errors for functions that return 0 (aka nullptr) on error
		template<typename Func, typename... Args>
		static auto ptrCall(Func, Args...);
		
		// Test function for ptrCall and sysCall
		template<typename... Args>
		static int testFunc(Args... args);
	};
}



/* ------------------------------------------------------------------ *\
 * ------------------------------------------------------------------ * 
 *  Templated functions                                               *
 * ------------------------------------------------------------------ *
\* ------------------------------------------------------------------ */

using std::RPSLib;

/* ------------------------------------------------------------------ *\
 *                                                                    *
 *          Name: testFunc                                            *
 *   Description: Test function for error-checking wrapper functions. *
 *    Parameters: Args... args - Parameter pack arguments             *
 *  Return value: int - Not used right now.                           *
 *                                                                    *
\* ------------------------------------------------------------------ */
template <typename... Args>
int RPSLib::testFunc(Args... args)
{
	std::cout << "testFunc arg1 is " << (args<<...) << std::endl;
	return 0;
}

/* ------------------------------------------------------------------ *\
 *                                                                    *
 *          Name: sysCall                                             *
 *   Description: System call wrapper function that performs error    *
 *                  checking. Exit the program on error, otherwise    *
 *                  pass the system call's return value back to the   *
 *                  calling context.                                  *
 *    Parameters: Func func - The system function to be called.       *
 *                Args... args - Parameter pack containing arguments  *
 *                  for the requested function.                       *
 *  Return value: auto - The called function's return value.          *
 *                                                                    *
\* ------------------------------------------------------------------ */
template<typename Func, typename... Args>
auto RPSLib::sysCall(Func func, Args... args)
{
	// Execute the system function call
	decltype(func(args...)) ret = func(args...);
	
	// Check error conditions:
	// - Positive errno indicates an error code set by the syscall.
	if(errno)
	{
		perror("System call error");
		exit(ret);
	}
	
	return ret;
}



/* ------------------------------------------------------------------ *\
 *                                                                    *
 *          Name: ptrCall                                             *
 *   Description: Wrapper function that performs error checking. If   *
 *                  Exit the program on error, otherwise    *
 *                  pass the system call's return value back to the   *
 *                  calling context. Assumes system calls return      *
 *                  integer-like values.                              *
 *    Parameters: <template> Func func - The system function to be    *
 *                  called.                                           *
 *                <template> Args... args - Arguments for the         *
 *                  requested function.                               *
 *  Return value: int - The called function's return value.           *
 *                                                                    *
\* ------------------------------------------------------------------ */
template<typename Func, typename... Args>
auto RPSLib::ptrCall(Func func, Args... args)
{
	using namespace std;
	
	// Execute the pointer-returning function call
	decltype(func(args...)) ret = func(args...);
	
	//cout << "ptrCall got: " << ret
	//	 << " with args \"" << (args<<...) << "\"" << endl;
	
	// Check error conditions:
	// - nullptr shall be interpreted as failure.
	if(!ret)
	{
		cout << "Error: Pointer function returned nullptr" << endl;
		exit(1);
	}
	
	return ret;
}

#endif