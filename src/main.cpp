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
    #include <termios.h>
    #include <alsa/asoundlib.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/samplefmt.h>
}

void print_help() {
    std::cout << "Command list:" << std::endl;
    std::cout << "  [a]dd <filename>: add a song" << std::endl;
    std::cout << "  [i]nfo <filename>: show song info" << std::endl;
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

bool isEnded = false;

std::mutex mtx;

const int INFO_MAX_WIDTH = 50;
int now_info_start_pos = 0;

void print_command_line (Controller &controller, std::string & current_catched_char) {
    while(!isEnded) {
        // get the lock 
        {
            std::lock_guard<std::mutex> lock(mtx);
            // if some song is selected, print current time
            // format to 2 decimal places after the decimal point
            std::cout << std::fixed;
            std::cout.precision(2);
            std::string info;
            if(controller.get_current_select_index() != -1) {
                double current_time, total_time;
                controller.get_time(current_time, total_time);
                info += "Name: " + controller.get_current_select_song_name() + " ";
                info += "Time: " + std::to_string(current_time) + "s/" + std::to_string(total_time) + "s ";
                info += "Tempo: " + std::to_string(controller.get_tempo()) + "x";
            } else {
                info += "No song selected";
            }
            info += "     ";
            // rolling print the info
            if(info.length() > INFO_MAX_WIDTH) {
                if(now_info_start_pos + INFO_MAX_WIDTH > info.length()) {
                    std::cout << info.substr(now_info_start_pos, info.length() - now_info_start_pos) + info.substr(0, INFO_MAX_WIDTH - (info.length() - now_info_start_pos));
                } else {
                    std::cout << info.substr(now_info_start_pos, INFO_MAX_WIDTH);
                }
                now_info_start_pos = (now_info_start_pos + 1) % (info.length());
            } else {
                std::cout << info + std::string(INFO_MAX_WIDTH - info.length(), ' ');
            }
            // print a bash-like prompt
            std::cout << " > ";
            std::cout << current_catched_char;
            // pop all space in the end
            while(current_catched_char.length() > 0 && current_catched_char[current_catched_char.length() - 1] == '\t') {
                current_catched_char.pop_back();
            }
            // move the cursor to the end of the line
            std::cout << "\r";
            std::cout << std::flush;
            // release the lock
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


int main(int argc, char *argv[]) {    
    freopen("logerr.txt", "w", stderr);

    print_help();

    Controller controller;

    std::string current_catched_char = "";


    // set terminal to non-canonical mode, close echo, disable backspace, hide cursor
    system("stty -echo; stty -icanon; stty erase ^-; setterm -cursor off");

    // start a thread to print the command line
    std::thread print_command_line_thread(print_command_line, std::ref(controller), std::ref(current_catched_char));
    std::string command_line;

    // main loop
    while(true) {
        // put the info before the command line 
        // catch the input and manually put it to the end of the line

        // catch a char
        char c = 0;
        c = getchar();
        // if it is a backspace, delete the last char
        if(c == 127 || c == 8 || c == 27) {
            if(current_catched_char.size() > 0) {
                std::lock_guard<std::mutex> lock(mtx);
                current_catched_char[current_catched_char.length() - 1] = '\t';
            }
            continue;
        } 
        // if it is a enter, execute the command
        else if(c == 10) {
            std::cout << std::endl;
            // execute the command
            command_line = current_catched_char;
            std::cout << std::endl;
            current_catched_char = "";
        } else if(c == ' ') {
            c = ' ';
        }

        if(c != 10) {
            current_catched_char += c;
            continue;
        } 
        
    
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
            case 'i':
                if(command == "info") {
                    std::string song_name;
                    double current_time, total_time;
                    song_name = controller.get_current_select_song_name();
                    if(controller.get_current_select_index() == -1) {
                        std::cout << "Error: no song selected" << std::endl;
                        break;
                    } else {
                        controller.get_time(current_time, total_time);
                        std::cout << "Song name: " << song_name
                            << "Time: " << current_time << "s/" << total_time << "s" << std::endl;
                    }
                } else {
                    std::cout << "Error: unknown command" << std::endl;
                }
                break;
            case 's':
                if(command == "show") {
                    auto song_list = controller.get_song_list();
                    std::cout << "Total " << song_list.size() << ", current: " << controller.get_current_select_index() << std::endl;
                    std::cout << "Song list:" << std::endl;
                    for(int i = 0; i < song_list.size(); i++) {
                        if(i == controller.get_current_select_index())
                            std::cout << "  [" << i << "]: " << song_list[i] << " (current)" << std::endl;
                        else
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
                    isEnded = true;
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

    // reopen the terminal, open icanon and echo, use backspace
    system("stty echo; stty icanon; stty erase ^H");
    // show the cursor
    system("setterm -cursor on");
    // wait for the thread to end
    if (print_command_line_thread.joinable()) {
        print_command_line_thread.join();
    }

    return 0;
}

