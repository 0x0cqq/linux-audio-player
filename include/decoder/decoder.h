#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <functional>
#include <mutex>

extern "C" {
    #include <alsa/asoundlib.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <libavutil/samplefmt.h>
    
}

#define ERROR_STR_SIZE 1024

class Decoder {
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

    int outNbSamples = 0; // 目标 frame 的 sample 个数
    int outChannel = 2;  // 重采样后输出的通道
    int outSampleRate = 44100;  // 重采样后输出的采样率
    enum AVSampleFormat outFormat = AV_SAMPLE_FMT_S16P; // 重采样后输出的格式

    // 用来控制播放的变量
    
    /**
     * @brief 是否正在播放
     * 
     */
    bool isPlaying = false;
    
    /**
     * @brief 是否有跳转信号，需要跳转到某个时间戳
     * 
     */
    bool haveJumpSignal = false;

    /**
     * @brief 跳转的目标时间戳，单位为秒
     * 
     */
    double jumpTarget = 0.0;
    double currentPos = 0.0;

    /**
     * @brief 是否播放完毕
     * 
     */
    bool isFinished = false;

    /**
     * @brief 用来控制跳转的互斥锁
     * 
     */
    std::mutex jumpMutex;
    std::mutex currentPosMutex;

    /**
     * @brief 音频流的索引
     * 
     */
    int stream_index = -1;

    /**
     * @brief 错误信息
     * 
     */
    char errors[ERROR_STR_SIZE];
public:
    Decoder();
    ~Decoder();
    
    void openFile(char const file_path[]);
    void release();
    void alloc();
    void decode(char const outputFile[], std::function<void(void *, size_t)> callback);

    /**
     * @brief 跳转播放
     * 
     * @param jumpTarget 跳转的目标时间戳，单位为秒
     * @return true 跳转成功
     * @return false 跳转失败（参数不合法等）
     */
    bool jump(double jumpTarget);

    /**
     * @brief 播放
     * 
     */
    void play();

    /**
     * @brief 暂停
     * 
     */
    void pause();

    bool finished() const;
    double getTime();
};
