/*
 ** selectserver.c -- a stream socket server demo
 */

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
#include "response_http.h"

#define PORT "4003" // adjustable
#define MAXDATASIZE 200 // max 100 per send.
#define BACKLOG 10 // number of pending connections.

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
  if ( sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr); // convert to IPv4 sockaddr_in
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
  int sockfd, new_fd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  struct sigaction sa;
  char s[INET6_ADDRSTRLEN];
  char query[MAXDATASIZE], *response;
  int rv;

  fd_set master; // master file descriptor list
  fd_set read_fds;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  
  int fdmax = sockfd;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // error handler —getaddrinfo()

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  } 

  p = servinfo;
  for(p = servinfo; p != NULL; p=servinfo->ai_next){
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      exit(EXIT_FAILURE);
    }

    int yes=1;
    //char yes='1'; // Solaris people use this

    // lose the pesky "Address already in use" error message
    if (setsockopt(sockfd ,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
      perror("setsockopt");
      exit(1);
    } 
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
      close(sockfd);
      perror("server: bind");
      exit(EXIT_FAILURE);
    }

    break;
  }

  freeaddrinfo(servinfo); // no need with the rest

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  FD_SET(sockfd, &master);

  printf("server: waiting for connections...\n");

  while(1) { // main accept() loop

    FD_ZERO(&read_fds);
    read_fds = master;
    
    if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
      perror("select");
      exit(EXIT_FAILURE);
    }
    if (FD_ISSET(sockfd, &read_fds)) {
      sin_size = sizeof their_addr;
      new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
      if (new_fd == -1) {
        perror("accept");
        continue;
      }

      inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
      printf("server: got connection from %s\n", s);

      if (recv(new_fd, query, MAXDATASIZE, 0) <= 0){
          perror("recv");
          break;
      } printf("Received:\n\n%s\n", query);

      response = calloc((strlen(HTTP_200_RESPONSE) + strlen(HTML_200_MESSAGE) + 5), sizeof(char));

      if (response == NULL) perror("calloc");
      sprintf(response, "%s\r\n%s", HTTP_200_RESPONSE,HTML_200_MESSAGE);
      if (send(new_fd, response, strlen(response), 0) == -1) {
          perror("send");
          exit(EXIT_FAILURE);
      };

      printf("connection %d in the main while has been closed\n", new_fd);
      close(new_fd);
    }
  }
  return 0;
}
