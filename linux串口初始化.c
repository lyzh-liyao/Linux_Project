bool CSerialInterface::initialSerial(const char *serialDev)
{
    serial_fd = open(serialDev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd < 0) {
        PRINT("can not open serial port %s\r\n", serialDev);
        return false;
    }   

    //串口主要设置结构体termios <termios.h>  
    struct termios options;

    /**1. tcgetattr函数用于获取与终端相关的参数。
    *参数fd为终端的文件描述符，返回的结果保存在termios结构体中
    */
    tcgetattr(serial_fd, &options);
    /**2. 修改所获得的参数*/
    options.c_cflag |= (CLOCAL | CREAD);//设置控制模式状态，本地连接，接收使能  
    options.c_cflag &= ~CSIZE;//字符长度，设置数据位之前一定要屏掉这个位  
    options.c_cflag &= ~CRTSCTS;//无硬件流控  
    options.c_cflag |= CS8;//8位数据长度  
    options.c_cflag &= ~CSTOPB;//1位停止位  
    options.c_iflag |= IGNPAR;//无奇偶检验位  
    options.c_oflag = 0; //输出模式  
    options.c_lflag = 0; //不激活终端模式  
    cfsetospeed(&options, B115200);//设置波特率  

    /**3. 设置新属性，TCSANOW：所有改变立即生效*/
    tcflush(serial_fd, TCIFLUSH);//溢出数据可以接收，但不读  
    tcsetattr(serial_fd, TCSANOW, &options);

    return true;
}
