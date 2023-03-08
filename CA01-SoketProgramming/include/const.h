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

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFER_SIZE 1024
#define MAX_CONNECTION_TO_THE_SERVER 50
#define MAX_CLIENT_IN_MEETING 50
#define MEETINGS 25
#define MAX_QUESTIONS 100

#define MAX_DELAY 60

#define TRUE  1
#define FALSE 0

#define DEFAULT_PORT 8081

#define NET_ADDRESS "127.0.0.1"
#define BROADCAST_ADDR "192.168.1.255"

#define STUDENT 'S'
#define TA 'T'

#define ASK_QUESTION 'Q'
#define SHOW_MEETINGS 'M'
#define JOIN_MEETING 'J'
#define SHOW_LIST_QUESTIONS 'L'
#define ANSWER 'A'
