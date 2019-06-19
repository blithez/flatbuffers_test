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
#include <netdb.h>

#define FLATBUFFERS_TRACK_VERIFIER_BUFFER_SIZE
#include "call_generated.h"
#include "flatbuffers/flatbuffers.h"

using namespace test;
using namespace flatbuffers;
using namespace std;

#define DOMAIN  "azhang23.xyz"
#define IP_ADDR "192.168.1.110"
#define PORT    21
int sock;

void int_handler(int signo)
{
    printf("force to exit\n");
    close(sock);
    exit(-1);
}

void getIp(const char *domain_str, struct in_addr &addr)
{
    struct hostent *hp;
    hp = gethostbyname(domain_str);
    if( !hp )
    {
        perror("failed to resolve domain");
    }
    
    printf("host name:%s\n",hp->h_name);
    
    switch(hp->h_addrtype)
    {
        case AF_INET:
            for(int i=0;hp->h_addr_list[i];++i)
            {
                memcpy( &addr, hp->h_addr_list[i], sizeof(struct in_addr) );
                //printf( "address:\t%s\n", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]) );
            }
            break;
        case AF_INET6:
            break;
        default:
            break;
    }

}

int main(int argc, char* argv[])
{
    signal(SIGINT,int_handler);
    
    #if 1
    // 创建监听的套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        perror("socket error");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    getIp( DOMAIN, addr.sin_addr );
    //inet_aton(IP_ADDR,(struct in_addr*)&addr.sin_addr.s_addr);

    if( connect(sock,(struct sockaddr*)&addr,sizeof(addr)) == -1 )
    {
        perror("failed to connect");
    }

    FlatBufferBuilder builder;

    type_t tp = type_t_a;

    float height = 1.0f;
    float width  = 1.0f;

    float x,y,z;

    switch(argc)
    {
        case 2:
            sscanf(argv[1],"%f",&height);
            break;
        case 3:
            sscanf(argv[1],"%f",&height);
            sscanf(argv[2],"%f",&width);
            break;
        case 4:
            tp = type_t_b;
            sscanf(argv[1],"%f",&x);
            sscanf(argv[2],"%f",&y);
            sscanf(argv[3],"%f",&z);
    }

    switch(tp)
    {
        case type_t_a:
        {
            data_t da(height,width);

            auto m = Createmsg_t(builder,tp,&da);
            builder.Finish(m);
        }
        break;
        case type_t_b:
        {
            _3d cd(x,y,z);
            printf( "x:%f,y:%f,z:%f\n",x,y,z );
            auto m = Createmsg_t(builder,tp,nullptr,&cd);
            builder.Finish(m);
        }
        break;
        default:
            exit(-1);
    }

    uint8_t* buf = builder.GetBufferPointer();
    int size = builder.GetSize();

    send(sock,buf,size,0);

    close(sock);
    #endif
    return 0;
}
