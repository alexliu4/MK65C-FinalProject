#include "networking.h"

int valid_num(char * s);
int num_from_string(char s);

int main(int argc, char **argv) {

  int server_socket;
  char buffer[BUFFER_SIZE];

  if (argc == 2)
    server_socket = client_setup( argv[1]);
  else
    server_socket = client_setup( TEST_IP );

  printf("which chatroom would you like to connect to?\n");
  printf("Please enter a valid chatroom id (0, 1, 2): ");
  fgets(buffer, sizeof(buffer), stdin);

  while (!valid_num(buffer) || strlen(buffer)>2){
    printf("Please enter a valid chatroom id (0, 1, 2): ");
    fgets(buffer, sizeof(buffer), stdin);
  }

  write(server_socket, buffer, sizeof(buffer));
  read(server_socket, buffer, sizeof(buffer));
  printf("%s", buffer);

  while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    write(server_socket, buffer, sizeof(buffer));
    read(server_socket, buffer, sizeof(buffer));
    printf("received: [%s]\n", buffer);
  }
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
