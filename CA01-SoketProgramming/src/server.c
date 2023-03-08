#include "../include/const.h"

struct Question {
    char Q_text[BUFFER_SIZE];
    int status;
    int meeting_port;
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

void submit_question() {

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

    char buffer[BUFFER_SIZE] = {0};

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
 
    while(TRUE) {
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
                        write(STDOUT, "Close client\n", sizeof("Close client"));
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    
                    //write(STDOUT, "buffer[0]", 30);
                    if (buffer[0] == ASK_QUESTION) {
                        write(STDOUT, "S", 1);
                        send(i, "HEY\n", 10,0);
                    }
                    else {
                        write(STDOUT, "S",1);
                            const char spacing[] = "#######################\n";
                        send(i, spacing, sizeof(spacing), 0);
                        for(int j = 0; j < question_count; j++) {
                            send(i, questions->Q_text, sizeof(questions->Q_text), 0);
                        }
                        send(i, spacing, sizeof(spacing), 0);
            
                    }
                    memset(buffer, 0, BUFFER_SIZE);
                }   
            }
        }
    }
}