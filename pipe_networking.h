#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define ACK "HOLA"

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000

void server_handshake_part1(int *from_client);
int server_handshake_part2(int *to_client, int *from_client);

int client_handshake(int *to_server);

#endif
