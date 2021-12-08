#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>

#define MAX_LINE 1024
#define NAME_LEN 20
#define SERVER_NAME_LEN_MAX 255

/* 0 repersernt O , 1 represent X*/
int player;
int winner;
/* Every position on table only have three situation : empty , O , X*/
char board[3][3];
/* When left_empty is zero Game Over!*/
int left_empty;
int num;
int colum;
int row;

void gameinit()
{
  winner = -1;
  player = 1;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      board[i][j] = ' ';
    }
  }
  left_empty = 9;
}

void show_current_board()
{

  printf(" _________________\n");
  printf("|     |     |     | \n");
  printf("|  %c  |  %c  |  %c  |\n", board[0][0], board[0][1], board[0][2]);
  printf("|_____|_____|_____|\n");
  printf("|     |     |     |\n");
  printf("|  %c  |  %c  |  %c  |\n", board[1][0], board[1][1], board[1][2]);
  printf("|_____|_____|_____|\n");
  printf("|     |     |     |\n");
  printf("|  %c  |  %c  |  %c  |\n", board[2][0], board[2][1], board[2][2]);
  printf("|_____|_____|_____|\n");
}

void end_check()
{
  int i;
}

typedef struct pthread_arg_t
{
  int new_socket_fd;
  struct sockaddr_in client_address;
  char *user_name;
} pthread_arg_t;

void print_board();

void *server_connect(int socket_fd, struct sockaddr_in *address)
{
  if (connect(socket_fd, (struct sockaddr *)address, sizeof *address) < 0)
  {
    printf("connect failed");
    exit(0);
  }
  else
    printf("Successful connecting\n");
}

void *connect_controler(void *arg)
{
  pthread_arg_t *pthread_arg = (pthread_arg_t *)arg;
  int socket_fd = pthread_arg->new_socket_fd;
  char server_mesg[MAX_LINE];

  while (1)
  {

    memset(server_mesg, 0, sizeof(server_mesg));

    //board_init

    int response_len = recv(socket_fd, server_mesg, MAX_LINE, 0);
    if (response_len < 0)
    {
      printf("Failed to recieve: %s\n", strerror(errno));
      exit(1);
    }
    else if (response_len == 0)
    {
      printf("\t[Info] Disconnected\n");
      break;
    }

    printf("[Server] %s", server_mesg);

    if (strncmp(server_mesg, "Game Start!\n", strlen("Game Start!\n")) == 0)
    {
      gameinit();
    }

    if (strcmp(server_mesg, "Win\n") == 0)
    {
      printf("YOU WIN THE GAME!\n");
      exit(0);
    }

    if (strcmp(server_mesg, "Lose\n") == 0)
    {
      printf("YOU LOSE THE GAME!\n");
      exit(0);
    }

    if (strcmp(server_mesg, "Tie\n") == 0)
    {
      printf("TIE!\n");
      exit(0);
    }

    if (strncmp(server_mesg, "place", 5) == 0)
    {
      char *ptr = strstr(server_mesg, "place ");
      ptr += strlen("place ");
      int num = atoi(ptr);
      int input_is_unvaild = 0;

      switch (num)
      {
      case 9:
        row = 0;
        colum = 2;
        break;
      case 8:
        row = 0;
        colum = 1;
        break;
      case 7:
        row = 0;
        colum = 0;
        break;
      case 6:
        row = 1;
        colum = 2;
        break;
      case 5:
        row = 1;
        colum = 1;
        break;
      case 4:
        row = 1;
        colum = 0;
        break;
      case 3:
        row = 2;
        colum = 2;
        break;
      case 2:
        row = 2;
        colum = 1;
        break;
      case 1:
        row = 2;
        colum = 0;
        break;
      default:
        input_is_unvaild = 1;
        break;
      }

      if (input_is_unvaild == 1)
      {
        send(socket_fd, "terminate\n", strlen("terminate\n"), 0);
        continue;
      }

      board[row][colum] = 'O';
      show_current_board();
      printf("Your Turn!\n");
      left_empty--;

      // Win check
      char key = ' ';
      // Check Rows
      for (int i = 0; i < 3; i++)
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] != ' ')
          key = board[i][0];
      // check Columns
      for (int i = 0; i < 3; i++)
        if (board[0][i] == board[1][i] && board[0][i] == board[2][i] && board[0][i] != ' ')
          key = board[0][i];
      // Check Diagonals
      if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[1][1] != ' ')
        key = board[1][1];
      if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[1][1] != ' ')
        key = board[1][1];

      //Declare Winner if any

      if (key == 'X')
      {
        send(socket_fd, "Player 1 Wins\n", strlen("Player 1 Wins\n"), 0);
        winner = 1;
      }

      if (key == 'O')
      {
        send(socket_fd, "Player 2 Wins\n", strlen("Player 2 Wins\n"), 0);
        winner = 2;
      }

      if (left_empty == 0)
      {
        send(socket_fd, "Tie\n", strlen("Tie\n"), 0);
      }
    }

    //printf("[Server] %s", server_mesg);

    fflush(stdout);
  }
}
void send_mesg(pthread_arg_t arg)
{
  pthread_arg_t pthread_arg = arg;
  int socket_fd = pthread_arg.new_socket_fd;
  char *user_name = pthread_arg.user_name;

  char client_mesg[MAX_LINE];
  char buff[MAX_LINE];
  memset(client_mesg, '\0', sizeof(client_mesg));
  memset(buff, '\0', sizeof(buff));

  send(socket_fd, user_name, strlen(user_name), 0);

  while (fgets(client_mesg, MAX_LINE, stdin) != NULL)
  {
    if (strstr(client_mesg, "place "))
    {
      char *ptr = strstr(client_mesg, "place ");
      ptr += strlen("place ");
      int num = atoi(ptr);

      int input_is_unvaild = 0;
      if (left_empty == 0)
      {
        // send tie
        printf("tie\n");
        exit(0);
      }

      switch (num)
      {
      case 9:
        row = 0;
        colum = 2;
        break;
      case 8:
        row = 0;
        colum = 1;
        break;
      case 7:
        row = 0;
        colum = 0;
        break;
      case 6:
        row = 1;
        colum = 2;
        break;
      case 5:
        row = 1;
        colum = 1;
        break;
      case 4:
        row = 1;
        colum = 0;
        break;
      case 3:
        row = 2;
        colum = 2;
        break;
      case 2:
        row = 2;
        colum = 1;
        break;
      case 1:
        row = 2;
        colum = 0;
        break;
      default:
        input_is_unvaild = 1;
        break;
      }

      if (input_is_unvaild == 1)
      {
        printf("Error Input\n");
        continue;
      }

      if(board[row][colum]!= ' ')
      {
        printf("Error Input\n");
        continue;
      }

      board[row][colum] = 'X';
      show_current_board();

      left_empty--;
    }
    send(socket_fd, client_mesg, strlen(client_mesg), 0);
    memset(client_mesg, '\0', sizeof(client_mesg));
  }
}

int main(int argc, char *argv[])
{
  int server_port;
  struct sockaddr_in address, client_addr;
  char server_addr[SERVER_NAME_LEN_MAX + 1] = {0};
  char user_name[NAME_LEN];
  pthread_arg_t pthread_arg;

  if (argc > 1)
  {
    strncpy(server_addr, argv[1], SERVER_NAME_LEN_MAX);
  }
  else
  {
    printf("Enter Server Address: ");
    scanf("%s", server_addr);
  }

  /* Get server port from command line arguments or stdin. */
  server_port = argc > 2 ? atoi(argv[2]) : 0;
  if (!server_port)
  {
    printf("Enter Port: ");
    scanf("%d", &server_port);
  }

  printf("Please enter your Username : ");
  scanf("%s", user_name);
  pthread_arg.user_name = user_name;
  pthread_arg.client_address = address;

  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(server_addr);
  address.sin_port = htons(server_port);
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  server_connect(socket_fd, &address);

  pthread_arg.new_socket_fd = socket_fd;

  pthread_t thread;
  pthread_create(&thread, NULL, connect_controler, &pthread_arg);

  send_mesg(pthread_arg);

  printf("closed client\n");
  close(socket_fd);
  pthread_exit(NULL);

  exit(0);
}