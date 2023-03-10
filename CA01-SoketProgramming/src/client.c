#include "../include/const.h"

char buffer[BUFFER_SIZE] = {0};
fd_set master_set, working_set;

char role[ROLE_SIZE] = {0};

int connect_to_server(int port) {
    struct sockaddr_in server_address, bc_address;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1, broadcast = 1;
    
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = inet_addr(NET_ADDRESS);
    
    const char error[] = "Error in connecting\n";
    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        write(STDERR, error, sizeof(error));
        abort();
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
    const char message[] = "1. answer question        (type: ans)\n2. show list_questions    (type: ls)\n";
    write(STDOUT, message, sizeof(message));
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
void request_ans(int server_fd) {
    send(server_fd, buffer, strlen(buffer), 0);
    recv(server_fd, buffer, BUFFER_SIZE, 0);
    write(STDOUT, buffer, strlen(buffer)); 
    memset(buffer, 0, BUFFER_SIZE);
}

void meeting_handler() {

}
int conect_to_meeting(int port) {
    int sock, broadcast = 1, opt = 1;
    struct sockaddr_in bc_address;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(port); 
    bc_address.sin_addr.s_addr = inet_addr(NET_ADDRESS);

    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));

    return sock;
}

int main(int argc, char *argv[]) {
    //signal(SIGALRM, alarm_handler);
    //siginterrupt(SIGALRM, 1);
    
    if (argc <= 1) {
        const char message[] = "on default port...!\n";
        write(STDOUT, message, sizeof(message));
    }
    int server_port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
    
    int server_fd = connect_to_server(server_port);
    int meeting_fd = -1;

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
                    else if (strcmp(buffer, ANSWER) == 0) {
                        request_ans(server_fd);
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
                    buffer[strlen(buffer)] = '\0';
                    if (strcmp(buffer, REQ_CONNECT) == 0) {
                        const char msg[] = "*** NEW MEETING FOR YOU ***\n";
                        write(STDOUT, msg, sizeof(msg));
                        memset(buffer, 0, BUFFER_SIZE);
                        recv(server_fd, buffer, BUFFER_SIZE,0);
                        write(STDOUT, msg, sizeof(msg));
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    else {
                        write(STDOUT, buffer, strlen(buffer));
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                }
                else if (i == meeting_fd) {

                }
            }
        }
    }
    return 0;
}
