/*  Author:         Fayssal Sallit
*   Major:          Computer Science
*   Due Date:       12/02/2021
*   Course:         CSC328 010
*   Professor:      Dr. Frye
*   Assignment:     RPS Client
*   Filename:       rps_client.cpp
*   Purpose:        This program is the client-side for 
*                   the game of rock, paper, scissors.
*   Programmed in:  C++
*   Compile:        g++ rps_client.cpp
*   Execute:        ./rps_client [hostname] [port number (optional)]
*/
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/wait.h>
#include "RPSLib.h"

//#define PORTNUM  44444  //default port number

int main (int argc, char **argv) {
	int port;
	std::string host;
	
	//Verify there are an appropriate amount of CLAs
	if (argc < 2 || argc > 3) {
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " [hostname] [port number]\n";
		return -1;
	}
	else if (argc == 3) {        //if port number given in CLA
		port = atoi(argv[2]);      //store port number
	}
	else {                    
		port = RPSLib::defaultPort;            //otherwise, use default port number
	}
	
	
	
	host = std::string(argv[1]);
	int sock = RPSLib::getSock(host, port);
	RPSLib::sysCall(connect,
		sock,
		(struct sockaddr*)&RPSLib::sin,
		sizeof RPSLib::sin
	);
	
	std::cout << "Waiting for other player..." << std::endl;
	
	//use library namespace
	struct RPSLib::msgData outgoing;
	struct RPSLib::msgData incoming;
	
	outgoing = {
		.type = RPSLib::READY,
		.text = ""
	};
	
	//tell server that client is ready
	RPSLib::sendMessage(sock, outgoing);
	int tries = 0;
	
	//check that server is ready
	incoming = RPSLib::recvMessage(sock);
    incoming.type = RPSLib::RETRY;
	std::string nick;
	std::string choice;
	int attempts = 0;
	
	//loop until valid/unique nickname is entered
	while(incoming.type !=  RPSLib::READY) {
		if(attempts > 0) {
			std::cout
				<< "The nickname you've entered is invalid or "
				<< "taken.\n";
		}
		attempts += 1;
		std::cout << "Please enter a valid nickname:  ";
		//store nickname in variable
		std::cin >> nick;
		std::cout<<"waiting for other player..."<<std::endl;
		//check if nickname is acceptable in server
		outgoing.type = RPSLib::NICK;
		outgoing.text = nick;
		RPSLib::sendMessage(sock, outgoing);
		incoming = RPSLib::recvMessage(sock);
	}
    RPSLib::sendMessage(sock, outgoing);
	
	incoming = RPSLib::recvMessage(sock);
	
	//check if server is ready to play
	if(incoming.type != RPSLib::GO) {
		std::cout << "Problem with server connection. Sorry!\n";
		return -1;
	}
	
	//until server sends the score, keep playing
	while (incoming.type != RPSLib::SCORE) {
		std::cout << "Please make your selection:\n";
		std::cout << "  R - rock\n  P - paper\n  S -  scissors\n";
		std::cin >> choice;
		
		//send the player's choice to server
		outgoing.type = RPSLib::CHOICE;
		outgoing.text = choice;
		RPSLib::sendMessage(sock, outgoing);
		incoming = RPSLib::recvMessage(sock);
        std::cout<<incoming.text<<std::endl;
        //send to reset
        //RPSLib::sendMessage(sock, outgoing);
	}
	
	//store then display the player's score
    std::cout<<incoming.text<<std::endl;
	incoming = RPSLib::recvMessage(sock);
	std::string score = incoming.text;
	std::cout << "---------------- FINAL SCORE ----------------\n " << score << "\n";
	
	while((incoming = RPSLib::recvMessage(sock)).type != RPSLib::STOP);
	close(sock);
	return 0;
}