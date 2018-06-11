#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFSIZE 1000
#define MAXSIZE 70000

char* serverType = "e";

void encrypt(char message[], char key[]) {
    char encChar;

    int keylen = strlen(key);
    int messagelen = strlen(message);


    for (int i=0; i<keylen; i++) {
        if (key[i] == ' ') {
            key[i] = '[';
        }
    }

    for (int i=0; i<messagelen; i++) {
        if (message[i] == ' ') {
            message[i] = '[';
        }
    }
    
    for (int i=0; i<messagelen; i++) {
        encChar = (message[i]-65 + key[i]-65) % 27;
        encChar += 65;
        message[i] = encChar;
    }
    
    for (int i=0; i<messagelen; i++) {
        if (message[i] == '[') {
            message[i] = ' ';
        }
    }
    return;
}
    

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFSIZE];
	struct sockaddr_in serverAddress, clientAddress;
    char key[MAXSIZE];
    char message[MAXSIZE];
    char input[MAXSIZE];
    int isFork = 0;
    int status = -1;
    int pid = -1;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    while(1) {
        // clean up other processes
        pid = waitpid(-1, &status, WNOHANG);
        while(pid > 0) {
            //printf("cleaned up fork\n");
            pid = waitpid(-1, &status, WNOHANG);
        }   

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0) error("ERROR on accept");

        // reset message and key arrays
        memset(message, '\0', MAXSIZE);
        memset(input, '\0', MAXSIZE);
        memset(key, '\0', MAXSIZE);

        // Get the message from the client and display it
        // TODO handle potential connection errors

        pid = fork();

        switch(pid) {
            case -1:
                perror("Error on fork");
                exit(1);

            case 0: 
                isFork=1;
                while (strstr(input, "@@") == NULL) {

                memset(buffer, '\0', BUFFSIZE);
                charsRead = recv(establishedConnectionFD, buffer, BUFFSIZE-1 , 0); // Read the client's message from the socket
                strcat(input, buffer);
                if (charsRead < 0) error("ERROR reading from socket");
                }
               
               // if message doesn't come from encrption client, reject connection
                if (strncmp(input, serverType, 1) != 0) {
                    memset(buffer, '\0', BUFFSIZE);
                    strcpy(buffer, "reject");
                    charsRead = send(establishedConnectionFD, buffer, BUFFSIZE, 0); // send rejection
                    close(establishedConnectionFD); // Close the existing socket which is connected to the client
                    close(listenSocketFD); // Close the listening socket
                    exit(0);
                } else {

                int fileNum = 0;
                int start = 0;
                for (int i=0; i<strlen(input); i++) {
                    if (input[i] == '\n') {
                        fileNum++;
                        if (fileNum == 2) {
                            strncpy(message, input+start+1, i-start-1);
                        }
                        if (fileNum == 3) {
                            strncpy(key, input+start+1, i-start-1);
                        }
                        start=i;
                    }
                }

                    
                encrypt(message, key);

//                    printf("SERVER: I received this from the client: \"%s\"\n", input);

                    // Send a Success message back to the client
                    charsRead = send(establishedConnectionFD, message, strlen(message), 0); // Send success back
                    if (charsRead < 0) error("ERROR writing to socket");
                
                    close(establishedConnectionFD); // Close the existing socket which is connected to the client
                    close(listenSocketFD); // Close the listening socket

                    exit(0);

                }
            } 
            
    }
    
	return 0; 
}
