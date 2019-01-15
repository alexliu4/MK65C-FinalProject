#include "networking.h"


int main(int argc, char **argv) {

  int server_socket;
  char buffer[BUFFER_SIZE];

  char username[BUFFER_SIZE];
  printf("\033[H\033[J");
  // system("cls");
  printf("Enter your username and press [enter] twice: ");
  fgets(username, BUFFER_SIZE, stdin);
  *strchr(username, '\n') = 0;
  strcat(username,": ");

  if (argc == 2)
    server_socket = client_setup( argv[1]);
  else
    server_socket = client_setup( TEST_IP );

  int fds[2];
  pipe(fds);

  int f = fork();
  if (f==0){
    close(fds[1]);
    dup2(fds[0], STDIN_FILENO);
    while(1){
      read(server_socket, buffer, sizeof(buffer));
      printf("\033[H\033[J");
      printf("%s\n", buffer);
      //printf("Me: ");
    }
  }
  else{
    close(fds[0]);
    dup2(fds[1], STDOUT_FILENO);
    while (1){
      fgets(buffer, sizeof(buffer), stdin);
      char temp[BUFFER_SIZE];
      strcpy(temp, buffer);
      strcpy(buffer, username);
      strcat(buffer, temp);
      //strcat(chat_history, buffer);
      write(server_socket, buffer, sizeof(temp));
    }
  }
/*
  while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    write(server_socket, buffer, sizeof(buffer));
    read(server_socket, buffer, sizeof(buffer));
    printf("received: %s\n", buffer);
  }
*/
}
