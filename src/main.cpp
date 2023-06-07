#include <iostream>
#include <sstream>
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

void print_help() {
    std::cout << "Command list:" << std::endl;
    std::cout << "  [a]dd <filename>: add a song" << std::endl;
    std::cout << "  [s]how: show music list" << std::endl;
    std::cout << "  [c]hoose: choose a song music index" << std::endl;
    std::cout << "  [t]empo <tempo_count>: set tempo to a number in [0.5, 2]" << std::endl;
    std::cout << "  [n]ext song" << std::endl;
    std::cout << "  [l]ast song" << std::endl; 
    std::cout << "  [f]orward <time>: forward time secs" << std::endl;
    std::cout << "  [b]ackward <time>: backward time secs" << std::endl;
    std::cout << "  [j]ump <time>: jump to time secs" << std::endl;
    std::cout << "  [q]uit: quit program" << std::endl;
    std::cout << "  [p]ause: pause" << std::endl;
    std::cout << "  [r]esume: resume" << std::endl;
    std::cout << "  [h]elp: reprint help" << std::endl;
}



int main(int argc, char *argv[]) {    
    Controller controller;

    print_help();


    while(true) {
        std::string command_line;
        getline(std::cin, command_line);
        // split to two parts by space
        std::stringstream ss(command_line);
        std::string command;
        ss >> command;
        switch (command[0]) {
            case 'a':
                if(command == "add") {
                    std::string filename;
                    ss >> filename;
                    controller.add_song(filename);
                    std::cout << "Success: song added, " << filename << std::endl;
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 's':
                if(command == "show") {
                    auto song_list = controller.get_song_list();
                    std::cout << "Total " << song_list.size() << " Song list:" << std::endl;
                    for(int i = 0; i < song_list.size(); i++) {
                        std::cout << "  [" << i << "]: " << song_list[i] << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'c':
                if(command == "choose") {
                    int index;
                    ss >> index;
                    bool res = controller.change_song(index);
                    if (res) {
                        std::cout << "Success: change to song " << index << ", name: " << controller.get_current_select_song_name() << std::endl;
                    } else {
                        std::cout << "Error: change song failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 't':
                if(command == "tempo") {
                    double tempo;
                    ss >> tempo;
                    bool res = controller.set_tempo(tempo);
                    if (res) {
                        std::cout << "Success: set tempo to " << tempo << std::endl;
                    } else {
                        std::cout << "Error: set tempo failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'n':
                if(command == "next") {
                    bool res = controller.next_song();
                    if (res) {
                        std::cout << "Success: next song, name: " << controller.get_current_select_song_name() << std::endl;
                    } else {
                        std::cout << "Error: next song failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'l':
                if(command == "last") {
                    bool res = controller.last_song();
                    if (res) {
                        std::cout << "Success: last song, name: " << controller.get_current_select_song_name() << std::endl;
                    } else {
                        std::cout << "Error: last song failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'f':
                if(command == "forward") {
                    double time;
                    ss >> time;
                    bool res = controller.forward(time);
                    if (res) {
                        std::cout << "Success: forward " << time << " secs" << std::endl;
                    } else {
                        std::cout << "Error: forward failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'b':
                if(command == "backward") {
                    double time;
                    ss >> time;
                    bool res = controller.backward(time);
                    if (res) {
                        std::cout << "Success: backward " << time << " secs" << std::endl;
                    } else {
                        std::cout << "Error: backward failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'j':
                if(command == "jump") {
                    double time;
                    ss >> time;
                    bool res = controller.jump(time);
                    if (res) {
                        std::cout << "Success: jump to " << time << " secs" << std::endl;
                    } else {
                        std::cout << "Error: jump failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'q':
                if(command == "quit") {
                    controller.quit();
                    std::cout << "Success: quit program" << std::endl;
                    goto end;
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'h':
                if(command == "help") {
                    print_help();
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'p':
                if(command == "pause") {
                    bool res = controller.pause();
                    if (res) {
                        std::cout << "Success: pause" << std::endl;
                    } else {
                        std::cout << "Error: pause failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 'r':
                if(command == "resume") {
                    bool res = controller.play();
                    if (res) {
                        std::cout << "Success: resume" << std::endl;
                    } else {
                        std::cout << "Error: resume failed" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
        }
    } 

end:


    return 0;
}

