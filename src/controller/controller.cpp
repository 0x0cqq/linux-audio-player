#include "controller/controller.h"

Controller::Controller() {
    av_log_set_level(AV_LOG_ERROR);
    current_select_index = -1;
    decoder = new Decoder();
    player = new Player(2, 44100, SND_PCM_FORMAT_S16_LE);
    play_thread = std::thread(&Controller::play_worker, this);
}

void Controller::play_worker() {
    // std::cout << "play_worker: started" << std::endl;
    decoder->decode([&](void *buffer, int outSamplesize) {
        player->play_to_pcm(buffer, outSamplesize);
    });
}


bool Controller::change_song(int index) {
    if(index == -1) {
        current_select_index = -1;
        return true;
    }
    // Index out of range
    if(index < 0 || index >= song_list.size()) {
        std::cerr << "index out of range" << std::endl;
        return false;
    }
    current_select_index = index;
    decoder->pause();
    decoder->openFile(song_list[index].c_str());
    decoder->play();
    return true;
}

bool Controller::pause() {
    if(current_select_index == -1) {
        std::cerr << "No song selected" << std::endl;
        return false;
    }
    decoder->pause();
    return true;
}

bool Controller::set_tempo(double tempo) {
    return decoder->changeTempo(tempo);
}

bool Controller::jump(double jumpTarget) {
    return decoder->jump(jumpTarget);
}

bool Controller::play() {
    if(current_select_index == -1) {
        std::cerr << "No song selected" << std::endl;
        return false;
    }
    decoder->play();
    return true;
}

void Controller::get_time(double &current_time, double &total_time) {
    current_time = decoder->getTime();
    total_time = decoder->getTotalTime();
}


void Controller::quit() {
    decoder->quit();
}

Controller::~Controller() {
    play_thread.join();
    delete decoder;
    delete player;
}