/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define SERVER "137.112.38.47"
#define MESSAGE "hello"
#define PORT 2526
#define BUFSIZE 1024
char* RHP_message;

char* construct_RHP_message(int version, int srcPort, int dstPort, char message[]) {
    size_t len = strlen(message);
    char *new_message = malloc(len + 1 + 1);
    strcpy(new_message, message);
    new_message[len] = '.';
    new_message[len+1] = '\0';
    printf("%s\n", message);
    printf("%s\n", new_message);
    return new_message;
}


int main() {
    int clientSocket, nBytes;
    char buffer[BUFSIZE];
    struct sockaddr_in clientAddr, serverAddr;

    /*Create UDP socket*/
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket");
        return 0;
    }

    /* Bind to an arbitrary return address.
     * Because this is the client side, we don't care about the address 
     * since no application will initiate communication here - it will 
     * just send responses 
     * INADDR_ANY is the IP address and 0 is the port (allow OS to select port) 
     * htonl converts a long integer (e.g. address) to a network representation 
     * htons converts a short integer (e.g. port) to a network representation */
    memset((char *) &clientAddr, 0, sizeof (clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(0);

    if (bind(clientSocket, (struct sockaddr *) &clientAddr, sizeof (clientAddr)) < 0) {
        perror("bind failed");
        return 0;
    }

    /* Configure settings in server address struct */
    memset((char*) &serverAddr, 0, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /* send a message to the server */
    RHP_message = construct_RHP_message(MESSAGE);
    if (sendto(clientSocket, MESSAGE, strlen(MESSAGE), 0,
            (struct sockaddr *) &serverAddr, sizeof (serverAddr)) < 0) {
        perror("sendto failed");
        return 0;
    }

    /* Receive message from server */
    nBytes = recvfrom(clientSocket, buffer, BUFSIZE, 0, NULL, NULL);
    // printf("Received %d bytes:\n", nBytes);
    // char hex_message[nBytes/2];
    // for(int i = 0, i < nBytes, i++){
    //     hex_message += buffer[i];
    // }
    // printf("Version: ", hex_message[0:1]);
    // printf("\n");
    for (int i = 0; i < nBytes; i++) {
        unsigned char c = buffer[i];

        printf("%02X ", c);         // hex
    }
    printf("   ");
    for (int i = 0; i < nBytes; i++) {
        unsigned char c = buffer[i];
        printf("%c", (c >= 32 && c <= 126) ? c : '.'); // ASCII printable or dot
    }
    printf("\n");
    //printf("Received from server: %c\n", hex_message);
    close(clientSocket);
    return 0;
}