#include<iostream>
#include<unistd.h>
#include<sys/socket.h>
#include <netdb.h>

/**
citations:
    1) TCP example by Brian Rogers, updated by Rabih Younes. Duke University.
*/

int main(int argc, char **argv){
    if(argc <4){
        std::cerr<<"Required no of arguments is not provided for this function"<<std::endl;
        return EXIT_FAILURE;
    }
    
    char * port = argv[1];
    char * no_players = argv[2];
    char * no_hops = argv[3];

    struct addrinfo hints;
    struct addrinfo *hosts;

    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AF_UNSPEC; //To return address family from both IPV4 and IPV6.
    hints.ai_socktype = SOCK_STREAM; //Connection based protocol (i.e. TCP).
    hints.ai_flags = AI_PASSIVE; // Return Socket will be suitable for bind and accept.

    int status = getaddrinfo(NULL, port, &hints, &hosts);
    if(status != 0){
        std::cerr<<"Error cannot get the addresses"<<std::endl;
        return EXIT_FAILURE;
    }

    int socket_fd = socket(hosts->ai_family, hosts->ai_socktype, hosts->ai_protocol);
    if(socket_fd == -1){
        std::cerr<<"Error cannot create socket"<<std::endl;
        return EXIT_FAILURE;
    }

    int yes =1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    status = bind(socket_fd, hosts->ai_addr, hosts->ai_addrlen);
    if(status == -1){
        std::cerr<<"Error cannot bind socket"<<std::endl;
        return EXIT_FAILURE;
    }

    status = listen(socket_fd, 100);
    if(status ==-1){
        std::cerr<<"Error cannot listen on socket"<<std::endl;
        return EXIT_FAILURE;
    }

    std::cout<<"Waiting for connection"<<std::endl;
    int client_connection_fd = accept(socket_fd, NULL, NULL);
    if(client_connection_fd == -1){
        std::cerr<<"Error cannot accept connection on socket"<<std::endl;
        return EXIT_FAILURE;
    }

    char buffer[512];
    recv(client_connection_fd, buffer, 512, 0);

    std::cout<<"Server received: "<<buffer<<std::endl;

    freeaddrinfo(hosts);
    close(socket_fd);

    return EXIT_SUCCESS;
}