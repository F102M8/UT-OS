#include "../include/const.h"
struct Question {
    char Q_text[BUFFER_SIZE];
    Q_level status;
    int meeting_port;
    int fd_meeting;
    int fd_S;
    int fd_TA;
};

char buffer[BUFFER_SIZE] = {0};
fd_set master_set, working_set;
fd_set student_set, TA_set;
fd_set req_ask, req_join, req_ans;
struct Question questions[MAX_QUESTIONS];
int question_count = 0;

int to_int(char str[])
{
    int len = strlen(str);
    int i, num = 0;
    
    for (i = len - 1; i < 0; i--) {
        if (((str[len - (i + 1)] - '0') > 9) || ((str[len - (i + 1)] - '0') < 0))  {
            return -1;
        }
        num = num * 10 + (str[len - (i + 1)] - '0') ;
    }
 
   return num;
}

void set_questions_buff() {
    for(int i = 0; i < MAX_QUESTIONS; i++) {
        questions[i].fd_S = -1;
        questions[i].fd_TA = -1;
        questions[i].meeting_port = -1;
        questions[i].status = ANSWERED;
    }
}

int create_port() {
    while(true) {
        int random_port= arc4random() % 9000 + 1000
    }
}
void add_to_meeting(int fd){
    
} 
void create_meeting() {

}

int setup_server(int port) {
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
void set_role(char role[], int fd) {
    char message[BUFFER_SIZE] = {0};
    if (role == STUDENT) {
        sprintf(message ,"+ client fd = %d: set as student.\n", fd);
        FD_SET(fd, &student_set);
    }
    else {
        sprintf(message ,"+ client fd = %d: set as TA.\n", fd);
        FD_SET(fd, &TA_set);
    }
    write(STDOUT, message, sizeof(message));
    send(fd, message, sizeof(message), 0);
}

void request_ask(int fd) {
    if(FD_ISSET(fd, &student_set)) {
        const char message[] = "+ Ask...\n";
        send(fd, message, sizeof(message), 0);
        FD_SET(fd, &req_ask);
    }
    else {
        if(FD_ISSET(fd, &TA_set)) {
            const char message[] = "- You're a TA!!!!\n";
            send(fd, message, sizeof(message), 0);
        }
        else {
            const char message[] = "- First set your role! \n";
            send(fd, message, sizeof(message), 0);
        }
    }
}
void submit_question(int fd) {
    const char message[] = "+ new question added successfully!\n";
    questions[question_count].fd_S = fd;
    strncpy(questions[question_count].Q_text, buffer, strlen(buffer));
    //questions[question_count].Q_text[strlen(buffer) - 1] = '\0';
    questions[question_count].status = WAITING;
    write(STDOUT, message, sizeof(message));
    send(fd, message, sizeof(message), 0);
    FD_CLR(fd, &req_ask);
    question_count ++;
}
void show_ls_questions(int fd) {
    if(FD_ISSET(fd, &TA_set)) {
        memset(buffer, 0, BUFFER_SIZE);
        char text[BUFFER_SIZE] = {0};
        sprintf(text, "LIST: \n");
        strcat(buffer, text);
        memset(text, 0, BUFFER_SIZE);
        for(int i = 0; i < question_count; i++) {
            if(questions[i].status == WAITING) {
                sprintf(text, "id: %d - %s\n", i, questions[i].Q_text);
                strcat(buffer, text);
                memset(text, 0, BUFFER_SIZE);   
            }
        }
        sprintf(text, END);
        strcat(buffer, text);
        memset(text, 0, BUFFER_SIZE);
        buffer[strlen(buffer)] = '\0';
        send(fd, buffer, BUFFER_SIZE, 0);

        memset(buffer, 0, BUFFER_SIZE);
    }
    else {
        if(FD_ISSET(fd, &student_set)) {
            const char message[] = "- Do not have permission! \n";
            send(fd, message, sizeof(message), 0);
        }
        else {
            const char message[] = "- First set your role! \n";
            send(fd, message, sizeof(message), 0);
        }
    }
}

void request_ans(int fd){
    if(FD_ISSET(fd, &TA_set)) {
        const char message[] = "+ enter id:...\n";
        send(fd, message, sizeof(message), 0);
        FD_SET(fd, &req_ans);
    }
    else {
         if(FD_ISSET(fd, &student_set)) {
            const char message[] = "- Do not have permission! \n";
            send(fd, message, sizeof(message), 0);
        }
        else {
            const char message[] = "- First set your role! \n";
            send(fd, message, sizeof(message), 0);
        }      
    }
}
void answer(int fd, int id) {
    if(id < 0) {
        const char message[] = "- not found! - (id >= 0) \n";
        send(fd, message, sizeof(message), 0);
    }
    else if(id > question_count) {
        const char message[] = "- not found! \n";
        send(fd, message, sizeof(message), 0);
    }
    else if(questions[id].status == UNDER_DISCUSSION) {
        const char message[] = "- under discussion! \n";
        send(fd, message, sizeof(message), 0);
    }
    else if(questions[id].status == ANSWERED) {
        const char message[] = "- already answered! \n";
        send(fd, message, sizeof(message), 0);
    }
    else {
        questions[id].status = UNDER_DISCUSSION;
        questions[id].fd_TA = fd;
        const char message[] = "+ OK \n";
        send(fd, message, sizeof(message), 0);
        create_meeting(id);
    }
    FD_CLR(fd, &req_ans);
}

int main(int argc,char const *argv[]) {
    if (argc <= 1) {
        const char message[] = "server on default port...!\n";
        write(STDOUT, message, sizeof(message));
    }
    int server_port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
    int server_fd = setup_server(server_port);

    FD_ZERO(&master_set);
    FD_ZERO(&TA_set);
    FD_ZERO(&student_set);
    FD_ZERO(&req_ans);
    FD_ZERO(&req_ask);
    FD_ZERO(&req_join);

    int max_fd = server_fd;
    //FD_SET(STDIN, &master_set);
    FD_SET(server_fd, &master_set);   
    set_questions_buff();

    int new_client_fd;

    while(true) {
        working_set = master_set;
        select(max_fd + 1, &working_set, NULL, NULL, NULL);

        for(int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) { 
                if (i == server_fd) {
                    new_client_fd = accept_client(server_fd);
                    FD_SET(new_client_fd, &master_set);
                    if (new_client_fd > max_fd)
                        max_fd = new_client_fd;
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
                        if(FD_ISSET(i, &TA_set)) {
                            FD_CLR(i, &TA_set);
                        }
                        else if (FD_ISSET(i, &student_set)) {
                            FD_CLR(i, &student_set);
                        }
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
                        continue;
                    }
                    char message[BUFFER_SIZE] = {0};
                    sprintf(message ,"Message from client fd = %d: recieved.\n", i);
                    write(STDOUT, message, sizeof(message));

                    if(strcmp(buffer, STUDENT) == 0) {
                        if(FD_ISSET(i, &TA_set)) {
                            const char message[] = "- Can not change your role! \n";
                            send(i, message, sizeof(message), 0);
                            continue;
                        }
                        else if(FD_ISSET(i, &student_set)) {
                            const char message[] = "- Already in list! \n";
                            send(i, message, sizeof(message), 0);
                            continue;
                        }
                        else {
                            set_role(STUDENT, i);
                        }
                    }
                    else if(strcmp(buffer, TA) == 0) {
                        if(FD_ISSET(i, &student_set)) {
                            const char message[] = "- Can not change your role! \n";
                            send(i, message, sizeof(message), 0);
                            continue;
                        }
                        else if(FD_ISSET(i, &TA_set)) {
                            const char message[] = "- Already in list! \n";
                            send(i, message, sizeof(message), 0);
                            continue;
                        }
                        else {
                            set_role(TA, i);
                        }
                    }
                    else if(strcmp(buffer, ASK_QUESTION) == 0) {
                        request_ask(i);
                    }
                    else if(strcmp(buffer, SHOW_LIST_QUESTIONS) == 0) {
                        show_ls_questions(i);
                    }
                    else if(strcmp(buffer, ANSWER) == 0) {
                        request_ans(i);
                    }
                    else {
                        if(FD_ISSET(i, &req_ask)) {
                            submit_question(i);
                        }
                        else if (FD_ISSET(i, &req_ans)) {
                            answer(i, to_int(buffer));
                        }
                        else {
                            const char message[] = "sth went wrong! \n";
                            send(i, message, sizeof(message), 0);
                        }
                        
                    }
                }
                    memset(buffer, 0, BUFFER_SIZE);
            } 
        }       
    }
}