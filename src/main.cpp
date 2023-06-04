#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "decoder/decoder.h"
#include "player/player.h"
#include "controller/controller.h"

extern "C" {
    #include <alsa/asoundlib.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/samplefmt.h>
}



int main(int argc, char *argv[]) {
    if(argc != 3) {
        std::cerr << "Wrong input args count" << std::endl;
        exit(1);
    }

    std::string song_name_1 = argv[1];
    std::string song_name_2 = argv[2];

    Controller controller;

    controller.add_song(song_name_1);
    controller.add_song(song_name_2);

    controller.change_song(0);

    std::cout << "播放十秒" << std::endl;

    sleep(10);

    controller.set_tempo(2.0);

    std::cout << "2 倍速播放十秒" << std::endl;

    sleep(10);

    controller.set_tempo(0.5);

    std::cout << "0.5 倍速播放十秒" << std::endl;

    sleep(10);

    double current_time, total_time;
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    controller.set_tempo(1.0);

    controller.jump(current_time - 10);

    std::cout << "跳转到 10 秒前" << std::endl;

    sleep(10);

    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    controller.pause();

    std::cout << "暂停10s" << std::endl;

    sleep(10);

    controller.play();

    std::cout << "播放10s" << std::endl;

    sleep(10);

    std::cout << "切换歌曲" << std::endl;

    controller.change_song(1);

    std::cout << "播放10s" << std::endl;

    sleep(10);

    std::cout << "切换回去" << std::endl;

    controller.change_song(0);

    std::cout << "播放10s" << std::endl;

    sleep(10);

    return 0;
}

