#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFFSIZE 1000
#define MAXSIZE 80000

//define client type
char* clientType = "e";


void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
    // variable declarations
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[MAXSIZE];
    char message[MAXSIZE];
    char sendBuffer[20];
    char* EOM = "@@";
    char* messageBreak = "\n";

    char* textFileName = argv[1];
    char* keyFileName = argv[2];

    int textFileLen;
    int keyFileLen;

    FILE* tfp;
    FILE* kfp;
    
    if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) error("CLIENT: ERROR opening socket");

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
        error("CLIENT: ERROR connecting");

    // add client type to beginning of message
    strcat(message, clientType);
    strcat(message, messageBreak);
    

    // read text file and append to message
    tfp = fopen(textFileName, "r");
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
    fgets(buffer, sizeof(buffer) - 1, tfp); // get text from file
    textFileLen = strlen(buffer);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
    // check for invalid characters in the file
    for (int i=0; i<strlen(buffer); i++) {
        if ((buffer[i] < 65 || buffer[i] > 90 ) && buffer[i] != ' ') {
            fprintf(stderr, "invalid character found in text file\n");
            exit(2);
        }
    }
    // add file contents to message
    strcat(message, buffer);
    strcat(message, messageBreak);
    fclose(tfp);


    // read keyfile and append to message
    kfp = fopen(keyFileName, "r");
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
    fgets(buffer, sizeof(buffer) - 1, kfp); // Get text from file
    keyFileLen = strlen(buffer);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
    // check for invalid characters in the file
    for (int i=0; i<strlen(buffer); i++) {
        if ((buffer[i] < 65 || buffer[i] > 90) && buffer[i] != ' ') {
            fprintf(stderr, "invalid character found in key file\n");
            exit(2);
        }
    }
    // add file contents to message
    strcat(message, buffer);
    strcat(message, messageBreak);
    fclose(kfp);

    // check key file is long enough for text file
    if (keyFileLen < textFileLen) {
        fprintf(stderr, "key file too short for text file\n");
        exit(2);
    }

    // add end of message terminator
    strcat(message, EOM);

    // Send message to server
    charsWritten = send(socketFD, message, strlen(message), 0); // Write to the server
    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
    if (charsWritten < strlen(message)) printf("CLIENT: WARNING: Not all data written to socket!\n");


    // Get return message from server
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end

    // if server is wrong type
    if (strcmp(buffer, "reject") == 0) {
        fprintf(stderr, "Wrong server type, cannot use otp_dec_d\n");
        exit(2);
    }

    // if not all content is read
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf( "%s\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}
