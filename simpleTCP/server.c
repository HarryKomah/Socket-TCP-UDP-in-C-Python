/*
** server.c -- a stream socket server demo
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

#define PORT "3490" // adjustable
#define MAXDATASIZE 100 // max 100 per send.
#define BACKLOG 10 // number of pending connections.

void sigchld_handle(int s)
{
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while(waitpid(-1, NULL, WNOHANG) > 0); // if 0 or negative, just ignore (0  for child is still in process)

  errno = saved_errno;
}

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
  char query[MAXDATASIZE], response[MAXDATASIZE];
  int rv;

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

  sa.sa_handler = sigchld_handle; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if(sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while(1) { // main accept() loop
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
    printf("server: got connection from %s with fd %d\n", s, new_fd);
    
    if (!fork()) {
      close(sockfd);
      // do any to the child, assuming child should be waiting for response.
      while(1) {
        printf("waiting...\n");
        if (recv(new_fd, query, MAXDATASIZE, 0) <= 0){
          perror("recv");
          break;
        } printf("query received: %s\n", query);
        printf("server: processing the query\n");
      query[0] = 'L';
      query[1] = 'O';
      query[2] = 'L';
      strcpy(response, query);
      if (send(new_fd, response, MAXDATASIZE, 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
      };
        printf("response sent back\n");
        
      }
      printf("connection %d has been terminated\n", new_fd);
      close(new_fd);
      exit(0);
    }
    printf("Here");
    close(new_fd);
  }
  return 0;
}