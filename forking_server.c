#include "networking.h"

void process(char *s);
void subserver(int from_client);
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

  //char * chat_history = chat_shared_mem('a', 1000000000)
  //memset(chat_history, 0, sizeof(chat_history));

  char * chat_history = calloc(100000, sizeof(char));
  
  int listen_socket;
  int listen_socket_1;
  int client;
  int f;
  listen_socket = server_setup();
  listen_socket_1 = listen_socket;

  int clients[NUM_CLIENTS];
  for(int i = 0; i < NUM_CLIENTS; i++){
    clients[i] = 0;
  }	
  char buffer[BUFFER_SIZE];

  fd_set read_fds;

  while (1) {
    FD_ZERO(&read_fds);
    FD_SET(listen_socket, &read_fds);

    for (int i = 0; i < NUM_CLIENTS; i++){
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
      if(client = server_connect(listen_socket)){
	for(int i = 0; i < NUM_CLIENTS && clients[i]!=client; i++){
	  if(!clients[i]){
	    clients[i] = client;
	    i = NUM_CLIENTS; // stop the loop	    
	  }
	}
      }
    }

    for (int i = 0; i<NUM_CLIENTS && clients[i]; i++){
      if(FD_ISSET(clients[i], &read_fds)){
	if(read(clients[i], buffer, sizeof(buffer))>0){
	  printf("[subserver %d] received: %s\n", getpid(), buffer);
	  strcat(chat_history, buffer);
	  for (int i = 0; i<NUM_CLIENTS && clients[i]; i++){
	    write(clients[i], chat_history, sizeof(buffer));
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

void subserver(int client_socket) {
  char buffer[BUFFER_SIZE];

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
