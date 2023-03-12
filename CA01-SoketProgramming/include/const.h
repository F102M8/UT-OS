#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFER_SIZE 1024
#define MAX_CONNECTION_TO_THE_SERVER 50
#define MAX_CLIENT_IN_MEETING 50
#define MEETINGS 25
#define MAX_QUESTIONS 100
#define MAX_TA 20
#define MAX_STUDENT 30

#define MAX_DELAY 5

#define ACCEPT "accepted! \n"
#define REJECT "rejected! \n"

#define HAS_ALARM  "has_alarm! \n"
#define SUCCESSFULY_DONE "Good job=) \n"
#define SEND_REPORT "send_report"
#define NO_ANSWER "NO ANS"

#define DEFAULT_PORT 8081

#define NET_ADDRESS "127.0.0.1"
#define BR_ADDR "255.255.255.255"
#define STUDENT "set_S"
#define TA "set_T"
#define ROLE_SIZE 10
#define ASK_QUESTION "ask"
#define SHOW_MEETINGS "show_m"
#define JOIN_MEETING "join"
#define SHOW_LIST_QUESTIONS "ls"
#define ANSWER "ans"

#define END "END_OF_LIST\n"
#define REQ_CONNECT "req_connect_to_meeting\n"

typedef enum
{
    WAITING,
    ANSWERED,
    UNDER_DISCUSSION,
    NO_NEED,
} Q_level;

typedef enum
{
    T,
    N,
    S,
} STATUS;
