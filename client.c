#include "networking.h"

int valid_num(char * s);
int num_from_string(char s);
int add_chat(int * my_chats, int chat);


int main(int argc, char **argv) {

  int server_socket;
  char buffer[BUFFER_SIZE];

  if (argc == 2)
    server_socket = client_setup( argv[1]);
  else
    server_socket = client_setup( TEST_IP );


  // code to first enter a room
  printf("\033[H\033[J");
  printf("which chatroom would you like to connect to?\n");
  printf("Please enter a valid chatroom id (0, 1, 2): ");
  fgets(buffer, sizeof(buffer), stdin);

  while (!valid_num(buffer) || strlen(buffer)>2){
    printf("Please enter a valid chatroom id (0, 1, 2): ");
    fgets(buffer, sizeof(buffer), stdin);
  }

  int * my_chats = malloc(NUM_CHATROOMS);
  add_chat(my_chats, num_from_string(*buffer));
  for(int i = 0; i < NUM_CHATROOMS; i++){
    printf("%d ", my_chats[i]);
  }

  write(server_socket, buffer, sizeof(buffer));
  read(server_socket, buffer, sizeof(buffer));
  printf("%s", buffer);

  // normal chat
  while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    write(server_socket, buffer, sizeof(buffer));
    read(server_socket, buffer, sizeof(buffer));
    // reading the buffer
    if (! strncmp("you have joined chatroom ", buffer, 25) ){
      memcpy( buffer, buffer + 25, 25 * sizeof(char));
      // add to user chat list
      add_chat(my_chats, num_from_string(*buffer));
      for(int i = 0; i < NUM_CHATROOMS; i++){
        printf("%d ", my_chats[i]);
      }
    }
    else {
      printf("received: [%s]\n", buffer);
    }
  }
}

int add_chat(int * my_chats, int chat){
  int slot = 0;
  // printf("chatroom: %d\n", slot);
  while(my_chats[slot] && slot<NUM_CHATROOMS){
    slot++;
  }
  // printf("slot: %d\n", slot);
  my_chats[slot] = chat;
  return chat;
}

int valid_num(char * s){
  if(isdigit(*s)==0){
    return 0;
  }
  if(num_from_string(*s)>=NUM_CHATROOMS){
    return 0;
  }
  return 1;
}

int num_from_string(char s){
  int num = s - '0';
  return num;
}
