#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define SIZE 3

void print_board(char board[SIZE][SIZE]);
int valid_move(char board[SIZE][SIZE], int move);