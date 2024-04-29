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

#define PORT "4950"

#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void){
  int sockfd;
  struct addrinfo hints, *p, *servinfo;
  int rv;
  int numbytes;
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  char msg[MAXBUFLEN];
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for(p = servinfo; p != NULL; p = servinfo->ai_next){
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      perror("listener: socket");
      continue;
    }

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
      close(sockfd);
      perror("listener: bind");
      continue;
    }
    break;
  }

  if(p == NULL) {
    fprintf(stderr, "listener: failed to bind socket\n");
    return 2;
  }

  freeaddrinfo(servinfo);

  printf("listener: waitingto recvfrom...\n");

  addr_len = sizeof their_addr;
  while(1){
    if ((numbytes = recvfrom(sockfd, msg, MAXBUFLEN-1, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1){
      perror("recvfrom");
      exit(1);
    }
  
    printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));

    printf("listener: packet is %d bytes long\n", numbytes);
    msg[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", msg);
    if (!strcmp(msg, "END")) break;
  }
  
    close(sockfd); // reminder: sockfd is the listener socket.
  

  return 0;
}