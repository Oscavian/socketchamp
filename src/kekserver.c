#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/*
#define CHECKNTERM(X) ({int __val = (X); (__val ==-1 ? \
                ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n", __LINE__, strerror(errno)); \
                exit(-1);-1;}) : __val); })
*/

#define CHECKNTERM(X) ({int __val = (X); \
    if(__val == -1) { \
        fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n", __LINE__, strerror(errno)); exit(-1); \
    }})

#define CHECKNNULL(X) ({int __val = (X); \
    if(__val == -1) { \
        fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n", __LINE__, strerror(errno)); return NULL; \
    }})

#define CHECK(X) ({int __val = (X); \
    if(__val == -1) { \
        fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n", __LINE__, strerror(errno)); \
    }})



#define BUF 2048
#define PORT 8888

int abortRequested = 0;
int server_socket = -1;
int client_socket = -1;

void* clientCommunication(void* data);
void signalHandler(int sig);

int main(){
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;

    int reuseValue = 1; // ??

    // create a socket
    CHECKNTERM(server_socket = socket(AF_INET, SOCK_STREAM, 0));


    //set socket options 1
    CHECKNTERM(setsockopt(server_socket,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &reuseValue,
                    sizeof(reuseValue)));
    
    //set socket options 2
    CHECKNTERM(setsockopt(server_socket,
                SOL_SOCKET,
                SO_REUSEPORT,
                &reuseValue,
                sizeof(reuseValue)));
    
    //init address
    memset(&address, 0, sizeof(address)); //zero out bits
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //assign address+port to socket
    CHECKNTERM(bind(server_socket, (struct sockaddr*)&address, sizeof(address)));

    //allow connection establashing
    CHECKNTERM(listen(server_socket, 5));

    while(!abortRequested) {

        fprintf(stdout, "Waiting for connections...\n");

        //accept connection setup

        addrlen = sizeof(struct sockaddr_in);

        CHECK(client_socket = accept(server_socket, (struct sockaddr*) &cliaddress, &addrlen));

        //TODO: abort handling
        // if(abortRequested){}

        //start client
        fprintf(stdout, "Client connected from %s:%d...\n",
                inet_ntoa(cliaddress.sin_addr),
                ntohs(cliaddress.sin_port));


        //connection established, start communicating...
        clientCommunication(&client_socket);

        //reset socket
        client_socket = -1;
    }

    return 0;

}

void* clientCommunication(void* data){

    char buf[BUF];
    int size;
    int* current_socket = (int*) data;

    //send welcome
    strcpy(buf, "Welcome! Please enter your commands...\r\n");
    CHECKNNULL(send(*current_socket, buf, strlen(buf), 0));

    do {
        //receive data

        size = recv(*current_socket, buf, BUF - 1, 0);

        if (size == -1) {
            if (abortRequested) {
                perror("recv error after aborted");
            } else {
                perror("recv error");
            }
            break;
            
        }

        if (size == 0) {
            fprintf(stdout, "Client closed remote socket\n");
            break;
        }

        //remove newline from client message
        if(buf[size - 2] == '\r' && buf[size -1] == '\n'){
            size -= 2;
        } else if(buf[size - 1] == '\n'){
            size -= 1;
        }

        //null terminate message & print to stdout
        buf[size] = '\0';
        fprintf(stdout, "Message received: %s\n", buf);

        //send confirmation msg
        CHECKNNULL(send(*current_socket, "OK", 3, 0));

    } while(strcmp(buf, "quit") != 0 && !abortRequested);

    if(*current_socket != -1) {

        CHECK(shutdown(*current_socket, SHUT_RDWR));
        CHECK(close(*current_socket));

    }

    return NULL;

}

void signalHandler(int sig) {
    if (sig == SIGINT) {
        fprintf(stdout, "Abort Requested...");
        abortRequested = 1;

        if (client_socket != -1) {
            CHECK(shutdown(client_socket, SHUT_RDWR));
            CHECK(close(client_socket));
            client_socket = -1;
        }

        if(server_socket != -1) {
            CHECK(shutdown(server_socket, SHUT_RDWR));
            CHECK(close(server_socket));
            server_socket = -1;
        }
    } else {
        exit(sig);
    }


}

