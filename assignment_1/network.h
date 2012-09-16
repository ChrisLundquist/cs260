#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

// We don't care what msvc says about strtok and printf
#define _CRT_SECURE_NO_WARNINGS
#include "windows.h"

//usual stuff
#include <iostream>
#include <string>

//winsock
#include "winsock2.h"

//need for getnameinfo() function
#include "Ws2tcpip.h"

// Hack because sockets aren't files in windows
#define close(x) closesocket(x);

//help the compiler figure out where winsock lives.  You could
//also add this to the linker but this means no 
//extra settings for the .csproj file
#pragma comment(lib, "ws2_32.lib")
#else

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#endif

#define BACKLOG 10   // how many pending connections queue will hold
#define BUFFER_SIZE 4096
#define PORT_LENGTH 6

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void read_config(char* address, char* port) {
    FILE* config = fopen("config.txt", "r");

	if(config == NULL) {
		fprintf(stderr,"Error reading config.txt\n");
		exit(-1);
	}
    fgets(address, INET6_ADDRSTRLEN, config);
    address = strtok(address," \t\r\n");
    fgets(port, PORT_LENGTH, config);
    port = strtok(port," \t\r\n");
    fclose(config);
}
