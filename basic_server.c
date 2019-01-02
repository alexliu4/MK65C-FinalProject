#include "pipe_networking.h"

void upper_case(char *lower){
  while (*lower++ = toupper(*lower));
}

static void sighandler(int signo){
  
  if(signo == SIGINT){
    //printf("removing WKP\n");
    remove("wkp");
    exit(0);
  }
}

int main() {
  signal(SIGINT, sighandler);
  
  int to_client;
  int from_client;

  char msg[BUFFER_SIZE];
    
  while(1){
    server_handshake_part1(&from_client);
    int client_pid = fork();
    if(client_pid){
      close(from_client);
      //printf("removing wkp\n");
      remove("wkp");
      //printf("removed wkp\n");
    }
    else{
      server_handshake_part2(&to_client, &from_client);     
      while(read(from_client, msg, sizeof(msg))){
	printf("%d: client data: %s", getpid(), msg);
	upper_case(msg);
	//printf("%s", msg);
	write(to_client, msg, sizeof(msg));
	printf("%d: processed data: %s", getpid(), msg);
      }
    }
  }
  return 0;
}
