/* 
 * File:     GameConfig.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 24, 2011, 2:02 PM
 */

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

struct GameConfig;

#include <string>
#include <queue>
#include <vector>

#define DEFAULT_FULLSCREEN true
#define DEFAULT_FPS 29
#define DEFAULT_XRES 800
#define DEFAULT_YRES 600
#define DEFAULT_BPP 16
#define DEFAULT_FOV 65

/**
 * Encapsulates parameters, options and configuration for the main controller.
 * This configuration is then used by the main controller to start and
 * run the simulation and run it's logic accordingly.
 * 
 * Contains a command line argument parser to read in those values.
 * Without parsing command line arguments or for unprovided arguments
 * there are sensible default values.
 * 
 * Please note that for technical reasons some values only take effect
 * when starting the program, others when starting a new mission
 * and some every start of a frame.
 * Therefore, changing display would require a complete restart.
 */
struct GameConfig {
    
    /** Gamepad input mapping type (z=zedwise, c=clockwise). */
    char map1;

    /** Id of Mission we are running. */
    int mission;

    /** Background music wav filename. */
    std::string bgm;

    /** Indicates pausing - no time advancement. */
    bool paused;

    /** Indicates fullscreenmode (does not change mode). */
    bool fullscreen;

    /** Enables wireframe drawing. */
    bool wireframe;

    /** Enables (fake) nightvision. */
    bool nightvision;

    /** Enables printing of game-pad button states. */
    bool printpad;

    /** Enforced frames per second. */
    float fps;

    /** Horizontal resolution. */
    int width;

    /** Vertical resolution. */
    int height;

    /** Color depth. */
    int depth;

    /** Multisampling/Antialiasing */
    int multisamples;

    /** Field Of View in degrees */
    int fov;

    /** Enables mouse input. */
    bool mouseInput;

    /**
     * Initialises default attribute parameters for running a game.
     */
    GameConfig();

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

};


#endif	/* GAME_H */

