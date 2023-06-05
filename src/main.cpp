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
    double current_time, total_time;
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

    std::cout << "播放 5 秒" << std::endl;

    sleep(5);
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    controller.set_tempo(2.0);

    std::cout << "2 倍速播放 5 秒" << std::endl;

    sleep(5);
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    controller.set_tempo(0.5);

    std::cout << "0.5 倍速播放 10 秒" << std::endl;

    sleep(10);
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    std::cout << "暂停" << std::endl;

    controller.pause();

    sleep(5);
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    std::cout << "播放" << std::endl;

    controller.play();

    sleep(5);
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    controller.jump(current_time - 5);

    std::cout << "跳转到 5 秒前" << std::endl;

    sleep(5);

    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    controller.pause();

    std::cout << "暂停 5 秒" << std::endl;

    sleep(5);
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    controller.play();

    std::cout << "播放 5 秒" << std::endl;

    sleep(5);
    controller.get_time(current_time, total_time);
    std::cout << "当前时间: " << current_time << " 总时间: " << total_time << std::endl;

    std::cout << "切换歌曲" << std::endl;

    controller.change_song(1);

    std::cout << "播放5s" << std::endl;

    sleep(5);

    std::cout << "切换回去" << std::endl;

    controller.change_song(0);

    std::cout << "播放5s" << std::endl;

    sleep(10);

    return 0;
}

