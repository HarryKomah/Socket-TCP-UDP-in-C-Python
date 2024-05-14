#include "socketUDP.h"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_port);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_port);
}

void setup_UDP(int *sockfd, char *UDP_PORT){
  struct addrinfo hints, *p, *servinfo;
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return;
  }

  for(p = servinfo; p != NULL; p = servinfo->ai_next){
    if((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      perror("listener: socket");
      continue;
    }

    if(bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1){
      close(*sockfd);
      perror("listener: bind");
      continue;
    }
    break;
  }

  if(p == NULL) {
    fprintf(stderr, "listener: failed to bind socket\n");
    return;
  }

  freeaddrinfo(servinfo);
}

void service_UDP(int *sockfd){
  int numbytes;
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  char msg[MAXBUFLEN];
  char s[INET6_ADDRSTRLEN];

   addr_len = sizeof their_addr;
  if ((numbytes = recvfrom(*sockfd, msg, MAXBUFLEN-1, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1){
      perror("recvfrom");
      exit(1);
    }
  
    printf("listener: got packet from %s port %hu\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s), *(in_port_t *)get_in_port((struct sockaddr *)&their_addr));

    printf("listener: packet is %d bytes long\n", numbytes);
    msg[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", msg);
    if (!strcmp(msg, "END")) return;
}