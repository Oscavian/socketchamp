#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUF 2048
#define PORT 8888

#define CHECKNTERM(X) ({int __val = (X); \
    if(__val == -1) { \
        fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n", __LINE__, strerror(errno)); exit(-1); \
    }})

#define CHECK(X) ({int __val = (X); \
    if(__val == -1) { \
        fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n", __LINE__, strerror(errno)); \
    }})

int main(int argc, char** argv){
    int client_socket;
    char buf[BUF];
    struct sockaddr_in address;
    int size;
    int isQuit;

    //create socket
    CHECKNTERM(client_socket = socket(AF_INET, SOCK_STREAM, 0));

    //init address
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if(argc < 2) {
        inet_aton("127.0.0.1", &address.sin_addr);
    } else {
        inet_aton(argv[1], &address.sin_addr);
    }

    //create connection
    CHECKNTERM(connect(client_socket,
                    (struct sockaddr*) &address,
                    sizeof(address)));

    
    fprintf(stdout, "Connection with server (%s) established\n", inet_ntoa(address.sin_addr));

    //receive data
    size = recv(client_socket, buf, BUF - 1, 0);

    if(size == -1) {
        perror("recv error");
    } else if (size == 0) {
        fprintf(stdout, "Server closed remote socket\n");
    } else {
        buf[size] = '\0';
        fprintf(stdout, "%s", buf);
    }

    do {
        buf[size] = '\0';
        fprintf(stdout, ">> ");

        if (fgets(buf, BUF, stdin) != NULL) {
            int size = strlen(buf);

            // remove new-line signs from string at the end
            if (buf[size - 2] == '\r' && buf[size - 1] == '\n') {
                size -= 2;
                buf[size] = 0;
            }
            else if (buf[size - 1] == '\n') {
                --size;
                buf[size] = 0;
            }
            isQuit = strcmp(buf, "quit") == 0;

            //send data
            if(send(client_socket, buf, size, 0) == -1) {

                perror("send error");
                break;
            }

            //receive feedback
            size = recv(client_socket, buf, BUF - 1, 0);

            if (size == -1) {
                perror("recv error");
                break;
            } else if(size == 0) {
                fprintf(stdout, "Server closed remote socket\n");
                break;
            } else {
                buf[size] = '\0';
                fprintf(stdout, "<< %s\n", buf);

                if(strcmp("OK", buf) != 0) {
                    fprintf(stderr, "<< Server error occured, abort\n");
                    break;
                }
            }
        }
        
    } while (!isQuit);

    if(client_socket != -1) {
        CHECK(shutdown(client_socket, SHUT_RDWR));
        CHECK(close(client_socket));
        client_socket = -1;
    }

    return EXIT_SUCCESS;


} 