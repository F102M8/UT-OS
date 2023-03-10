#include "../include/const.h"

char buffer[BUFFER_SIZE] = {0};
fd_set master_set, working_set;

char role[ROLE_SIZE] = {0};
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

void welcome() {
    const char message[] = "Set your role:\n1.STUDENT (type set_S)\n2.TA      (type set_T)\n";
    write(STDOUT, message, sizeof(message));
}
void TA_handler() {

}
void student_handler() {
    const char message[] = "1. ask new question (type: ask)\n2. show meeting     (type: show_m)\n3. join to meeting  (type: join)\n";
    write(STDOUT, message, sizeof(message));
}

void ask_question(int server_fd) {
    send(server_fd, buffer, strlen(buffer), 0);
    recv(server_fd, buffer, BUFFER_SIZE, 0);
    write(STDOUT, buffer, strlen(buffer)); 
    memset(buffer, 0, BUFFER_SIZE);
}

int main(int argc, char const *argv) {
    //signal(SIGALRM, alarm_handler);
    //siginterrupt(SIGALRM, 1);

    if (argc <= 1) {
        const char message[] = "on default port...!\n";
        write(STDOUT, message, sizeof(message));
    }
    int server_port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
    int server_fd = connect_to_server(server_port);
    //int port = recieve_port(port);
    //int client_fd = setup(port);


    FD_ZERO(&master_set);
    int max_fd = server_fd;
    FD_SET(STDIN, &master_set);
    FD_SET(server_fd, &master_set); 

    welcome();

    while(true) {
        working_set = master_set;
        select(max_fd + 1, &working_set, NULL, NULL, NULL);

        for(int i = 0;i < max_fd + 1; i++) {
            if(FD_ISSET(i, &working_set)) {
                if(i == STDIN) {
                    read(STDIN, buffer, BUFFER_SIZE);
                    buffer[strlen(buffer) - 1] = '\0';
                    if((strcmp(buffer, STUDENT) == 0)) {
                        role[sizeof(STUDENT)] = STUDENT;
                        send(server_fd, buffer, strlen(buffer), 0);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));  
                        if (buffer[0] == '+'){ 
                            student_handler();
                        }
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    else if(strcmp(buffer, TA) == 0) {
                        role[sizeof(TA)] = TA;
                        send(server_fd, buffer, strlen(buffer), 0);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));  
                        if (buffer[0] == '+'){ 
                            TA_handler();
                        }
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    else if (strcmp(buffer, ASK_QUESTION) == 0) {
                        ask_question(server_fd);
                    }
                    else if (strcmp(buffer, SHOW_LIST_QUESTIONS) == 0) {
                        send(server_fd, buffer, strlen(buffer), 0);
                        memset(buffer, 0, BUFFER_SIZE);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer)); 
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    else {
                        send(server_fd, buffer, strlen(buffer), 0);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                }
                else if (i == server_fd) {
                    recv(server_fd, buffer, BUFFER_SIZE, 0);
                    write(STDOUT, buffer, strlen(buffer));
                    memset(buffer, 0, BUFFER_SIZE);
                }
            }
        }
    }
    return 0;
}