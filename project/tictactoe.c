#include "tictactoe.h"

// 보드 출력
void print_board(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}


// 유효한 움직임인지 확인
int valid_move(char board[SIZE][SIZE], int move) {
    int row = (move - 1) / SIZE;
    int col = (move - 1) % SIZE;
    return board[row][col] != 'X' && board[row][col] != 'O';
}