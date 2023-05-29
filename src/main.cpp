#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decoder/decoder.h"
#include "player/player.h"

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
    Decoder decoder;
    Player player(2, 44100, SND_PCM_FORMAT_S16_LE);
    decoder.openFile(argv[1]);
    decoder.decode(argv[2], [&](void *buffer, int outSamplesize) {
        player.play_to_pcm(buffer, outSamplesize);
    });

    return 0;
}

