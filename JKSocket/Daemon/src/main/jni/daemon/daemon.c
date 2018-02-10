#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "log.h"

#define LOG_TAG         "Daemon"
#define BUFFER_LENGTH 1024

//创建的localsocket的绝对路径
#define PATH "/data/data/com.jared.jnidaemon/app_socket/localsocket"
#define PATH_PRE "/data/data/"
#define PATH_END "/app_socket/localsocket"
char package_name[128];
char socket_path[128];
int server_sockfd;
int client_map[100];

/* signal term handler */
static void sigterm_handler(int signo) {
    LOGD(LOG_TAG, "handle signal: %d ", signo);
    if (server_sockfd > 0)
        close(server_sockfd);
}

void *client_process(void *args) {
    int fd = *((int *) args);
    char data_recv[BUFFER_LENGTH];
    const char *data_send = "OK";
    int i_recvBytes;

    while (1) {
        //Reset data.
        memset(data_recv, 0, BUFFER_LENGTH);

        i_recvBytes = read(fd, data_recv, BUFFER_LENGTH);
        if (i_recvBytes == 0) {
            LOGE(LOG_TAG, "Maybe the client has closed");
            break;
        }
        if (i_recvBytes == -1) {
            LOGE(LOG_TAG, "read error!");
            break;
        }
        if (strcmp(data_recv, "quit") == 0) {
            LOGE(LOG_TAG, "Quit command!");
            break;
        }
        LOGD(LOG_TAG, "read from client : %s", data_recv);
        if (write(fd, data_send, strlen(data_send)) == -1) {
            break;
        }
    }

    LOGD(LOG_TAG, "Process exit, fd: %d ", fd);
    close(fd);
    pthread_exit(0);
}

int create_socket_server(int client_num) {
    /* delete the socket file */
    unlink(socket_path);

    /* create a socket */
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        LOGE(LOG_TAG, "socket error!");
        return -1;
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, socket_path);

    /* bind with the local file */
    if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        LOGE(LOG_TAG, "binder error %s !", socket_path);
        return -1;
    }

    /* listen */
    listen(server_sockfd, client_num);

    char ch[BUFFER_LENGTH];
    int client_sockfd;
    struct sockaddr_un client_addr;
    socklen_t len = sizeof(client_addr);

    while (1) {
        /* accept a connection */
        client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &len);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, (void *) (&client_process),
                           (void *) (&client_sockfd)) == -1) {
            LOGE(LOG_TAG, "pthread_create error!");
            continue;
        }
    }

    return 0;
}

void args_process(int argc, char *argv[]) {
    int i = 0;
    for (i = 0; i < argc; i++) {
        if (!strcmp("-p", argv[i])) {
            strcpy(package_name, argv[i + 1]);
            LOGD(LOG_TAG, "package name: %s", package_name);
        }
    }

    strcpy(socket_path, PATH_PRE);
    strcat(socket_path, package_name);
    strcat(socket_path, PATH_END);
}

int main(int argc, char *argv[]) {

    LOGI(LOG_TAG, "Copyright (c) 2018, eastmoon<chenjianneng1117@gmail.com>");

    LOGI(LOG_TAG, "=========== daemon start =======");

    args_process(argc, argv);

    /* add signal */
    signal(SIGTERM, sigterm_handler);

    create_socket_server(5);
}
