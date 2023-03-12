#include "../include/const.h"
struct Question {
    char Q_text[BUFFER_SIZE];
    Q_level status;
    int meeting_port;
    int fd_meeting;
    int fd_S;
    int fd_TA;
    char Q_ans[BUFFER_SIZE];
};

char buffer[BUFFER_SIZE] = {0};
fd_set master_set, working_set;
fd_set student_set, TA_set;
fd_set req_ask, req_join, req_ans;
fd_set meetings_set;
fd_set on_meeting_set;
fd_set must_submit_report;
struct Question questions[MAX_QUESTIONS];
int question_count = 0;

int to_int(char str[]) {
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
char* to_string(char str[], int num) {
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

void set_questions_buff() {
    for(int i = 0; i < MAX_QUESTIONS; i++) {
        questions[i].fd_S = -1;
        questions[i].fd_TA = -1;
        questions[i].meeting_port = -1;
        questions[i].status = NO_NEED;
    }
}

int create_port(int server_port, int id) {
    return (server_port + id + 1);
}

void add_to_meeting(int fd, char sport[]) {
    bool find_port = false;
    int port = atoi(sport);
    for(int i = 0; i < question_count; i++) {
        if((questions[i].status == UNDER_DISCUSSION) && questions[i].meeting_port == port) {
              find_port = true;
        }
    }
    if (find_port) {
        send(fd, REQ_CONNECT, sizeof(REQ_CONNECT), 0);
        recv(fd, buffer, buffer, 0);
        memset(buffer, 0, BUFFER_SIZE);
        char s_port[BUFFER_SIZE] = {0};
        snprintf (s_port, BUFFER_SIZE, "%d",port);
        send(fd, s_port, BUFFER_SIZE, 0);
        
    }
    else {
        send(fd, REJECT, sizeof(REJECT), 0);
    }
    FD_CLR(fd, &req_join);
    FD_CLR(fd, &on_meeting_set);
} 
void req_add_to_meeting(int fd) {
    if(FD_ISSET(fd, &student_set)) {
        show_ls_meetings(fd);
        const char message[] = "+ Enter port...\n";
        send(fd, message, sizeof(message), 0);
        FD_SET(fd, &req_join);
    }
    else {
        if(FD_ISSET(fd, &TA_set)) {
        const char message[] = "- Why? create your own meeting and answer another question! \n";
            send(fd, message, sizeof(message), 0);
        }
        else {
            const char message[] = "- First set your role! \n";
            send(fd, message, sizeof(message), 0);
        }
    }
}

int create_meeting(int port) {
    int sock, broadcast = 1, opt = 1;
    char buffer_m[BUFFER_SIZE] = {0};
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

int setup_server(int port) {
    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

void show_ls_meetings(int fd) {
    if(FD_ISSET(fd, &student_set)) {
        memset(buffer, 0, BUFFER_SIZE);
        char text[BUFFER_SIZE] = {0};
        sprintf(text, "LIST: \n");
        strcat(buffer, text);
        memset(text, 0, BUFFER_SIZE);
        for(int i = 0; i < question_count; i++) {
            if(questions[i].status == UNDER_DISCUSSION) {
                sprintf(text, "question id: %d - %s - on port: %d \n", i, questions[i].Q_text, questions[i].meeting_port);
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
        if(FD_ISSET(fd, &TA_set)) {
            const char message[] = "- why?! \n";
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
void answer(int server_port, int fd, char sid[]) {  
    int id = atoi(sid);
    if(id < 0) {
        const char message[] = "- not found! - (id >= 0) \n";
        send(fd, message, sizeof(message), 0);
    }
    else if(id > question_count - 1) {
        const char message[] = "- not found! \n";
        send(fd, message, sizeof(message), 0);
    }
    else if((questions[id].status == UNDER_DISCUSSION) || (questions[id].status == NEED_TO_SUBMIT)) {
        const char message[] = "- under discussion! \n";
        send(fd, message, sizeof(message), 0);
    }
    else if(questions[id].status == ANSWERED) {
        const char message[] = "- Already answered! \n";
        send(fd, message, sizeof(message), 0);
    }
    else if(FD_ISSET(questions[id].fd_S, &on_meeting_set)) {
        const char message[] = "- Student is bussy for now, pls answer another question \n";
        send(fd, message, sizeof(message), 0);
    }
    else if (questions[id].status == NO_NEED) {
        const char message[] = "- NO NEED \n";
        send(fd, message, sizeof(message), 0);
    }
    else {
        questions[id].status = UNDER_DISCUSSION;
        questions[id].fd_TA = fd;
        const char message[] = "+ OK \n";
        send(questions[id].fd_TA, message, sizeof(message), 0);
        int port = create_port(server_port, id);
        char s_port[BUFFER_SIZE] = {0};
        snprintf (s_port, BUFFER_SIZE, "%d",port);
        int meeting_fd = create_meeting(port);
        questions[id].meeting_port = port;

        send(questions[id].fd_TA, REQ_CONNECT, sizeof(REQ_CONNECT), 0);
        recv(questions[id].fd_TA, buffer, buffer, 0);
        memset(buffer, 0, BUFFER_SIZE);
        send(questions[id].fd_TA, s_port, BUFFER_SIZE, 0);
        send(questions[id].fd_S, REQ_CONNECT, sizeof(REQ_CONNECT), 0);
        recv(questions[id].fd_S, buffer, buffer, 0);
        memset(buffer, 0, BUFFER_SIZE);
        send(questions[id].fd_S, s_port, BUFFER_SIZE, 0);
        FD_SET(meeting_fd, &meetings_set);
        FD_SET(questions[id].fd_S, &on_meeting_set);
        FD_SET(questions[id].fd_TA, &on_meeting_set);
    }
    FD_CLR(fd, &req_ans);
}

int main(int argc,char const *argv[]) {
    int file_fd ;
    file_fd = creat("report.txt", O_RDWR);
    write(file_fd, "report: \n", strlen("report: \n")); 
    
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
   // FD_SET(STDIN, &master_set);
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
                        FD_CLR(i, &on_meeting_set);
                        for(int j = 0; j < question_count; j++) {
                            if((questions[j].fd_S == i) && (questions[j].status == WAITING)  ){
                                questions[j].status = NO_NEED;
                            }
                        }
                        continue;
                    }
                    char message[BUFFER_SIZE] = {0};
                    sprintf(message ,"Message from client fd = %d: recieved.\n", i);
                    write(STDOUT, message, sizeof(message));

                    if(strcmp(buffer, STUDENT) == 0) {
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
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
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
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
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
                        request_ask(i);
                    }
                    else if(strcmp(buffer, SHOW_LIST_QUESTIONS) == 0) {
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
                        show_ls_questions(i);
                    }
                    else if(strcmp(buffer, ANSWER) == 0) {
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
                        request_ans(i);
                    }
                    else if(strcmp(buffer, SHOW_MEETINGS) == 0) {
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
                        show_ls_meetings(i);
                    }
                    else if(strcmp(buffer, JOIN_MEETING) == 0) {
                        FD_CLR(i, &req_ans);
                        FD_CLR(i, &req_ask);
                        FD_CLR(i, &req_join);
                        req_add_to_meeting(i);
                    }
                    else if(strcmp(buffer, HAS_ALARM) == 0) {
                        FD_CLR(i, &on_meeting_set);
                        for(int j = 0; j < question_count; j++) {
                            if(questions[j].fd_TA == i) {
                                questions[j].fd_TA = -1;
                                questions[j].status = WAITING;
                                FD_CLR(questions[j].fd_S, &on_meeting_set);
                            }
                        }
                    }
                    else if(strcmp(buffer, SUCCESSFULY_DONE) == 0) {
                        FD_CLR(i, &on_meeting_set);
                        for(int j = 0; j < question_count; j++) {
                            if(questions[j].fd_TA == i) {

                                questions[j].status = NEED_TO_SUBMIT;
                                FD_CLR(questions[j].fd_S, &on_meeting_set);
                                questions[j].fd_TA = -1;    
                                memset(buffer, 0, BUFFER_SIZE);
                                send(questions[j].fd_S, SEND_REPORT, sizeof(SEND_REPORT), 0);
                                recv(questions[j].fd_S, buffer, BUFFER_SIZE, 0);
                                buffer[strlen(buffer)] = '\0';
                                char ans[BUFFER_SIZE] = {0};
                                strncpy(ans, buffer, strlen(buffer));
                                char msg[BUFFER_SIZE] = {0};
                                sprintf(msg, ">> Your problem : %s\n>> TA's answer: %s\n Do you get it? (y/n) \n ", questions[j].Q_text, ans);
                                send(questions[j].fd_S, msg, sizeof(msg), 0);
                                memset(buffer, 0, BUFFER_SIZE);
                                FD_SET(questions[j].fd_S, &must_submit_report);
                                strncpy (questions[j].Q_ans, ans, strlen(ans));
                                /**
                                recv(questions[j].fd_S, buffer, BUFFER_SIZE, 0);
                                if(buffer[0] == 'y') {
                                    strncpy (questions[j].Q_ans, ans, strlen(ans));
                                    char text[BUFFER_SIZE] = {0};
                                    sprintf(text, "%d q: %s \t  ans: %s \n", j, questions[j].Q_text, questions[j].Q_ans);
                                    write(file_fd, text, strlen(text));
                                    write(STDOUT, "New answer submited\n", sizeof("New answer submited\n"));
                                }
                                else {
                                    questions[j].status = WAITING;
                                }
                                */
                            }
                        }
                    }
                    else {
                        if(FD_ISSET(i, &req_ask)) {
                            submit_question(i);
                        }
                        else if (FD_ISSET(i, &req_ans)) {
                            answer(server_port, i, buffer);
                        }
                        else if (FD_ISSET(i, &req_join)) {
                            add_to_meeting(i, buffer);
                        }
                        else if (FD_ISSET(i, &must_submit_report)) {
                            for(int j = 0; j < question_count; j++) {
                                if ((questions[j].status == NEED_TO_SUBMIT) && (questions[j].fd_S == i)) {
                                    if(buffer[0] == 'y') {
                                        char text[BUFFER_SIZE] = {0};
                                        sprintf(text, "%d q: %s \t  ans: %s \n", j, questions[j].Q_text, questions[j].Q_ans);
                                        write(file_fd, text, strlen(text));
                                        write(STDOUT, "New answer submited\n", sizeof("New answer submited\n"));
                                        questions[j].status = WAITING;
                                    }
                                    else {
                                        memset(questions[j].Q_ans, 0, BUFFER_SIZE);
                                        questions[j].status = WAITING;
                                    }
                            
                                }
                            }
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
    close(file_fd);
}