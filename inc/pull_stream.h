// #pragma once
// #include "common.h"

// using namespace std;

// class PullStream
// {
// private:
//     string pullUrl_;
//     AVFormatContext *avFormatContext;
//     AVCodecContext *videoCodec;
//     AVPacket *avPacket;
//     AVFrame *avFrame;
//     SwsContext *swsContext;
//     uint8_t *VideoData[AV_NUM_DATA_POINTERS];
//     int lines[AV_NUM_DATA_POINTERS];
// public:
//     PullStream(/* args */);
//     ~PullStream();
//     int Init(string strUrl);
//     int Read(uint8_t *dst, uint32_t dst_size);
// };

// PullStream::PullStream(/* args */)
// {
// }

// int PullStream::Init(string strUrl)
// {

// #if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
//     av_register_all();
// #endif
//     avformat_network_init();

//     pullUrl_ = strUrl;
//     // qDebug()<<"URL = " << strUrl;
//     AVDictionary *options = NULL; // 参数对象
//     // 在打开码流前指定各种参数比如:探测时间/超时时间/最大延时等
//     // 设置缓存大小,1080p可将值调大
//     av_dict_set(&options, "buffer_size", "8192000", 0);
//     // 以tcp方式打开,如果以udp方式打开将tcp替换为udp
//     av_dict_set(&options, "rtsp_transport", "tcp", 0);
//     // 设置超时断开连接时间,单位微秒,3000000表示3秒
//     av_dict_set(&options, "stimeout", "3000000", 0);
//     av_dict_set(&options, "rw_timeout", "3000", 0); // 单位:ms
//     // 打开视频流
//     avFormatContext = avformat_alloc_context();
//     // 这里是设置超时的防止调用FFmpeg库时卡死。必须设置在这里不然功能可能失效，interruptCallback是回调函数，m_CurTime是传入回调函数的参数，详情请百度。
//     //  avFormatContext->interrupt_callback.callback = interruptCallback;
//     //  avFormatContext->interrupt_callback.opaque =(void *)(&m_CurTime);
//     avFormatContext->avio_flags |= AVIO_FLAG_NONBLOCK;
//     const AVCodec *pCodec = NULL;
//     // m_CurTime = QTime::currentTime().addSecs(3);
//     int result = avformat_open_input(&avFormatContext, pullUrl_.c_str(), NULL, &options);
//     if (result < 0)
//     {
//         return -1;
//     }
//     // 释放设置参数
//     if (options != NULL)
//     {
//         av_dict_free(&options);
//     }
//     // 查找流信息
//     //  m_CurTime = QTime::currentTime().addSecs(3);
//     if (avformat_find_stream_info(avFormatContext, NULL) < 0)
//     {
//         // emit ErrofInfo(QString("获取流信息错误"));
//         return -1;
//     }
//     // 查找输入流
//     //  videoStreamIndex = -1;
//     for (uint32_t i = 0; i < avFormatContext->nb_streams; i++)
//     {
//         AVStream *pStream = avFormatContext->streams[i];
//         if (pStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
//         {
//             videoCodec = avcodec_alloc_context3(NULL);
//             avcodec_parameters_to_context(videoCodec, pStream->codecpar);
//             pCodec = avcodec_find_decoder(videoCodec->codec_id);
//             //  videoStreamIndex = i;
//             break;
//         }
//     }
//     if (!videoCodec)
//     {
//         return -1;
//     }
//     // 打开视频流解码器失败
//     int iRet = avcodec_open2(videoCodec, pCodec, NULL);
//     if (iRet != 0)
//     {
//         //  emit ErrofInfo(QString("打开视频流解码器失败"));
//         return -1;
//     }
//     // 打印关于输入或输出格式的详细信息
//     //   av_dump_format(avFormatContext,videoStreamIndex, m_strUrl.toStdString().data(), 0);
//     avPacket = av_packet_alloc();
//     avFrame = av_frame_alloc();
//     // 获取缩放上下文失败
//     swsContext = sws_getContext(videoCodec->width, videoCodec->height, videoCodec->pix_fmt, videoCodec->width, videoCodec->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
//     if (swsContext == NULL)
//     {
//         return -1;
//     }

//     char *rgb = new char[videoCodec->width*videoCodec->height*3];
//     memset(rgb, 0 , videoCodec->width*videoCodec->height*3);
//     VideoData[0] = (uint8_t *)rgb;
//     lines[0] = videoCodec->width * 3;
//     // m_bInitFlag = true;
//     return 0;
// }

// int PullStream::Read(uint8_t *dst, uint32_t dst_size)
// {
//     // m_CurTime = QTime::currentTime().addSecs(5);
//     char errors[512] = {0};
//     av_packet_unref(avPacket); // 如果没有该语句av_read_frame会导致内存泄漏
//     int temp_ret = av_read_frame(avFormatContext, avPacket);
//     if (temp_ret < 0)
//     {
//         av_strerror(temp_ret, errors, sizeof(errors));
//         //    qDebug()<<"av_read_frame return error" << (uint32_t)temp_ret << "info" << errors;
//         //    emit ErrofInfo("视频连接断开，请检查网络连接!");
//         //    break;
//     }
//     // 判断当前包是视频还是音频
//     //    index = avPacket->stream_index;
//     //    if (index != videoStreamIndex) {
//     //        //非视频流
//     //         continue;
//     //    }
//     // 解码视频流
//     temp_ret = avcodec_send_packet(videoCodec, avPacket);
//     if (0 != temp_ret)
//     {
//         // 解码错误
//         //    continue
//         ;
//     }
//     if (avcodec_receive_frame(videoCodec, avFrame) != temp_ret)
//     {
//         // 从解码器中读取数据错误
//         //    continue;
//     }
//     // 将数据转成一张图片
//     sws_scale(swsContext, (const uint8_t *const *)avFrame->data, avFrame->linesize, 0, videoCodec->height, VideoData, lines);
//     //    QImage image(VideoData[0], videoCodec->width,  videoCodec->height, QImage::Format_RGB888);
//     //    if (!image.isNull()) {
//     //        emit receiveImage(image);
//     //    }
//     av_packet_unref(avPacket);

//     return 0;
// }

// PullStream::~PullStream()
// {
//     if (swsContext != NULL)
//     {
//         sws_freeContext(swsContext);
//         swsContext = NULL;
//     }

//     if (avPacket != NULL)
//     {
//         av_packet_unref(avPacket);
//         avPacket = NULL;
//     }

//     if (avFrame != NULL)
//     {
//         av_frame_free(&avFrame);
//         avFrame = NULL;
//     }
//     if (videoCodec != NULL)
//     {
//         avcodec_close(videoCodec);
//         videoCodec = NULL;
//     }
//     if (avFormatContext != NULL)
//     {
//         avformat_close_input(&avFormatContext);
//         avFormatContext = NULL;
//     }
// }


 
#pragma once
 
#include <stdio.h>
#include <istream>
#include <iostream>
#include <inttypes.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>
#include <thread>

 
// ffmpeg
#ifdef __cplusplus
extern "C"
{
#endif
 
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavdevice/avdevice.h>
#include <libavformat/version.h>
#include <libavutil/mathematics.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libavutil/frame.h>
 
#ifdef __cplusplus
}
#endif
 
// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
 
namespace Stream
{
    class MediaDecode
    {
    public:
        MediaDecode(const std::string &_rtspUrl);
        ~MediaDecode();
 
        //初始化
        bool init();
        //开始取流
        void startGrabbing();
        //停止取流
        void stopGrabbing();
 
        //取图  rgb格式
        cv::Mat grabImage();
 
    private:
        // rtsp地址
        std::string rtspUrl;
        //初始化结果
        bool isInit{false};
        //是否停止
        std::atomic<bool> isStop{false};
        //frame队列
        // LockFreeQueue<cv::Mat> pAVFrameQueue{3};
 
        //帧率
        double fps = 0;
        //帧间隔
        double interval = 0;
 
        //ffmpeg的全局上下文
        AVFormatContext *pAVFormatContext = 0;
        //ffmpeg流信息
        AVStream *pAVStream = 0;
        //ffmpeg编码上下文
        AVCodecContext *pAVCodecContext = 0;
        //ffmpeg编码器
        const AVCodec *pAVCodec = 0;
        //ffmpag单帧数据包
        AVPacket *pAVPacket = 0;
        //ffmpeg单帧缓存
        AVFrame *pAVFrame = 0;
        //ffmpeg数据字典，用于配置一些编码器属性等
        AVDictionary *pAVDictionary = 0;
 
        //函数执行结果
        int ret = 0;
        //视频流所在的序号
        int videoIndex = -1;
 
    private:
        // yuv frame换rgb mat
        cv::Mat avFrameToMat(AVFrame *avframe, int width, int height);
    };
}
