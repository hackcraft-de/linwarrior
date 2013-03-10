#include "GameMain.h"

#include "de/hackcraft/game/GameConfig.h"
#include "de/hackcraft/game/GameMission.h"
#include "de/hackcraft/game/OpenMission.h"
#include "de/hackcraft/game/userkeys.h"

#include "de/hackcraft/io/Pad.h"
#include "de/hackcraft/io/Shaderfile.h"

#include "de/hackcraft/lang/Runnable.h"
#include "de/hackcraft/lang/Thread.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/openal/AL.h"

#include "de/hackcraft/psi3d/GLF.h"
#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Console.h"

#include "de/hackcraft/util/GapBuffer.h"

#include "de/hackcraft/util/concurrent/Threadpool.h"

#include "de/hackcraft/world/World.h"
#include "de/hackcraft/world/Entity.h"

#include "de/hackcraft/world/sub/misc/MiscSystem.h"
#include "de/hackcraft/world/sub/misc/rInputsource.h"

#include "de/hackcraft/world/sub/trigger/rAlert.h"

#include <cassert>
#include <iosfwd>
#include <iostream>
#include <sstream>

#include <SDL/SDL.h>
#include <SDL/SDL_timer.h>


Logger* GameMain::logger = Logger::getLogger("de.hackcraft.game.GameMain");

GameMain* GameMain::instance = NULL;


// Called atexit
void cleanup() {
    std::cout << "Thank you for playing.\n";
    SDL_Quit();
    alutExit();
    GameMain::instance->alEnableSystem(false);
    delete GameMain::instance;
    GameMain::instance = NULL;
}


class JobRender : public Runnable {
public:
    int state;
    
    JobRender() {
        state = 0;
    };

    void run() {
        int i = 0;
        while (state == 0) {
            //float s = 0.5 + 0.4 * sin(i * M_PI / 10.0);
            //float c = 0.5 + 0.4 * cos(i * M_PI / 10.0);
            //GL::glClearColor(0.1+s, 0.1+c, 0.9, 1.0);
            //GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //cMain::drawPlaque();
            //cMain::drawLog();
            //SDL_GL_SwapBuffers();
            Thread::sleep(1.0 / DEFAULT_FPS * 1000.0);
            i++;
        }
    }

};


class JobBGM : public Runnable {
public:
    void run() {
        while (true) {
            //cout << "Buffering BGM.\n";
            Thread::sleep(1000);
        }
    }
};


class JobOutput : public Runnable {
public:
    void run() {
        while (true) {
            GameMain::instance->updateLog();
            Thread::sleep(200);
        }
    }
};


GameMain::GameMain() {
    
    instance = this;
    
    config = new GameConfig();
    
    world = NULL;
    camera = NULL;
    
    pad1 = NULL;
    map1 = NULL;

    threadpool = new Threadpool();
    
    mouseWheel = 0;
    overlayEnabled = !true;

    memset(keystate_, 0, sizeof(keystate_));
    
    log = new GapBuffer();
    
    console = new GapBuffer();    
    console->write("Console program output is printed here...");
    
    cmdline = new GapBuffer();
    cmdline->write("Console command line input is inserted here...");
    
    stdout_ = NULL;
    
    joy0 = NULL;
}


void GameMain::initGL(int width, int height) {
    if (true) {
        std::string glinfo;
        glinfo = (const char*) GL::glGetString(GL_RENDERER);
        logger->debug() << glinfo << "\n";
        glinfo = (const char*) GL::glGetString(GL_VENDOR);
        logger->debug() << glinfo << "\n";
        glinfo = (const char*) GL::glGetString(GL_VERSION);
        logger->debug() << glinfo << "\n";
        glinfo = (const char*) GL::glGetString(GL_EXTENSIONS);
        logger->debug() << glinfo << "\n";
        if (glinfo.find("GL_EXT_texture3D", 0) == std::string::npos) {
            logger->error() << "NO SUPPORT for GL_EXT_texture3D !!!\n";
        }
    }

    GL::glViewport(0, 0, width, height);

    rgba fogColor = {1.0, 1.0, 1.0, 1.0};
    //float fogDensity = 0.003f; // original
    float fogDensity = 0.006f;

    GL::glEnable(GL_BLEND);
    GL::glEnable(GL_DEPTH_TEST);
    GL::glEnable(GL_MULTISAMPLE);

    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //GL::glEnable(GL_LINE_SMOOTH);
    //GL::glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //GL::glEnable(GL_POLYGON_SMOOTH);


    GL::glClearAccum(0.0, 0.0, 0.0, 0.0);
    GL::glClearColor(0.0, 0.125, 0.0, 0.0);
    //GL::glClearDepth(1.0);
    GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

    GL::glShadeModel(GL_SMOOTH);
    GL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //GL::glDepthMask(GL_TRUE);
    //GL::glDepthFunc(GL_LEQUAL);

    GL::glFogi(GL_FOG_MODE, GL_EXP);
    GL::glFogfv(GL_FOG_COLOR, fogColor);
    GL::glFogf(GL_FOG_DENSITY, fogDensity);

    GL::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Lighting
    {
        GL::glEnable(GL_LIGHTING);
        float p[] = {0, 500, 0, 1};
        float a[] = {0.95, 0.5, 0.5, 1};
        GL::glLightfv(GL_LIGHT0, GL_POSITION, p);
        GL::glLightfv(GL_LIGHT0, GL_AMBIENT, a);
        GL::glLightfv(GL_LIGHT0, GL_DIFFUSE, a);
        GL::glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
        GL::glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001);
        GL::glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00001);
        GL::glEnable(GL_COLOR_MATERIAL);
        //GL::glEnable(GL_NORMALIZE);
        GL::glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    }

    // Init instant console font.
    GLF::glUploadFont();
}


void GameMain::initMission() {
    world = new World();
    
    if (config->mission == 1) {
        mission = new GameMission();
    } else {
        mission = new OpenMission();
    }
    
    //world->subsystems.push_back(mission);
    camera = mission->init(world);
    //pad1 = camera->pad;
    pad1 = new Pad();
    MiscSystem::getInstance()->findInputsourceByEntity(camera->oid)->setPad(pad1);
    
    if (config->map1 == 'z') {
        map1 = map_zedwise;
    } else if (config->map1 == 'c') {
        map1 = map_clockwise;
    } else {
        map1 = map_zedwise;
    }

}


void GameMain::applyFilter(int width, int height) {
    static bool fail = false;
    static GL::GLenum postprocess = 0;

    if (postprocess == 0 && !fail) {
        char* vtx = Shaderfile::loadShaderFile("/base/prgs/post.vert");
        if (vtx) logger->trace() << "--- Vertex-Program Begin ---\n" << vtx << "\n--- Vertex-Program End ---\n";
        char* fgm = Shaderfile::loadShaderFile("/base/prgs/post.frag");
        if (fgm) logger->trace() << "--- Fragment-Program Begin ---\n" << fgm << "\n--- Fragment-Program End ---\n";
        fail = (vtx == NULL || fgm == NULL) || (vtx[0] == 0 || fgm[0] == 0);
        if (!fail) {
            std::stringstream str;
            postprocess = GLS::glCompileProgram(vtx, fgm, str);
            if (!str.str().empty()) {
                logger->error() << str.str() << "\n";
            }
        }
        delete[] vtx;
        delete[] fgm;
    }

    if (fail) return;

    GL::glFlush();

    GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GL::glDisable(GL_LIGHTING);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_DEPTH_TEST);
        GL::glDisable(GL_CULL_FACE);
        GL::glEnable(GL_TEXTURE_2D);
        GL::glColor4f(1, 1, 1, 1);

        GL::glUseProgramObjectARB(postprocess);
        {
            // Capture Color.
            static long screencolor = 0;
            if (screencolor == 0) screencolor = GLS::glBindTextureMipmap2D(0, true, true, false, false, width, height, NULL);
            GL::glUniform1i(GL::glGetUniformLocation(postprocess, "tex"), 0);
            GL::glActiveTexture(GL_TEXTURE0);
            GL::glBindTexture(GL_TEXTURE_2D, screencolor);
            GL::glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 0, 0, width, height, 0);

            // Capture Depth.
            static long screendepth = 0;
            if (screendepth == 0) screendepth = GLS::glBindTextureMipmap2D(0, true, true, false, false, width, height, NULL, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
            GL::glUniform1i(GL::glGetUniformLocation(postprocess, "dep"), 1);
            GL::glActiveTexture(GL_TEXTURE1);
            GL::glBindTexture(GL_TEXTURE_2D, screendepth);
            GL::glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

            GL::glBegin(GL_QUADS);
            {
                GL::glTexCoord2i(1, 0);
                GL::glVertex3f(+1.0f, -1.0f, 0);
                GL::glTexCoord2i(0, 0);
                GL::glVertex3f(-1.0f, -1.0f, 0);
                GL::glTexCoord2i(0, 1);
                GL::glVertex3f(-1.0f, +1.0f, 0);
                GL::glTexCoord2i(1, 1);
                GL::glVertex3f(+1.0f, +1.0f, 0);
            }
            GL::glEnd();
        }
        GL::glUseProgramObjectARB(0);
    }
    GL::glPopAttrib();
}


void GameMain::updateFrame(int elapsed_msec) {
    
    if (config->paused) {
        // Delete Fragged Objects of previous frames.
        world->bagFragged();
        return;
    }
    
    mission->advanceTime(elapsed_msec);

    // Re-Cluster spatial index.
    world->clusterObjects();

    int subframes = 1;

    loopi(subframes) {
        // calculate the time that the last frame took.
        world->advanceTime(elapsed_msec / subframes);

        // Deliver overdue messages to objects and groups of objects.
        world->dispatchMessages();

        // Calculate new animation state and the like (if not paused).
        world->animateObjects();

        // Calculate positions through transformations.
        GL::glPushMatrix();
        {
            GL::glLoadIdentity();
            world->transformObjects();
        }
        GL::glPopMatrix();
    }
}


void GameMain::drawFrame() {
    //logger->trace() << "elapsed: " << (elapsed_msec / 1000.0f) << "\n";
    //if (hurlmotion) elapsed = int(elapsed * 1.0f);

    static bool picking = false;
    const int SELECTIONSIZE = 1024;
    GL::GLuint selection[SELECTIONSIZE];
    if (picking) {
        // With Projection:
        //GL::glGetIntegerv(GL_VIEWPORT, viewport);
        //gluPickMatrix(cx, viewport[3]-cy, 5, 5, viewport);
        //gluPerspective(45,ratio,0.1,1000);
        GL::glSelectBuffer(SELECTIONSIZE, selection);
        GL::glRenderMode(GL_SELECT);
        GL::glInitNames();
        // With Modelview:
    } else {
        GL::glRenderMode(GL_RENDER);
    }

    // Draw area zones while paused (debug/editmode kind of).
    rAlert::sDrawzone = config->paused;

    // Draw fewer frames? Only every n'th frame.
    int nthframe = 1;
    static int cnt = 0;
    cnt++;
    if (cnt % nthframe != 0) return;

    // SOLID / WIREFRAME drawing scheme.
    if (!config->wireframe) {
        GL::glClearColor(0.2, 0.2, 0.2, 1.0);
        GL::glEnable(GL_FOG);
        GL::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        GL::glEnable(GL_CULL_FACE);
        GL::glEnable(GL_LIGHTING);
        GL::glClear(GL_DEPTH_BUFFER_BIT); // With Background.
    } else {
        GL::glClearColor(0.2, 0.2, 0.2, 1.0);
        GL::glEnable(GL_FOG);
        GL::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //GL::glDisable(GL_CULL_FACE);
        GL::glDisable(GL_LIGHTING);
        GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // W/o background.
    }

    GLS::glPushPerspectiveProjection(config->fov, 0.07, 300);
    {
        // Setup camera.
        GL::glLoadIdentity();
        if (camera) {
            camera->camera();
            camera->listener();
        }

        // Find objects in visible range and prepare rendering for POV.
        world->setupView(camera->pos0, camera->ori0);

        // Draw the surrounding Sky- and Ground-Dome/Box
        world->drawBack();

        // Draw the Objects themselves.
        world->drawSolid();

        // Draw the Object's translucent effects.
        world->drawEffect();

        bool postprocessing = true;
        if (postprocessing) applyFilter(config->width, config->height);

        // Draw the Head-Up-Display of the currently spectating Object.
        camera->drawHUD();
    }
    GLS::glPopProjection();

    float motionblur = 0.0f;
    if (config->nightvision > 0.0001f) {
        GL::glFlush();
        GLS::glAccumBlurInverse(0.83f);
    } else if (motionblur > 0.0001) {
        GL::glFlush();
        GLS::glAccumBlur(motionblur);
    }
    
    if (picking) {
        int entries = GL::glRenderMode(GL_RENDER);
        logger->debug() << "selected: " << entries << "\n";
        // Picking-Entry: n, minz, maxz, n_names
        GL::GLuint* p = selection;

        loopi(entries) {
            GL::GLuint n = *p++;
            GL::GLuint minz = *p++;
            GL::GLuint maxz = *p++;
            logger->trace() << n << " " << minz << " " << maxz << "\n";

            loopj(n) {
                GL::GLuint name = *p++;
                logger->trace() << " " << name;
            }
            logger->trace() << "\n";
        }
    }
    //picking = !picking;

    if (config->paused || overlayEnabled) drawLog();
}


void GameMain::drawLog() {
    GLS::glPushOrthoProjection(-0.0,+1.0,-1.0,+0.0,-100,+100);
    {
        GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GL::glDisable(GL_CULL_FACE);
            GL::glDisable(GL_LIGHTING);
            GL::glDisable(GL_DEPTH_TEST);
            GL::glDepthMask(false);
            GL::glColor4f(1,1,1,1);
            GL::glPushMatrix();
            {
                GL::glLoadIdentity();
                GL::glScalef(1.0/120.0, 1.0/60.0, 1.0);
                GL::glTranslatef(0,1.0,-100);
                char buffer[256*128];
                log->printConsole(buffer, 120, 60/3);
                Console::glDrawConsole(buffer, 120*60/3, 120, 0);

                GL::glTranslatef(0,-60/3.0f,0);
                console->printConsole(buffer, 120, 60/3);
                Console::glDrawConsole(buffer, 120*60/3, 120, 0);

                GL::glTranslatef(0,-60/3.0f,0);
                cmdline->printConsole(buffer, 120, 60/3);
                Console::glDrawConsole(buffer, 120*60/3, 120, 0);
            }
            GL::glPopMatrix();
        }
        GL::glPopAttrib();
    }
    GLS::glPopProjection();
}


void GameMain::updateKey(Uint8 keysym) {
    if (keysym == SDLK_TAB) {
        overlayEnabled ^= true;
    }

    if (overlayEnabled) {
        GapBuffer* text = cmdline;
        if (keysym == SDLK_LEFT) text->stepLeft();
        else if (keysym == SDLK_RIGHT) text->stepRight();
        else if (keysym == SDLK_UP) text->stepUp();
        else if (keysym == SDLK_DOWN) text->stepDown();
        else if (keysym == SDLK_RETURN) text->write('\n');
        else if (keysym == SDLK_BACKSPACE) text->deleteLeft();
        else if (keysym == SDLK_SPACE) text->write(' ');
        else if (keysym == SDLK_COLON) text->write(':');
        else if (keysym == SDLK_SEMICOLON) text->write(';');
        else if (keysym == SDLK_MINUS) text->write('-');
        else if (keysym == SDLK_SLASH) text->write('/');
        else if (keysym == SDLK_BACKSLASH) text->write('\\');
        else if (keysym == SDLK_LESS) text->write('<');
        else if (keysym == SDLK_EQUALS) text->write('=');
        else if (keysym == SDLK_GREATER) text->write('>');
        else if (keysym == SDLK_AT) text->write('@');
        else if (keysym >= SDLK_a && keysym <= SDLK_z) {
            text->write('a' + (keysym-SDLK_a));
        } else {
            //text->write("?");
        }
        //text->print();
        //text->printState(false);

        return;
    }

    if (keysym == _WIREFRAME_KEY) {
        config->wireframe = !config->wireframe;
    } else if (keysym == _NIGHTVISION_KEY) {
        config->nightvision = !config->nightvision;
    } else if (keysym == _PAUSE_KEY) {
        config->paused = !config->paused;
    } else {
        if (keysym == SDLK_MINUS) World::getInstance()->setViewdistance(fmax(100, World::getInstance()->getViewdistance() - 50));
        else if (keysym == SDLK_PLUS) World::getInstance()->setViewdistance(fmin(3000, World::getInstance()->getViewdistance() + 50));
    }
}


void GameMain::updatePad(Pad* pad, SDL_Joystick* joy, int* mapping) {

    if (pad == NULL) return;

    int default_mapping[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15,
        16, 17, 18, 19,
        20, 21, 22, 23,
        24, 25, 26, 27,
        28, 29, 30, 31
    };
    int* map = default_mapping;
    if (mapping != NULL) map = mapping;

    pad->reset();
    if (joy != NULL) {

        loopi(4) {
            pad->setAxis((Pad::Axes) i, SDL_JoystickGetAxis(joy, i) / 32000.0f);
        }

        loopi(32) {
            if (SDL_JoystickGetButton(joy, map[i])) pad->setButton((Pad::Buttons)i, true);
        }
        //pad->print();
    }

    Uint8 *keystate = SDL_GetKeyState(NULL);
    if (keystate == NULL) return;
    if (overlayEnabled) return;

    // First Analogue Control Stick
    if (keystate[SDLK_LEFT] == 1) pad->setAxis(Pad::AX_LR1, -1.0f);
    if (keystate[SDLK_RIGHT] == 1) pad->setAxis(Pad::AX_LR1, +1.0f);
    if (keystate[SDLK_UP] == 1) pad->setAxis(Pad::AX_UD1, -1.0f);
    if (keystate[SDLK_DOWN] == 1) pad->setAxis(Pad::AX_UD1, +1.0f);
    // Start and Select
    if (keystate[SDLK_RETURN] == 1) pad->setButton(Pad::BT_START, true);
    if (keystate[SDLK_BACKSPACE] == 1) pad->setButton(Pad::BT_SELECT, true);
    // Lx and Rx Buttons
    if (keystate[SDLK_w] == 1) pad->setButton(Pad::BT_L1, true);
    if (keystate[SDLK_a] == 1) pad->setButton(Pad::BT_L2, true);
    if (keystate[SDLK_r] == 1) pad->setButton(Pad::BT_R1, true);
    if (keystate[SDLK_g] == 1) pad->setButton(Pad::BT_R2, true);
    // Primary Buttons
    if (keystate[SDLK_s] == 1) pad->setButton(Pad::BT_PL, true);
    if (keystate[SDLK_e] == 1) pad->setButton(Pad::BT_PU, true);
    if (keystate[SDLK_d] == 1) pad->setButton(Pad::BT_PD, true);
    if (keystate[SDLK_f] == 1) pad->setButton(Pad::BT_PR, true);
    // Second Analogue Stick or Coolie-Hat:
    if (keystate[SDLK_LALT] == 0) {
        if (keystate[SDLK_j]) pad->setAxis(Pad::AX_LR2, -1.0f);
        if (keystate[SDLK_l]) pad->setAxis(Pad::AX_LR2, +1.0f);
        if (keystate[SDLK_i]) pad->setAxis(Pad::AX_UD2, -1.0f);
        if (keystate[SDLK_k]) pad->setAxis(Pad::AX_UD2, +1.0f);
    } else {
        if (keystate[SDLK_j]) pad->setButton(Pad::BT_HL, true);
        if (keystate[SDLK_l]) pad->setButton(Pad::BT_HR, true);
        if (keystate[SDLK_i]) pad->setButton(Pad::BT_HU, true);
        if (keystate[SDLK_k]) pad->setButton(Pad::BT_HD, true);
    };
    // Alternate Coolie-Hat keys.
    if (keystate[SDLK_INSERT] == 1) pad->setButton(Pad::BT_PL, true);
    if (keystate[SDLK_PAGEUP] == 1) pad->setButton(Pad::BT_PU, true);
    if (keystate[SDLK_PAGEDOWN] == 1) pad->setButton(Pad::BT_PD, true);
    if (keystate[SDLK_DELETE] == 1) pad->setButton(Pad::BT_PR, true);
    // Stick Buttons
    if (keystate[SDLK_PERIOD] || keystate[SDLK_HOME]) pad->setButton(Pad::BT_J1B, true);
    if (keystate[SDLK_COMMA] || keystate[SDLK_END]) pad->setButton(Pad::BT_J2B, true);


    // Optional Mouse Input.
    if (config->mouseInput) {
        int mx = 0;
        int my = 0;
        int w2 = config->width >> 1;
        int h2 = config->height >> 1;
        unsigned char mb = SDL_GetRelativeMouseState(&mx, &my);
        SDL_WarpMouse(w2, h2);
        SDL_PumpEvents();
        int mx_ = 0;
        int my_ = 0;
        SDL_GetRelativeMouseState(&mx_, &my_);
        //cout << (mx/(float)w2) << " " << (my/(float)h2) << " " << ((int)mb) << "\n";

        pad->setAxis(Pad::AX_LR2, pad->getAxis(Pad::AX_LR2) + 10.0f * mx / (float) w2);
        pad->setAxis(Pad::AX_UD2, pad->getAxis(Pad::AX_UD2) - 10.0f * my / (float) h2);

        if (mouseWheel > 0) pad->setButton(Pad::BT_PL, true);
        if (mb & SDL_BUTTON_MMASK) pad->setButton(Pad::BT_PU, true);
        if (mouseWheel < 0) pad->setButton(Pad::BT_PD, true);
        //if (mb & SDL_BUTTON_X1MASK) pad->setButton(cPad::BT_CROSS, true);
        //if (mb & SDL_BUTTON_X2MASK) pad->setButton(cPad::BT_CIRCLE, true);
        if (mb & SDL_BUTTON_RMASK) pad->setButton(Pad::BT_L2, true);
        if (mb & SDL_BUTTON_LMASK) pad->setButton(Pad::BT_R2, true);

    }
    
    assert(sizeof(keystate_) == 512);
    memcpy(keystate_, keystate, sizeof(keystate_));
}


int GameMain::alEnableSystem(bool en) {
    static ALCdevice *dev = NULL;
    static ALCcontext *ctx = NULL;
    static bool isenabled = false;

    if (en && !isenabled) {
        logger->info() << "Enabling OpenAL\n";
        dev = alcOpenDevice(NULL);
        if (!dev) {
            logger->error() << "Could not open OpenAL device.\n";
            return 1;
        }
        ctx = alcCreateContext(dev, NULL);
        alcMakeContextCurrent(ctx);
        if (!ctx) {
            logger->error() << "Could not make OpenAL context current.\n";
            return 1;
        }
        isenabled = true;
        return 0;
    } else if (!en && isenabled) {
        logger->debug() << "Disabling OpenAL\n";
        alcMakeContextCurrent(NULL);
        alcDestroyContext(ctx);
        alcCloseDevice(dev);
        isenabled = false;
        return 0;
    } else {
        return 1;
    }
}


void GameMain::drawPlaque() {
    GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GL::glDisable(GL_LIGHTING);
        GL::glDisable(GL_FOG);
        GL::glDisable(GL_DEPTH_TEST);
        GL::glDisable(GL_CULL_FACE);
        GLS::glPushOrthoProjection();
        {
            GL::glPushMatrix();
            {
                GL::glLoadIdentity();
                GL::glTranslatef(0, 1, 0);
                GL::glScalef(1.0f / 80.0f, 1.0f / 40.0f, 1.0f);
                
                GL::glDisable(GL_TEXTURE_2D);
                GL::glBegin(GL_QUADS);
                {
                    GL::glColor4f(0.6, 0.6, 0.6, 1);
                    GL::glVertex3f(0,0,0);
                    GL::glVertex3f(80,0,0);
                    GL::glVertex3f(80,-1,0);
                    GL::glVertex3f(0,-1,0);

                    GL::glColor4f(0.8, 0.8, 0.8, 0.9);
                    GL::glVertex3f(0,-1,0);
                    GL::glColor4f(0.5, 0.5, 0.5, 0.6);
                    GL::glVertex3f(80,-1,0);
                    GL::glColor4f(0.5, 0.5, 0.5, 0.3);
                    GL::glVertex3f(80,-39,0);
                    GL::glColor4f(0.5, 0.5, 0.5, 0.6);
                    GL::glVertex3f(0,-39,0);

                    GL::glColor4f(0.6, 0.6, 0.6, 1);
                    GL::glVertex3f(0,-39,0);
                    GL::glVertex3f(80,-39,0);
                    GL::glVertex3f(80,-40,0);
                    GL::glVertex3f(0,-40,0);
                }
                GL::glEnd();
                
                GL::glColor4f(1, 1, 0, 1);
                GLF::glprint("LinWarrior 3D  (Build " __DATE__ ") by hackcraft.de");
                
                GL::glColor4f(0, 1, 0, 1);
                GL::glTranslatef(0, -34, 0);
                GLF::glprint("IJKL-Keys   : Aim Weapons");
                GL::glTranslatef(0, -1, 0);
                GLF::glprint("Cursor-Keys : Steer Base");
                GL::glTranslatef(0, -1, 0);
                GLF::glprint("SEDF-Keys   : Main Action Buttons");
                GL::glTranslatef(0, -1, 0);
                GLF::glprint("AW-/RG-Keys : Left/Right Action Buttons");
                GL::glTranslatef(0, -1, 0);
                GL::glTranslatef(0, -1, 0);
                GL::glColor4f(1, 1, 0, 1);
                GLF::glprint("PROCESSING DATA...this may take a while...");
            }
            GL::glPopMatrix();
        }
        GLS::glPopProjection();
    }
    GL::glPopAttrib();
}


void GameMain::updateLog() {
    //cout << "Redirecting text output.\n";
    if (!oss.str().empty()) {
        log->write(oss.str().c_str());
        printf("%s", oss.str().c_str());
        oss.clear();
        oss.str("");
        fflush(stdout);
    }
}


int GameMain::init(int argc, char** args) {
    
    //JobBGM* job_bgm = new JobBGM();
    //threadpool->addJob(job_output);
    
    //JobRender* job_render = new JobRender();
    //threadpool->addJob(job_output);

    // Remember original stdout/cout stream and redirect cout if enabled.
    stdout_ = std::cout.rdbuf();
    bool redirectOutput = true;
    if (redirectOutput) {
        JobOutput* job_output = new JobOutput();
        threadpool->addJob(job_output);
        std::cout.rdbuf( oss.rdbuf() );
    }

    std::cout << "LinWarrior 3D  (Build " __DATE__ ") by hackcraft.de" << "\n";

    logger->info() << "Reading Commandline Arguments...\n";
    if (config->parseArgs(argc, args)) {
        config->printHelp();
        return 1;
    }

    logger->info() << "Initializing SDL Video- and -Input-Subsystems...\n";
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        logger->error() << "Unable to init SDL: " << SDL_GetError() << "\n";
        return 1;
    }

    logger->info() << "Initializing Video-Mode and -Parameters...\n";
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if (config->multisamples) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config->multisamples);
    }
    SDL_Surface* screen = NULL;
    if (config->fullscreen) screen = SDL_SetVideoMode(config->width, config->height, 0, SDL_OPENGLBLIT | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    else screen = SDL_SetVideoMode(config->width, config->height, 0, SDL_OPENGLBLIT | SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        logger->error() << "Unable to set " << config->width << " x " << config->height << "video: " << SDL_GetError() << "\n";
        return 1;
    }

    logger->info() << "Initialising GLEW...\n";
    if (GL::glewInit() != GLEW_OK) {
        logger->error() << "Unable to init GLEW!\n";
        return 1;
    }

    logger->info() << "Setting Custom Window-Title...\n";
    char* title;
    char* icon;
    SDL_WM_GetCaption(&title, &icon);
    SDL_WM_SetCaption("LinWarrior 3D  (Build " __DATE__ ") by hackcraft.de", icon);

    logger->info() << "Initializing OpenAL audio...\n";
    if (alEnableSystem(true)) {
        logger->error() << "Could not enable OpenAL.\n";
    }
    //if (!alutInit(&argc, args)) {
    if (!alutInitWithoutContext(NULL, NULL)) {
        logger->error() << "Unable to init ALUT: " << alGetError() << "\n";
        // Maybe just let it run without sound?
        // Hope for no followup errors.
        //return 1;
    }
    logger->debug() << "ALUT supported file formats: " << alutGetMIMETypes(ALUT_LOADER_BUFFER) << "\n";
    logger->debug() << "ALUT supported memory formats: " << alutGetMIMETypes(ALUT_LOADER_MEMORY) << "\n";
    alGetError();
    //alDopplerFactor(2.0);
    //alSpeedOfSound(343.3*0.1);

    if (0) {
        logger->info() << "Loading Ambient Background Music...\n";
        try {
            ALuint buffer;
            ALuint source;

            //buffer = alutCreateBufferHelloWorld();
            buffer = alutCreateBufferFromFile(config->bgm.c_str());
            if (buffer == AL_NONE) throw "Could not create buffer from file for background music.";
            alGenSources(1, &source);
            if (alGetError() != AL_NO_ERROR) throw "Could not generate background music source.";
            alSourcei(source, AL_BUFFER, buffer);
            alSourcef(source, AL_PITCH, 1.0f);
            alSourcef(source, AL_GAIN, 1.0f);

            float zero[] = {0.0f, 0.0f, 0.0f};
            alSourcefv(source, AL_POSITION, zero);
            alSourcefv(source, AL_VELOCITY, zero);
            alSourcei(source, AL_LOOPING, AL_TRUE);
            alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
            alSourcePlay(source);
        } catch (const char* s) {
            logger->error() << "Sorry no bgm possible.\n" << s << "\n";
        }
    }

    logger->info() << "Initialising OpenGL Settings...\n";
    initGL(screen->w, screen->h);


    logger->info() << "Initialising Input Settings...\n";
    SDL_ShowCursor(SDL_DISABLE);
    joy0 = (void*) SDL_JoystickOpen(0);

    logger->info() << "Installing Cleanup Hook...\n";
    atexit(cleanup);

    logger->info() << "Breeding Minions...\n";
    int maxminions = 3;
    threadpool->addThreads(maxminions);

    bool loadscreen = true;
    if (loadscreen) {
        logger->info() << "Showing load screen...\n";
        GL::glClearColor(0.4, 0.4, 0.45, 1.0);
        GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawPlaque();
        SDL_GL_SwapBuffers();
    }

    logger->info() << "Initialising Mission...\n";
    initMission();

    // Signal state change.
    //job_render->state = 1;
    //Thread::sleep(200);
    
    return 0;
}


int GameMain::run(int argc, char** args) {
    
    int err = init(argc, args);
    if (err != 0) {
        return err;
    }

    logger->info() << "Entering Mainloop...\n";
    
    bool done = false;
    unsigned long start_ms = SDL_GetTicks();
    double spf = 1.0 / (double) config->fps;
    
    while (!done) {
        //logger->trace() << "loop\n";

        // message processing loop
        mouseWheel = 0;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // check for messages
            switch (event.type) {
                    // exit if the window is closed
                case SDL_QUIT:
                {
                    done = true;
                    break;
                }
                    // check for keypresses
                case SDL_KEYDOWN:
                {
                    // Keymapping for virtual gamepad.
                    updateKey(event.key.keysym.sym);
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE) done = true;
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    if (event.button.button == SDL_BUTTON_WHEELUP) {
                        mouseWheel++;
                    } else if (event.button.button == SDL_BUTTON_WHEELDOWN) {
                        mouseWheel--;
                    }
                    break;
                }

            } // switch
        } // while poll event

        // Possibly print out info on pushed buttons.
        if (config->printpad) loopi(32) {
            if (SDL_JoystickGetButton((SDL_Joystick*) joy0, i)) logger->trace() << "Button" << i << "\n";
        }
        // Transfer real Joystick-/Gamepad-Input into the virtual gamepad
        // of the player by using a (re-)mapping.
        updatePad(pad1, (SDL_Joystick*) joy0, map1);
        // After securing Matrix: update world and draw frame.
        //GL::glPushMatrix();
        {
            //cout << "[\n";
            updateFrame(spf * 1000.0f);
            drawFrame();
            //cout << "]\n";
        }
        //GL::glPopMatrix();

        // Lock framerate and give back idle time to the os,
        // other processes or threads.
        {
            spf = (1.0f / config->fps);
            // msec since frame started (end of last frame).
            long delta_ms = SDL_GetTicks() - start_ms;
            // Necessary delay to lock frame to that frame rate.
            const unsigned long frame_ms = (long) (spf * 1000);
            long delay_ms = frame_ms - delta_ms;
            if (delay_ms > 0) {
                // Wake me up *before* time is up
                const long min_delay_ms = 3;
                if (delay_ms >= min_delay_ms) { // Some can only sleep >= x ms.
                    long os_delay_ms = delay_ms - min_delay_ms;
                    Thread::sleep(os_delay_ms);
                    //logger->trace() << os_delay_ms << "\n";
                }
                // debug timing
                //logger->trace() << "framebudget " << frame_ms << "ms of which " << frame_ms - delay_ms << "ms were used, remaining to waste " << delay_ms << "ms." << "\n";
                //logger->trace() << "framebudget " << frame_ms << "ms - " << frame_ms - delay_ms << "ms = " << delay_ms << "ms" << "\n";
                // Syncing in the last msecs by hand:
                int c = 0;
                while (true) {
                    if (SDL_GetTicks() - start_ms >= frame_ms) break;
                    c++;
                }
            }
        }
        start_ms = SDL_GetTicks();

        //glFinish();
        SDL_GL_SwapBuffers();

    } // end main loop

    deinit();

    return 0;
}


void GameMain::deinit() {
    
    logger->info() << "Shutting Down...\n";

    threadpool->shutdown();
    delete threadpool;
    
    std::cout.rdbuf( stdout_ );
    //console.print();
}