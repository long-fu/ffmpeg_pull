// /*opencv库*/
// // #include <opencv2\opencv.hpp>
// // #include <opencv2/opencv.hpp>
// #include <opencv2/opencv.hpp>
// #include "common.h"
// #include <iostream>

// using namespace std;
// // using namespace cv;

// void open_rtsp(const char *rtsp)
// {
//     unsigned int i;
//     int ret;
//     int video_st_index = -1;
//     int audio_st_index = -1;
//     AVFormatContext *ifmt_ctx = NULL;
//     AVPacket pkt;
//     AVStream *st = NULL;
//     char errbuf[64];
//     AVDictionary *optionsDict = NULL;

// #if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
//     av_register_all();                                     // Register all codecs and formats so that they can be used.
// #endif

//     avformat_network_init();                               // Initialization of network components
//     av_dict_set(&optionsDict, "rtsp_transport", "tcp", 0); // 采用tcp传输	,,如果不设置这个有些rtsp流就会卡着
//     av_dict_set(&optionsDict, "stimeout", "2000000", 0);   // 如果没有设置stimeout

//     av_init_packet(&pkt); // initialize packet.
//     pkt.data = NULL;
//     pkt.size = 0;
//     bool nRestart = false;
//     AVStream *pVst;
//     uint8_t *buffer_rgb = NULL;
//     AVCodecContext *pVideoCodecCtx = NULL;
//     AVFrame *pFrame = av_frame_alloc();
//     AVFrame *pFrameRGB = av_frame_alloc();
//     int got_picture;
//     SwsContext *img_convert_ctx = NULL;
//     AVCodec *pVideoCodec = NULL;

//     if ((ret = avformat_open_input(&ifmt_ctx, rtsp, 0, &optionsDict)) < 0)
//     { // Open the input file for reading.
//         printf("Could not open input file '%s' (error '%s')\n", rtsp, av_make_error_string(errbuf, sizeof(errbuf), ret));
//         goto EXIT;
//     }

//     if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0)
//     { // Get information on the input file (number of streams etc.).
//         printf("Could not open find stream info (error '%s')\n", av_make_error_string(errbuf, sizeof(errbuf), ret));
//         goto EXIT;
//     }

//     for (i = 0; i < ifmt_ctx->nb_streams; i++)
//     { // dump information
//         av_dump_format(ifmt_ctx, i, rtsp, 0);
//     }

//     // for (i = 0; i < ifmt_ctx->nb_streams; i++)
//     // { // find video stream index
//     //     st = ifmt_ctx->streams[i];
//     //     switch (st->codec->codec_type)
//     //     {
//     //     case AVMEDIA_TYPE_AUDIO:
//     //         audio_st_index = i;
//     //         break;
//     //     case AVMEDIA_TYPE_VIDEO:
//     //         video_st_index = i;
//     //         break;
//     //     default:
//     //         break;
//     //     }
//     // }

//     if (-1 == video_st_index)
//     {
//         printf("No H.264 video stream in the input file\n");
//         goto EXIT;
//     }

//     // if (!faceCascade.load(faceCascadeFileName))
//     {
//         cout << "脸模型文件加载失败" << endl;
//         // return;
//     }
//     // if (!eyesCascade.load(eyesCasecadeFileName))
//     {
//         cout << "人眼模型文件加载失败" << endl;
//         // return;
//     }

//     while (1)
//     {
//         do
//         {
//             ret = av_read_frame(ifmt_ctx, &pkt); // read frames
// #if 1
//             cout << pkt.size << endl;
//             // decode stream
//             if (!nRestart)
//             {
//                 pVst = ifmt_ctx->streams[video_st_index];
//                 pVideoCodecCtx = pVst->codec;
//                 pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
//                 if (pVideoCodec == NULL)
//                     return;
//                 // pVideoCodecCtx = avcodec_alloc_context3(pVideoCodec);

//                 if (avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) < 0)
//                     return;
//                 nRestart = true;
//             }

//             if (pkt.stream_index == video_st_index)
//             {
//                 fprintf(stdout, "pkt.size=%d,pkt.pts=%lld, pkt.data=0x%x.", pkt.size, pkt.pts, (unsigned int)pkt.data);
//                 int av_result = avcodec_decode_video2(pVideoCodecCtx, pFrame, &got_picture, &pkt);

//                 if (got_picture)
//                 {
//                     fprintf(stdout, "decode one video frame!\n");
//                 }

//                 if (av_result < 0)
//                 {
//                     fprintf(stderr, "decode failed: inputbuf = 0x%x , input_framesize = %d\n", pkt.data, pkt.size);
//                     return;
//                 }
// #if 1
//                 if (got_picture)
//                 {
//                     int bytes = avpicture_get_size(AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height);
//                     buffer_rgb = (uint8_t *)av_malloc(bytes);
//                     avpicture_fill((AVPicture *)pFrameRGB, buffer_rgb, AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height);

//                     img_convert_ctx = sws_getContext(pVideoCodecCtx->width, pVideoCodecCtx->height, pVideoCodecCtx->pix_fmt,
//                                                      pVideoCodecCtx->width, pVideoCodecCtx->height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
//                     if (img_convert_ctx == NULL)
//                     {

//                         printf("can't init convert context!\n");
//                         return;
//                     }
//                     sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pVideoCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
//                     IplImage *pRgbImg = cvCreateImage(cvSize(pVideoCodecCtx->width, pVideoCodecCtx->height), 8, 3);
//                     memcpy(pRgbImg->imageData, buffer_rgb, pVideoCodecCtx->width * 3 * pVideoCodecCtx->height);
//                     Mat Img = cvarrToMat(pRgbImg, true);

//                     cvShowImage("GB28181Show", pRgbImg);

//                     // DetectFace(Img);
//                     cvWaitKey(10);
//                     cvReleaseImage(&pRgbImg);
//                     sws_freeContext(img_convert_ctx);
//                     av_free(buffer_rgb);
//                 }
// #endif
//             }

// #endif
//         } while (ret == AVERROR(EAGAIN));

//         if (ret < 0)
//         {
//             printf("Could not read frame ---(error '%s')\n", av_make_error_string(errbuf, sizeof(errbuf), ret));
//             goto EXIT;
//         }

//         if (pkt.stream_index == video_st_index)
//         { // video frame
//             printf("Video Packet size = %d\n", pkt.size);
//         }
//         else if (pkt.stream_index == audio_st_index)
//         { // audio frame
//             printf("Audio Packet size = %d\n", pkt.size);
//         }
//         else
//         {
//             printf("Unknow Packet size = %d\n", pkt.size);
//         }

//         av_packet_unref(&pkt);
//     }

// EXIT:

//     if (NULL != ifmt_ctx)
//     {
//         avformat_close_input(&ifmt_ctx);
//         ifmt_ctx = NULL;
//     }

//     return;
// }

// int main(int argc, char *argv[])
// {
//     while (1)
//         open_rtsp("rtsp://192.168.17.1/myz.mkv");
//     return 0;
// }
#include "common.h"

int main(int argc, char *argv[])

{

    AVFormatContext *pFormatCtx;

    int i, videoindex;

    AVCodecContext *pCodecCtx;

    const AVCodec *pCodec;

    AVFrame *pFrame, *pFrameYUV;

    uint8_t *out_buffer;

    AVPacket *packet;

    int ret, got_picture;

    struct SwsContext *img_convert_ctx;

    // 改成你自己的 URL

    char filepath[] = "rtsp://192.168.2.xx:8554/1";
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100) 
    av_register_all();
#endif
    avformat_network_init();

    pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) ////打开网络流或文件流

    {

        printf("Couldn't open input stream.\n");

        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)

    {

        printf("Couldn't find stream information.\n");

        return -1;
    }

    videoindex = -1;

    for (i = 0; i < pFormatCtx->nb_streams; i++)

        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)

        {

            videoindex = i;

            break;
        }

    if (videoindex == -1)

    {

        printf("Didn't find a video stream.\n");

        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;

    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    if (pCodec == NULL)

    {

        printf("Codec not found.\n");

        return -1;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)

    {

        printf("Could not open codec.\n");

        return -1;
    }

    pFrame = av_frame_alloc();

    pFrameYUV = av_frame_alloc();

    out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));

    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

    // Output Info---输出一些文件（RTSP）信息

    printf("---------------- File Information ---------------\n");

    av_dump_format(pFormatCtx, 0, filepath, 0);

    printf("-------------------------------------------------\n");

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,

                                     pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    FILE *fpSave;

    if ((fpSave = fopen("geth264.h264", "ab")) == NULL) // h264保存的文件名

        return 0;

    for (;;)

    {

        //------------------------------

        if (av_read_frame(pFormatCtx, packet) >= 0)

        {

            if (packet->stream_index == videoindex)

            {

                fwrite(packet->data, 1, packet->size, fpSave); // 写数据到文件中
            }

            av_free_packet(packet);
        } else {
            break;
        }
    }

    //--------------

    av_frame_free(&pFrameYUV);

    av_frame_free(&pFrame);

    avcodec_close(pCodecCtx);

    avformat_close_input(&pFormatCtx);

    return 0;
}