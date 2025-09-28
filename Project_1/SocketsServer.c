  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

  void error(char *msg)
  {
      perror(msg);
      exit(1);
  }

   int main(int argc, char *argv[])
   {
        int sockfd, newsockfd, portno, clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        if (argc < 2) {
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
        }
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
           error("ERROR opening socket");

        // bzero((char *) &serv_addr, sizeof(serv_addr));
        memset(&serv_addr, 0, sizeof(serv_addr)); // clear server address

        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0) 
                 error("ERROR on binding");
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
             error("ERROR on accept");
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");

        printf("Here is the message: %s\n",buffer);
        printf("Client IP: %s\n", inet_ntoa(cli_addr.sin_addr));
        printf("Server IP: %s\n", inet_ntoa(serv_addr.sin_addr));

        n = write(newsockfd,"I got your message",18);

        if (n < 0) error("ERROR writing to socket");
        
        printf("Please enter the message: ");
        bzero(buffer,256); // clear buffer 
        fgets(buffer,255,stdin); // gather input
        n = write(newsockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");
        bzero(buffer,256); // clear buffer
        n = read(newsockfd,buffer,255);
        if (n < 0) 
            error("ERROR reading from socket");
        printf("%s\n",buffer);
        return 0; 
   }


