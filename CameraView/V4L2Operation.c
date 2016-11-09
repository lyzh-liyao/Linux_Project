#include <V4L2Operation.h>

VideoBuffer*  buffers = NULL;
//显示驱动信息
int print_devinfo(int fd){
    struct v4l2_capability cap;
    int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        printf("VIDIOC_QUERYCAP failed (%d)\n", ret);
        return ret;
    }
    // Print capability infomations
    printf("Capability Informations:\n");
    printf(" driver: %s\n", cap.driver);
    printf(" card: %s\n", cap.card);
    printf(" bus_info: %s\n", cap.bus_info);
    printf(" version: %08X\n", cap.version);
    printf(" capabilities: %08X\n", cap.capabilities);
    return 0;
}

//显示所有支持的格式
void print_devservice(int fd){
    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("Supportformat:\n");
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
    {
        printf("\t%d.%d.%s\n",fmtdesc.index+1,fmtdesc.pixelformat,fmtdesc.description);
        fmtdesc.index++;
    }
}
// 设置视频格式
int set_videoformat(int fd){
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = VIDEO_WIDTH;
    fmt.fmt.pix.height      = VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat = VIDEO_FORMAT;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    int ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (ret < 0) {
        printf("VIDIOC_S_FMT failed (%d)\n", ret);
        return ret;
    }
    return 0;
}
//查看当前格式
int get_videoimage_size(int fd){
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
    if (ret < 0) {
        printf("VIDIOC_G_FMT failed (%d)\n", ret);
        return ret;
    }
    // Print Stream Format
    printf("Stream Format Informations:\n");
    printf(" type: %d\n", fmt.type);
    printf(" width: %d\n", fmt.fmt.pix.width);
    printf(" height: %d\n", fmt.fmt.pix.height);
    char fmtstr[8];
    memset(fmtstr, 0, 8);
    memcpy(fmtstr, &fmt.fmt.pix.pixelformat, 4);
    printf(" pixelformat: %s\n", fmtstr);
    printf(" field: %d\n", fmt.fmt.pix.field);
    printf(" bytesperline: %d\n", fmt.fmt.pix.bytesperline);
    printf(" sizeimage: %d\n", fmt.fmt.pix.sizeimage);
    printf(" colorspace: %d\n", fmt.fmt.pix.colorspace);
    printf(" priv: %d\n", fmt.fmt.pix.priv);
    printf(" raw_date: %s\n", fmt.fmt.raw_data);
    return fmt.fmt.pix.sizeimage;
}

//地址映射
int video_bind(int fd){
    // 请求分配内存
    struct v4l2_requestbuffers reqbuf;
    reqbuf.count = BUFFER_COUNT;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;

    int ret = ioctl(fd , VIDIOC_REQBUFS, &reqbuf);
    if(ret < 0) {
        printf("VIDIOC_REQBUFS failed (%d)\n", ret);
        return ret;
    }

    // 获取空间
     buffers = calloc( reqbuf.count, sizeof(*buffers) );
    struct v4l2_buffer buf;
    unsigned int i = 0;
    for (i = 0; i < reqbuf.count; i++)
    {
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd , VIDIOC_QUERYBUF, &buf);
        if(ret < 0) {
            printf("VIDIOC_QUERYBUF (%d) failed (%d)\n", i, ret);
            return ret;
        }

        // mmap buffer
        buffers[i].length = buf.length;
        buffers[i].start = (char *) mmap(0, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            printf("mmap (%d) failed: %s\n", i, strerror(errno));
            return -1;
        }

        // Queen buffer
        ret = ioctl(fd , VIDIOC_QBUF, &buf);
        if (ret < 0) {
            printf("VIDIOC_QBUF (%d) failed (%d)\n", i, ret);
            return -1;
        }
        printf("Frame buffer %d: address=0x%x, length=%d\n", i, (unsigned int)buffers[i].start, buffers[i].length);
    }
    // 开始录制
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        printf("VIDIOC_STREAMON failed (%d)\n", ret);
        return ret;
    }
    return 0;
}

//地址解除映射
int video_unbind(){
    // Release the resource
    int i = 0;
    for (i=0; i< 4; i++)
    {
        munmap(buffers[i].start, buffers[i].length);
    }
    printf("Camera test Done.\n");
    return 0;
}

int  take_picture(int fd, VideoBuffer* vb){
    //select阻塞等待响应
    fd_set    fds ;
    struct timeval   tv;
    FD_ZERO(&fds);
    FD_SET(fd,  &fds);
    tv.tv_sec = 2;       /* Timeout. */
    tv.tv_usec = 0;
    int iret = select(fd + 1, &fds, NULL, NULL, &tv);
    if(iret == 0){
        printf("select 超时\r\n");
        return -1;
    }
    // Get frame 获取图片buf
    struct v4l2_buffer buf;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    int ret = ioctl(fd, VIDIOC_DQBUF, &buf);
    if (ret < 0) {
        printf("VIDIOC_DQBUF failed (%d)\n", ret);
        return ret;
    }
    vb->length = buf.length;
    memcpy(vb->start, buffers[buf.index].start,buf.length);
    // 还回帧容器
    ret = ioctl(fd, VIDIOC_QBUF, &buf);
    if (ret < 0) {
        printf("VIDIOC_QBUF failed (%d)\n", ret);
        return ret;
    }
    return 0;
}

