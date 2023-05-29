set(MAIN_SRC main.cpp)

set(SRC 
    controller/controller.cpp
    decoder/decoder.cpp
    player/player.cpp
)

set(FFMPEG_LIBS
    avcodec
    avfilter
    avformat
    avutil
    swresample
)

SET(ALSA_LIBS
    asound
)

add_library(libaudioplayer ${SRC})
target_link_libraries(libaudioplayer ${FFMPEG_LIBS} ${ALSA_LIBS})

add_executable(audio_player ${MAIN_SRC})
target_link_libraries(audio_player libaudioplayer)