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
#define DEFAULT_PORT 7433
#define MAXLINE 4096

int soket_init();
int socket_distroy(int socket_fd);
int socket_accept(int socket_fd, char* buf, int len);
#endif
