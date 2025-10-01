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

char* construct_RHP_message(char version, char srcPort[], char dstPort[], char type, char message[]) {
    char length = strlen(message);
    char *new_message;
    // Allocate appropriate space
    int buffer_length = 0;
    if (length%16==0){//even number of octets in message, add a buffer
        int buffer_length = 1;
    }
    new_message = malloc(9+length+buffer_length+1);

    char *message_pointer;
    message_pointer = new_message;
    // Copy each input to the new message
    *message_pointer = version;
    message_pointer += 1;
    *message_pointer = srcPort;
    message_pointer += 2;
    *message_pointer = dstPort;
    message_pointer += 2;
    *message_pointer = (length<<4) + type;
    message_pointer += 2;
    if(buffer_length==1){
        *message_pointer = 00000000;
        message_pointer += 1;
    }
    strcpy(message_pointer, message);
    message_pointer += length; 
    *message_pointer = compute_checksum(new_message, 9+length+buffer_length);
    message_pointer += 2;


    printf("%s\n", message);
    printf("%s\n", new_message);
    return new_message;
}

char[] compute_checksum(char* data, int data_length){
    char total[3];  // Running sum
    char cur[3];    // variable for current 2 bytes
    for(int i = 0; i < data_length/2-1; i++){
        strncpy(cur, data+i, 2);    // Gather current 2 bytes
        cur*>>8;
        total* = total* + cur*;     
        if(total*>'FFFF'){
            total = total - '10000' + '1';
        }
    }

    // Invert total
    return 
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
    RHP_message = construct_RHP_message(12, "7418", "656C", 0, "RHP message received (missing buffer).");
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