#pragma once
#include <vector>
#include <string>
#include <thread>
#include <filesystem>

#include "decoder/decoder.h"
#include "player/player.h"

class Controller {

private:
    /**
     * @brief 所有的歌曲的列表
     * 
     */
    std::vector<std::string> song_list;
    /**
     * @brief 当前选择的歌曲的 Index, -1 为未选择
     * 
     */
    int current_select_index;

    Decoder *decoder;

    Player *player;

    std::thread play_thread;
    
public:

    Controller();

    ~Controller();

    inline int get_current_select_index() const { 
        return current_select_index; 
    }
    inline void set_current_select_index(int index) { 
        current_select_index = index; 
    }
    inline std::string get_current_select_song_name() const { 
        return current_select_index == -1 ?  "No Song" : song_list[current_select_index]; 
    }
    inline void delete_list_index(int index) {
        if(index >= current_select_index) {
            current_select_index -= 1;
        }
        song_list.erase(song_list.begin() + index);
    }

    inline bool add_song(std::string song_name) {
        // check if song_name is not exist in the file system
        for(auto &song : song_list) {
            if(song == song_name) {
                return false;
            }
        }
        if(std::filesystem::exists(song_name)) {
            song_list.push_back(song_name);
            return true;
        } else {
            return false;
        }
    }
    
    std::vector<std::string> get_song_list() {
        return song_list;
    }

    bool pause();

    bool play();

    double get_tempo() {
        return decoder->getTempo();
    }

    bool set_tempo(double tempo);

    bool jump(double jumpTarget);

    void get_time(double &current_time, double &total_time);

    bool change_song(int index);

    bool next_song() {
        if(current_select_index == -1) {
            return false;
        }
        return change_song((current_select_index + 1) % song_list.size());
    }
    
    bool last_song() {
        if(current_select_index == -1) {
            return false;
        }
        return change_song((current_select_index - 1 + song_list.size()) % song_list.size());
    }

    void play_worker();

    void quit();

    bool forward(double time) {
        if(current_select_index == -1) {
            return false;
        }
        double current_time, total_time;
        get_time(current_time, total_time);
        double target_time = current_time + time;
        if(target_time > total_time) {
            target_time = total_time;
        }
        jump(target_time);
        return true;
    } 

    bool backward(double time) {
        if(current_select_index == -1) {
            return false;
        }
        double current_time, total_time;
        get_time(current_time, total_time);
        double target_time = current_time - time;
        if(target_time < 0) {
            target_time = 0;
        }
        jump(target_time);
        return true;
    }

};
