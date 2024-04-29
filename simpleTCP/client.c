#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "3490" // adjustable
#define MAXDATASIZE 100

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv){
  int sockfd, numbytes;
  struct addrinfo hints, *servinfo, *p;
  char s[INET6_ADDRSTRLEN];
  char buf[MAXDATASIZE];
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // error handler —getaddrinfo()
  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
  } 

  p = servinfo;
  if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
    perror("server: socket");
    exit(EXIT_FAILURE);
  }

  if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
    close(sockfd);
    perror("server: connect");
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(servinfo); // no need with the rest
  while(1){
    printf("%d\n", send(sockfd, argv[2], strlen(argv[2]), 0) == -1);

    printf("%d\n", (recv(sockfd, buf, MAXDATASIZE, 0) > 0));
    printf("response received: %s\n", buf);
    printf("server: process response and optimizing timetable\n");

    scanf("%s", argv[2]);
  }
}