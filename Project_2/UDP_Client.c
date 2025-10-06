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
unsigned char* RHP_message;
uint16_t compute_checksum(unsigned char*, int);
void print_hex_and_text(unsigned char*, int);
void parse_RHP_message(unsigned char*, int);


unsigned char* construct_RHP_message(char version, int srcPort, int dstPort, char type, char message[]) {
    unsigned char length = strlen(message)+1;
    // Allocate appropriate space
    int buffer_length = 0;
    if ((length%2)==0){//even number of octets in message, add a buffer
        buffer_length = 1;
    }
    uint16_t checksum;
    unsigned char *new_message = malloc(9+length+buffer_length+1);
    // Populate the RHP message byte by byte
    new_message[0] = version;
    new_message[1] = srcPort&0x00FF;
    new_message[2] = (srcPort&0xFF00)>>8;
    new_message[3] = dstPort&0x00FF;
    new_message[4] = (dstPort&0xFF00)>>8;
    new_message[5] = length&0xFF;
    new_message[6] = (type<<4) + ((length&0xF00)>>8);
    int start = 7;
    if(buffer_length==1){   // Add buffer if needed
        new_message[7] = 0;
        start++;
    }
    int i;
    for(i = start; i < start + length; i++){    // loop to populate payload field
        new_message[i] = message[i-start];
    }
    
    checksum = compute_checksum(new_message, 9+length+buffer_length);
    new_message[i] = (checksum & 0xFF00) >> 8;
    new_message[i+1] = checksum & 0x00FF;
    return new_message;
}

// Function to compute checksum
uint16_t compute_checksum(unsigned char* data, int data_length){
    uint32_t total;    // Running sum
    uint16_t cur;      // variable for current 2 bytes
    total=0;
    cur=0;
    for(int i = 0; i < data_length; i+=2){
        uint16_t cur = ((uint8_t) data[i]<<8);
        cur |= (uint8_t) data[i+1];     // Gather current 2 bytes
        total += cur;                   // Add current 2 bytes to running total
        if(total>0x00FFFF){             // handle overflow
            total = total - 0x10000 + 0x1;
        }
    }
    total = ~(total)&0xFFFF;                // Invert total and mask          
    return (uint16_t) total;
}

int main() {
    int clientSocket, nBytes;
    unsigned char buffer[BUFSIZE];
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

    
    // RHP_message = construct_RHP_message(12, 0x1874, 0x6C65, 0, "RHP message received (missing buffer)");
    RHP_message = construct_RHP_message(12, 3044, 0x1874, 0, MESSAGE);
    int payload_length = (RHP_message[5]+((RHP_message[6]&0x0F)<<8));   // calculate payload length
    int message_length;
    if((payload_length%2)==0){
        message_length = payload_length + 10;
    }
    else{
        message_length = payload_length + 9;
    }                                                                   // calculate message length
    
    // print_hex_and_text(RHP_message, message_length);   // debug
    printf("Sending RHP message: %s\n", MESSAGE);
    for(int i = 0; i<10; i++){
        /* send a message to the server */
        if (sendto(clientSocket, RHP_message, message_length, 0, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) < 0) {
            perror("sendto failed");
            return 0;
        }
        /* Receive message from server */
        nBytes = recvfrom(clientSocket, buffer, BUFSIZE, 0, NULL, NULL);
        if(compute_checksum(buffer, nBytes)==0){
            printf("Checksum passed.\n");
            break;
        }
        else{
            printf("Checksum failed, resending message...\n");
        }
    }
    free(RHP_message);
    // print_hex_and_text(buffer, nBytes); // debug
    parse_RHP_message(buffer, nBytes);      // print parsed output
    close(clientSocket);
    return 0;
}

// Function that prints entire PDU for the layer in hex and ascii
void print_hex_and_text(unsigned char* buffer, int nBytes){
    printf("This is the hex: ");
    for (int i = 0; i < nBytes; i++) {
        unsigned char c = buffer[i];
        printf("%02X ", c);         // hex
    }
    printf("   \n\n");
    printf("This is the message: ");
    for (int i = 0; i < nBytes; i++) {
        unsigned char c = buffer[i];
        printf("%c", (c >= 32 && c <= 126) ? c : '.'); // ASCII printable or dot
    }
    printf("\n\n");
}


// Parses and prints the received message and metadata
void parse_RHP_message(unsigned char* buffer, int nBytes){
    int start;
    printf("Message received: ");
    int length = ((buffer[6]&0x0F)<<8)+(buffer[5]);
    if ((length%2)==0){//even number of octets in message, there is a buffer
        start = 8;
    }   
    else{
        start = 7;
    }
    int i;
    for(i = start; i<(start+length); i++) {
        unsigned char c = buffer[i];
        printf("%c", c);
    }
    printf("\nRHP version: %d\n", buffer[0]);
    printf("RHP type: %d\n", ((buffer[6]&0xF0)>>4));
    // printf("Communication ID: %d\n", buffer[0]);
    printf("length: %d\n", length);
    printf("checksum: 0x%02X%02X\n", (unsigned char) buffer[i], (unsigned char) buffer[i+1]);
    printf("Source Port: 0x%X%X\n", buffer[2], buffer[1]);
    printf("Destination Port: 0x%X%X\n", buffer[4], buffer[3]);
}