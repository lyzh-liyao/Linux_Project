
#include <V4L2Operation.h>
#include <SocketOperation.h>
#include <signal.h>
#define IMAGE_SCALE 20
int fps_count = 0;
void cal_fps(int signum){
    signum = signum;
    printf("FPS:%d\r\n", fps_count);
    fps_count = 0;
    alarm(1);
}
int main()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGALRM, cal_fps);
    //建立socket连接
    int socket_fd = soket_init();
    printf("socket_fd : %d", socket_fd);
    // 打开设备
    int fd;
    fd = open(CAMERA_DEVICE, O_RDWR, 0);
    if (fd < 0) {
        printf("Open %s failed\n", CAMERA_DEVICE);
        return -1;
    }
    // 获取驱动信息
    print_devinfo(fd);
    //显示所有支持的格式
    print_devservice(fd);
    // 设置视频格式
    set_videoformat(fd);
    // 获取视频格式
    int imagesize = get_videoimage_size(fd);
    //地址映射
    video_bind(fd);

    // Process the frame
   VideoBuffer vb;
   vb.start = calloc(imagesize,sizeof(char));
//   int count = 0;//测试摄像头fps
//   while(1){
//       int res = take_picture(fd,&vb);
//       printf("%d\r\n",count++);
//       continue;
//   }


    //在图片流的头部首先发送图片大小 以便客户端分割
   int send_image_size = imagesize / IMAGE_SCALE;
   char image_len_buf[4] = {0};
   image_len_buf[0] = (send_image_size >> 24) & 0xFF;
   image_len_buf[1] = (send_image_size >> 16) & 0xFF;
   image_len_buf[2] = (send_image_size >>   8) & 0xFF;
   image_len_buf[3] = (send_image_size >>   0) & 0xFF;
   while(1){
       printf("======waiting for client's request======\n");
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
            printf("收到客户端请求：%s:%d\r\n", ip,port);
        }

        //发送图片流头部（单张图片大小）
        if(send(connect_fd, image_len_buf, 4, 0) == -1)
                        perror("send ImageStreamHead error");
        printf("Head Send Success\r\n");
       alarm(1);
       while(1){
           int res = take_picture(fd,&vb);
           fps_count++;
           if(res < 0){
               printf("take_picture   ERROR\r\n");
               continue;
           }

           int need = send_image_size;
           int offset = 0;
           while(need !=  0){//坚持写够send_image_size个字节
                if((res = send(connect_fd, vb.start + offset, need, 0) ) < 0){
                    perror("send error");
                    goto wbreak;
                }
                need -= res;
                offset += res;
                if(need != 0)
                    printf("未完整写入，循环坚持写够\r\n");
           }
            //printf("loop end res:%d \r\n",res);
             usleep(33333);
           // socket_accept(socket_fd, vb.start, vb.length);
       }
       alarm(0);
wbreak:
       close(connect_fd);
       printf("退出连接\r\n");
   }

//    FILE *fp = fopen(CAPTURE_FILE, "wb");
//    if (fp == NULL) {
//        printf("open frame data file failed\n");
//        return -1;
//    }
//    fwrite(vb.start, sizeof(char), vb.length, fp);
//    fclose(fp);
    free(vb.start);
    printf("Capture one frame saved in %s,len:%d\n", CAPTURE_FILE,vb.length);


    //地址解除映射
    video_unbind();
    close(fd);
    return 0;
}
