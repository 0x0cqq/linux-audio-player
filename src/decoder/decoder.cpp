#include "decoder/decoder.h"

Decoder::Decoder() {
    alloc();
}

Decoder::~Decoder() {
    release();
}

void Decoder::alloc() {
    format_ctx = avformat_alloc_context();
    packet = av_packet_alloc();
    frame = av_frame_alloc();
}

int Decoder::init_atempo_filter(AVFilterGraph **pGraph, AVFilterContext **src, AVFilterContext **out, char *value) {
    // init
    AVFilterGraph *graph = avfilter_graph_alloc();
    
    // accept abuffer for receving input
    const AVFilter *abuffer = avfilter_get_by_name("abuffer");
    AVFilterContext *abuffer_ctx = avfilter_graph_alloc_filter(graph, abuffer, "src");

    // set parameter: 匹配原始音频采样率sample rate，数据格式sample_fmt， channel_layout声道
    if (avfilter_init_str(abuffer_ctx, "sample_rate=44100:sample_fmt=s16:channel_layout=stereo") < 0) {
        fprintf(stderr, "error init abuffer filter\n");
        return -1;
    } 

    // init atempo filter
    const AVFilter *atempo = avfilter_get_by_name("atempo");
    AVFilterContext *atempo_ctx = avfilter_graph_alloc_filter(graph, atempo, "atempo");

    // 这里采用av_dict_set设置参数
    AVDictionary *args = NULL;
    av_dict_set(&args, "tempo", value, 0);//这里传入外部参数，可以动态修改
    if (avfilter_init_dict(atempo_ctx, &args) < 0) {
        fprintf(stderr, "error init atempo filter\n");
        return -1;
    }

    const AVFilter *aformat = avfilter_get_by_name("aformat");
    AVFilterContext *aformat_ctx = avfilter_graph_alloc_filter(graph, aformat, "aformat");
    if (avfilter_init_str(aformat_ctx, "sample_rates=44100:sample_fmts=s16:channel_layouts=stereo") < 0) {
        fprintf(stderr, "error init aformat filter\n");
        return -1;
    }

    // 初始化sink用于输出
    const AVFilter *sink = avfilter_get_by_name("abuffersink");
    AVFilterContext *sink_ctx = avfilter_graph_alloc_filter(graph, sink, "sink");

    if (avfilter_init_str(sink_ctx, NULL) < 0) {//无需参数
        fprintf(stderr, "error init sink filter\n");
        return -1;
    }

    // 链接各个filter上下文
    if (avfilter_link(abuffer_ctx, 0, atempo_ctx, 0) != 0) {
        fprintf(stderr, "error link to atempo filter\n");
        return -1;
    }
    if (avfilter_link(atempo_ctx, 0, aformat_ctx, 0) != 0) {
        fprintf(stderr, "error link to aformat filter\n");
        return -1;
    }
    if (avfilter_link(aformat_ctx, 0, sink_ctx, 0) != 0) {
        fprintf(stderr, "error link to sink filter\n");
        return -1;
    }
    if (avfilter_graph_config(graph, NULL) < 0) {
        fprintf(stderr, "error config filter graph\n");
        return -1;
    }

    *pGraph = graph;
    *src = abuffer_ctx;
    *out = sink_ctx;
    fprintf(stderr, "init filter success...\n");
    return 0;
}

void Decoder::openFile(char const file_path[]) {
    if (!format_ctx || !packet || !frame) {
        throw(std::runtime_error("Failed to alloc"));
    }

    // 打开音频文件
    if (avformat_open_input(&format_ctx, file_path, NULL, NULL) != 0) {
        throw(std::runtime_error("Failed to open audio file"));
    }

    // 获取音频流信息
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        throw(std::runtime_error("Failed to find stream information"));
    }

    // 寻找音频流
    stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    if (stream_index < 0) {
        throw(std::runtime_error("Failed to find audio stream"));
    }
    fprintf(stderr, "stream_index: %d\n", stream_index);

    // 初始化音频解码器上下文
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        throw(std::runtime_error("Failed to allocate codec context"));
    }

    // 设置解码器上下文参数
    if (avcodec_parameters_to_context(codec_ctx, format_ctx->streams[stream_index]->codecpar) < 0) {
        throw(std::runtime_error("Failed to copy codec parameters to codec context"));
    }


    // 打开音频解码器
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        throw(std::runtime_error("Failed to open codec"));
    }

    fprintf(stderr, "解码器名称: %s\n通道数: %d\n通道布局: %ld \n采样率: %d \n采样格式: %d\n", codec->name, codec_ctx->channels, av_get_default_channel_layout(codec_ctx->channels), codec_ctx->sample_rate, codec_ctx->sample_fmt);

    fprintf(stderr, "to\n通道数: %d\n通道布局: %ld \n采样率: %d \n采样格式: %d\n", outChannel, av_get_default_channel_layout(outChannel), outSampleRate, outFormat);

    // 获取音频转码器并设置采样参数初始化
    swr_ctx = swr_alloc_set_opts(0,
                                av_get_default_channel_layout(outChannel),
                                outFormat,
                                outSampleRate,
                                av_get_default_channel_layout(codec_ctx->channels),
                                codec_ctx->sample_fmt,
                                codec_ctx->sample_rate,
                                0,
                                0);
    ret = swr_init(swr_ctx);
    if (ret < 0) {
        throw(std::runtime_error("Failed to swr_init(pSwrContext)"));
    }
}

void Decoder::decode(char const outputFile[], std::function<void(void *, size_t)> callback) {

    FILE *outfile = fopen(outputFile, "wb");
    if(!outfile) {
        throw(std::runtime_error("outfilie fopen failed!"));
    }

    if (init_atempo_filter(&filter_graph, &in_ctx, &out_ctx, "2.0") != 0) {
        throw(std::runtime_error("Codec not init audio filter!"));
    }

    // 用来存储返回值
    int ret = 0;
    
    while (true) {
        if (!isPlaying) {
            // 进入等待状态，直到 isPlaying 为 true
            // 是否可以用 condition_variable?
            continue;
        }
        // 如果有跳转 
        {
            // 获取锁
            std::lock_guard<std::mutex> lock(jumpMutex);
            if(haveJumpSignal) {
                // 跳转到目标时间戳
                ret = av_seek_frame(format_ctx, stream_index, int64_t(jumpTarget * AV_TIME_BASE), AVSEEK_FLAG_BACKWARD);
                if (ret < 0) {
                    av_strerror(ret, errors, ERROR_STR_SIZE);
                    av_log(NULL, AV_LOG_ERROR, "Failed to av_seek_frame, %d(%s)\n", ret, errors);
                    throw std::runtime_error("Failed to av_seek_framem, " + std::string(errors));
                }
                haveJumpSignal = false;
            }
        }
        // 读取一帧数据的数据包
        ret = av_read_frame(format_ctx, packet);
        if(ret < 0) {
            // 读取失败, 文件读完了
            isPlaying = false;
            continue; 
        }
        // 将封装包发往解码器
        if (packet->stream_index == stream_index) {
            // fprintf(stderr, "stream_index: %d\n", packet->stream_index);
            ret = avcodec_send_packet(codec_ctx, packet);
            if (ret) {
                av_strerror(ret, errors, ERROR_STR_SIZE);
                av_log(NULL, AV_LOG_ERROR, "Failed to avcodec_send_packet, %d(%s)\n", ret, errors);
                fprintf(stderr, "Failed to avcodec_send_packet\n");
                break;
            }

            while (!avcodec_receive_frame(codec_ctx, frame)) {
                // fprintf(stderr, "frame->nb_samples: %d\n", frame->nb_samples);
                // 获取每个采样点的字节大小
                numBytes = av_get_bytes_per_sample(outFormat);
                // 修改采样率参数后，需要重新获取采样点的样本个数
                outNbSamples = av_rescale_rnd(frame->nb_samples, outSampleRate, codec_ctx->sample_rate, AV_ROUND_ZERO);
                uint8_t *buffer[outChannel];

                av_samples_alloc(buffer, NULL, outChannel, outNbSamples, outFormat, 0);


                // 重采样
                int realOutNbSamples = swr_convert(swr_ctx, buffer, outNbSamples, (const uint8_t **)frame->data, frame->nb_samples);

                // callback(buffer, buffer_size);

                // filter
                if (av_buffersrc_add_frame(in_ctx, frame) < 0) {
                    fprintf(stderr, "Failed to allocate filtered frame\n");
                    break;
                }

                while (av_buffersink_get_frame(out_ctx, frame) >= 0) {
                    // 重采样
                    realOutNbSamples = swr_convert(swr_ctx, buffer, outNbSamples, (const uint8_t **)frame->data, frame->nb_samples);

                    // 第一次显示
                    static int show = 1;
                    if (show == 1) {
                        fprintf(stderr, "numBytes: %d\n nb_samples: %d\n to outNbSamples: %d\n", numBytes, frame->nb_samples, outNbSamples);
                        show = 0;
                    }

                    uint8_t output_buffer[numBytes * realOutNbSamples * outChannel];
                    int cnt = 0;

                    // 使用 LRLRLRLRLRL（采样点为单位，采样点有几个字节，交替存储到文件，可使用pcm播放器播放）
                    for (int index = 0; index < realOutNbSamples; index++) {
                        for (int channel = 0; channel < codec_ctx->channels; channel++) {
                            // fwrite(frame->data[channel] + numBytes * index, 1, numBytes, outfile);
                            for(int i = 0; i < numBytes; i++) {
                                output_buffer[cnt++] = buffer[channel][numBytes * index + i];
                            }
                            fwrite(buffer[channel] + numBytes * index, 1, numBytes, outfile);
                        }
                    }
                    assert(cnt == numBytes * realOutNbSamples * outChannel);
                    callback(output_buffer, realOutNbSamples);

                    av_frame_unref(frame);
                    break;
                }

                av_freep(&buffer[0]);
                av_packet_unref(packet);
            }
        }
    }

    fclose(outfile);
}


bool Decoder::jump(double jumpTarget) {
    std::lock_guard<std::mutex> lock(jumpMutex);
    if (jumpTarget < 0 || jumpTarget > format_ctx->duration / AV_TIME_BASE) {
        return false;
    }
    this->jumpTarget = jumpTarget;
    haveJumpSignal = true;
    return true;
}

bool Decoder::finished() const {
    return isFinished;
}

void Decoder::play() {
    isPlaying = true;
}

void Decoder::pause() {
    isPlaying = false;
}

void Decoder::release() {
    // 释放回收资源
    av_free(outData[0]);
    av_free(outData[1]);
    swr_free(&swr_ctx);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    avformat_free_context(format_ctx);
}