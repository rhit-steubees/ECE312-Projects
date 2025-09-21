/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
   gcc server2.c 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    username[strlen(username)-1] = '\0' //strip the newline from the username       
    printf("\nWaiting for connection...\n");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
//     while (1) {

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        cli_ip = inet_ntoa(cli_addr.sin_addr);
        
        //  if (newsockfd < 0) 
        //      error("ERROR on accept");
        //  pid = fork();
        //  if (pid < 0)
        //      error("ERROR on fork");
        //  if (pid == 0)  {
        //      close(sockfd);
             handle_communication(newsockfd, cli_ip);
             close(newsockfd);
             exit(0);
   //      }
        //  else close(newsockfd);
    //  } /* end of while */
     return 0; /* we never get here */
}

/******** handle_communication() *********
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void handle_communication (int sock, char* cli_ip)
{
    int n;
    char buffer[256];
    char quit[256] = "quit\n";
    char cli_username[256];

    n = write(sock,username,strlen(username)); // send username to client
    if (n < 0) error("ERROR writing to socket"); 

    n = read(sock,cli_username,255); //read client username
    if (n < 0) error("ERROR reading from socket");   

    printf("Connection established with %s (%s)\n", cli_ip, cli_username);


    while(1){
        // Reads client message and prints it
        memset(buffer, 0, 256); //clear buffer
        n = read(sock,buffer,255); //read message from socket
        if (n < 0) error("ERROR reading from socket");
        if(strcmp(quit, buffer)==0){
            //Termination procedure
            printf("Exiting communication.");
            exit(0);
        }
        printf("<%s> %s\n", cli_username, buffer);  // print message
    

        // Send return message
        printf("<%s>", username);
        memset(buffer, 0, 256); //clear buffer
        fgets(buffer,255,stdin); // gather input        
        n = write(sock,buffer,strlen(buffer));
        if (n < 0) error("ERROR writing to socket");
        if(strcmp(quit, buffer)==0){
            //Termination procedure
            printf("Exiting communication.");
            exit(0);
        }

    }


}