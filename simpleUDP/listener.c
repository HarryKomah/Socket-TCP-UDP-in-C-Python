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

#include "socketUDP.h"

int main(void){
  int sockfd;

  setup_UDP(&sockfd, PORT);

  printf("listener: waitingto recvfrom...\n");

 
  while(1){
    service_UDP(&sockfd);
  }
  
  close(sockfd); // reminder: sockfd is the listener socket.
  

  return 0;
}