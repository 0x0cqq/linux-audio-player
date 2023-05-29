#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <functional>

extern "C" {
    #include <alsa/asoundlib.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/samplefmt.h>
    
}

#define ERROR_STR_SIZE 1024

class Decoder
{
private:
    AVFormatContext *format_ctx = nullptr;
    AVCodecContext *codec_ctx = nullptr;
    AVCodec *codec = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;
    SwrContext *swr_ctx = nullptr;
    FILE *outfile = nullptr;

    int ret = 0;
    int numBytes = 0;
    uint8_t* outData[2] = {0};

    int dstNbSamples = 0; // 解码目标的采样率
    int outChannel = 2;  // 重采样后输出的通道
    int outSampleRate = 44100;  // 重采样后输出的采样率
    enum AVSampleFormat outFormat = AV_SAMPLE_FMT_S16P; // 重采样后输出的格式

    bool isFinished = false;
    int stream_index = -1;

    char errors[ERROR_STR_SIZE];
public:
    Decoder(){};
    ~Decoder();

    void openFile(char const file_path[]);
    void release();
    void alloc();
    void decode(char const outputFile[]);

    bool finished() const;
};


#endif