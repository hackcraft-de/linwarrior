// main.cpp

#include "main.h"

#include "psi3d/snippetsgl.h"
#include "psi3d/instfont.h"

// To dis-/enable zone drawing.
#include "cAlert.h"

#include <iostream>

#include <SDL/SDL_timer.h>
#include <AL/alc.h>
#ifndef __WIN32
#include <AL/alext.h>
#endif
#include <AL/alut.h>


cGame cMain::game;
int cMain::mouseWheel = 0;

unsigned int* gInstantfont = NULL;
DEFINE_glprintf

cGame::cGame()
: pad1(NULL),
map1(map_zedwise),

camera(NULL),

world(NULL),

mission(0),
bgm("data/freesound/ambient.wav"),

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

void cGame::printHelp() {
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

int cGame::parseArgs(int argc, char** args) {
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

void cGame::initMission() {
    this->world = new cWorld();
    assert(this->world != NULL);
    if (this->mission == 1) {
        this->world->mMission = new cEmptyMission();
        assert(this->world->mMission != NULL);
        this->camera = this->world->mMission->init(this->world);
        this->pad1 = this->camera->controlled->pad;
    } else {
        this->world->mMission = new cOpenMission();
        assert(this->world->mMission != NULL);
        this->camera = this->world->mMission->init(this->world);
        this->pad1 = this->camera->controlled->pad;
    }
    assert(this->camera != NULL);
    assert(this->pad1 != NULL);
}

void cMain::initGL(int width, int height) {    
#ifdef __WIN32
    glTexImage3D = SDL_GL_GetProcAddress("glTexImage3D");
    assert(glTexImage3D != NULL);
#endif

    if (true) {
        std::string glinfo;
        glinfo = (const char*) glGetString(GL_RENDERER);
        std::cout << glinfo << std::endl << std::endl;
        glinfo = (const char*) glGetString(GL_VENDOR);
        std::cout << glinfo << std::endl << std::endl;
        glinfo = (const char*) glGetString(GL_VERSION);
        std::cout << glinfo << std::endl << std::endl;
        glinfo = (const char*) glGetString(GL_EXTENSIONS);
        std::cout << glinfo << std::endl << std::endl;
        if (glinfo.find("GL_EXT_texture3D", 0) == std::string::npos) {
            cObject::ENABLE_TEXTURE_3D = 0;
        }
    }

    glViewport(0, 0, width, height);

    float fogColor[4] = {1.0, 1.0, 1.0, 1.0};
    //float fogDensity = 0.003f; // original
    float fogDensity = 0.006f;

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_POLYGON_SMOOTH);


    glClearAccum(0.0, 0.0, 0.0, 0.0);
    glClearColor(0.0, 0.125, 0.0, 0.0);
    //glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LEQUAL);

    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, fogDensity);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Lighting
    {
        glEnable(GL_LIGHTING);
        float p[] = {0, 500, 0, 1};
        float a[] = {0.95, 0.5, 0.5, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, p);
        glLightfv(GL_LIGHT0, GL_AMBIENT, a);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, a);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00001);
        glEnable(GL_COLOR_MATERIAL);
        //glEnable(GL_NORMALIZE);
        glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    }

    // Init instant console font.
    glUploadFont();
}

void cMain::drawFrame(int elapsed_msec) {
    //std::cout << "elapsed: " << (elapsed_msec / 1000.0f) << std::endl;
    //if (hurlmotion) elapsed = int(elapsed * 1.0f);

    static bool picking = false;
    const int SELECTIONSIZE = 1024;
    GLuint selection[SELECTIONSIZE];
    if (picking) {
        // With Projection:
        //glGetIntegerv(GL_VIEWPORT, viewport);
        //gluPickMatrix(cx, viewport[3]-cy, 5, 5, viewport);
        //gluPerspective(45,ratio,0.1,1000);
        glSelectBuffer(SELECTIONSIZE,selection);
        glRenderMode(GL_SELECT);
        glInitNames();
        // With Modelview:
    } else {
        glRenderMode(GL_RENDER);
    }

    if (!game.paused) {
        // Re-Cluster spatial index.
        game.world->clusterObjects();

        int subframes = 1;

        loopi(subframes) {
            // calculate the time that the last frame took.
            game.world->advanceTime(elapsed_msec / subframes);

            // Deliver overdue messages to objects and groups of objects.
            game.world->dispatchMessages();

            // Calculate new animation state and the like (if not paused).
            game.world->animateObjects();

            // Calculate positions through transformations.
            glPushMatrix();
            {
                glLoadIdentity();
                game.world->transformObjects();
            }
            glPopMatrix();
        }
    } else {
        // Delete Fragged Objects of previous frames.
        game.world->bagFragged();
    }

    // Draw area zones while paused (debug/editmode kind of).
    cAlert::sDrawzone = game.paused;

    // Draw fewer frames? Only every n'th frame.
    int nthframe = 1;
    static int cnt = 0;
    cnt++;
    if (cnt % nthframe != 0) return;

    // SOLID / WIREFRAME drawing scheme.
    if (!game.wireframe) {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glEnable(GL_FOG);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);
        glClear(GL_DEPTH_BUFFER_BIT); // With Background.
    } else {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glEnable(GL_FOG);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // W/o background.
    }


    SGL::glPushPerspectiveProjection(game.fov);
    {

        // Setup camera.
        glLoadIdentity();
        if (game.camera) {
            game.camera->multEyeMatrix();
            game.camera->setAsAudioListener();
        }

        // Find objects in visible range.
        float* origin = game.camera->traceable->pos.data();
        float maxrange = game.world->getViewdistance();
        float min[] = {origin[0] - maxrange, origin[2] - maxrange};
        float max[] = {origin[0] + maxrange, origin[2] + maxrange};
        std::list<cObject*>* visobjects = game.world->getGeoInterval(min, max, true);
        assert(visobjects != NULL);
        //cout << "vis:" << objects->size() << " vs " << mObjects.size() << endl;

        // Draw the surounding Sky- and Ground-Dome/Box
        game.world->drawBack();

        // Draw the Objects themselves.
        game.world->drawSolid(game.camera, visobjects);

        // Draw the Object's translucent effects.
        game.world->drawEffect(game.camera, visobjects);

        // Delete list of visible objects.
        delete visobjects;

        // Draw the Head-Up-Display of the currently spectating Object.
        game.camera->drawHUD();

    }
    SGL::glPopProjection();

    //glFlush();

    float motionblur = 0.2f;
    if (game.nightvision) {
        SGL::glAccumBlurInverse(0.83f);
    } else if (motionblur > 0) {
        SGL::glAccumBlur(motionblur);
    }

    if (picking) {
        int entries = glRenderMode(GL_RENDER);
        cout << "selected: " << entries << endl;
        // Picking-Entry: n, minz, maxz, n_names
        GLuint* p = selection;
        loopi(entries) {
            GLuint n = *p++;
            GLuint minz = *p++;
            GLuint maxz = *p++;
            cout << n << " " << minz << " " << maxz;
            loopj(n) {
                GLuint name = *p++;
                cout << " " << name;
            }
            cout << "\n";
        }
    }
    //picking = !picking;
}

void cMain::updateKey(Uint8 keysym) {
    if (keysym == _TEXTURE_KEY) {
        cObject::ENABLE_TEXTURE_3D = !cObject::ENABLE_TEXTURE_3D;
    } else if (keysym == _WIREFRAME_KEY) {
        game.wireframe = !game.wireframe;
    } else if (keysym == _NIGHTVISION_KEY) {
        game.nightvision = !game.nightvision;
    } else if (keysym == _PAUSE_KEY) {
        game.paused = !game.paused;
    } else {
        if (keysym == SDLK_1) cWorld::instance->setViewdistance(100);
        else if (keysym == SDLK_2) cWorld::instance->setViewdistance(150);
        else if (keysym == SDLK_3) cWorld::instance->setViewdistance(200);
        else if (keysym == SDLK_4) cWorld::instance->setViewdistance(250);
        else if (keysym == SDLK_5) cWorld::instance->setViewdistance(300);
        else if (keysym == SDLK_6) cWorld::instance->setViewdistance(350);
        else if (keysym == SDLK_7) cWorld::instance->setViewdistance(400);
        else if (keysym == SDLK_8) cWorld::instance->setViewdistance(450);
        else if (keysym == SDLK_9) cWorld::instance->setViewdistance(500);
        else if (keysym == SDLK_0) cWorld::instance->setViewdistance(5000);
    }
}

void cMain::updatePad(cPad* pad, SDL_Joystick* joy, int* mapping) {
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
            pad->setAxis((cPad::Axes) i, SDL_JoystickGetAxis(joy, i) / 32000.0f);
        }

        loopi(32) {
            if (SDL_JoystickGetButton(joy, map[i])) pad->setButton((cPad::Buttons)i, true);
        }
        //pad->print();
    }

    Uint8 *keystate = SDL_GetKeyState(NULL);
    if (keystate == NULL) return;
    // First Analogue Control Stick
    if (keystate[SDLK_LEFT] == 1) pad->setAxis(cPad::AX_LR1, -1.0f);
    if (keystate[SDLK_RIGHT] == 1) pad->setAxis(cPad::AX_LR1, +1.0f);
    if (keystate[SDLK_UP] == 1) pad->setAxis(cPad::AX_UD1, -1.0f);
    if (keystate[SDLK_DOWN] == 1) pad->setAxis(cPad::AX_UD1, +1.0f);
    // Start and Select
    if (keystate[SDLK_RETURN] == 1) pad->setButton(cPad::BT_START, true);
    if (keystate[SDLK_BACKSPACE] == 1) pad->setButton(cPad::BT_SELECT, true);
    // Lx and Rx Buttons
    if (keystate[SDLK_w] == 1) pad->setButton(cPad::BT_L1, true);
    if (keystate[SDLK_a] == 1) pad->setButton(cPad::BT_L2, true);
    if (keystate[SDLK_r] == 1) pad->setButton(cPad::BT_R1, true);
    if (keystate[SDLK_g] == 1) pad->setButton(cPad::BT_R2, true);
    // Primary Buttons
    if (keystate[SDLK_s] == 1) pad->setButton(cPad::BT_PL, true);
    if (keystate[SDLK_e] == 1) pad->setButton(cPad::BT_PU, true);
    if (keystate[SDLK_d] == 1) pad->setButton(cPad::BT_PD, true);
    if (keystate[SDLK_f] == 1) pad->setButton(cPad::BT_PR, true);
    // Second Analogue Stick or Coolie-Hat:
    if (keystate[SDLK_LALT] == 0) {
        if (keystate[SDLK_j]) pad->setAxis(cPad::AX_LR2, -1.0f);
        if (keystate[SDLK_l]) pad->setAxis(cPad::AX_LR2, +1.0f);
        if (keystate[SDLK_i]) pad->setAxis(cPad::AX_UD2, -1.0f);
        if (keystate[SDLK_k]) pad->setAxis(cPad::AX_UD2, +1.0f);
    } else {
        if (keystate[SDLK_j]) pad->setButton(cPad::BT_HL, true);
        if (keystate[SDLK_l]) pad->setButton(cPad::BT_HR, true);
        if (keystate[SDLK_i]) pad->setButton(cPad::BT_HU, true);
        if (keystate[SDLK_k]) pad->setButton(cPad::BT_HD, true);
    };
    // Stick Buttons
    if (keystate[SDLK_PERIOD]) pad->setButton(cPad::BT_J1B, true);
    if (keystate[SDLK_COMMA]) pad->setButton(cPad::BT_J2B, true);


    // Optional Mouse Input.
    if (game.mouseInput) {
        int mx = 0;
        int my = 0;
        int w2 = game.width>>1;
        int h2 = game.height>>1;
        unsigned char mb = SDL_GetRelativeMouseState(&mx, &my);
        SDL_WarpMouse(w2, h2);
        SDL_PumpEvents();
        int mx_ = 0;
        int my_ = 0;
        SDL_GetRelativeMouseState(&mx_, &my_);
        //cout << (mx/(float)w2) << " " << (my/(float)h2) << " " << ((int)mb) << "\n";

        pad->setAxis(cPad::AX_LR2, pad->getAxis(cPad::AX_LR2) + 10.0f*mx/(float)w2);
        pad->setAxis(cPad::AX_UD2, pad->getAxis(cPad::AX_UD2) - 10.0f*my/(float)h2);
        
        if (mouseWheel > 0) pad->setButton(cPad::BT_PL, true);
        if (mb & SDL_BUTTON_MMASK) pad->setButton(cPad::BT_PU, true);
        if (mouseWheel < 0) pad->setButton(cPad::BT_PD, true);
        //if (mb & SDL_BUTTON_X1MASK) pad->setButton(cPad::BT_CROSS, true);
        //if (mb & SDL_BUTTON_X2MASK) pad->setButton(cPad::BT_CIRCLE, true);
        if (mb & SDL_BUTTON_RMASK) pad->setButton(cPad::BT_L2, true);
        if (mb & SDL_BUTTON_LMASK) pad->setButton(cPad::BT_R2, true);

    }
}

int alEnableSystem(bool en) {
    static ALCdevice *dev = NULL;
    static ALCcontext *ctx = NULL;
    static bool isenabled = false;

    if (en && !isenabled) {
        cout << "Enabling OpenAL\n";
        dev = alcOpenDevice(NULL);
        if (!dev) {
            //fprintf(stderr, "Could not open OpenAL device.\n");
            cout << "Could not open OpenAL device.\n";
            return 1;
        }
        ctx = alcCreateContext(dev, NULL);
        alcMakeContextCurrent(ctx);
        if (!ctx) {
            //fprintf(stderr, "Could not make OpenAL context current.\n");
            cout << "Could not make OpenAL context current.\n";
            return 1;
        }
        isenabled = true;
        return 0;
    } else if (!en && isenabled) {
        cout << "Disabling OpenAL\n";
        alcMakeContextCurrent(NULL);
        alcDestroyContext(ctx);
        alcCloseDevice(dev);
        isenabled = false;
        return 0;
    } else {
        return 1;
    }
}

void cleanup() {
    SDL_Quit();
    alutExit();
    alEnableSystem(false);
    std::cout << "Thank you for playing.\n";
}


#include <SDL/SDL_thread.h>

SDL_mutex* jobsmutex;
std::queue<int (*)(void*)> jobs;

int minion(void* data) {
    unsigned int id = SDL_ThreadID();
    cout << "Minion " << id << " at your service!\n";
    int (*job)(void*) = minion;
    bool done = false;
    while(!done) {
        // Grab a new job.
        int (*nextjob)(void*) = NULL;
        SDL_mutexP(jobsmutex);
        {
            if (!jobs.empty()) {
                nextjob = jobs.front();
                jobs.pop();
            } // else look at secondary jobs.
        }
        SDL_mutexV(jobsmutex);
        // Work on job if any available.
        if (nextjob != NULL) {
            cout << "Minion " << id << " is fulfilling your wish!\n";
            job = nextjob;
            int result = job(NULL);
            cout << "Minion " << id << " job is finished with result " << result << "!\n";
        } else if (job != NULL && nextjob == NULL) {
            // Wait for a job to do.
            cout << "Minion " << id << " is awaiting your command!\n";
            job = nextjob;
            SDL_Delay(10);
        } else {
            SDL_Delay(10);
        }
    }
    return 0;
}

int job_render(void* data) {
    while(true) {
        //cout << "Buffering BGM.\n";
        SDL_Delay(1.0f/DEFAULT_FPS * 1000);
    }
    return 0;
}

int job_bgm(void* data) {
    while(true) {
        //cout << "Buffering BGM.\n";
        SDL_Delay(1000);
    }
    return 0;
}

std::stringstream oss;

int job_output(void* data) {
    while(true) {
        //cout << "Redirecting text output.\n";
        if (!oss.str().empty()) {
            printf("%s", oss.str().c_str());
            oss.clear();
            oss.str("");
            fflush(stdout);
        }
        SDL_Delay(200);
    }
    return 0;
}

int cMain::sdlmain(int argc, char** args) {
    jobs.push(job_bgm);
    jobs.push(job_output);
    jobs.push(job_render);
    std::cout.rdbuf( oss.rdbuf() );

    std::cout << "LinWarrior 3D  (Build " __DATE__ ") by hackcraft.de" << std::endl << std::endl;

    cout << "Reading Commandline Arguments...\n";
    if (game.parseArgs(argc, args)) {
        game.printHelp();
        return 1;
    }
    double spf = 1.0 / (double) game.fps;

    cout << "Initializing SDL Video- and -Input-Subsystems...\n";
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        std::cout << "Unable to init SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    cout << "Initializing Video-Mode and -Parameters...\n";
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if (game.multisamples) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, game.multisamples);
    }
    SDL_Surface* screen = NULL;
    if (game.fullscreen) screen = SDL_SetVideoMode(game.width, game.height, 0, SDL_OPENGLBLIT | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    else screen = SDL_SetVideoMode(game.width, game.height, 0, SDL_OPENGLBLIT | SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        cout << "Unable to set " << game.width << " x " << game.height << "video: " << SDL_GetError() << "\n";
        return 1;
    }

    cout << "Initialising GLEW...\n";
    if (glewInit() != GLEW_OK) {
        cout << "Unable to init GLEW!\n";
        return 1;
    }

    cout << "Setting Custom Window-Title...\n";
    char* title;
    char* icon;
    SDL_WM_GetCaption(&title, &icon);
    SDL_WM_SetCaption("LinWarrior 3D  (Build " __DATE__ ") by hackcraft.de", icon);

    cout << "Initializing OpenAL audio...\n";
    if (alEnableSystem(true)) {
        cout << "Could not enable OpenAL.\n";
    }
    //if (!alutInit(&argc, args)) {
    if (!alutInitWithoutContext(NULL, NULL)) {
        std::cout << "Unable to init ALUT: " << alGetError() << std::endl;
        // Maybe just let it run without sound?
        // Hope for no followup errors.
        //return 1;
    }
    cout << "ALUT supported file formats: " << alutGetMIMETypes(ALUT_LOADER_BUFFER) << "\n";
    cout << "ALUT supported memory formats: " << alutGetMIMETypes(ALUT_LOADER_MEMORY) << "\n";
    alGetError();
    //alDopplerFactor(2.0);
    //alSpeedOfSound(343.3*0.1);

    if (0) {
        cout << "Loading Ambient Background Music...\n";
        try {
            ALuint buffer;
            ALuint source;

            //buffer = alutCreateBufferHelloWorld();
            buffer = alutCreateBufferFromFile(game.bgm.c_str());
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
            cout << "Sorry no bgm possible.\n" << s << "\n";
        }
    }

    cout << "Initialising OpenGL Settings...\n";
    initGL(screen->w, screen->h);


    cout << "Initialising Input Settings...\n";
    SDL_ShowCursor(SDL_DISABLE);
    SDL_Joystick* joy0 = SDL_JoystickOpen(0);

    cout << "Installing Cleanup Hook...\n";
    atexit(cleanup);

    cout << "Breeding Minions...\n";
    int maxminions = 3;
    SDL_Thread* minions[maxminions];
    loopi(maxminions) {
        minions[i] = SDL_CreateThread (minion, &i);
    }

    bool loadscreen = true;
    if (loadscreen) {
        cout << "Showing load screen...\n";
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            glClearColor(0.1, 0.1, 0.9, 1.0);
            glDisable(GL_LIGHTING);
            glDisable(GL_FOG);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            SGL::glPushOrthoProjection();
            {
                glPushMatrix();
                {
                    glLoadIdentity();
                    glTranslatef(0,1,0);
                    glScalef(1.0f/80.0f, 1.0f/40.0f, 1.0f);
                    glColor4f(1,1,0,1);
                    glprintf("LinWarrior 3D  (Build " __DATE__ ") by hackcraft.de");
                    glColor4f(0,1,0,1);
                    glTranslatef(0,-34,0);
                    glprintf("IJKL-Keys   : Aim Weapons");
                    glTranslatef(0,-1,0);
                    glprintf("Cursor-Keys : Steer Base");
                    glTranslatef(0,-1,0);
                    glprintf("SEDF-Keys   : Main Action Buttons");
                    glTranslatef(0,-1,0);
                    glprintf("AW-/RG-Keys : Left/Right Action Buttons");
                    glTranslatef(0,-1,0);
                    glTranslatef(0,-1,0);
                    glColor4f(1,1,0,1);
                    glprintf("PROCESSING DATA...this may take a while...");
                }
                glPopMatrix();
            }
            SGL::glPopProjection();
        }
        glPopAttrib();
        SDL_GL_SwapBuffers();
    }

    cout << "Initialising Mission...\n";
    game.initMission();

    cout << "Entering Mainloop...\n";
    bool done = false;
    unsigned long start_ms = SDL_GetTicks();
    while (!done) {
        //cout << "loop\n";

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
                    if(event.button.button == SDL_BUTTON_WHEELUP) {
                        mouseWheel++;
                    } else if(event.button.button == SDL_BUTTON_WHEELDOWN) {
                        mouseWheel--;
                    }
                    break;
                }

            } // switch
        } // while poll event

        // Possibly print out info on pushed buttons.
        if (game.printpad) loopi(32) {
            if (SDL_JoystickGetButton(joy0, i)) std::cout << "Button" << i << std::endl;
        }
        // Transfer real Joystick-/Gamepad-Input into the virtual gamepad
        // of the player by using a (re-)mapping.
        updatePad(game.pad1, joy0, game.map1);
        // After securing Matrix: update world and draw frame.
        //glPushMatrix();
        {
            //cout << "[\n";
            drawFrame(spf * 1000.0f);
            //cout << "]\n";
        }
        //glPopMatrix();

        // Lock framerate and give back idle time to the os,
        // other processes or threads.
        {
            spf = (1.0f / game.fps);
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
                    alutSleep(os_delay_ms * 0.001);
                    //SDL_Delay(os_delay_ms);
                    //std::cout << os_delay_ms << std::endl;
                }
                // debug timing
                //std::cout << "framebudget " << frame_ms << "ms of which " << frame_ms - delay_ms << "ms were used, remaining to waste " << delay_ms << "ms." << std::endl;
                //std::cout << "framebudget " << frame_ms << "ms - " << frame_ms - delay_ms << "ms = " << delay_ms << "ms" << std::endl;
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

    cout << "Shutting Down...\n";

    // Better set global signal and SDL_WaitThread.
    loopi(maxminions) {
        SDL_KillThread(minions[i]);
    }

    return 0;
}


//
// GOD begins below!
//

#ifdef wonly_WIN32

extern "C" int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow) {
    int argc = 2;
    char *args[] = {"Nothing", "Nothing", "Nothing"};
#else

int main(int argc, char **args) {
#endif
    try {
        return cMain::sdlmain(argc, args);
    } catch (char* s) {
        cout << "Fatal exception caught:\n" << s << endl;
    } catch (const char* s) {
        cout << "Fatal exception caught:\n" << s << endl;
    }
}

