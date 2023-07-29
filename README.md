# Rock-paper-scissors-CSC-328

## Overview

Semester group project for Network and Secure Programming at Kutztown University, Fall 2021. This Unix application consists of two parts:
- A game server program that runs on the KU computer science department server (hardcoded). The main process accepts two connections at a time for a game session of rock, paper, scissors, forking a child process to serve each player. Once the match concludes, the child processes shut down and the parent process listens for another pair of connections. The parent process's listening port and the number of rounds in a match are configurable as CLI parameters.
- A game client program where players interact with each other. Players choose session-unique nicknames and wait to be paired with a partner, then play a match as configured by the server. The client plays a single match before shutting down. Players must specify the server's hostname as a CLI parameter, optionally followed by a connection port number.

The application also utilizes a work-saving static class library which defines a variety of things used by both the client and server.

## Team breakdown

Tim Lentz: Server

Fayssal Sallit: Client

Nathan Wilson Rew: Library
