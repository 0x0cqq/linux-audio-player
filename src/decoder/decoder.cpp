#include "decoder/decoder.h"

bool Decoder::finished() const {
    return isFinished;
}

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

void Decoder::decode(char const outputFile[]) {
    FILE *outfile = fopen(outputFile, "wb");
    if(!outfile)
    {
        throw(std::runtime_error("outfilie fopen failed!"));
    }

    outData[0] = (uint8_t *)av_malloc(1152 * 8);
    outData[1] = (uint8_t *)av_malloc(1152 * 8);

    // 读取一帧数据的数据包
    while (av_read_frame(format_ctx, packet) >= 0)
    {   
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

            while (!avcodec_receive_frame(codec_ctx, frame))
            {
                // fprintf(stderr, "frame->nb_samples: %d\n", frame->nb_samples);
                // 获取每个采样点的字节大小
                numBytes = av_get_bytes_per_sample(outFormat);
                // 修改采样率参数后，需要重新获取采样点的样本个数
                dstNbSamples = av_rescale_rnd(frame->nb_samples, outSampleRate, codec_ctx->sample_rate, AV_ROUND_ZERO);
                // 重采样
                swr_convert(swr_ctx, outData, dstNbSamples, (const uint8_t **)frame->data, frame->nb_samples);

                // 第一次显示
                static int show = 1;
                if (show == 1) {
                    fprintf(stderr, "numBytes: %d\n nb_samples: %d\n to dstNbSamples: %d\n", numBytes, frame->nb_samples, dstNbSamples);
                    show = 0;
                }
                // 使用LRLRLRLRLRL（采样点为单位，采样点有几个字节，交替存储到文件，可使用pcm播放器播放）
                for (int index = 0; index < dstNbSamples; index++) {
                    for (int channel = 0; channel < codec_ctx->channels; channel++) {
                        // fwrite(frame->data[channel] + numBytes * index, 1, numBytes, outfile);
                        fwrite(outData[channel] + numBytes * index, 1, numBytes, outfile);
                    }
                }
                av_packet_unref(packet);
            }
        }
    }

    fclose(outfile);
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