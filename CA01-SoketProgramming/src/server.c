#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <signal.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFER_SIZE 1024
#define MAX_CONNECTION_TO_THE_SERVER 50

#define TRUE  1
#define FALSE 0

#define DEFAULT_PORT 8081

int setupServer(int port) {
    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); //TCP
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    listen(server_fd, MAX_CONNECTION_TO_THE_SERVER);

    const char message[] ="Server is running.../nWaiting for clients...";
    write(STDOUT, message, sizeof(message));

    return server_fd;
}

int accept_client(int server_fd) {
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr *) &client_address, (socklen_t*) &address_len);

    const char message[] = "New client connected\n";
    write(STDOUT, message, sizeof(message));

    return client_fd;
}

int main(int argc, char const *argv[]) {
    char buffer[BUFFER_SIZE] = {0};
    
    if (argc <= 1) {
        const char message[] = "server on default port...!";
        write(STDOUT, message, sizeof(message));
    }
    int server_port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
    int server_fd = setupServer(server_port);

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    int max_fd = server_fd;
    FD_SET(server_fd, &master_set);   

    while(TRUE) {
        working_set = master_set;
        select(max_fd + 1, &working_set, NULL, NULL, NULL);
        
        for(int i = 0; i < max_fd + 1; i++) {

            
        }
    }
}


