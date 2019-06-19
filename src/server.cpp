#include <iostream>
#include <cstring>
#include <cstdint>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#define FLATBUFFERS_TRACK_VERIFIER_BUFFER_SIZE
#include "call_generated.h"
#include "flatbuffers/flatbuffers.h"

using namespace test;
using namespace flatbuffers;
using namespace std;

#define PORT 21
int server_sock;

void int_handler(int signo)
{
    printf("force to exit\n");
    close(server_sock);
    exit(-1);
}

int main()
{
    signal(SIGINT, int_handler);

    // 创建监听的套接字
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("socket error");
        exit(1);
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(server_sock, (struct sockaddr *)&server, sizeof(server));

    if (ret == -1)
    {
        perror("bind error");
        exit(1);
    }

    // 设置监听
    ret = listen(server_sock, 1);
    if (ret == -1)
    {
        perror("listen error");
        exit(1);
    }

    struct sockaddr_in client;
    socklen_t len;
    int cfd;
    u_int8_t buffer[100];
    uint8_t *ptr;
    uint32_t size_recv;
    uint32_t total_size;

    for (;;)
    {
        cfd = -1;
        len = sizeof(client);
        cfd = accept(server_sock, (struct sockaddr *)&client, &len);

        memset(buffer, 0, 100);
        ptr = (uint8_t *)buffer;
        size_recv = 0;
        total_size = 0;

        /*
        receive data
         */
        do
        {
            size_recv = read(cfd, ptr, 100);
            ptr += size_recv;
            total_size += size_recv;
        } while (size_recv != 0);

        Verifier v(buffer, total_size);

        if (Verifymsg_tBuffer(v))
        {
            cout << v.GetComputedSize() << endl;
            auto mm = Getmsg_t(buffer);
            switch (mm->type())
            {
            case type_t_a:
            {
                auto d = mm->data();
                printf("x:%f,y:%f\n", d->x(), d->y());
            }
            break;
            case type_t_b:
            {
                auto coordinate = mm->coordinates();
                printf("x:%f,y:%f,z:%f\n", coordinate->x(), coordinate->y(), coordinate->z());
            }
            break;
            default:
                break;
            }
        }

        close(cfd);
    }

    return 0;
}
