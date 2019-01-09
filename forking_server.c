#include "networking.h"

void process(char *s);
void subserver(int from_client);
int is_full(int * chatroom);
int next_slot(int * chatroom);
int add_client(int * chatroom, int slot);

int main() {
  int ** chatrooms = malloc(3 * sizeof(int*));
  for (int i = 0; i < 3; i++){
    chatrooms[i] = malloc(3 * sizeof(int));
   /* 
    for (int j = 0; j < 3; j++){
       chatrooms[i][j] = 30000;
       printf("%d ", chatrooms[i][j]);
    }
    */
  }
  int listen_socket;
  int f;
  listen_socket = server_setup();

  while (1) {

    int client_socket = server_connect(listen_socket);
    f = fork();
    if (f == 0)
      subserver(client_socket);
    else
      close(client_socket);
  }
}

void subserver(int client_socket) {
  char buffer[BUFFER_SIZE];
  read(client_socket, buffer, sizeof(buffer));
  printf("subserver %d wants to connect to chatroom: %s", getpid(), buffer);

  while (read(client_socket, buffer, sizeof(buffer))) {
    printf("[subserver %d] received: [%s]\n", getpid(), buffer);
    process(buffer);
    write(client_socket, buffer, sizeof(buffer));
  }//end read loop
  close(client_socket);
  exit(0);
}

void process(char * s) {
  while (*s) {
    if (*s >= 'a' && *s <= 'z')
      *s = ((*s - 'a') + 13) % 26 + 'a';
    else  if (*s >= 'A' && *s <= 'Z')
      *s = ((*s - 'a') + 13) % 26 + 'a';
    s++;
  }
}

