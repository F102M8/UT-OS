#include "../include/const.h"

int setup(int port) {
    int fd, broadcast = 1, opt = 1;
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in bc_address;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(port); 
    bc_address.sin_addr.s_addr = inet_addr(BROADCAST_ADDR);

    bind(fd, (struct sockaddr *)&bc_address, sizeof(bc_address));
    

    return fd;
}

int connect_to_server(int port) {
    struct sockaddr_in server_address;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
   
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = inet_addr(NET_ADDRESS);
    
    const char error[] = "Error in connecting\n";
    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        write(STDERR, error, sizeof(error));
    }
    else  { 
        const char message[] = "connected to the server!\n";
        write(STDOUT, message, sizeof(message));
    }
    return fd;
}

void alarm_handler(int signal) {
    char message[BUFFER_SIZE] = "Too late\n";
    write(STDOUT, message, sizeof(message));
}

char set_role() {
    char role;
    const char message[] = "Set your role:\n1.STUDENT (type S)\n2.TA      (type T)\n";
    write(STDOUT, message, sizeof(message));
    read(STDIN, role, 1);
    //error handeling 
    write(STDOUT, "Welcome!\n", sizeof("Welcome!\n"));
    return role;
}

void TA_handler() {

}
void student_handler() {

}

int main(int argc, char const *argv) {
    //signal(SIGALRM, alarm_handler);
    //siginterrupt(SIGALRM, 1);

    char buffer[BUFFER_SIZE] = {0};
    
    if (argc <= 1) {
        const char message[] = "On default port...!\n";
        write(STDOUT, message, sizeof(message));
    }
    
    int server_port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
    int server_fd = connect_to_server(server_port);
    //int port = recieve_port(port);
    //int client_fd = setup(port);

    char role = set_role();

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    int max_fd = server_fd;
    FD_SET(server_fd, &master_set); 

    while(TRUE) {
        working_set = master_set;
        select(max_fd + 1, &working_set, NULL, NULL, NULL);

        for(int i = 0;i < max_fd + 1; i++) {
            if(FD_ISSET(i, &working_set)) {
                if(i == STDIN) {
                    read(STDIN, buffer, BUFFER_SIZE);
                    if(buffer[0] == ASK_QUESTION) {
                        write(STDOUT, buffer, BUFFER_SIZE);

                        send(server_fd, buffer, strlen(buffer), 0);
                        memset(buffer, 0, BUFFER_SIZE);;
                    }
                    if(strcmp(buffer, SHOW_MEETINGS) == 0) {
                        send(server_fd, buffer, strlen(buffer), 0);
                        memset(buffer, 0, BUFFER_SIZE);
                        
                    }

                }
                else if (i == server_fd) {
                    recv(server_fd, buffer, BUFFER_SIZE, 0);
                    write(1, buffer, strlen(buffer));
                    memset(buffer, 0, BUFFER_SIZE);
                }
            }
        }
    }
    return 0;
}