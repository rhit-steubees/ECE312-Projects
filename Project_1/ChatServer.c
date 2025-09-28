/* A simple server for a messaging app in the internet domain using TCP
   The port number is passed as an argument 
   gcc server2.c 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

char username[256];

void handle_communication(int, char*); /* function prototype */

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, pid, n;
    struct sockaddr_in serv_addr, cli_addr;
    char* cli_ip;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    memset(&serv_addr, 0, sizeof(serv_addr)); // clear server address
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
    printf("Provide username: ");
    fgets(username,255,stdin); // gather username input 
    username[strlen(username)-1] = '\0'; //strip the newline from the username       
    printf("\nWaiting for connection...\n");
    fflush(stdout);
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    cli_ip = inet_ntoa(cli_addr.sin_addr);
    handle_communication(newsockfd, cli_ip);
    close(newsockfd);
    exit(0);
    return 0; 
}

/******** handle_communication() *********
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void handle_communication (int sock, char* cli_ip)
{
    int n, pid;
    char read_buffer[256], write_buffer[256];
    char quit[256] = "quit\n";  // special "quit" message
    char cli_username[256];

    n = write(sock,username,strlen(username)); // send username to client
    if (n < 0) error("ERROR writing to socket"); 

    n = read(sock,cli_username,255); //read client username
    if (n < 0) error("ERROR reading from socket");   

    printf("Connection established with %s (%s)\n", cli_ip, cli_username);
    fflush(stdout);
    pid = fork(); // Parent will read, child will write
    if (pid < 0) error("ERROR on fork");
    if (pid!=0){ // if parent, read messages from client
        while(1){
            // Reads client message and prints it
            memset(read_buffer, 0, 256); //clear buffer
            n = read(sock,read_buffer,255); //read message from socket
            if (n < 0) error("ERROR reading from socket");
            if(strcmp(quit, read_buffer)==0){
                //Termination procedure
                n = write(sock,quit,strlen(quit)); // send message to client to quit
                if (n < 0) error("ERROR writing to socket");
                kill(pid, SIGKILL); // Kill child process
                printf("Exiting communication.");
                exit(0);    // Exit
            }
            printf("\n<%s> %s", cli_username, read_buffer);  // print message
            fflush(stdout);
            printf("<%s> ", username);
            fflush(stdout);
        }
    }
    else{   // if child, write messages to client
        while(1){
            // Send return message
            printf("<%s> ", username);
            fflush(stdout);
            memset(write_buffer, 0, 256); //clear buffer
            fgets(write_buffer,255,stdin); // gather input        
            n = write(sock,write_buffer,strlen(write_buffer));
            if (n < 0) error("ERROR writing to socket");
        }
    }

}