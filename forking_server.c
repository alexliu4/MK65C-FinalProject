#include "networking.h"

void process(char *s);
void subserver(int from_client);
int num_from_string(char s);
int add_client(int chatroom_id, int * chatrooms, int client_socket);
int get_chat_from_client(int * chatroom, int client);
int remove_client(int * chatrooms, int client_socket);
int * create_chat(int * chatrooms, int client_socket);
int full(int chatroom, int * chatrooms, int client_socket);
void list_chat(int * chatrooms, int client_socket);

// int totChat = (int) malloc(sizeof(int));
// int totClients = (int) malloc(sizeof(int));
int totChat = NUM_CHATS;
int totClients = NUM_CLIENTS;

/*
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
  //shmctl(shmid, IPC_RMID, NULL);
  return chat_history;
  }
*/


int main() {
  /*key_t key = 123;
  int shmid = shmget(key, 100000 * totChat, IPC_CREAT|0666);
  char (*chat_hist)[5][100000];
  chat_hist = shmat(shmid, 0, 0);
*/
  int chatrooms[totChat * totClients];
  for (int i = 0; i < totChat * totClients; i++){
    chatrooms[i] = 0;
  }


  char chat_hist[5][100000];
  for (int i = 0; i < totChat; i++){
    char title[100];
    sprintf(title, "=====\nCHAT %d\n=====\n", i);
    strcpy(chat_hist[i], title);
  }

  for (int i = 0; i < totChat; i++){
    //printf("chat_hist[%d]: %s", i, (*chat_hist)[i]);
  }

  //char * chat_history = chat_shared_mem('a', 1000000000)
  //memset(chat_history, 0, sizeof(chat_history));

//  char * chat_history = calloc(100000, sizeof(char));

  int listen_socket;
  int listen_socket_1;
  int client;
  listen_socket = server_setup();
  listen_socket_1 = listen_socket;

  int clients[totClients];
  for(int i = 0; i < totClients; i++){
    clients[i] = 0;
  }
  char buffer[BUFFER_SIZE];

  fd_set read_fds;

  while (1) {
    FD_ZERO(&read_fds);
    FD_SET(listen_socket, &read_fds);

    for (int i = 0; i < totClients; i++){
      if(clients[i]>0){
        FD_SET(clients[i], &read_fds);
        if(clients[i] > listen_socket_1){
	  listen_socket_1 = clients[i];
        }
      }
    }

    //select will block until either fd is ready
    select(listen_socket_1 + 1, &read_fds, NULL, NULL, NULL);

    //if listen_socket triggered select
    if (FD_ISSET(listen_socket, &read_fds)){
      // client socket <- the special id to read and write to client
      if((client = server_connect(listen_socket))){
	for(int i = 0; i < totClients && clients[i]!=client; i++){
	  if(!clients[i]){
	    // client socket id is stored in clients array
	    clients[i] = client;
	    i = totClients; // stop the loop
	  }
	}
      }
    }

    /*// generates key
    key_t key = ftok("networking.h", 'A');
    printf("key: %d\n", key);

    // gets id
    int shmid = shmget(key, totChat * totClients, 0666|IPC_CREAT);
    printf("id: %d\n", shmid);

    // attach to shared memory
    int * chatrooms = shmat(shmid, 0, 0);
    if(chatrooms == (void*) -1){
      perror("shmat");
    }*/
    /*int chatrooms[totChat*totClients];
    for (int i = 0; i < totChat*totClients; i++){
      chatrooms[i] = 0;
    }*/
    //read from clients
    for (int i = 0; i < totClients && clients[i]; i++){
      if(FD_ISSET(clients[i], &read_fds)){
	if(read(clients[i], buffer, sizeof(buffer))>0){
  	  int chat = get_chat_from_client(chatrooms, clients[i]);
	  printf("[subserver %d] received: %s from chatroom %d\n", getpid(), buffer, chat);

	  // joining chatrooms
	  char * tempbuff;
    // special commands
	  if ( (tempbuff = strchr(buffer, '~')) ){

      /* ==========================================JOIN COMMAND========================================== */
	    if (! strncmp("~join ", tempbuff, 6) ){
	      // printf("NEEDS TO JOIN A NEW SERVER!!!\n");
	      memcpy(tempbuff, tempbuff + 6, 6 * sizeof(char));
	      printf("BUFFER: %s\n", tempbuff);
	      // printf("PID: %d\n", getpid());
	      printf("subserver %d wants to connect to chatroom: %s\n", getpid(), tempbuff);
	      int chatroom_id = num_from_string(*tempbuff);

        // checks capacity
        if (full(chatroom_id, chatrooms, clients[i])){
          // notification to say you cannot join
  	      sprintf(buffer, "Chatroom %d is full. You cannot join :(\n", chatroom_id);
  	      printf("info in buffer: %s\n", buffer);
  	      write(clients[i], buffer, sizeof(buffer));

        } else {
          printf("chatroom_id: %d\n", chatroom_id);
  	      // adding the client to chatroom and the main server's lists of clients in chats
  	      remove_client(chatrooms, clients[i]);
  	      add_client(chatroom_id, chatrooms, clients[i]);
  	      for (int i=0; i < totChat * totClients; i++){
  		        printf("%d ", chatrooms[i]);
  	      }

  	      // notification to say you have joined
  	      sprintf(buffer, "you have joined chatroom %d\n", chatroom_id);
  	      printf("info in buffer: %s\n", buffer);
  	      write(clients[i], buffer, sizeof(buffer));
        }
	    }
      /* ==========================================DONE JOIN COMMAND========================================== */

      /* ==========================================CREATE COMMAND========================================== */
      if (! strncmp("~create ", tempbuff, 8) ){
	    //   memcpy(tempbuff, tempbuff + 8, 8 * sizeof(char));
	    //   printf("BUFFER: %s\n", tempbuff);
	    //   // printf("PID: %d\n", getpid());
	    //   printf("subserver %d wants to connect to chatroom: %s\n", getpid(), tempbuff);
	    //   int chatroom_id = num_from_string(*tempbuff);
      //
      //   // checks capacity
      //   if (full(chatroom_id, chatrooms, clients[i])){
      //     // notification to say you cannot join
  	  //     sprintf(buffer, "Chatroom %d is full. You cannot join :(\n", chatroom_id);
  	  //     printf("info in buffer: %s\n", buffer);
  	  //     write(clients[i], buffer, sizeof(buffer));
      //
      //   } else {
      //     printf("chatroom_id: %d\n", chatroom_id);
  	  //     // adding the client to chatroom and the main server's lists of clients in chats
  	  //     remove_client(chatrooms, clients[i]);
  	  //     add_client(chatroom_id, chatrooms, clients[i]);
  	  //     for (int i=0; i < totChat * totClients; i++){
  		//         printf("%d ", chatrooms[i]);
  	  //     }
      //
  	  //     // notification to say you have joined
  	  //     sprintf(buffer, "you have joined chatroom %d\n", chatroom_id);
  	  //     printf("info in buffer: %s\n", buffer);
  	  //     write(clients[i], buffer, sizeof(buffer));
      //   }
	    // }

	  }
	  else {
	    // normal portion otherwise
	    //int chat = get_chat_from_client(chatrooms, client
	    strcat(chat_hist[chat], buffer);
	    // for (int i = 0; i<totChat; i++){
    	//       printf("chat_hist[%d]:\n %s", i, chat_hist[i]);
  	  //   }
	    for (int i = 0; i < totClients && clients[i]; i++){
	      int this_chat = get_chat_from_client(chatrooms, clients[i]);
	      // printf("this client: %d\n", clients[i]);
	      // printf("this chat: %d\n", this_chat);
	      if(this_chat == chat){
	        write(clients[i], chat_hist[chat], sizeof(buffer));
	      }
	    }
	  }
	}
	else{
	  close(clients[i]);
	  clients[i] = 0;
	}
      }
    }
  }
  return 0;
}

int num_from_string(char s){
  int num = s - '0';
  return num;
}

int get_chat_from_client(int * chatrooms, int client){
  int i = 0;
  for (int i = 0; i <  totClients * totChat; i++){
    // printf("inside function: %d \n", chatrooms[i]);
  }
  while(i < totClients * totChat){
    //printf("client: %d, chatrooms[%d]: %d\n", client, i, chatrooms[i]);
    if(chatrooms[i] == client){
      // printf("the chat of client %d is %d\n", client, i/totClients);
      return i / totClients;
    }
    i++;
  }
  // printf("the chat of client %d is %d\n", client, -1);
  return -1;
}


// chatroom number to enter; array of users in chatrooms; special client socket code to read and write
int add_client(int chatroom, int * chatrooms, int client_socket){
  int slot = chatroom;
  printf("chatroom: %d\n", chatroom);
  // // chatroom format 0 1 2 0 1 2 0 1 2
  // int col = 1;
  // while(chatrooms[slot] && slot< totChat * totClients){
  //   slot = col * totChat + chatroom;
  //   col++;
  // }
  // chatroom format 0 0 0 1 1 1 2 2 2
  // printf("%d\n", totClients);
  slot = chatroom * totClients;
  int increment = 0;
  while(increment < totClients && chatrooms[slot]){
    increment++;
    slot += increment;
  }
  printf("slot: %d\n", slot);
  chatrooms[slot] = client_socket;
  return slot;
}

// array of users in chatrooms; special client socket code to read and write
int remove_client(int * chatrooms, int client_socket){
  int slot = 0;
  // chatroom format 0 0 0 1 1 1 2 2 2
  // printf("%d\n", totClients);
  int increment = 0;
  while(increment < totClients && chatrooms[slot] != client_socket){
    increment++;
    slot += increment;
  }
  printf("slot: %d\n", slot);
  int chatroom = slot / totClients;
  printf("chatroom: %d\n", chatroom);
  chatrooms[slot] = 0;
  return slot;
}

// If a user wishes to create their own chat
int * create_chat(int * chatrooms, int client_socket){
  printf("ORIGINAL: %d\n", totChat);
  totChat++;
  printf("NEW: %d\n", totChat);
  int * clients = (int *)malloc(sizeof(int) * totChat * totClients);
  for (int i = 0; i < totChat * totClients; i++){
    clients[i] = chatrooms[i];
  }
  add_client(totChat, clients, client_socket);
  return clients;
}

// Checks if a chatroom is full
int full(int chatroom, int * chatrooms, int client_socket){
  int slot = chatroom * totClients;
  int i = 0;
  while (i < totClients && !chatrooms[slot]){
    i++;
    slot++;
  }
  if (i < totClients){
    return 0;
  } else {
    return 1;
  }
}

// lists all the chats
void list_chat(int * chatrooms, int client_socket){
  for (int i = 0; i < totChat; i++){
    printf("=====\nCHAT %d\n=====\n", i);
  }
}

// void subserver(int client_socket) {
//   char buffer[BUFFER_SIZE];
//
//   while (read(client_socket, buffer, sizeof(buffer))) {
//
//     printf("[subserver %d] received: [%s]\n", getpid(), buffer);
//     process(buffer);
//     write(client_socket, buffer, sizeof(buffer));
//   }//end read loop
//   close(client_socket);
//   exit(0);
// }
