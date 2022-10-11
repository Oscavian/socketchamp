/* Core includes */
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define SERVER_PORT 80

#define MAXLINE 4096

int main(int argc, char** argv){

    int sockfd, n;
    int sendbytes;

    struct sockaddr_in servaddr;

    char payload[MAXLINE];
    char recvline[MAXLINE];

    if (argc != 2){
        perror("usage: ./webclient <ip>");
        return EXIT_FAILURE;
    }

    /**
     * create a new socket 
     * AF_INET - Address Family Internet
     * SOCK_STREAM - Stream Socket, contraiy would be datagram socket
     * 0 - TCP
     */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("error creating socket");
        return EXIT_FAILURE;
    }

    /* set all bits of servaddr to 0 */
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT); /* convert port number to network format */

    /* convert String IP into binary format */
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
        perror("inet_pton error");
        return EXIT_FAILURE;
    }

    /* open connection on socket file desc. at servaddr */
    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0){
        perror("connect error");
        return EXIT_FAILURE;
    }


    /* prepare payload */
    sprintf(payload, "GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(payload);

    /**
     * send the payload as request over the network
     * in this case there's only one try, normally you would retry until no -1 is returned
     */
    if (write(sockfd, payload, sendbytes) != sendbytes){
        perror("write error");
        return EXIT_FAILURE;
    }

    while( (n = read(sockfd, recvline, MAXLINE-1)) > 0) {

        fprintf(stdout, "%s", recvline);
        memset(recvline, 0, MAXLINE);
    }

    if (n < 0) {
        perror("read error");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;




    





    






}
