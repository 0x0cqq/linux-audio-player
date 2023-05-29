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


int main() {
    const char* file_path = "../sample-15s.mp3";
    const char* outputFile = "../output2.pcm";
    Decoder decoder;
    Player player(2, 44100, SND_PCM_FORMAT_S16_LE);
    decoder.openFile(file_path);
    decoder.decode(outputFile, [&](void *buffer, int outSamplesize) {
        player.play_to_pcm(buffer, outSamplesize);
    });

    return 0;
}

