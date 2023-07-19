#include "pull_stream.h"
 
namespace Stream
{
    MediaDecode::MediaDecode(const std::string &_rtspUrl) : rtspUrl(_rtspUrl)
    {
    }
 
    MediaDecode::~MediaDecode()
    {
        isStop = true;
        if (pAVFrame)
        {
            av_frame_free(&pAVFrame);
            pAVFrame = 0;
        }
        if (pAVPacket)
        {
            av_packet_free(&pAVPacket);
            pAVPacket = 0;
        }
        if (pAVCodecContext)
        {
            avcodec_close(pAVCodecContext);
            pAVCodecContext = 0;
        }
        if (pAVFormatContext)
        {
            avformat_close_input(&pAVFormatContext);
            avformat_free_context(pAVFormatContext);
            pAVFormatContext = 0;
        }
    }
 
    bool MediaDecode::init()
    {
        //分配
        pAVFormatContext = avformat_alloc_context();
        pAVPacket = av_packet_alloc();
        pAVFrame = av_frame_alloc();
 
        if (!pAVFormatContext || !pAVPacket || !pAVFrame)
        {
            std::cout << "failed to alloc \n";
            return false;
        }
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100) 
        //注册所有容器和编解码器
        av_register_all();
#endif
        avformat_network_init();
 
        //打开视频流
        ret = avformat_open_input(&pAVFormatContext, rtspUrl.c_str(), 0, 0);
        if (ret)
        {
            std::cout << "failed to open rtsp \n";
            return false;
        }
 
        //探测流媒体信息
        ret = avformat_find_stream_info(pAVFormatContext, 0);
        if (ret < 0)
        {
            std::cout << "failed to find stream \n";
            return false;
        }
 
        //提取视频信息
        for (int index = 0; index < pAVFormatContext->nb_streams; index++)
        {
            // pAVCodecContext = pAVFormatContext->streams[index]->codec;
            // pAVCodecContext = pAVFormatContext->video_codec;
            pAVStream = pAVFormatContext->streams[index];
            switch (pAVCodecContext->codec_type)
            {
            case AVMEDIA_TYPE_UNKNOWN:
                std::cout << "流序号:" << index << "类型为:"
                          << "AVMEDIA_TYPE_UNKNOWN \n";
                break;
            case AVMEDIA_TYPE_VIDEO:
                std::cout << "流序号:" << index << "类型为:"
                          << "AVMEDIA_TYPE_VIDEO \n";
                videoIndex = index;
                break;
            case AVMEDIA_TYPE_AUDIO:
                std::cout << "流序号:" << index << "类型为:"
                          << "AVMEDIA_TYPE_AUDIO \n";
                break;
            case AVMEDIA_TYPE_DATA:
                std::cout << "流序号:" << index << "类型为:"
                          << "AVMEDIA_TYPE_DATA \n";
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                std::cout << "流序号:" << index << "类型为:"
                          << "AVMEDIA_TYPE_SUBTITLE \n";
                break;
            case AVMEDIA_TYPE_ATTACHMENT:
                std::cout << "流序号:" << index << "类型为:"
                          << "AVMEDIA_TYPE_ATTACHMENT \n";
                break;
            case AVMEDIA_TYPE_NB:
                std::cout << "流序号:" << index << "类型为:"
                          << "AVMEDIA_TYPE_NB \n";
                break;
            default:
                break;
            }
            // 已经找打视频品流
            if (videoIndex != -1)
            {
                break;
            }
        }
 
        if (videoIndex == -1 || !pAVCodecContext)
        {
            std::cout << "failed to find video stream \n";
            return false;
        }
 
        //找到解码器
        // pAVCodec = avcodec_find_decoder_by_name("h264_nvmpi");
        pAVCodec = avcodec_find_decoder_by_name("h264");
        if (!pAVCodec)
        {
            std::cout << "fialed to find decoder \n";
            return false;
        }
 
        //打开解码器
        // 设置缓存大小 1024000byte
        av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
        // 设置超时时间 20s
        av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
        // 设置最大延时 3s
        av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
        // 设置打开方式 tcp/udp
        av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);
        ret = avcodec_open2(pAVCodecContext, pAVCodec, &pAVDictionary);
        if (ret)
        {
            std::cout << "failed to open codec \n";
            return false;
        }
 
        // 显示视频相关的参数信息（编码上下文）
        std::cout << "比特率:" << pAVCodecContext->bit_rate << std::endl;
        std::cout << "宽高:" << pAVCodecContext->width << "x" << pAVCodecContext->height << std::endl;
        std::cout << "格式:" << pAVCodecContext->pix_fmt << std::endl; // AV_PIX_FMT_YUV420P 0
        std::cout << "帧率分母:" << pAVCodecContext->time_base.den << std::endl;
        std::cout << "帧率分子:" << pAVCodecContext->time_base.num << std::endl;
        std::cout << "帧率分母:" << pAVStream->avg_frame_rate.den << std::endl;
        std::cout << "帧率分子:" << pAVStream->avg_frame_rate.num << std::endl;
        std::cout << "总时长:" << pAVStream->duration / 10000.0 << "s" << std::endl;
        std::cout << "总帧数:" << pAVStream->nb_frames << std::endl;
 
        fps = pAVStream->avg_frame_rate.num * 1.0f / pAVStream->avg_frame_rate.den;
        interval = 1 * 1000 / fps;
        std::cout << "平均帧率:" << fps << std::endl;
        std::cout << "帧间隔:" << interval << "ms" << std::endl;
 
        isInit = true;
        return isInit;
    }
 
    void MediaDecode::startGrabbing()
    {
        if (!isInit)
        {
            return;
        }
 
        std::thread t([&]()
                      {
                    while (!isStop)
        {
        
        while (av_read_frame(pAVFormatContext, pAVPacket) >= 0)
        {
            if (pAVPacket->stream_index == videoIndex)
            {
                //对读取的数据包进行解码
                ret = avcodec_send_packet(pAVCodecContext, pAVPacket);
                if (ret)
                {
                    std::cout << "failed to avcodec_send_packet(pAVCodecContext, pAVPacket) ,ret =" << ret << std::endl;
                    break;
                }
                while (!avcodec_receive_frame(pAVCodecContext, pAVFrame))
                {
                    //存入队列
                    // cv::Mat mat = avFrameToMat(pAVFrame, pAVFrame->width, pAVFrame->height).clone();
                    // pAVFrameQueue.push_without_wait(mat);
                }
            }
        }
        } });
        t.detach();
    }
 
    void MediaDecode::stopGrabbing()
    {
        isStop = true;
    }
 
    cv::Mat MediaDecode::grabImage()
    {
        cv::Mat mat;
        // pAVFrameQueue.pop_without_wait(mat);
        return mat;
    }
 
    cv::Mat MediaDecode::avFrameToMat(AVFrame *avframe, int width, int height)
    {
        if (width <= 0)
            width = avframe->width;
        if (height <= 0)
            height = avframe->height;
        struct SwsContext *sws_ctx = NULL;
        sws_ctx = sws_getContext(avframe->width, avframe->height, (enum AVPixelFormat)avframe->format, width, height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
        cv::Mat mat;
        mat.create(cv::Size(width, height), CV_8UC3);
        AVFrame *bgr24frame = av_frame_alloc();
        bgr24frame->data[0] = (uint8_t *)mat.data;
        // avpicture_fill((AVPicture *)bgr24frame, bgr24frame->data[0], AV_PIX_FMT_BGR24, width, height);
        sws_scale(sws_ctx, (const uint8_t *const *)avframe->data, avframe->linesize, 0, avframe->height, // from cols=0,all rows trans
                  bgr24frame->data, bgr24frame->linesize);
        av_free(bgr24frame);
        sws_freeContext(sws_ctx);
        return mat;
    }
}



/*==================================================================================
 *                  将AVFrame(YUV420格式)保存为JPEG格式的图片
 ===================================================================================*/
int SaveAsJPEG(AVFrame* pFrame, int width, int height, int index)
{
    // 输出文件路径
    // char out_file[MAX_PATH] = {0};
    // sprintf_s(out_file, sizeof(out_file), "%s%d.jpg", "E:/QT/test_ffmpegSavePic/ffmpeg/output/", index);
 
    // // 分配AVFormatContext对象
    // AVFormatContext* pFormatCtx = avformat_alloc_context();
 
    // // 设置输出文件格式
    // pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
 
    // // 创建并初始化一个和该url相关的AVIOContext
    // if( avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0)
    // {
    //     printf("Couldn't open output file.");
    //     return -1;
    // }
 
    // // 构建一个新stream
    // AVStream* pAVStream = avformat_new_stream(pFormatCtx, 0);
    // if( pAVStream == NULL )
    // {
    //     return -1;
    // }
 
    // // 设置该stream的信息
    // AVCodecContext* pCodecCtx = pAVStream->codec;
 
    // pCodecCtx->codec_id   = pFormatCtx->oformat->video_codec;
    // pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    // pCodecCtx->pix_fmt    = AV_PIX_FMT_YUVJ420P;
    // pCodecCtx->width      = width;
    // pCodecCtx->height     = height;
    // pCodecCtx->time_base.num = 1;
    // pCodecCtx->time_base.den = 25;
 
    // //打印输出相关信息
    // av_dump_format(pFormatCtx, 0, out_file, 1);
 
    // //================================== 查找编码器 ==================================//
    // const AVCodec* pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    // if( !pCodec )
    // {
    //     printf("Codec not found.");
    //     return -1;
    // }
 
    // // 设置pCodecCtx的解码器为pCodec
    // if( avcodec_open2(pCodecCtx, pCodec, NULL) < 0 )
    // {
    //     printf("Could not open codec.");
    //     return -1;
    // }
 
    // //================================Write Header ===============================//
    // avformat_write_header(pFormatCtx, NULL);
 
    // int y_size = pCodecCtx->width * pCodecCtx->height;
 
    // //==================================== 编码 ==================================//
    // // 给AVPacket分配足够大的空间
    // AVPacket pkt;
    // av_new_packet(&pkt, y_size * 3);
 
    // //
    // int got_picture = 0;
    // int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
    // if( ret < 0 )
    // {
    //     printf("Encode Error.\n");
    //     return -1;
    // }
    // if( got_picture == 1 )
    // {
    //     pkt.stream_index = pAVStream->index;
    //     ret = av_write_frame(pFormatCtx, &pkt);
    // }
 
    // av_packet_free(&&pkt);
 
    // //Write Trailer
    // av_write_trailer(pFormatCtx);
 
 
    // if( pAVStream )
    // {
    //     avcodec_close(pAVStream->codec);
    // }
    // avio_close(pFormatCtx->pb);
    // avformat_free_context(pFormatCtx);
 
    return 0;
}
using namespace std;

int run_stsp() {
        //  av_register_all(); //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
//=========================== 创建AVFormatContext结构体 ===============================//
    //分配一个AVFormatContext，FFMPEG所有的操作都要通过这个AVFormatContext来进行
    AVFormatContext *pFormatCtx = avformat_alloc_context();
 //==================================== 打开文件 ======================================//
    char *file_path = "E:/QT/test_ffmpegSavePic/ffmpeg/test.avi";//这里必须使用左斜杠
    int ret = avformat_open_input(&pFormatCtx, file_path, NULL, NULL);
    if(ret != 0)
    {
        cout << "open error!" << endl;
        return -1;
    }
 
//循环查找视频中包含的流信息，直到找到视频类型的流
    //便将其记录下来 保存到videoStream变量中
    int i;
    int videoStream;
 
    //=================================== 获取视频流信息 ===================================//
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        cout << "Could't find stream infomation." << endl;
        return -1;
    }
 
    videoStream = -1;
 
    for (i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }
    }
 
    //如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1)
    {
        cout << "Didn't find a video stream." << endl;
        return -1;
    }
 
 //=================================  查找解码器 ===================================//
    AVCodecContext* pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        cout << "Codec not found." << endl;
        return -1;
    }
 //================================  打开解码器 ===================================//
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)// 具体采用什么解码器ffmpeg经过封装  我们无须知道
    {
        cout << "Could not open codec." << endl;
        return -1;
    }

 //================================ 设置数据转换参数 ================================//
    SwsContext * img_convert_ctx;
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, //源地址长宽以及数据格式
                                     pCodecCtx->width, pCodecCtx->height,AV_PIX_FMT_YUVJ420P,    //目的地址长宽以及数据格式
                                     SWS_BICUBIC, NULL, NULL, NULL);//算法类型  AV_PIX_FMT_YUVJ420P   AV_PIX_FMT_BGR24
 
//==================================== 分配空间 ==================================//
    //一帧图像数据大小
    int numBytes = avpicture_get_size(AV_PIX_FMT_YUVJ420P, pCodecCtx->width,pCodecCtx->height);
 
    unsigned char *out_buffer;
    out_buffer = (unsigned char *) av_malloc(numBytes * sizeof(unsigned char));
 
    AVFrame * pFrame;
    pFrame = av_frame_alloc();
    AVFrame * pFrameRGB;
    pFrameRGB = av_frame_alloc();
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_YUVJ420P,pCodecCtx->width, pCodecCtx->height);
    //会将pFrameRGB的数据按RGB格式自动"关联"到buffer  即pFrameRGB中的数据改变了 out_buffer中的数据也会相应的改变
 
//=========================== 分配AVPacket结构体 ===============================//
    int y_size = pCodecCtx->width * pCodecCtx->height;
    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据
    
    return 0;
}