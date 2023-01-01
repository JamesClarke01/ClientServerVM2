#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"
#include <unistd.h>

#define HOME_PAGE "HTTP/1.1 200 File Found\r\nContent-Length: 124\r\nConnection: close\r\n\r\n<HTML><HEAD><TITLE>File Found</TITLE></HEAD><BODY><h2>FILE Found</h2><hr><p>This file was sent by James' Awesome HTTP Server.</p></BODY></HTML>"

#define ERROR_PAGE "HTTP/1.1 404 File Not Found\r\nContent-Length:215\r\nConnection: close\r\n\r\n<HTML><HEAD><TITLE>File NoFound</TITLE></HEAD><BODY><h2>FILE Not Found</h2><hr><p>Your requestedFILE was not found.</p></BODY></HTML>"

static const int MAXPENDING = 5; //maximum number of outstanding connections

int main(int argc, char *argv[]) 
{
	char sendbuffer[BUFSIZE];
	char recvbuffer[BUFSIZE];
	int numBytes = 0;

	//for storing get request data
	char cmd[16];
	char path[64];
	char vers[16];

	if (argc != 2) //test for correct number of arguments
	{
		DieWithUserMessage("Parameter", "<Server Port>");
	}

	
	in_port_t servPort= atoi(argv[1]); //first arg, local port
	
	
	int servSock; //Socket descriptor for server
	
	//create socket for incomming connections
	if((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		DieWithSystemMessage("socket() failed");
	}
	
	// Construct local address structure
	struct sockaddr_in servAddr;                  // Local address
	memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
	servAddr.sin_family = AF_INET;                // IPv4 address family
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
	servAddr.sin_port = htons(servPort);          // Local port


	// Bind to the local address (this is done so socket will always be on port 80 for example)
	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
	{
		DieWithSystemMessage("bind() failed");
	}

	// Mark the socket so it will listen for incoming connections
	if (listen(servSock, MAXPENDING) < 0)
	{
		DieWithSystemMessage("listen() failed");
	}

	for (;;) //infinite loop
	{
		//wait for client to connect
		
		//accept
		int clntSock = accept(servSock, (struct sockaddr *) NULL, NULL);

		if (clntSock < 0)
		{
			DieWithSystemMessage("accept() failed");
		}
		
		//client socket is connected to a client!
				

		//recieve
		//recieve all data up to the buffer size-1, then stick a null terminator on the end
		while((numBytes = recv(clntSock, recvbuffer, BUFSIZE - 1, 0)) > 0)
		{
		
			recvbuffer[numBytes] = '\0'; //null terminate the recieve buffer
		
			
			if (strstr(recvbuffer, "\r\n\r\n") > 0)
			{
				break;
			}
			
		}
		if(numBytes < 0)
		{
			DieWithSystemMessage("recv() failed");
		}		
		
		

		//extracting HTTP components
		sscanf( recvbuffer, "%s %s %s", cmd, path, vers);
		
		printf("cmd %s\n", cmd);
		printf("path %s\n", path);
		printf("vers %s\n", vers);

		if(!strcmp(path, "/index.html")) //if index has been requested
		{
			printf("Path requested\n");

			snprintf(sendbuffer, sizeof(sendbuffer), HOME_PAGE); //put in outgoing buffer
		}
		else
		{
			snprintf(sendbuffer, sizeof(sendbuffer), ERROR_PAGE);
		}	

		ssize_t numBytesSent = send(clntSock, sendbuffer, strlen(sendbuffer),0);// send to the client
		


		close(clntSock); //close client socket
		
	

	}
	
	


}
