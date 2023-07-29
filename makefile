# File: makefile                                                           \
  Author: Nathan Wilson Rew                                                \
  Description: Compilation/linking commands for the CSC328 RPS group       \
                 project. Main commands of interest are as follows:        \
				 1. all - Create demo, client, and server programs         \
				 2. demo - Create library demo program from source code    \
				 3. client - Create client program from source code        \
				 4. server - Create server program from source code        \
				 5. clean - Delete all object and executable files         \
				 6. again - Clear the console during make invocation



all: demo client server



# Demo program linking
demo: RPSLib.o libDemo.o
	-g++ -o libDemo libDemo.o RPSLib.o

# Demo program object code
libDemo.o: RPSLib.h libDemo.cpp
	-g++ -c libDemo.cpp -std=c++17

# Library object code
RPSLib.o: RPSLib.h RPSLib.cpp
	-g++ -c RPSLib.cpp -std=c++17



# Client program linking
client: RPSLib.o rps_client.o
	-g++ -o rps_client rps_client.o RPSLib.o

# Client program object code
rps_client.o:
	-g++ -c rps_client.cpp -std=c++17



# Server program linking
server: RPSLib.o rps_server.o
	-g++ -o rps_server rps_server.o RPSLib.o

# Server program object code
rps_server.o:
	-g++ -c rps_server.cpp -std=c++17



clean:
	-rm *.o

again:
	@clear
	@ls
	@printf "\n"

# g++ -c filename.cpp
# g++ -o execname filename.o include1.o include2.o ...