#include "GameConfig.h"

#include "de/hackcraft/psi3d/macros.h"

#include "de/hackcraft/io/Filesystem.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

#include <string.h>


GameConfig::GameConfig()
:
mission(0),
bgm("/org/freesound/ambient.wav"),

paused(false),
fullscreen(DEFAULT_FULLSCREEN),
wireframe(false),
nightvision(false),

printpad(false),
fps(DEFAULT_FPS),
width(DEFAULT_XRES),
height(DEFAULT_YRES),
depth(DEFAULT_BPP),
multisamples(0),
fov(DEFAULT_FOV),
mouseInput(true) {
}

void GameConfig::printHelp() {
    std::cout << "\n"
            << "LinWarrior" << "\n"
            << "  by Benjamin Pickhardt (benjamin.pickhardt*udo.edu)" << "\n"
            << "\n"
            << "Build:" << "\n"
            << "  " << __DATE__ << "\n"
            << "  " << __TIME__ << "\n"
            << "\n"
            << "Options:" << "\n"
            << "\n"
            << "  --help          prints this help screen" << "\n"
            << "  --path          add a data path that takes precedence" << "\n"
            << "  --resolution    followed by resolution WIDTH or WIDTHxHEIGHT" << "\n"
            << "  --samples       folowed by the number of multisamples 0,2,4,8,... " << "\n"
            << "  --window        start in windowed mode" << "\n"
            << "  --joypad        followed by a (default) or b." << "\n"
            << "                  The a-Type Gamepad is noticeable by zedwise" << "\n"
            << "                  ordered buttons and b-Type by clockwise buttons" << "\n"
            //<< "  --mission       followed by mission name (todo)" << "\n"
            << "  --lockmouse     grab mouse and enable mouselook 0/1" << "\n"
            << "  --bgm           play background music wav file" << "\n"
            << "\n"
            << "  All options can be abbreviated by their first letter:" << "\n"
            << "    --help is the same as -h" << "\n"
            << "\n"
            << "\n";
}

int GameConfig::parseArgs(int argc, char** args) {
    // Walk through arguments until all consumed or an error occured.

    loopi(argc) {
        if (arg_i("--help")) {
            return 1;
        } else if (arg_i("--path")) {
            advance_i();
            std::string s = args[i];
            std::cout << s << "\n";
            Filesystem::getDirectoryPrefixes()->push_front(s);
        } else if (arg_i("--resolution")) {
            advance_i();
            std::string s = args[i];
            int x0 = s.find('x', 0);
            int x1 = s.find('x', x0 + 1);
            // Is it WIDTHxHEIGHT? or else just Width?
            if (x0 > 0) {
                // Width
                std::string w = s.substr(0, x0 - 0);
                this->width = atoi(w.c_str());
                // Height
                std::string h = s.substr(x0 + 1, x1 - (x0 + 1));
                this->height = atoi(h.c_str());
                // Depth
                if (x1 > 0) {
                    std::string d = s.substr(x1 + 1);
                    this->depth = atoi(d.c_str());
                    // unused value
                }
            } else {
                // Just take the width and find a fitting height.
                this->width = atoi(s.c_str());
                double aspect = 4.0f / 3.0f;
                if (this->width > 1024) aspect = 16.0f / 9.0f;
                this->height = (int) (this->width / aspect);
                this->height += (this->height % 2); // make even just in case.
            }
            //std::cout << "Resolution: " << this->width << " x " << this->height << "\n";
        } else if (arg_i("--window")) {
            this->fullscreen = false;
        } else if (arg_i("--samples")) {
            advance_i();
            std::string s = args[i];
            this->multisamples = atoi(s.c_str());
        } else if (arg_i("--mission")) {
            advance_i();
            std::string s = args[i];
            this->mission = atoi(s.c_str());
        } else if (arg_i("--lockmouse")) {
            advance_i();
            std::string s = args[i];
            this->mouseInput = (s.compare("1") == 0) ? true : false;
        } else if (arg_i("--bgm")) {
            advance_i();
            std::string s = args[i];
            this->bgm = s;
        }
    }
    return 0;
}

