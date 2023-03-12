#include "../include/const.h"

char buffer[BUFFER_SIZE] = {0};
fd_set master_set, working_set;

bool on_meeting;
bool has_alarm = false;

int connect_to_server(int port)
{
    struct sockaddr_in server_address, bc_address;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1, broadcast = 1;

    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(NET_ADDRESS);

    const char error[] = "Error in connecting\n";
    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        write(STDERR, error, sizeof(error));
        abort();
    }
    else
    {
        const char message[] = "connected to the server!\n";
        write(STDOUT, message, sizeof(message));
    }

    return fd;
}

void alarm_handler(int signal) { 
    has_alarm = true;
    char message[BUFFER_SIZE] = "Too late\n";
    write(STDOUT, message, sizeof(message));
}
void alarm_handler2(int signal) { 
    has_alarm = true;
    char message[BUFFER_SIZE] = "sorry - no answer from TA \n";
    write(STDOUT, message, sizeof(message));
}

void welcome()
{
    const char message[] = "Set your role:\n1.STUDENT (type set_S)\n2.TA      (type set_T)\n";
    write(STDOUT, message, sizeof(message));
}
void TA_handler()
{
    const char message[] = "1. answer question        (type: ans)\n2. show list_questions    (type: ls)\n";
    write(STDOUT, message, sizeof(message));
}
void student_handler()
{
    const char message[] = "1. ask new question (type: ask)\n2. show meeting     (type: show_m)\n3. join to meeting  (type: join)\n";
    write(STDOUT, message, sizeof(message));
}

void ask_question(int server_fd)
{
    send(server_fd, buffer, strlen(buffer), 0);
    recv(server_fd, buffer, BUFFER_SIZE, 0);
    write(STDOUT, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);
}
void request_ans(int server_fd)
{
    send(server_fd, buffer, strlen(buffer), 0);
    recv(server_fd, buffer, BUFFER_SIZE, 0);
    write(STDOUT, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);
}

int create_port(int server_port, int id)
{
    return (server_port + id + 1);
}
void meeting_handler()
{
}
/*int conect_to_meeting(int port)
{
    int sock, broadcast = 1, opt = 1;
    struct sockaddr_in bc_address;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr(NET_ADDRESS);

    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    char msg[BUFFER_SIZE] = {0};
    sprintf(msg, "***you added to meeting!***\n");
    write(STDOUT, msg, sizeof(msg));
    return sock;
}*/
/*int broadcat(int port)
{
    // signal(SIGALRM, alarm_handler);
    // siginterrupt(SIGALRM, 1);
    int sock, broadcast = 1, opt = 1;
    struct sockaddr_in bc_address;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr(NET_ADDRESS);

    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    char msg[BUFFER_SIZE] = {0};
    
    sendto(sock, "HI\n\n", strlen("HI\n\n"), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));

    sprintf(msg, "***you and your student added to meeting!\n***");
    write(STDOUT, msg, sizeof(msg));
    sendto(sock, "HI\n\n", strlen("HI\n\n"), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));

    return sock;
}*/
int main(int argc, char *argv[]) {
    // signal(SIGALRM, alarm_handler);
    // siginterrupt(SIGALRM, 1);
    STATUS status = N;
    if (argc <= 1) {
        const char message[] = "on default port...!\n";
        write(STDOUT, message, sizeof(message));
    }
    int server_port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;

    int server_fd = connect_to_server(server_port);
    int meeting_fd = 999;
    int meeting_port = 999;

    FD_ZERO(&master_set);
    int max_fd = server_fd;
    FD_SET(STDIN, &master_set);
    FD_SET(server_fd, &master_set);
    on_meeting = false;
    welcome();
    while (true) {
        working_set = master_set;
        select(max_fd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == STDIN) {
                    read(STDIN, buffer, BUFFER_SIZE);
                    buffer[strlen(buffer) - 1] = '\0';

                    if ((strcmp(buffer, STUDENT) == 0)) {
                        status = S;
                        send(server_fd, buffer, strlen(buffer), 0);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));
                        if (buffer[0] == '+')
                        {
                            student_handler();
                        }
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    else if (strcmp(buffer, TA) == 0){              
                        status = T;          
                        send(server_fd, buffer, strlen(buffer), 0);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));
                        if (buffer[0] == '+')
                        {
                            TA_handler();
                        }
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    else if (strcmp(buffer, ASK_QUESTION) == 0)
                    {
                        if (!on_meeting) {
                            ask_question(server_fd);
                        }
                        else {
                            write(STDOUT, "- Do not have permission! \n", sizeof("- Do not have permission! \n"));
                        }
                    }
                    else if (strcmp(buffer, SHOW_LIST_QUESTIONS) == 0)
                    {
                        send(server_fd, buffer, strlen(buffer), 0);
                        memset(buffer, 0, BUFFER_SIZE);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    else if (strcmp(buffer, ANSWER) == 0) {
                        if (!on_meeting) {
                            request_ans(server_fd);
                        }
                        else {
                            write(STDOUT, "- Do not have permission! \n", sizeof("- Do not have permission! \n"));
                        }
                    }
                    else if (strcmp(buffer, SHOW_LIST_QUESTIONS) == 0) {
                        send(server_fd, buffer, strlen(buffer), 0);
                        memset(buffer, 0, BUFFER_SIZE);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));
                        memset(buffer, 0, BUFFER_SIZE); 
                    }
                    else if (strcmp(buffer, JOIN_MEETING) == 0) {
                        if(!on_meeting) {
                            send(server_fd, buffer, strlen(buffer), 0);
                            memset(buffer, 0, BUFFER_SIZE);
                            recv(server_fd, buffer, BUFFER_SIZE, 0);
                            write(STDOUT, buffer, strlen(buffer));
                            memset(buffer, 0, BUFFER_SIZE);                     
                            recv(server_fd, buffer, BUFFER_SIZE, 0);
                            write(STDOUT, buffer, strlen(buffer));
                            memset(buffer, 0, BUFFER_SIZE);
                            read(STDIN, buffer, BUFFER_SIZE);
                            buffer[strlen(buffer) - 1] = '\0';
                            send(server_fd, buffer, strlen(buffer), 0);
                            
                        }
                        else {
                            write(STDOUT, "- Do not have permission! \n", sizeof("- Do not have permission! \n"));
                        }
                    }
                    else
                    {
                        send(server_fd, buffer, strlen(buffer), 0);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        write(STDOUT, buffer, strlen(buffer));
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                }
                else if (i == server_fd) {
                    recv(server_fd, buffer, BUFFER_SIZE, 0);
                    buffer[strlen(buffer)] = '\0';
                    char commit[BUFFER_SIZE] = {0};
                    strncpy(commit, buffer, strlen(buffer));
                    memset(buffer, 0, BUFFER_SIZE);
                    if (strcmp(commit, REQ_CONNECT) == 0) {
                        send(server_fd, ACCEPT, strlen(ACCEPT), 0);
                        recv(server_fd, buffer, BUFFER_SIZE, 0);
                        buffer[strlen(buffer)] = '\0';
                        meeting_port = atoi(buffer);
                        char msg[BUFFER_SIZE] = {0};
                        sprintf(msg, "*** NEW MEETING FOR YOU ***\nport: %d\n", meeting_port);
                        write(STDOUT, msg, sizeof(msg));
                        memset(buffer, 0, BUFFER_SIZE);

                        //meeting_fd = (status == S) ? conect_to_meeting(meeting_port) : broadcat(meeting_port);
                        
                        //meeting_fd = conect_to_meeting(meeting_port);
                        
                        int broadcast = 1, opt = 1;
                        struct sockaddr_in bc_address;

                        meeting_fd = socket(AF_INET, SOCK_DGRAM, 0);
                        setsockopt(meeting_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
                        setsockopt(meeting_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

                        bc_address.sin_family = AF_INET;
                        bc_address.sin_port = htons(meeting_port);
                        bc_address.sin_addr.s_addr = inet_addr(BR_ADDR);

                        bind(meeting_fd, (struct sockaddr *)&bc_address, sizeof(bc_address));
                        FD_SET(meeting_fd, &master_set);
                       
                        if (meeting_fd > max_fd)
                            max_fd = meeting_fd;
                        char msg2[BUFFER_SIZE] = {0};
                        sprintf(msg2, "***you added to meeting!***\n");
                        write(STDOUT, msg2, sizeof(msg2));
                        //if(status == T)  
                        //    sendto(meeting_fd, "HI2\n\n", strlen("HI2\n\n"), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));
                        on_meeting = true;
                        
                        if(true) {
                            signal(SIGALRM, alarm_handler);
                            siginterrupt(SIGALRM, 1);
                           char msg3[BUFFER_SIZE] = {0};
                            sprintf(msg3, "You have 60s to answer! \n");
                            write(STDOUT, msg3, sizeof(msg3)); 
                            alarm(MAX_DELAY);
                            read(STDIN, buffer, BUFFER_SIZE);
                            alarm(0);
                            if(has_alarm) {
                                send(server_fd, HAS_ALARM, sizeof(HAS_ALARM),0);
                                on_meeting = false;
                                has_alarm = false;
                            }
                            else {
                                send(server_fd, SUCCESSFULY_DONE, sizeof(SUCCESSFULY_DONE), 0);
                                on_meeting = false;
                                //***** send port...
                            }
                        }
                        if(status == S) {
                            signal(SIGALRM, alarm_handler2);
                            siginterrupt(SIGALRM, 1);
                            char msg3[BUFFER_SIZE] = {0};
                            sprintf(msg3, "please wait just for  1 minute! \n");
                            write(STDOUT, msg3, sizeof(msg3)); 
                            alarm(MAX_DELAY);
                            recv(meeting_fd, buffer, BUFFER_SIZE, 0);
                            alarm(0);
                            if(has_alarm) {
                                send(server_fd, NO_ANSWER, sizeof(NO_ANSWER),0);
                                on_meeting = false;
                                has_alarm = false;
                            }
                            else {
                                send(server_fd, SEND_REPORT, sizeof(SEND_REPORT), 0);
                                on_meeting = false;
                                //recv(meeting_fd, )
                                //**********broad cast
                            }
                        }
                    }
                    
                    else
                    {
                        write(STDOUT, commit, strlen(commit));
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                }
                else {
                    recv(meeting_fd, buffer, BUFFER_SIZE, 0);
                    write(STDOUT, buffer, strlen(buffer));
                    memset(buffer, 0, BUFFER_SIZE);
                }
            }
        }
    }
    return 0;
}