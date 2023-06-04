#include "controller/controller.h"

Controller::Controller() {
    current_select_index = -1;
    decoder = new Decoder();
    player = new Player(2, 44100, SND_PCM_FORMAT_S16_LE);
    play_thread = std::thread(&Controller::play_worker, this);
}

void Controller::play_worker() {
    std::cout << "play_worker: started" << std::endl;
    decoder->decode([&](void *buffer, int outSamplesize) {
        player->play_to_pcm(buffer, outSamplesize);
    });
}


void Controller::change_song(int index) {
    if(index == -1) {
        current_select_index = -1;
        return;
    }
    // Index out of range
    if(index < 0 || index >= song_list.size()) {
        std::cerr << "index out of range" << std::endl;
        return;
    }
    current_select_index = index;
    decoder->openFile(song_list[index].c_str());
    decoder->play();
}

void Controller::pause() {
    decoder->pause();
}

void Controller::set_tempo(double tempo) {
    decoder->changeTempo(tempo);
}

void Controller::jump(double jumpTarget) {
    decoder->jump(jumpTarget);
}

void Controller::play() {
    if(current_select_index == -1) {
        std::cerr << "No song selected" << std::endl;
        return;
    }
    decoder->play();
}

void Controller::get_time(double &current_time, double &total_time) {
    current_time = decoder->getTime();
    total_time = decoder->getTotalTime();
}

Controller::~Controller() {
    play_thread.join();
    delete decoder;
    delete player;
}