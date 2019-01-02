#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
void server_handshake_part1(int *from_client){
  //printf("Server making WKP\n");
  mkfifo("wkp", 0644);
  //printf("Server made WKP\n");

  //printf("Server opening WKP\n");
  *from_client = open("wkp", O_RDONLY);
  //printf("Server opened WKP\n");
}

int server_handshake_part2(int *to_client, int *from_client) {
  char name[HANDSHAKE_BUFFER_SIZE];
  char msg[HANDSHAKE_BUFFER_SIZE];

  //printf("Server reading WKP\n");
  read(*from_client, name, sizeof(name));
  printf("%d: Server read \"%s\"\n", getpid(), name);

  //printf("Server opening private pipe\n");
  *to_client = open(name, O_WRONLY);
  //printf("Server opened private pipe\n");

  //printf("Server writing to private pipe\n");
  write(*to_client, ACK, sizeof(ACK));
  printf("%d: Server wrote \"%s\"\n", getpid(), ACK);

  //printf("Server reading from WKP\n";)
  read(*from_client, msg, sizeof(msg));
  printf("%d: Server read \"%s\"\n", getpid(), msg);
  printf("Handshake complete!\n");
  
  return *from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  char pid[HANDSHAKE_BUFFER_SIZE];
  char msg[HANDSHAKE_BUFFER_SIZE];

  sprintf(pid, "%d", getpid());

  //printf("Client creating private pipe\n");
  mkfifo(pid, 0644);
  //printf("Client created private pipe\n");

  //printf("Client opening private pipe\n");
  *to_server = open("wkp", O_WRONLY);
  //printf("Client opened private pipe\n");

  //printf("Client writing to WKP\n");
  write(*to_server, pid, sizeof(pid));
  printf("Client wrote\"%s\"\n", pid);

  //printf("Client opening private pipe\n");
  int from_server = open(pid, O_RDONLY);
  //printf("Client opening private pipe\n");

  //printf("Client reading private pipe\n");
  read(from_server, msg, sizeof(msg));
  printf("Client read \"%s\"\n", msg);

  //printf("Client removing private pipe\n");
  remove(pid);
  //printf("Client removed private pipe\n");

  //printf("Client writing to WKP\n");
  write(*to_server, ACK, sizeof(ACK));
  printf("Client wrote \"%s\"\n", ACK);

  printf("Handshake complete!\n==================\n");
  
  return from_server;
}
