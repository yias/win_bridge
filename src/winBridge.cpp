

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>

#include "ros/ros.h"
#include "win_bridge/vtmsg.h"


void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;

     int integer_msg;                           // variable to store the message
     int iResult;                               // variable to check the received data

     struct sockaddr_in serv_addr, cli_addr;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));


     portno = 11411; // port for listening the windows machine
     std::cout<<"Listening to port " << portno<<"\n";



     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");

     listen(sockfd,5);

     clilen = sizeof(cli_addr);


     // initialize the node
     ros::init(argc, argv, "win_bridge");

     ros::NodeHandle n;

     win_bridge::vtmsg msg;
     ros::Publisher winPub=n.advertise<win_bridge::vtmsg>("win_pub", 100);

     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) error("ERROR on accept");

     ros::Rate loop_rate(100);


     while (true) {

        // read for the client
        iResult=recv(newsockfd,&integer_msg,4,0);

        std::cout<<"bytes received: "<<iResult<<"\n";
        // check for correct message receive
        if(iResult>0){

            // set the message to the correct form
            msg.vote=ntohl(integer_msg);

            // publish the message
            winPub.publish(msg);

        }



        ros::spinOnce();
        if(!ros::ok()){
            break;
        }

        loop_rate.sleep();

     }



     //bzero(buffer,256);



     //n = read(newsockfd,buffer,255);

     //if (n < 0) error("ERROR reading from socket");

     //printf("Here is the message: %s\n",buffer);

     //n = write(newsockfd,"I got your message",18);
     //if (n < 0) error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);


     return 0;
}
