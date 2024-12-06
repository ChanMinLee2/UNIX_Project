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

void getTime(char *buf) {
    char days[10];

    time_t tim; 
    time(&tim); // tim 생성
    tzset(); // tzname에 시간대 할당
    
    // tm 구조체 생성
    struct tm *t = localtime(&tim);

    // 요일 처리
    if (t->tm_wday == 0)
        strcpy(days, "일");
    else if (t->tm_wday == 1)
        strcpy(days, "월");
    else if (t->tm_wday == 2)
        strcpy(days, "화");
    else if (t->tm_wday == 3)
        strcpy(days, "수");
    else if (t->tm_wday == 4)
        strcpy(days, "목");
    else if (t->tm_wday == 5)
        strcpy(days, "금");
    else if (t->tm_wday == 6)
        strcpy(days, "토");

    sprintf(buf, "%d년 %.2d월 %.2d일 %s요일 %.2d시 %.2d분 %.2d초 %s\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, days, t->tm_hour, t->tm_min, t->tm_sec, tzname[0]);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    pid_t client_pid;

    int history = open("history.txt", O_WRONLY | O_APPEND); // 이어쓰기 전용 열기

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

    // 기록 적기
    char buf[100];
    getTime(buf);
    write(history, buf, strlen(buf));
    for (int i = 0; i < SIZE; i++) {
        
        for (int j = 0; j < SIZE; j++) {
            sprintf(buf, "%c ", board[i][j]);
            write(history, buf, 2);
        }
        write(history, "\n", 1);
    }

    close(history);
    close(new_socket);
    close(server_fd);
    return 0;
}
