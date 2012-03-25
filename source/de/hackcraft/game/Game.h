/* 
 * File:     Game.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 24, 2011, 2:02 PM
 */

#ifndef GAME_H
#define GAME_H

struct Game;

#include "de/hackcraft/io/Pad.h"

#include "de/hackcraft/world/sub/mission/MissionSystem.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/Entity.h"

#include "OpenMission.h"

#define DEFAULT_FULLSCREEN true
#define DEFAULT_FPS 29
#define DEFAULT_XRES 800
#define DEFAULT_YRES 600
#define DEFAULT_BPP 16
#define DEFAULT_FOV 65

/**
 * Structurizes/glues what's necessary to run a game-mission including
 * io-configuration. This is a singleton. It should not be used
 * by any other class and is maintained directly by the main code.
 */
struct Game {
    /// Keyboard/Joystick inputs sent to which Virtual-Gamepad?
    Pad* pad1;

    /// Mapping of real joystick/gamepad to virtual gamepad.
    int* map1;

    /// Seeing the world through which object's eyes?
    Entity* camera;

    /// Instance of the world we are on.
    World *world;

    /// Id of Mission we are running.
    int mission;

    /// Background music wav filename.
    std::string bgm;

    /// Indicates pausing - no time advancement.
    bool paused;

    /// Indicates fullscreenmode (does not change mode).
    bool fullscreen;

    /// Enables wireframe drawing.
    bool wireframe;

    /// Enables (fake) nightvision.
    bool nightvision;

    /// Enables printing of game-pad button states.
    bool printpad;

    /// Enforced frames per second.
    float fps;

    /// Horizontal resolution.
    int width;

    /// Vertical resolution.
    int height;

    /// Color depth.
    int depth;

    /// Multisampling/Antialiasing
    int multisamples;

    /// Field Of View in degrees
    int fov;

    /// Enables mouse input.
    bool mouseInput;

    /**
     * Initialises default attribute parameters for running a game.
     */
    Game();

    /**
     * Prints commandline help.
     */
    void printHelp();

    /**
     * Parses commandline arguments and sets game attributes accordingly.
     * See printHelp for arguments or use "-h" to print help.
     * \return 0 on success, 1 on failure.
     */
    int parseArgs(int argc, char** args);

    /**
     * Starts and initializes Mission according to the current game attributes.
     */
    void initMission();

};


#endif	/* GAME_H */

