#include "../include/const.h"
char buffer[BUFFER_SIZE] = {0};
int TA_fds[MAX_TA];
int student_fds[MAX_STUDENT];
int student_count = 0, TA_count = 0;

struct Question {
    char Q_text[BUFFER_SIZE];
    int status;
    int meeting_port;
    int fd_S;
    int fd_TA;
};

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

    const char message[] ="Server is running...\nWaiting for clients...\n";
    write(STDOUT, message, sizeof(message));
    return server_fd;
}
int accept_client(int server_fd) {
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr *) &client_address, (socklen_t*) &address_len);

    const char message[] = "New client accepted.\n";
    write(STDOUT, message, sizeof(message));

    return client_fd;
}
bool has_role(int fd,int fd_set[]) {
    for (int i = 0; i < sizeof(fd_set); i++) {
        if (fd == fd_set[i])
            return true;
    }
    return false;
}
void set_role(char role[], int fd, int role_set[], int index) {
    role_set[index] = fd; 
    char message[BUFFER_SIZE] = {0};
    if (role == STUDENT)
        sprintf(message ,"$ client fd = %d: set as student.\n", fd);
    else
        sprintf(message ,"$ client fd = %d: set as TA.\n", fd);
    write(STDOUT, message, sizeof(message));
    send(fd, message, sizeof(message), 0);
    index ++;
}
void remove_client(int fd) {

}

void submit_question(int fd) {

}
void ask_question(int fd) {
    if(!has_role(fd, student_fds)) {
        if(has_role(fd, TA_fds)) {
            const char message[] = "- seriously?! \n";
            send(fd, message, sizeof(message), 0);
        }
        else {
            const char message[] = "- First set your role! \n";
            send(fd, message, sizeof(message), 0);
        }
        send(fd, REJECT, sizeof(REJECT), 0);
    }
    else {
        const char message[] = "- Ask...\n";
        send(fd, message, sizeof(message), 0);
        send(fd, ACCEPT, sizeof(ACCEPT), 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
}
void make_new_meeting() {


}

void add_client_to_meeting() {

}

void change_question_status() {

}

int main(int argc, char const *argv[]) {
    struct Question questions[MAX_QUESTIONS];
    int question_count = 0;
    if (argc <= 1) {
        const char message[] = "server on default port...!\n";
        write(STDOUT, message, sizeof(message));
    }
    int server_port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
    int server_fd = setupServer(server_port);

    fd_set master_set, working_set;
    
    FD_ZERO(&master_set);
    int max_fd = server_fd;
    FD_SET(server_fd, &master_set);   

    int new_client_fd;


    while(true) {
        working_set = master_set;
        select(max_fd + 1, &working_set, NULL, NULL, NULL);
        for(int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) { 
                if (i == server_fd) {
                    new_client_fd = accept_client(server_fd);
                    FD_SET(new_client_fd, &master_set);
                    max_fd = new_client_fd > max_fd ? new_client_fd : max_fd;
                    char message[BUFFER_SIZE] = {0};
                    sprintf(message ,"New client connected. fd = %d\n", new_client_fd);
                    write( STDOUT, message, sizeof(message));
                }
                else {
                    memset(buffer, 0, BUFFER_SIZE);
                    int bytes_received = recv(i, buffer, BUFFER_SIZE, 0);
                    if (bytes_received == 0) { 
                        char message[BUFFER_SIZE] = {0};
                        sprintf(message ,"Close client fd = %d\n", i);
                        write(STDOUT, message, sizeof(message));
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    char message[BUFFER_SIZE] = {0};
                    sprintf(message ,"$ Message from client fd = %d: recieved.\n", i);
                    write(STDOUT, message, sizeof(message));
                    if(strcmp(buffer, STUDENT) == 0) {
                        if(has_role(i, TA_fds)) {
                            const char message[] = "- Can not change your role! \n";
                            send(i, message, sizeof(message), 0);
                            continue;
                        }
                        else if(has_role(i, student_fds)) {
                            const char message[] = "- Already in list! \n";
                            send(i, message, sizeof(message), 0);
                            continue;
                        }
                        else {
                            set_role(STUDENT, i, student_fds, student_count);
                        }
                    }
                    if(strcmp(buffer, TA) == 0) {
                        if(has_role(i, student_fds)) {
                            const char message[] = "- Can not change your role! \n";
                            send(i, message, sizeof(message), 0);
                        }
                        else if(has_role(i, TA_fds)) {
                            const char message[] = "- Already in list! \n";
                            send(i, message, sizeof(message), 0);
                        }
                        else {
                            set_role(TA, i, TA_fds, TA_count);
                        }
                    }
                    if(strcmp(buffer, ASK_QUESTION) == 0) {
                        ask_question(i);
                    }
                    memset(buffer, 0, BUFFER_SIZE);
                }   
            }
        }
    }

}
