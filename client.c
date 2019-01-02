#include "pipe_networking.h"


int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  char msg[BUFFER_SIZE];
  char line[BUFFER_SIZE];

  while(1){

    printf("Send data to the server: ");
    fgets(line, 100, stdin);
    write(to_server, line, sizeof(line));
    read(from_server, msg, sizeof(msg));
    printf("Server processed data: %s\n", msg);
  }
}
