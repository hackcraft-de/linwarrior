/**
 * File:     main.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 */

#ifndef _MAIN_H
#define _MAIN_H

#include "psi3d/macros.h"
#include "userkeys.h"

#include "cPad.h"
#include "cController.h"
#include "cBackground.h"
#include "cMission.h"
#include "cWorld.h"
#include "cObject.h"
#include "cWeapon.h"
#include "cMech.h"
#include "cBuilding.h"
#include "cPadmap.h"
#include "cTree.h"

#include <SDL/SDL.h>

#define DEFAULT_FULLSCREEN true
#define DEFAULT_FPS 29
#define DEFAULT_XRES 800
#define DEFAULT_YRES 600
#define DEFAULT_BPP 16
#define DEFAULT_FOV 60

/**
 * Structurizes/glues what's necessary to run a game-mission including
 * io-configuration. This is a singleton. It should not be used
 * by any other class and is maintained directly by the main code.
 */
struct cGame {
    /// Keyboard/Joystick inputs sent to which Virtual-Gamepad?
    cPad* pad1;

    /// Mapping of real joystick/gamepad to virtual gamepad.
    int* map1;

    /// Seeing the world through which object's eyes?
    cObject* camera;

    /// Instance of the world we are on.
    cWorld *world;

    /// Id of Mission we are running.
    int mission;

    /// Background music wav filename.
    string bgm;

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
    cGame();
    
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


/**
 * Encapsulates low level system-, startup- and io-code.
 * Actually it is just a collection of what would be functions otherwise.
 */
struct cMain {
    static cGame game;

    // SDL doesn't count mouse wheel movement.
    static int mouseWheel;

    /// Sets initial OpenGL mode parameters (dis-/enables and values).
    static void initGL(int width, int height);

    /// Draws a single frame after updating the world for the given delta time.
    static void drawFrame(int elapsed_msec);

    /// Handles "special" keys for things like rendering options.
    static void updateKey(Uint8 keysym);

    /// Reads joystick/keyboard input and maps it to to a Gamepad structure.
    static void updatePad(cPad* pad, SDL_Joystick* joy, int* mapping);

    /// Called directly by the main entry point.
    static int sdlmain(int argc, char** args);
};


#endif
