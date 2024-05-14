#ifndef _UDP_SOCK
#define _UDP_SOCK 1
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT "4950"
#define MAXBUFLEN 100
#endif


void *get_in_addr(struct sockaddr *sa);
void *get_in_port(struct sockaddr *sa);

void setup_UDP(int *sockfd, char *UDP_PORT);

void service_UDP(int *sockfd);