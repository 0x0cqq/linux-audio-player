#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decoder.h"

extern "C" {
    #include <alsa/asoundlib.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/samplefmt.h>
}


int main() {
    const char* file_path = "sample-15s.mp3";
    const char* outputFile = "output2.pcm";
    return 0;
}

