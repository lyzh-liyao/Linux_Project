#include <SocketOperation.h>

#define IMAGE_SCALE 20
int soket_init(){
    int    socket_fd;
    struct sockaddr_in     servaddr;
    //初始化Socket
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    //初始化
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(DEFAULT_PORT);//设置的端口为DEFAULT_PORT
    //将本地地址绑定到所创建的套接字上
    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    //开始监听是否有客户端连接
    if( listen(socket_fd, 1) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    return socket_fd;
}

int socket_distroy(int socket_fd){
    close(socket_fd);
    return 0;
}

int socket_accept(int socket_fd, char* buf, int len){
    while(1){
        int     connect_fd;
        //阻塞直到有客户端连接
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)\r\n",strerror(errno),errno);
            continue;
        }

        {//处理客户端信息
            socklen_t rsa_len = sizeof(struct sockaddr_in);
            struct sockaddr_in rsa;
           char *ip = NULL;
           int port = 0;
            if(getpeername(connect_fd, (struct sockaddr *)&rsa, &rsa_len) == 0)
            {
                ip = inet_ntoa(rsa.sin_addr);
                port = ntohs(rsa.sin_port);
            }
            //printf("收到客户端请求：%s:%d\r\n", ip,port);
        }
        if(send(connect_fd, buf, len/IMAGE_SCALE,0) == -1)
                        perror("send error");
        close(connect_fd);
        usleep(33333);
        break;
    }
    return 0;
}

