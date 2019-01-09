#include "networking.h"

void process(char *s);
void subserver(int from_client);
int is_full(int * chatroom);
int next_slot(int * chatroom);
int add_client(int * chatroom, int slot);
int num_from_string(char s);

int main() {
  int ** chatrooms = malloc(NUM_CHATROOMS * sizeof(int*));
  for (int i = 0; i < 3; i++){
    chatrooms[i] = malloc(NUM_USERS * sizeof(int));
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

  //code to add to chatroom
  read(client_socket, buffer, sizeof(buffer));
  printf("subserver %d wants to connect to chatroom: %s", getpid(), buffer);
  
  int chatroom_id = num_from_string(*buffer);
  //chatrooms[chatroom_id][next_slot(chatrooms[chatroom_id])] = getpid();
  
  sprintf(buffer, "you have joined chatroom %d\n", chatroom_id);
  printf("info in buffer: %s", buffer);
  write(client_socket, buffer, sizeof(buffer));

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

int num_from_string(char s){
  int num = s - '0';
  return num;
}

int next_slot(int * chatroom){
  int slot = 0;
  while(slot < NUM_USERS && slot){
    slot++;
  }
  return slot;
}
