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

#define CHECK(X) ({int __val = (X); (__val == -1 ? \
                ({fprintf(stderr, "ERROR (" __FILE__ ":%d) -- %s\n",__LINE__, stderror(errno)); \
                exit(-1);-1;}) : __val); })

#define BUF 4096
#define PORT 8888

int abortRequested = 0;
int server_socket = -1;
int client_socket = -1;

void check(int retval, char* err){
    if(retval < 0) {
        fprintf(stderr, "%s", err);
        return EXIT_FAILURE;
        
    }
}


int main(){
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;

    int reuseValue = 1; // ??

    // create a socket
    CHECK(server_socket = socket(AF_INET, SOCK_STREAM, 0));


    //set socket options 1
    CHECK(setsockopt(server_socket,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &reuseValue,
                    sizeof(reuseValue)));
    
    //set socket options 2
    CHECK(setsockopt(server_socket,
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
    CHECK(bind(server_socket, (struct sockaddr*)&address, sizeof(address)));

    //allow connection establashing
    CHECK(listen(server_socket, 5));

    while(!abortRequested) {

        fprintf(stdout, "Waiting for connections...\n");

        //accept connection setup


    }

    return 0;

}