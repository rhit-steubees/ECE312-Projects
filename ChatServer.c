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

void handle_communication(int); /* function prototype */

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, pid;
     struct sockaddr_in serv_addr, cli_addr;

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
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
//     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
        //  if (newsockfd < 0) 
        //      error("ERROR on accept");
        //  pid = fork();
        //  if (pid < 0)
        //      error("ERROR on fork");
        //  if (pid == 0)  {
        //      close(sockfd);
             handle_communication(newsockfd);
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
void handle_communication (int sock)
{
    int n;
    char buffer[256];
    char quit[256] = "quit\n";

        
    while(1){
        // Reads client message and prints it
        memset(buffer, 0, 256); //clear buffer
        n = read(sock,buffer,255); //read message from socket
        if (n < 0) error("ERROR reading from socket");
        if(strcmp(quit, buffer)==0){
            //Termination procedure
            print("Exiting communication.");
            exit(0);
        }
        printf("Here is the message: %s\n",buffer);  // print message
        
    
        // // Sends acknowledgement
        // n = write(sock,"I got your message",18);     // acknowledge
        // if (n < 0) error("ERROR writing to socket");

        // Send return message
        printf("Please enter the message: ");
        memset(buffer, 0, 256); //clear buffer
        fgets(buffer,255,stdin); // gather input        
        n = write(sock,buffer,strlen(buffer));
        if (n < 0) error("ERROR writing to socket");
        if(strcmp(quit, buffer)==0){
            //Termination procedure
            print("Exiting communication.");
            exit(0);
        }

        // // Read acknowledgement
        // memset(buffer, 0, 256); //clear buffer
        // n = read(sock,buffer,255); //read message from socket
        // if (n < 0) error("ERROR reading from socket");
        // printf("%s\n",buffer);

    }


}