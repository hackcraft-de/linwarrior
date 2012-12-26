/**
 * File:     main.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 */

#ifndef _GAMEMAIN_H
#define _GAMEMAIN_H

#include "de/hackcraft/game/Game.h"

#include "de/hackcraft/util/GapBuffer.h"

#include <string>

#include <SDL/SDL.h>

class Logger;
class Runnable;
class Semaphore;

/**
 * Encapsulates low level system-, startup- and io-code.
 * Actually it is just a collection of what would be functions otherwise.
 */
class GameMain {
private:
    static Logger* logger;
public:
    /** Points to the main instance, necessary because of c-callbacks/threads. */
    static GameMain* instance;
    
private:
    /** Current game configuration. */
    Game game;

    /** SDL doesn't count mouse wheel movement. */
    int mouseWheel;

    /** Old keystates */
    Uint8 keystate_[512];

    /** Mutex for job queue - minions wait until they can grab a job. */
    Semaphore* jobMutex;

    /** The job queue itself - minions grab jobs here. */
    std::queue<Runnable*>* jobQueue;

    /** stdout is redirected to this stringstream. */
    std::stringstream oss;

    /** Program output log. */
    GapBuffer log;

    /** Console terminal program output. */
    GapBuffer console;

    /** Commandline input buffer for console. */
    GapBuffer cmdline;

    /** Command & Control Overlay enabled => redirect keyboard */
    bool overlayEnabled;

private:
    /** Apply post-processing filter right after drawing frame. */
    void applyFilter(int width, int height);
    
private:
    /** Sets initial OpenGL mode parameters (dis-/enables and values). */
    void initGL(int width, int height);
    
    /** Handles "special" keys for things like rendering options. */
    void updateKey(Uint8 keysym);

    /** Reads joystick/keyboard input and maps it to to a Gamepad structure. */
    void updatePad(Pad* pad, SDL_Joystick* joy, int* mapping);

    /** Updating the world for the given delta time. */
    void updateFrame(int elapsed_msec);

    /** Draws a single frame. */
    void drawFrame();

public:
    /** Constructor sets instance pointer among other things. */
    GameMain();

    /** Called directly by the main entry point. */
    int run(int argc, char** args);

    /** Enables and disables OpenAL Audio-System. */
    int alEnableSystem(bool en);

    /** Experiment. */
    void drawLog();

    /** Experiment. */
    void drawPlaque();

    /** Experiment. */
    void updateLog();
};


#endif
