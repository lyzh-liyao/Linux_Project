#ifndef __V4L2OPERATION_H__
#define __V4L2OPERATION_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#define CAMERA_DEVICE "/dev/video1"
#define CAPTURE_FILE "/home/arm/frame.jpg"

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FORMAT V4L2_PIX_FMT_MJPEG  //V4L2_PIX_FMT_MJPEG //V4L2_PIX_FMT_YUYV
#define BUFFER_COUNT 4

typedef struct
{
    void* start;
    unsigned int length;
}VideoBuffer;
//显示设备信息
int print_devinfo(int fd);
//显示所有支持的格式
void print_devservice(int fd);
// 设置视频格式
int set_videoformat(int fd);
//查看当前格式-图片大小
int get_videoimage_size(int fd);
//地址映射
int video_bind(int fd);
//地址解除映射
int video_unbind();
int take_picture(int fd, VideoBuffer* vb);

#endif
