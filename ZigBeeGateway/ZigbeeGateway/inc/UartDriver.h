#ifndef UARTDRIVER_H
#define UARTDRIVER_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
void set_speed(int fd, int speed);
int set_Parity(int fd,int databits,int stopbits,int parity);
int Uart_Init(void);
#endif // UARTDRIVER_H
