
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

#define NET_ADDRESS "127.0.0.1"

#define STUDENT 1
#define TA 2

int role;
//-----------------------------------------------------------

int connect_to_server(int port) {
    struct sockaddr_in server_address;
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = inet_addr(NET_ADDRESS);
    
    const char error[] = "Error in connecting\n";
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        write(STDERR, error, sizeof(error));
    }
    else  { 
        const char message[] = "Welcome!\n";
        write(STDOUT, message, sizeof(message));
    }
    return client_fd;
}

void alarm_handler(int signal) {
    char message[BUFFER_SIZE] = "Too late\n";
    write(STDOUT, message, sizeof(message));
}

void set_role() {
    char message[] = "1. student\n 2.TA\n";
    write(STDOUT, message, sizeof(message));
    read(STDIN, role, 1);
}

void TA_handler() {

}
void student_handler() {

}

int main(int argc, char const *argv) {
    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);

    char buffer[BUFFER_SIZE] = {0};

    int server_fd;

    
    set_role();

    while(TRUE) {

    }
}   
