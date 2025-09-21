   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netdb.h> 
   #include <arpa/inet.h>

   void error(char *msg)
   {
       perror(msg);
       exit(0);
   }

   int main(int argc, char *argv[])
   {

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256], username[256], serv_username[256];
    char quit[256] = "quit\n";

    if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    //  bzero((char *) &serv_addr, sizeof(serv_addr));
    memset(&serv_addr, 0, sizeof(serv_addr)); // clear server address

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Provide username: ");
    fgets(username,255,stdin); // gather username input   
    username[strlen(username)-1] = '\0' //strip the newline from the username
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");

    n = read(sockfd,serv_username,255); //read server username
    if (n < 0) error("ERROR reading from socket");

    n = write(sockfd,username,strlen(username)); // send username to server
    if (n < 0) error("ERROR writing to socket"); 
   
    printf("\nConnection established with %s (%s)\n", inet_ntoa(serv_addr.sin_addr), serv_username);

    while(1){

        // Send message
        printf("<%s>", username);
        memset(buffer, 0, 256); //clear buffer
        fgets(buffer,255,stdin); // gather input
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) error("ERROR writing to socket");
        if(strcmp(quit, buffer)==0){
            //Termination procedure
            printf("Exiting communication.");
            exit(0);
        }

        // // Read acknowledgement
        // memset(buffer, 0, 256); //clear buffer
        // n = read(sockfd,buffer,255); //read message from socket
        // if (n < 0) error("ERROR reading from socket");
        // printf("%s\n",buffer);

        // Reads server return message and prints it
        memset(buffer, 0, 256); //clear buffer
        n = read(sockfd,buffer,255); //read message from socket
        if (n < 0) error("ERROR reading from socket");
        if(strcmp(quit, buffer)==0){
            //Termination procedure
            printf("Exiting communication.");
            exit(0);
        }
        printf("<%s> %s\n",serv_username, buffer);  // print message

        // // Send acknowledgement
        // n = write(sockfd,"I got your message",18);     // acknowledge
        // if (n < 0) error("ERROR writing to socket");



    }   
    //  printf("Please enter the message: ");
    //  bzero(buffer,256);
    //  fgets(buffer,255,stdin);
    //  n = write(sockfd,buffer,strlen(buffer));
    //  if (n < 0) 
    //       error("ERROR writing to socket");

    //  //Read acknowlegement
    //  bzero(buffer,256); // clear buffer
    //  n = read(sockfd,buffer,255);
    //  if (n < 0) error("ERROR reading from socket");
    //  printf("%s\n",buffer);


    //  //Read return message
    //  bzero(buffer,256); // clear buffer
    //  n = read(sockfd,buffer,255);
    //  if (n < 0) error("ERROR reading from socket");
    //  printf("%s\n",buffer);

    //  n = write(sockfd,"I got your message",18);
     
     return 0;
   }

