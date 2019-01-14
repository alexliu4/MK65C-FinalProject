#include "networking.h"

void process(char *s, char *chat_history);
void subserver(int from_client, int * chatrooms, char * chat_history_0);
int is_full(int * chatroom);
int add_client(int chatroom_id, int * chatrooms, int client_socket);
int num_from_string(char s);
char * create_chat_history(char * str, int size);
int * clients_in_chatroom(int * chatrooms, int chatroom);

int * clients_in_chatroom(int * chatrooms, int chatroom){
  int * clients = malloc(NUM_USERS * sizeof(int)); 
  int slot = chatroom * NUM_USERS;
  int increment = 0;
  while (increment < NUM_USERS && chatrooms[slot+increment]){
    clients[increment] = chatrooms[slot];
    increment++;
  }
  int i = 0;
  while(i < NUM_USERS && clients[i]){
    printf("%d ", clients[i]);
    i++;
  }
  return clients;
}

char * chat_shared_mem(char id, int size){

  key_t key = ftok("forking_server.c", id);
  printf("key: %d\n", key);

  // gets id
  int shmid = shmget(key, size, 0666|IPC_CREAT);
  printf("id: %d\n", shmid);

  // attach to shared memory
  char * chat_history = shmat(shmid, 0, 0);
  if(chat_history == (void*) -1){
    perror("shmat");
  }
  return chat_history;
}

int main() {

  char * chat_history_0 = chat_shared_mem('a', 1000); 

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
  printf("listen_socket is %d\n", listen_socket);
  while (1) {
    int client_socket = server_connect(listen_socket);
    printf("listen_socket: %d ", listen_socket);
    f = fork();
    if (f == 0)
      subserver(client_socket, chatrooms, chat_history_0);
    else
      close(client_socket);
  }
}

void subserver(int client_socket, int * chatrooms, char * chat_history_0) {
  char buffer[BUFFER_SIZE];

  //code to add to chatroom
  read(client_socket, buffer, sizeof(buffer));
  printf("subserver %d wants to connect to chatroom: %s", getpid(), buffer);

  int chatroom_id = num_from_string(*buffer);
  printf("chatroom_id: %d\n", chatroom_id);

  // adds client to chat
  add_client(chatroom_id, chatrooms, client_socket);
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
      add_client(chatroom_id, chatrooms, client_socket);
      for (int i=0; i<NUM_CHATROOMS*NUM_USERS; i++){
          printf("%d ", chatrooms[i]);
      }

      // notification to say you have joined
      sprintf(buffer, "you have joined chatroom %d\n", chatroom_id);
      printf("info in buffer: %s\n", buffer);
      write(client_socket, buffer, sizeof(buffer));
    }
    else {
      process(buffer, chat_history_0);
      //int * clients = clients_in_chatroom(chatrooms, 0);
      write(client_socket, chat_history_0, sizeof(buffer));
      
    }
  }//end read loop

  shmdt(chatrooms);
  close(client_socket);
  exit(0);
}

void checker(int pid, int chatName, int * chatrooms){

}

void process(char * s, char * chat_history) {
  strcat(chat_history, s);
}

int num_from_string(char s){
  int num = s - '0';
  return num;
}

int add_client(int chatroom, int * chatrooms, int client_socket){
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
  int increment = 0;
  while(increment < NUM_USERS && chatrooms[slot]){
    increment++;
    slot += increment;
  }
  printf("slot: %d\n", slot);
  chatrooms[slot] = client_socket;
  return slot;
}

