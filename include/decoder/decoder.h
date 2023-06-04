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
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
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

    // atempo filter
    AVFilterContext* in_ctx = nullptr;
    AVFilterContext* out_ctx = nullptr;
    AVFilterGraph *filter_graph = nullptr;

    int ret = 0;
    int numBytes = 0;

    int outNbSamples = 0; // 目标 frame 的 sample 个数
    int outChannel = 2;  // 重采样后输出的通道
    int outSampleRate = 44100;  // 重采样后输出的采样率
    enum AVSampleFormat outFormat = AV_SAMPLE_FMT_S16P; // 重采样后输出的格式

    // 是否已经打开文件
    volatile bool isFileOpened = false;

    // 播放状态 Part
    /**
     * @brief 是否正在播放
     * 
     */
    volatile bool isPlaying = false;

    // 跳转 Part

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

    /**
     * @brief 用来控制跳转的互斥锁
     * 
     */
    std::mutex jumpMutex;
    
    // 播放位置 Part
    /**
     * @brief 当前的播放位置，单位为秒
     * 
     */
    double currentPos = 0.0;

    /**
     * @brief 修改当前播放位置的互斥锁，防止 getTime 和 frame 修改 pos 冲突
     * 
     */
    std::mutex currentPosMutex;

    // 播放速度 Part

    /**
     * @brief 当前的播放速度
     * 
     */
    double currentTempo = 1.0;
    /**
     * @brief 目标的播放速度（通过 changeTempo 函数改变）
     * 
     */
    double targetTempo = 0.0;
    /**
     * @brief 是否有改变播放速度的信号
     * 
     */
    bool isTempoChanged = false;

    /**
     * @brief 用来控制改变播放速度的互斥锁
     * 
     */
    std::mutex tempoMutex;

    /**
     * @brief 用来控制 openFile 的互斥锁，防止 openFile 操作和播放操作相互冲突
     * 
     */
    std::mutex openFileMutex;


    /**
     * @brief 音频流的索引
     * 
     */
    int stream_index = -1;

    /**
     * @brief 错误信息的临时存储
     * 
     */
    char errors[ERROR_STR_SIZE];


    int init_atempo_filter(AVFilterGraph **pGraph, AVFilterContext **src, AVFilterContext **out, const char *value);

public:
    Decoder();
    ~Decoder();
    
    void openFile(char const file_path[]);
    void release();
    void decode(std::function<void(void *, size_t)> callback);

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

    /**
     * @brief 获取当前播放的时间戳
     * 
     * @return double 以秒为单位的时间
     */
    double getTime();

    /**
     * @brief 修改播放速度
     * 
     * @param tempo 播放速度，范围为 0.5 ~ 2.0
     * @return true 修改成功
     * @return false 修改失败
     */
    bool changeTempo(double tempo);
};
