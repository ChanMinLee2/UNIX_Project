#include "tictactoe.h"

char board[SIZE][SIZE];

void sig_handler(int signo) {
    printf("You Win!\n");
    exit(0);
}

int main() {
    int sock = 0, valread, length;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    pid_t pid;


    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 주소 설정
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 서버 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // server에 pid 전달
    pid = getpid();
    send(sock, &pid, sizeof(pid), 0);

    // signal handler 등록
    signal(SIGUSR1, sig_handler);
    while (1) {
        // 보드 상태 수신
        recv(sock, board, sizeof(board), 0);
        print_board(board);

        int move;
        printf("Waiting for opponent's move\n");
        length = recv(sock, board, sizeof(board), 0);

        // 상대가 승리했다고 소켓을 보낸 경우
        if (board[0][0] == 'W' && board[0][1] == 'i' && board[0][2] == 'n') {
            recv(sock, board, sizeof(board), 0);
            print_board(board);
            printf("You Lose!\n");
            break;
        }

        // 입력 받기
        print_board(board);
        printf("Enter your move (1-9): ");
        scanf("%d", &move);

        // 올바른 입력인지 체크
        while (!valid_move(board, move)) {
            printf("Invalid move. Try again: ");
            scanf("%d", &move);
        }

        send(sock, &move, sizeof(move), 0);

    }

    close(sock);
    return 0;
}
