/* ********************************************************************* */
/*    Author:           Tim Lentz                                        */
/*    Major:            Computer Science                                 */
/*    Creation Date:    11/4/2021                                        */
/*    Due Date:         12/2/2021                                        */
/*    Course:           csc328 010                                       */
/*    Professor Name:   Dr. Frye                                         */
/*    Assignment:       RPS Server                                       */
/*    File Name:        rps_server.cpp                                   */
/*    Purpose:          This program will utilize TCP and UDP sockets    */
/*                      in a client program.                             */
/*    Language Used:    C++                                              */
/*    Compile:          g++ -o server.out rps_server.cpp RPSLib.o        */
/*    Execute:          ./server.out                                     */
/* ********************************************************************* */
#include "RPSLib.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/wait.h>
#include <regex>

#define BACKLOG    2
#define HOSTNAME   "acad.kutztown.edu"
//pipes
#define READ    0
#define WRITE   1
#define MAXNICK 512
//RPS Game
#define ROCK      1
#define PAPER     2
#define SCISSORS  3
#define WIN       0
#define LOSE      1
#define TIE       3


//this is a work in progress
//print statements are for debuging
//server name for client should be acad.kutztown.edu, port 44444

using namespace std;
/*****************************************************************************/
/*    Funtction Name:   createPipes                                          */
/*    Description:      creat pipes for communication between child and      */
/*                      parent processes                                     */
/*    Parameters:       int[][] p2c:  parent to child pipes                  */
/*                      int[][] c2p:  child to child parent                  */
/*                                    outer array designates process         */
/*                                    inner array designates Read/Write      */
/*    Return Value:     void                                                 */
/*****************************************************************************/
void createPipes(int p2c[2][2], int c2p[2][2]);
/*****************************************************************************/
/*    Funtction Name:   closePipes                                           */
/*    Description:      close endspipes when they are no longer needed       */
/*    Parameters:       char      r:  'p' to close initial pipe ends parent  */
/*                                    'c' to close initial pipe ends child   */
/*                      int[][] p2c:  parent to child pipes                  */
/*                      int[][] c2p:  child to child parent                  */
/*                                    outer array designates process         */
/*                                    inner array designates Read/Write      */
/*    Return Value:     void                                                 */
/*****************************************************************************/
void closePipes(char r, int p2c[2][2], int c2p[2][2]);
/*****************************************************************************/
/*    Funtction Name:   parentProcess                                        */
/*    Description:      main section of code to be executed by the parent    */
/*    Parameters:       int  sockfd:  file descriptor of passive socket      */
/*                      int  turns:  number of turns of RPS                  */
/*                      int[][] p2c:  parent to child pipes                  */
/*                      int[][] c2p:  child to child parent                  */
/*                                    outer array designates process         */
/*                                    inner array designates Read/Write      */
/*    Return Value:     void                                                 */
/*****************************************************************************/
void parentProcess(int sockfd,int turns, int p2c[2][2], int c2p[2][2]);
/*****************************************************************************/
/*    Funtction Name:   childProcess                                         */
/*    Description:      main section of code to be executed by the child     */
/*    Parameters:       int  sockfd:  file descriptor of passive socket      */
/*                      int  connectfd:  file descriptor of connected socket */
/*                      int  turns:   number of turns of RPS                 */
/*                      int  pNum:    index number of player connected to    */
/*                                    this child                             */
/*                      int[][] p2c:  parent to child pipes                  */
/*                      int[][] c2p:  child to child parent                  */
/*                                    outer array designates process         */
/*                                    inner array designates Read/Write      */
/*    Return Value:     void                                                 */
/*****************************************************************************/
void childProcess(int connectfd, int sockfd, int pNum, int turns, int p2c[2][2], int c2p[2][2]);
/*****************************************************************************/
/*    Funtction Name:   checkNick                                            */
/*    Description:      checks the nicknames for uniquness                   */
/*    Parameters:       int[][] p2c:  parent to child pipes                  */
/*                      int[][] c2p:  child to child parent                  */
/*                                    outer array designates process         */
/*                                    inner array designates Read/Write      */
/*                      msgData nick[] : nicknames for both players          */
/*    Return Value:     void                                                 */
/*****************************************************************************/
int checkNick(int p2c[2][2], int c2p[2][2], struct RPSLib::msgData nick[2]);
/*****************************************************************************/
/*    Funtction Name:   referee                                              */
/*    Description:      compares player input to decide a winner or loser    */
/*    Parameters:       int[][] p2c:  parent to child pipes                  */
/*                                    outer array designates process         */
/*                                    inner array designates Read/Write      */
/*                      msgData nick[]: choices for both players             */
/*    Return Value:     void                                                 */
/*****************************************************************************/
int referee(struct RPSLib::msgData msg[2], int p2c[2][2]);
/*****************************************************************************/
/*    Funtction Name:   connectLoop                                          */
/*    Description:      accepts connections, forks, then exicutes            */
/*                      appropriate code for parent or child                 */
/*    Parameters:       int[][] p2c:  parent to child pipes                  */
/*                      int[][] c2p:  child to child parent                  */
/*                                    outer array designates process         */
/*                                    inner array designates Read/Write      */
/*                      int sockfd:   file descriptor for socket             */
/*                      int turns:    number of turns in each game           */
/*    Return Value:     void                                                 */
/*****************************************************************************/
void connectLoop(int sockfd,int turns, int p2c[2][2], int c2p[2][2]);



int main(int argc, char **argv){
    /****************************Command Line Args*****************************/
    int turns = 1;
    int portnum = RPSLib::defaultPort;
    
    if(2 > argc || argc > 3){
        cout<<"Usage: "<<argv[0]<<" [rounds] [port number]"<<endl;
        exit(1);
    }
    if(argc == 3){
        if(stoi(argv[2]) >= 0 && stoi(argv[2]) <= 65535){
            portnum = atoi(argv[2]);
        }else{
            cout<<"Usage: "<<argv[0]<<" [rounds] [port number]"<<endl;
            cout<<"[port number] must be an int from 0 to 65535"<<endl;
            exit(1);
        }
    }
    if(atoi(argv[1]) > 0){
        turns = atoi(argv[1]);
    }else{
        cout<<"Usage: "<<argv[0]<<" [rounds] [port number]"<<endl;
        cout<<"[rounds] must be an int greater than 0"<<endl;
        exit(1);
    }
    /****************************Command Line Args*****************************/   
    /***************************initialize variables***************************/
    // p2c: stores file descriptors refering to ends of parent to child pipe
    // c2p: stores file descriptors refering to ends of child to parent pipe
    int p2c[2][2], c2p[2][2];
    //number of players connected
    int numPlayers = 0;
    //file descripters for sockets
    int sockfd, connectfd;
    /***************************initialize variables***************************/
	
    //create a socket
    sockfd = RPSLib::getSock(HOSTNAME,portnum);
    
    //bind name to socket
    RPSLib::sysCall(bind,
		sockfd,
		(struct sockaddr *)&RPSLib::sin,
		sizeof(sockaddr_in)
	);
    
    //make socket passive
    RPSLib::sysCall(listen, sockfd, BACKLOG);
	
    //loop to accept connections (this is a Daemon Server)
    connectLoop(sockfd, turns, p2c, c2p);
    
    cout<<"ERROR, END OF PROCESS REACHED"<<endl;
    return 0;
}

void createPipes(int p2c[2][2], int c2p[2][2]){
    RPSLib::sysCall(pipe,p2c[0]);
    RPSLib::sysCall(pipe,p2c[1]);
    RPSLib::sysCall(pipe,c2p[0]);
    RPSLib::sysCall(pipe,c2p[1]);
}

void closePipes(char r, int p2c[2][2], int c2p[2][2]){
    if(r == 'p'){
        close(p2c[0][READ]);
        close(p2c[1][READ]);
        close(c2p[0][WRITE]);
        close(c2p[1][WRITE]);       
    }else{
        close(p2c[0][WRITE]);
        close(p2c[1][WRITE]);
        close(c2p[0][READ]);
        close(c2p[1][READ]);
    }
}

void parentProcess(int sockfd, int turns, int p2c[2][2], int c2p[2][2]){
    closePipes('p', p2c, c2p);
    struct RPSLib::msgData msgIn[2];
    struct RPSLib::msgData msgOut[2];
    struct RPSLib::msgData nick[2];
    int result;
    int score[2] = {0,0};
    
    //read READY from children
    for( int i = 0; i<2; i++){
        msgIn[i] = RPSLib::recvMessage(c2p[i][READ]);
    }
    //send READY back to children when both are confirmed
    for( int i = 0; i<2; i++){
        msgIn[i].type = RPSLib::READY;
        RPSLib::sendMessage(p2c[i][WRITE], msgIn[i]);
    }
    
    //get both nicknames
    nick[0] = RPSLib::recvMessage(c2p[0][READ]);
    nick[1] = RPSLib::recvMessage(c2p[1][READ]);
    //confirm uniquness of nicknames
    while(checkNick(p2c, c2p, nick)){
        nick[1] = RPSLib::recvMessage(c2p[1][READ]);
    }
    
    //play the input number of turns
    msgOut[0].type = RPSLib::READY;
    msgOut[1].type = RPSLib::READY;
    for (int a = 0; a < turns; a++){
        for( int i = 0; i<2; i++){
            msgIn[i] = RPSLib::recvMessage(c2p[i][READ]);
        }
        //get result for this round
        result = referee(msgIn, p2c);
        //set an appropriate message
        if(result == LOSE){
            msgOut[0].text = "LOSS: " + msgIn[0].text + " is beaten by " + msgIn[1].text;
            msgOut[1].text = "WIN: " + msgIn[1].text + " beats " + msgIn[0].text;
            score[1]++;
        }
        else if(result == WIN){
            msgOut[0].text = "WIN: " + msgIn[0].text + " beats " + msgIn[1].text;
            msgOut[1].text = "LOSS: " + msgIn[1].text + " is beaten by " + msgIn[0].text;
            score[0]++;
        }
        else{
            msgOut[0].text = msgIn[0].text + " ties with " + msgIn[1].text;
            msgOut[1].text = msgIn[1].text + " ties with " + msgIn[0].text;
        }
        if(a == turns-1){
            msgOut[0].type = RPSLib::SCORE;
            msgOut[1].type = RPSLib::SCORE;
        }
        //send the message
        RPSLib::sendMessage(p2c[0][WRITE], msgOut[0]);
        RPSLib::sendMessage(p2c[1][WRITE], msgOut[1]);
    }
    
    //send final score
    //wait untill both children signal ready
    RPSLib::recvMessage(c2p[0][READ]);
    //set the message type
    msgOut[0].type = RPSLib::SCORE;
    msgOut[1].type = RPSLib::SCORE;
    //set the message text
    if(score[0] < score[1]){
        msgOut[0].text = "YOU LOSE...\n" + nick[0].text + "'s score: " + to_string(score[0]) +
            "\n" +nick[1].text + "'s score: " + to_string(score[1]);
        msgOut[1].text = "YOU WIN!\n" + nick[0].text + "'s score: " + to_string(score[0]) +
            "\n" +nick[1].text + "'s score: " + to_string(score[1]);
    }
    else if(score[0] > score[1]){
        msgOut[0].text = "YOU WIN!\n" + nick[0].text + "'s score: " + to_string(score[0]) +
            "\n" +nick[1].text + "'s score: " + to_string(score[1]);
        msgOut[1].text = "YOU LOSE...\n" + nick[0].text + "'s score: " + to_string(score[0]) +
            "\n" +nick[1].text + "'s score: " + to_string(score[1]);
    }
    else{
        msgOut[0].text = "Tie..\n" + nick[0].text + "'s score: " + to_string(score[0]) +
            "\n" +nick[1].text + "'s score: " + to_string(score[1]);
        msgOut[1].text = "Tie...\n" + nick[0].text + "'s score: " + to_string(score[0]) +
            "\n" +nick[1].text + "'s score: " + to_string(score[1]);
    }
    //send the message
    RPSLib::sendMessage(p2c[0][WRITE], msgOut[0]);
    RPSLib::sendMessage(p2c[1][WRITE], msgOut[1]);
    
    //wait for both children to end
    for(int i = 0; i<2 ; i++){
        if(wait(NULL) == -1){
                perror("Error waiting for child process: ");
                exit(1);
            }
    }
    //close pipes
    closePipes('c', p2c, c2p);
}

void childProcess(int connectfd, int sockfd, int pNum, int turns, int p2c[2][2], int c2p[2][2]){
    //close unused ends of pipes
    closePipes('c', p2c, c2p);
    
    struct RPSLib::msgData msgOut =
	{
		.type = RPSLib::RETRY,
		.text = "childprocessready"
	};
	
    //recieve ready and send to parent
    RPSLib::sendMessage(c2p[pNum][WRITE], RPSLib::recvMessage(connectfd));
    
    //forward message from parent to client
    RPSLib::sendMessage(connectfd, RPSLib::recvMessage(p2c[pNum][READ]));

    do{
        //read nickname and send to parent
        RPSLib::sendMessage(c2p[pNum][WRITE], RPSLib::recvMessage(connectfd));
    
        //recieve respone from parent
        msgOut = RPSLib::recvMessage(p2c[pNum][READ]);
        
        //forward msg to client
        RPSLib::sendMessage(connectfd, msgOut);
    }while(msgOut.type == RPSLib::RETRY);
	
	RPSLib::recvMessage(connectfd);
    //send go to player
    msgOut.type = RPSLib::GO;
    RPSLib::sendMessage(connectfd, msgOut);
    //repeat for # of turns
    for(int i =0; i < turns; i++){
        
        msgOut = RPSLib::recvMessage(connectfd);
        RPSLib::sendMessage(c2p[pNum][WRITE], msgOut/*RPSLib::recvMessage(connectfd)*/);
        RPSLib::sendMessage(connectfd, RPSLib::recvMessage(p2c[pNum][READ]));
        RPSLib::recvMessage(connectfd);
    }
	
    //send final score
    RPSLib::sendMessage(c2p[pNum][WRITE], msgOut);
    RPSLib::sendMessage(connectfd, RPSLib::recvMessage(p2c[pNum][READ]/*,64*/));
    
    //send stop
    msgOut.type = RPSLib::STOP;
    RPSLib::sendMessage(connectfd,msgOut);
    
    //close and exit
    closePipes('p', p2c, c2p);
    close(sockfd);
    cout<<"Child "<<pNum<<" terminated"<<endl;
    exit(0);
}

int checkNick(int p2c[2][2], int c2p[2][2], struct RPSLib::msgData nick[2])
{
    struct RPSLib::msgData msgOut =
	{
		.type = RPSLib::READY,
		.text = "checknickready"
	};
    
    if(strcmp(nick[0].text.c_str(),nick[1].text.c_str())){
        RPSLib::sendMessage(p2c[0][WRITE], msgOut);
        RPSLib::sendMessage(p2c[1][WRITE], msgOut);
        return 0;
        
    }
    else{
        msgOut.type = RPSLib::RETRY;
        RPSLib::sendMessage(p2c[1][WRITE], msgOut);
        return 1;
    }
    
}

int referee(struct RPSLib::msgData msg[2], int p2c[2][2]){
    int choice[2];
    if(strncasecmp(msg[0].text.c_str(),msg[1].text.c_str(), 1)){
        for(int i = 0; i<2; i++){
            //determine rock, paper, or scissors
            if(strncasecmp(msg[0].text.c_str(),"r",1)){
                if(strncasecmp(msg[0].text.c_str(),"s",1)){
                    choice[i] = PAPER;
                }
                else{
                    choice[i] = SCISSORS;
                }
            }
            else{
                choice[i] = ROCK;
            }
        }
        //determine winner and loser
        if(choice[0] == ROCK && choice[1] == PAPER){
            return LOSE;
        }
        else if(choice[0] == PAPER && choice[1] == SCISSORS){
            return LOSE;
        }
        else if(choice[0] == SCISSORS && choice[1] == ROCK){
            return LOSE;
        }
        else{
            return WIN;
        }
    }
    else{
        return TIE;
    }
}



void connectLoop(int sockfd, int turns, int p2c[2][2], int c2p[2][2]){
    int connectfd, numPlayers = 0;
    struct sockaddr_in clientaddr;
    
	//create pipes
    createPipes(p2c, c2p);
    
    //len of struct for adress 
    socklen_t len = sizeof(struct sockaddr);
    
    // pid: process id of child processes
    pid_t pid[2];
    
    do{
        //accept connections
		cout<<"Accepting connections..."<<endl;
		connectfd = RPSLib::sysCall(accept,
			sockfd,
			(sockaddr*)&clientaddr,
			&len
		);
        //Fork when a client connects
		pid[numPlayers] = fork();
		if(pid[numPlayers] == -1)
		{
			perror("Fork error");
			exit(-1);
		}
        if(pid[numPlayers] > 0){//parent process
            numPlayers++;
        }//child process
        else{
            //do stuff for child process
            childProcess(connectfd,sockfd, numPlayers, turns, p2c, c2p);
        }
    }while(numPlayers<2);
    
    //do stuff for parent process
    parentProcess(sockfd, turns, p2c, c2p);
    
    //loop (this is a daemon server)
    connectLoop(sockfd, turns, p2c, c2p);
}
