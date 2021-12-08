#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/errno.h>
#include <string.h>

#define MAXLINE 512
#define MAX_MEM 10
#define NAME_LEN 20
#define LISTENQ 5
#define MAX_GAME 5

typedef struct pthread_arg_t
{
    int socket_id;
    struct sockaddr_in client_address;
} pthread_arg_t;

int socket_fd, connect_fd[MAX_MEM];
int gamestatus[MAX_MEM];
char user_list[MAX_MEM][NAME_LEN];

void *connect_controler(void *arg)
{
    char user_name[NAME_LEN];
    char msg_rcv[MAXLINE];
    char msg_send[MAXLINE * 2];

    pthread_arg_t *pthread_arg = (pthread_arg_t *)arg;
    int socket_id = pthread_arg->socket_id;
    struct sockaddr_in client_address = pthread_arg->client_address;

    // receiving user name from client
    int length = recv(connect_fd[socket_id], user_name, NAME_LEN, 0);
    if (length > 0)
    {

        strncpy(user_list[socket_id], user_name, strlen(user_name) - 1);
        /*TODO: remove \n from end*/
        printf("\t[Info] User Name: %s\n", user_list[socket_id]);
    }

    // receiving message from client
    while (1)
    {
        memset(msg_rcv, '\0', sizeof(msg_rcv));
        memset(msg_send, '\0', sizeof(msg_send));

        length = recv(connect_fd[socket_id], msg_rcv, MAXLINE, 0);
        if (length > 0)
        { //Message received
            msg_rcv[length] = 0;
            printf("\t[Client] message by %s", user_list[socket_id]);
            printf("\n\t%s", msg_rcv);

            if (strcmp(msg_rcv, "ls\n") == 0)
            {
                //printf("debug: rcv ls\n");
                strcpy(msg_send, "\n[Server] Players Online:\n");
                for (int i = 0; i < MAX_MEM; i++)
                {
                    if (connect_fd[i] != -1)
                    {
                        char buffer[20];
                        sprintf(buffer, "No.%2d ", i);
                        strcat(msg_send, buffer);
                        strcat(msg_send, user_list[i]);
                        strcat(msg_send, "\n");
                    }
                }
                send(connect_fd[socket_id], msg_send, strlen(msg_send), 0);
            }
            if (strcmp(msg_rcv, "invite\n") == 0)
            {
                strcpy(msg_send, "\n[Server] Press the number of Player to invite:\n");
                for (int i = 0; i < MAX_MEM; i++)
                {
                    if (connect_fd[i] != -1)
                    {
                        char buffer[20];
                        sprintf(buffer, "No.%2d ", i);
                        strcat(msg_send, buffer);
                        strcat(msg_send, user_list[i]);
                        strcat(msg_send, "\n");
                    }
                }
                send(connect_fd[socket_id], msg_send, strlen(msg_send), 0);

                memset(msg_rcv, '\0', sizeof(msg_rcv));

                length = recv(connect_fd[socket_id], msg_rcv, MAXLINE, 0);
                int invite_id = atoi(msg_rcv);
                if (invite_id != 0)
                {
                    memset(msg_send, '\0', sizeof(msg_send));
                    strcat(msg_send, "Error: Invalid User ID.Please invite again.");
                    send(connect_fd[socket_id], msg_send, strlen(msg_send), 0);
                }

                memset(msg_send, '\0', sizeof(msg_send));
                strcat(msg_send, "recive invite from ");
                strcat(msg_send, user_list[socket_id]);
                strcat(msg_send, " accpt challenge (y/n)?\n");
                send(connect_fd[socket_id], "Wait for reply...\n", strlen("Wait for reply...\n"), 0);
                send(connect_fd[invite_id], msg_send, strlen(msg_send), 0);

                gamestatus[socket_id] = invite_id;
                gamestatus[invite_id] = socket_id;
            }
            if (strcmp(msg_rcv, "y\n") == 0)
            {

                send(connect_fd[socket_id], "Game Start!\n", strlen("Game Start!\n"), 0);
                send(connect_fd[gamestatus[socket_id]], "Game Start!\n", strlen("Game Start!\n"), 0);

                usleep(100);

                send(connect_fd[socket_id], "Please enter [place (num)] to place X:\n", strlen("Please enter [place (num)] to place X:\n"), 0);
                send(connect_fd[gamestatus[socket_id]], "Wait for another player...\n", strlen("Wait for another player...\n"), 0);
            }
            else if (strcmp(msg_rcv, "n\n") == 0)
            {
                memset(msg_send, '\0', sizeof(msg_send));
                strcat(msg_send, user_list[gamestatus[socket_id]]);
                strcat(msg_send, "decline your invitation\n");
                send(connect_fd[socket_id], msg_send, strlen(msg_send), 0);
            }

            if (strncmp(msg_rcv, "place", 5) == 0)
            {
                send(connect_fd[gamestatus[socket_id]], msg_rcv, strlen(msg_rcv), 0);
            }

            if (strcmp(msg_rcv, "terminate\n") == 0)
            {
                memset(msg_send, '\0', sizeof(msg_send));
                strcat(msg_send, "Game End! BYEBYE\n");
                send(connect_fd[socket_id], msg_send, strlen(msg_send), 0);
                send(connect_fd[gamestatus[socket_id]], msg_send, strlen(msg_send), 0);
                close(connect_fd[socket_id]);
                close(connect_fd[gamestatus[socket_id]]);

                gamestatus[socket_id] = -1;
                socket_id = -1;
            }

            if (strcmp(msg_rcv, "Player 1 Wins\n") == 0)
            {
                send(connect_fd[socket_id], "Win\n", strlen("Win\n"), 0);
                send(connect_fd[gamestatus[socket_id]], "Lose\n", strlen("Lose\n"), 0);
            }

            if (strcmp(msg_rcv, "Player 2 Wins\n") == 0)
            {
                send(connect_fd[socket_id], "Lose\n", strlen("Lose\n"), 0);
                send(connect_fd[gamestatus[socket_id]], "Win\n", strlen("Win\n"), 0);
            }

            if (strcmp(msg_rcv, "Tie\n") == 0)
            {
                send(connect_fd[socket_id], "Tie\n", strlen("Tie\n"), 0);
                send(connect_fd[gamestatus[socket_id]], "Tie\n", strlen("Tie\n"), 0);
            }
        }
        else
        {
            memset(user_list[socket_id], '\0', sizeof(user_list[socket_id]));
            gamestatus[socket_id] = -1;
            connect_fd[socket_id] = -1;
        }
    }


}

int main(int argc, char *argv[])
{

    long i;
    int port;
    pthread_t thread;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sock_len;
    char buff[MAXLINE];
    pthread_arg_t *pthread_arg;

    port = argc > 1 ? atoi(argv[1]) : 0;
    if (!port)
    {
        printf("Enter Port: ");
        scanf("%d", &port);
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        printf("Failed to create socket: %s\n", strerror(errno));
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Failed to bind: %s\n", strerror(errno));
        exit(1);
    }

    listen(socket_fd, LISTENQ);
    if (listen(socket_fd, LISTENQ) < 0)
    {
        printf("Failed to listen: %s\n", strerror(errno));
        exit(1);
    }
    else
    {

        printf("\t[Info] Listening ...\n");
    }

    for (i = 0; i < MAX_MEM; i++)
    {
        gamestatus[i] = -1;
        connect_fd[i] = -1;
    }

    memset(user_list, '\0', sizeof(user_list));
    printf("\t[Info] Initialize Done\n");

    while (1)
    {
        pthread_arg = (pthread_arg_t *)malloc(sizeof *pthread_arg);
        if (!pthread_arg)
        {
            printf("Failed to malloc pthread argument: %s\n", strerror(errno));
            continue;
        }

        sock_len = sizeof pthread_arg->client_address;
        for (i = 0; (i < MAX_MEM) && (connect_fd[i] != -1); i++)
            ; // look for free connect_fd

        pthread_arg->socket_id = i;

        connect_fd[i] = accept(socket_fd, (struct sockaddr *)&pthread_arg->client_address, &sock_len);
        if (connect_fd[i] == -1)
        {
            printf("Failed to accept: %s\n", strerror(errno));
            free(pthread_arg);
            continue;
        }

        printf("\t[Info] Waiting for user name ...\n");

        if (pthread_create(malloc(sizeof(pthread_t)), NULL, connect_controler, (void *)pthread_arg) != 0)
        {
            perror("pthread_create");
            free(pthread_arg);
            continue;
        }
    }

    return 0;
}