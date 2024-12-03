#include "tictactoe.h"
#include <fcntl.h>
#include <time.h>

extern char *tzname[2]; //시간대를 위한 변수

char board[SIZE][SIZE];
int current_player = 1;

// 초기화
void initialize_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = '1' + (i * SIZE + j);
        }
    }
}

// 승리 조건 체크
int check_winner() {
    for (int i = 0; i < SIZE; i++) {
        // 행 체크
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) return 1;
        // 열 체크
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) return 1;
    }
    // 대각선 체크
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) return 1;
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) return 1;
    return 0;
}

// 움직임 적용
void apply_move(int move, char mark) {
    int row = (move - 1) / SIZE;
    int col = (move - 1) % SIZE;
    board[row][col] = mark;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    pid_t client_pid;

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 대기
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");

    // 클라이언트 연결
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Player connected!\n");

    // client로부터 pid 받기
    recv(new_socket, &client_pid, sizeof(client_pid), 0);
    initialize_board();
    while (1) {
        // 보드 상태 전송
        send(new_socket, board, sizeof(board), 0);
        print_board(board);

        // 사용자 입력 받기
        int move;
        printf("Enter your move (1-9): ");
        scanf("%d", &move);

        while (!valid_move(board, move)) {
            printf("Invalid move. Try again: ");
            scanf("%d", &move);
        }

        apply_move(move, 'X');

        // 승리 조건 확인
        if (check_winner()) {
            printf("You win!\n");
            send(new_socket, "Win", 5, 0); // 승리 여부 전송
            sleep(1);
            send(new_socket, board, sizeof(board), 0); // 보드 상태 전송
            break;
        }

        // 보드 상태 전송
        send(new_socket, board, sizeof(board), 0);

        // 상대방 움직임 대기
        printf("Waiting for opponent's move...\n");
        recv(new_socket, &move, sizeof(move), 0);
        apply_move(move, 'O');

        if (check_winner()) {
            print_board(board);
            printf("Opponent wins!\n");

            // client에 시그널을 보내 서버가 이겼다는 것을 나타내기 (client는 scanf를 실행 중)
            if (kill(client_pid, SIGUSR1) != 0) {
                perror("kill");
            }
            break;
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
