#include "../include/const.h"

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
        const char message[] = "connected to the server!\n";
        write(STDOUT, message, sizeof(message));
    }
    return client_fd;
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

    char role = set_role();

    while(TRUE) {
        memset(buffer, 0, BUFFER_SIZE);
        read(STDIN, buffer, BUFFER_SIZE);
        send(server_fd, buffer, BUFFER_SIZE, 0);
        
    }
    return 0;
}