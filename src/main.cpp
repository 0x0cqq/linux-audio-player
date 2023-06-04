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

    sleep(15);

    controller.change_song(1);

    return 0;
}

