#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include "decoder/decoder.h"
#include "player/player.h"

extern "C" {
    #include <alsa/asoundlib.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/samplefmt.h>
}

void play_worker(Decoder &decoder, Player &player, std::string output_file_path) {
    std::cout << "play_worker: start playing" << std::endl;
    decoder.decode(output_file_path.c_str(), [&](void *buffer, int outSamplesize) {
        player.play_to_pcm(buffer, outSamplesize);
    });
}


int main(int argc, char *argv[]) {
    if(argc != 3) {
        std::cerr << "Wrong input args count" << std::endl;
        exit(1);
    }
    Decoder decoder;
    Player player(2, 44100, SND_PCM_FORMAT_S16_LE);
    decoder.openFile(argv[1]);

    std::string output_file_path = argv[2];

    std::thread t1(play_worker, std::ref(decoder), std::ref(player), output_file_path);


    decoder.play();

    sleep(15);

    auto time = decoder.getTime();

    std::cout << "Time: " << time << std::endl;

    decoder.jump(time - 5);

    sleep(2);

    auto time2 = decoder.getTime();

    std::cout << "Time: " << time2 << std::endl;

    sleep(5);

    auto time3 = decoder.getTime();

    std::cout << "Time: " << time3 << std::endl;

    t1.join();

    return 0;
}

