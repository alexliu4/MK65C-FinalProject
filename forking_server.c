#include "networking.h"

void process(char *s);
void subserver(int from_client, int * chatrooms);
int is_full(int * chatroom);
int add_client(int chatroom_id, int * chatrooms);
int num_from_string(char s);

int main() {
// generates key
  key_t key = ftok("networking.h", 'A');
  printf("key: %d\n", key);

  // gets id
  int shmid = shmget(key, NUM_CHATROOMS*NUM_USERS, 0666|IPC_CREAT);
  printf("id: %d\n", shmid);

  // attach to shared memory
  int * chatrooms = shmat(shmid, 0, 0);
  if(chatrooms == (void*) -1){
    perror("shmat");
  }

  //reset chatrooms
  for (int i=0; i<NUM_CHATROOMS*NUM_USERS; i++){
      	chatrooms[i] = 0;
	printf("%d ", chatrooms[i]);
  }

  /*int ** chatrooms = malloc(NUM_CHATROOMS * sizeof(int*));
  for (int i = 0; i < 3; i++){
    chatrooms[i] = malloc(NUM_USERS * sizeof(int));
  }*/

  int listen_socket;
  int f;
  listen_socket = server_setup();

  while (1) {

    int client_socket = server_connect(listen_socket);
    f = fork();
    if (f == 0)
      subserver(client_socket, chatrooms);
    else
      close(client_socket);
  }
}

void subserver(int client_socket, int * chatrooms) {
  char buffer[BUFFER_SIZE];

  //code to add to chatroom
  read(client_socket, buffer, sizeof(buffer));
  printf("subserver %d wants to connect to chatroom: %s", getpid(), buffer);

  int chatroom_id = num_from_string(*buffer);
  printf("chatroom_id: %d\n", chatroom_id);

  // adds client to chat
  add_client(chatroom_id, chatrooms);
  // adds client to servers list of clients in chats
  for (int i=0; i<NUM_CHATROOMS*NUM_USERS; i++){
      printf("%d ", chatrooms[i]);
  }
  // message to client
  sprintf(buffer, "you have joined chatroom %d\n", chatroom_id);
  printf("info in buffer: %s", buffer);
  write(client_socket, buffer, sizeof(buffer));

  // reads the messages and checks for join keyword
  while (read(client_socket, buffer, sizeof(buffer))) {
    printf("[subserver %d] received: [%s]\n", getpid(), buffer);
    // if "join <chat> is typed"
    if (! strncmp("join", buffer, 4) ){
      printf("NEEDS TO JOIN A NEW SERVER!!!\n");
      char * ans = buffer + 5;
      memcpy( buffer, buffer + 5, 5 * sizeof(char));
      printf("BUFFER: %s\n", buffer);
      printf("ANS: %s\n", ans);
      printf("%d\n", getpid());
      printf("subserver %d wants to connect to chatroom: %s\n", getpid(), buffer);
      int chatroom_id = num_from_string(*buffer);
      printf("chatroom_id: %d\n", chatroom_id);
      // adding the client to chatroom and the main server's lists of clients in chats
      add_client(chatroom_id, chatrooms);
      for (int i=0; i<NUM_CHATROOMS*NUM_USERS; i++){
          printf("%d ", chatrooms[i]);
      }

      // notification to say you have joined
      sprintf(buffer, "you have joined chatroom %d\n", chatroom_id);
      printf("info in buffer: %s\n", buffer);
      write(client_socket, buffer, sizeof(buffer));
    }
    else {
      process(buffer);
      write(client_socket, buffer, sizeof(buffer));
    }
  }//end read loop

  shmdt(chatrooms);
  close(client_socket);
  exit(0);
}

void checker(int pid, int chatName, int * chatrooms){

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

int add_client(int chatroom, int * chatrooms){
  int slot = chatroom;
  printf("chatroom: %d\n", chatroom);
  // // chatroom format 0 1 2 0 1 2 0 1 2
  // int col = 1;
  // while(chatrooms[slot] && slot< NUM_CHATROOMS * NUM_USERS){
  //   slot = col * NUM_CHATROOMS + chatroom;
  //   col++;
  // }
  // chatroom format 0 0 0 1 1 1 2 2 2
  printf("%d\n", NUM_USERS);
  slot = chatroom * NUM_USERS;
  while(chatrooms[slot]){
    slot++;
  }
  printf("slot: %d\n", slot);
  chatrooms[slot] = getpid();
  return slot;
}
