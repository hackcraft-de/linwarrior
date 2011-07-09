/**
 * File:     main.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 */

#ifndef _MAIN_H
#define _MAIN_H

#include "de/hackcraft/psi3d/macros.h"

#include "de/hackcraft/userkeys.h"

#include "de/hackcraft/world/comp/computer/rController.h"

#include "de/hackcraft/world/comp/weapon/rWeapon.h"

#include "de/hackcraft/world/comp/rPadmap.h"

#include "de/hackcraft/util/GapBuffer.h"
#include "de/hackcraft/util/Pad.h"

#include "de/hackcraft/world/cBackground.h"
#include "de/hackcraft/world/cMission.h"
#include "de/hackcraft/world/cWorld.h"

#include "de/hackcraft/world/object/cObject.h"
#include "de/hackcraft/world/object/cMech.h"
#include "de/hackcraft/world/object/cBuilding.h"
#include "de/hackcraft/world/object/cTree.h"

#include <SDL/SDL.h>
#include <string>

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
struct cGame {
    /// Keyboard/Joystick inputs sent to which Virtual-Gamepad?
    Pad* pad1;

    /// Mapping of real joystick/gamepad to virtual gamepad.
    int* map1;

    /// Seeing the world through which object's eyes?
    cObject* camera;

    /// Instance of the world we are on.
    cWorld *world;

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
 * Thread-runnable object interface.
 */
class Runnable {
public:
    virtual void run() = 0;
};


/**
 * Models a minion worker thread that fetches jobs
 * from the main job queue.
 */
class Minion : public Runnable {
private:
    /// Pointing to the main job-mutex.
    SDL_mutex* jobMutex;
    /// Pointing to the main job-queue.
    std::queue<int (*)(void*) >* jobQueue;
public:
    void run();
};


/**
 * Encapsulates low level system-, startup- and io-code.
 * Actually it is just a collection of what would be functions otherwise.
 */
class cMain {
public:
    friend class Minion;
    /// Points to the main instance, necessary because of c-callbacks/threads.
    static cMain* instance;
    
private:
    /// Current game configuration.
    cGame game;

    /// SDL doesn't count mouse wheel movement.
    int mouseWheel;

    /// Mutex for job queue - minions wait until they can grab a job.
    SDL_mutex* jobMutex;

    /// The job queue itself - minions grab jobs here.
    std::queue<int(*)(void*)>* jobQueue;

    /// stdout is redirected to this stringstream.
    std::stringstream oss;

    /// Console log.
    GapBuffer console;

    /// Commandline input buffer for console.
    GapBuffer cmdline;

    /// Command & Control Overlay enabled => redirect keyboard
    bool overlayEnabled;

private:
    /// For loading post-processing filter.
    char* loadTextFile(const char* filename);

    /// Apply post-processing filter right after drawing frame.
    void applyFilter(int width, int height);
    
private:
    /// Sets initial OpenGL mode parameters (dis-/enables and values).
    void initGL(int width, int height);
    
    /// Handles "special" keys for things like rendering options.
    void updateKey(Uint8 keysym);

    /// Reads joystick/keyboard input and maps it to to a Gamepad structure.
    void updatePad(Pad* pad, SDL_Joystick* joy, int* mapping);

    /// Updating the world for the given delta time.
    void updateFrame(int elapsed_msec);

    /// Draws a single frame.
    void drawFrame();

public:
    /// Constructor sets instance pointer among other things.
    cMain();

    /// Called directly by the main entry point.
    int run(int argc, char** args);

    /// Enables and disables OpenAL Audio-System.
    int alEnableSystem(bool en);

    /// Experiment.
    void drawLog();

    /// Experiment.
    void drawPlaque();

    /// Experiment.
    void updateLog();
};


#endif
