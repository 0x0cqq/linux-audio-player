#pragma once
#include <vector>
#include <string>
#include <thread>

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

    inline void add_song(std::string song_name) {
        song_list.push_back(song_name);
    }

    void pause();

    void play();

    void change_song(int index);

    void play_worker();

};
