//#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include "UartDriver.h"
#include "SocketOperation.h"
#define UartBuffSize 1024
#define SocketBuffSize 1024
typedef struct _Socekt_List_T Socekt_List_T;
struct _Socekt_List_T{
    int Socket_fd;
    int count;
    Socekt_List_T* Next;
    Socekt_List_T* Prev;

};
Socekt_List_T Socket_Head = {0};
//pthread_rwlock_t Socket_Head_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t Socket_Head_Mutex = PTHREAD_MUTEX_INITIALIZER;
int UartFD = -1;
int SocketFD = -1;


void* UartToSocket(void* arg){
    char UartBuff[UartBuffSize] = {0};
    Socekt_List_T* tmp_Socket_List = NULL;
    while(1){
        int cnt = read(UartFD, UartBuff, UartBuffSize);
        printf("收到串口数据：%d\r\n", cnt);
        pthread_mutex_lock(&Socket_Head_Mutex);
        tmp_Socket_List = Socket_Head.Next;
        while(tmp_Socket_List  !=  &Socket_Head){
            if((cnt = send(tmp_Socket_List->Socket_fd, UartBuff, cnt, 0))  < 0){
                printf("UartToSocket Error retcode:%d,errno:%d,%s\r\n", cnt, errno,strerror(errno));
			
		   Socekt_List_T* p = tmp_Socket_List;
		   tmp_Socket_List = tmp_Socket_List->Prev;
                p->Prev->Next = p->Next;
                p->Next->Prev = p->Prev;
                close(p->Socket_fd); 
                free(p); 
            }else{
                printf("UartToSocket字节数%d\r\n", cnt);
            }
	    printf("uartfd:%d,socketfd:%d\r\n", UartFD, tmp_Socket_List->Socket_fd);
            tmp_Socket_List = tmp_Socket_List->Next;
        }
        pthread_mutex_unlock(&Socket_Head_Mutex);
    }
}

void* SocketToUart(void* arg){
    char SocketBuff[SocketBuffSize] = {0};
    Socekt_List_T* tmp_Socket_List;
    int cnt = 0;
    while(1){
        pthread_mutex_lock(&Socket_Head_Mutex);
        tmp_Socket_List = Socket_Head.Next;
        while(tmp_Socket_List != &Socket_Head){
            if((cnt = recv(tmp_Socket_List->Socket_fd, SocketBuff, SocketBuffSize, 0)) <= 0){

            }else{
                printf("收到Socket数据：%d\r\n", cnt);
                if((cnt = write(UartFD, SocketBuff, cnt)) < 0){
                    printf("Uart Write Error retcode:%d,errno:%d,%s\r\n", cnt, errno,strerror(errno));
                    return NULL;
                }else{
                    printf("SocketToUart字节数%d\r\n", cnt);
                }
            }
            tmp_Socket_List = tmp_Socket_List->Next;
        }
        pthread_mutex_unlock(&Socket_Head_Mutex);
        usleep(50 * 1000);
    }
}

void signal_init(int no){
	printf("signal_init\r\n"); 
	close(UartFD);
	close(SocketFD);
	exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT,signal_init);
    UartFD = Uart_Init();
    if(UartFD < 0) return 0;
    printf("UartFD:%d\r\n", UartFD);
    SocketFD = Soket_Init();
    printf("SocketFD:%d\r\n", SocketFD);
    int ret = -1;
    Socket_Head.Next = &Socket_Head;
    Socket_Head.Prev = &Socket_Head;
    pthread_t UartToSocket_P, SocketToUart_P;
    ret = pthread_create(&UartToSocket_P, NULL, UartToSocket, NULL);
    if(ret != 0)
        perror("UartToSocket_P 创建失败\r\n");
    ret = pthread_create(&SocketToUart_P, NULL, SocketToUart, NULL);
    if(ret != 0)
        perror("UartToSocket_P 创建失败\r\n");
    while(1) {
        int connect_fd;
        //阻塞直到有客户端连接
        printf("等待客户端连接\r\n");
        if( (connect_fd = accept(SocketFD, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d,%s)\r\n",strerror(errno),errno,strerror(errno));
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
            printf("收到客户端请求：%s:%d\r\n", ip,port);
        }
         Socekt_List_T* tmp_Socket_List = malloc(sizeof(Socekt_List_T));
         tmp_Socket_List->Socket_fd = connect_fd;
         fcntl(connect_fd,F_SETFL,O_NONBLOCK);
         pthread_mutex_lock(&Socket_Head_Mutex);
        if(Socket_Head.Next == &Socket_Head){
            Socket_Head.Next = tmp_Socket_List;
            Socket_Head.Prev = tmp_Socket_List;
            tmp_Socket_List->Next = &Socket_Head;
            tmp_Socket_List->Prev = &Socket_Head;
        }else{
            tmp_Socket_List->Next = Socket_Head.Next;
            tmp_Socket_List->Prev = &Socket_Head;
            Socket_Head.Next = tmp_Socket_List;
        }
        Socket_Head.count++;
        pthread_mutex_unlock(&Socket_Head_Mutex);
    }

    printf("Close...\n");
    close(UartFD);

    return 0;
}
