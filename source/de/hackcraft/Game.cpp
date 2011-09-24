#include "Game.h"

#include "de/hackcraft/userkeys.h"

#include "de/hackcraft/psi3d/macros.h"

#include <cassert>
#include <cstdlib>


Game::Game()
: pad1(NULL),
map1(map_zedwise),

camera(NULL),

world(NULL),

mission(0),
bgm("data/org/freesound/ambient.wav"),

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

void Game::printHelp() {
    std::cout << std::endl
            << "LinWarrior" << std::endl
            << "  by Benjamin Pickhardt (benjamin.pickhardt*udo.edu)" << std::endl
            << std::endl
            << "Build:" << std::endl
            << "  " << __DATE__ << std::endl
            << "  " << __TIME__ << std::endl
            << std::endl
            << "Options:" << std::endl
            << std::endl
            << "  --help          prints this help screen" << std::endl
            << "  --resolution    followed by resolution WIDTH or WIDTHxHEIGHT" << std::endl
            << "  --samples       folowed by the number of multisamples 0,2,4,8,... " << std::endl
            << "  --window        start in windowed mode" << std::endl
            << "  --joypad        followed by a (default) or b." << std::endl
            << "                  The a-Type Gamepad is noticeable by zedwise" << std::endl
            << "                  ordered buttons and b-Type by clockwise buttons" << std::endl
            //<< "  --mission       followed by mission name (todo)" << std::endl
            << "  --lockmouse     grab mouse and enable mouselook 0/1" << std::endl
            << "  --bgm           play background music wav file" << std::endl
            << std::endl
            << "  All options can be abbreviated by their first letter:" << std::endl
            << "    --help is the same as -h" << std::endl
            << std::endl
            << std::endl;
}

int Game::parseArgs(int argc, char** args) {
    // Walk through arguments until all consumed or an error occured.

    loopi(argc) {
        if (arg_i("--help")) {
            return 1;
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
            std::cout << "Resolution: " << this->width << " x " << this->height << std::endl;
        } else if (arg_i("--joypad")) {
            advance_i();
            std::string s = args[i];
            if (s.compare("a")) {
                this->map1 = map_zedwise;
            } else if (s.compare("b")) {
                this->map1 = map_clockwise;
            } else {
                printHelp();
            }
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

void Game::initMission() {
    this->world = new World();
    assert(this->world != NULL);
    if (this->mission == 1) {
        this->world->mMission = new Mission();
        assert(this->world->mMission != NULL);
        this->camera = this->world->mMission->init(this->world);
        this->pad1 = this->camera->pad;
    } else {
        this->world->mMission = new OpenMission();
        assert(this->world->mMission != NULL);
        this->camera = this->world->mMission->init(this->world);
        this->pad1 = this->camera->pad;
    }
    assert(this->camera != NULL);
    assert(this->pad1 != NULL);
}

