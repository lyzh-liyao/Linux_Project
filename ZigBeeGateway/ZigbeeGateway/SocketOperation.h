#ifndef __SOCKETOPERATION_H__
#define __SOCKETOPERATION_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define DEFAULT_PORT 8080

int Soket_Init(void);
int Socket_Distroy(int socket_fd);
int Socket_Accept(int socket_fd, char* buf, int len);
#endif
