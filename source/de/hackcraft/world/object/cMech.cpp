// cMech.cpp

#include "cMech.h"

#include "de/hackcraft/io/Pad.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/util/Propmap.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/camera/CameraSystem.h"
#include "de/hackcraft/world/sub/camera/rCamera.h"

#include "de/hackcraft/world/sub/chat/ChatSystem.h"
#include "de/hackcraft/world/sub/chat/rChatMember.h"

#include "de/hackcraft/world/sub/computer/ComputerSystem.h"
#include "de/hackcraft/world/sub/computer/rComcom.h"
#include "de/hackcraft/world/sub/computer/rController.h"
#include "de/hackcraft/world/sub/computer/rForcom.h"
#include "de/hackcraft/world/sub/computer/rNavcom.h"

#include "de/hackcraft/world/sub/misc/MiscSystem.h"
#include "de/hackcraft/world/sub/misc/rLightsource.h"
#include "de/hackcraft/world/sub/misc/rSoundsource.h"

#include "de/hackcraft/world/sub/mobile/MobileSystem.h"
#include "de/hackcraft/world/sub/mobile/rMobile.h"

#include "de/hackcraft/world/sub/model/ModelSystem.h"
#include "de/hackcraft/world/sub/model/rBillboard.h"
#include "de/hackcraft/world/sub/model/rRigged.h"

#include "de/hackcraft/world/sub/physics/PhysicsSystem.h"
#include "de/hackcraft/world/sub/physics/rCollider.h"
#include "de/hackcraft/world/sub/physics/rTraceable.h"

#include "de/hackcraft/world/sub/weapon/WeaponSystem.h"
#include "de/hackcraft/world/sub/weapon/rTarcom.h"
#include "de/hackcraft/world/sub/weapon/rTarget.h"
#include "de/hackcraft/world/sub/weapon/rWeapon.h"
#include "de/hackcraft/world/sub/weapon/rWepcom.h"


#include <cassert>
#include <sstream>
#include <typeinfo>


Logger* cMech::logger = Logger::getLogger("de.hackcraft.world.object.cMech");

int cMech::sInstances = 0;

std::map<int, long> cMech::sTextures;


cMech::cMech(Propmap* props) {
    
    Propmap& cnf = *props;
    
    int n;
    float x, y, z;
    
    logger->debug() << cnf.getProperty("mech.model", "no model") << "\n";
    
    logger->debug() << cnf.getProperty("mech.pos", "no position") << "\n";
    n = sscanf(cnf.getProperty("mech.pos", "0 0 0").c_str(), " ( %f , %f , %f ) ", &x, &y, &z);
    vec3 pos;
    vector_set(pos, x, y, z);
    
    logger->debug() << cnf.getProperty("mech.rot", "no rotation") << "\n";
    n = sscanf(cnf.getProperty("mech.rot", "0 0 0").c_str(), " ( %f , %f , %f ) ", &x, &y, &z);
    vec3 rot;
    vector_set(rot, x, y, z);
    
    init(pos, rot, cnf.getProperty("mech.model", "frogger"));
    
    // Mount weapons
    
    const char* mpts[] = {
        "Center",
        "LTorsor", "RTorsor",
        "LLoArm", "LUpArm",
        "RLoArm", "RUpArm"
    };
    
    for (int i = 0; i < 7; i++) {
        
        std::stringstream sstr;
        sstr << "mech." << mpts[i];
        std::string s = sstr.str();
        
        if (!cnf.contains(s)) continue;
        
        std::string wpn = cnf.getProperty(s, std::string(""));
        
        if (wpn.compare("Plasma") == 0) {
            logger->debug() << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponPlasmagun());
            
        } else if (wpn.compare("Homing") == 0) {
            logger->debug() << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponHoming());
            
        } else if (wpn.compare("Raybeam") == 0) {
            logger->debug() << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponRaybeam());
            
        } else if (wpn.compare("Machinegun") == 0) {
            logger->debug() << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponMachinegun());
            
        } else if (wpn.compare("Explosion") == 0) {
            logger->debug() << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponExplosion());
            
        } else {
            logger->warn() << "No weapon selected for " << mpts[i] << "\n";
        }
    }

    name = cnf.getProperty("mech.name", "unnamed");
    nameable->name = cnf.getProperty("mech.displayname", "???");
    controller->enabled = (cnf.getProperty("mech.ai", "false").compare("true") == 0);
    mobile->immobile = (cnf.getProperty("mech.immobile", "false").compare("true") == 0);
}


cMech::cMech(float* pos, float* rot, std::string modelName) {
    init(pos, rot, modelName);
}


void cMech::init(float* pos, float* rot, std::string modelName) {
    sInstances++;
    if (sInstances == 1) {
        // First one initializes.
        //registerRole(new rMobile((cObject*)NULL), FIELDOFS(mobile), ROLEPTR(cMech::mobile));
        //registerRole(new rRigged((cObject*)NULL), FIELDOFS(rigged), ROLEPTR(cMech::rigged));
        //registerRole(new rComputerised((cObject*)NULL), FIELDOFS(computerised), ROLEPTR(cMech::computerised));
    }

    pad = new Pad;

    collider = new rCollider(this);
    traceable = new rTraceable(this);
    PhysicsSystem::getInstance()->add(collider);
    PhysicsSystem::getInstance()->add(traceable);

    rigged = new rRigged(this);
    nameable = new rBillboard(this);
    ModelSystem::getInstance()->add(rigged);
    ModelSystem::getInstance()->add(nameable);
    
    target = new rTarget(this);
    tarcom = new rTarcom(this);
    wepcom = new rWepcom(this);
    WeaponSystem::getInstance()->add(target);
    WeaponSystem::getInstance()->add(tarcom);
    WeaponSystem::getInstance()->add(wepcom);
    
    controller = new rController(this);
    comcom = new rComcom(this);
    forcom = new rForcom(this);
    navcom = new rNavcom(this);
    ComputerSystem::getInstance()->add(controller);
    ComputerSystem::getInstance()->add(comcom);
    ComputerSystem::getInstance()->add(forcom);
    ComputerSystem::getInstance()->add(navcom);
    
    chatMember = new rChatMember(this);
    ChatSystem::getInstance()->add(chatMember);
    
    camra = new rCamera(this);
    CameraSystem::getInstance()->add(camra);
    
    mobile = new rMobile(this);
    MobileSystem::getInstance()->add(mobile);
    
    soundsource = new rSoundsource(this);
    lightsource = new rLightsource(this);
    MiscSystem::getInstance()->add(soundsource);
    MiscSystem::getInstance()->add(lightsource);
    
    if (rot != NULL) vector_scale(mobile->bse, rot, PI_OVER_180);

    if (pos != NULL) vector_cpy(traceable->pos, pos);
    vector_cpy(traceable->old, traceable->pos);
    float axis[] = {0, 1, 0};
    quat_rotaxis(traceable->ori, mobile->bse[1], axis);

    // Mech Speaker
    if (1) {
        try {
            if (1) {
                soundsource->loadWithWav("data/base/device/pow.wav");
            } else {
                ALuint buffer;
                //buffer = alutCreateBufferHelloWorld();
                buffer = alutCreateBufferFromFile("data/base/device/pow.wav");
                //if (buffer == AL_NONE) throw "could not load pow.wav";
                ALuint* soundsource = &traceable->sound;
                alGenSources(1, soundsource);
                if (alGetError() == AL_NO_ERROR && alIsSource(*soundsource)) {
                    alSourcei(*soundsource, AL_BUFFER, buffer);
                    alSourcef(*soundsource, AL_PITCH, 1.0f);
                    alSourcef(*soundsource, AL_GAIN, 1.0f);
                    alSourcefv(*soundsource, AL_POSITION, traceable->pos);
                    alSourcefv(*soundsource, AL_VELOCITY, traceable->vel);
                    alSourcei(*soundsource, AL_LOOPING, AL_FALSE);
                }
            }
        } catch (...) {
            logger->warn() << "Sorry, no mech sound possible.\n";
        }
    }

    try {                
        rigged->loadModel(rigged->resolveFilename(modelName));
    } catch (const char* s) {
        logger->error() << "CATCHING: " << s << "\n";
        rigged->loadModel(rigged->resolveFilename("frogger"));
    }

    traceable->radius = 1.75f;
    traceable->cwm2 = 0.4f /*very bad cw*/ * traceable->radius * traceable->radius * M_PI /*m2*/;
    traceable->mass = 11000.0f; // TODO mass is qubic to size.
    traceable->mass_inv = 1.0f / traceable->mass;

    vector_cpy(this->pos0, traceable->pos);
    quat_cpy(this->ori0, traceable->ori);
    this->radius = traceable->radius;

    vector_cpy(target->pos0, this->pos0);
    
    vector_cpy(collider->pos0, this->pos0);
    quat_cpy(collider->ori0, this->ori0);
    collider->radius = this->radius;

    explosion = new rWeaponExplosion(this);
    mountWeapon((char*) "CTorsor", explosion, false);

    // Bindings for components

    // COLLIDER
    {
        // from Self
        collider->addBinding(&collider->pos0, &pos0, sizeof(vec3));
        collider->addBinding(&collider->ori0, &ori0, sizeof(quat));
        // from RIGGED
        collider->addBinding(&collider->radius, &rigged->radius, sizeof(float));
        collider->ratio = 0.0f;
        collider->addBinding(&collider->height, &rigged->height, sizeof(float));
    }
    
    // DAMAGEABLE
    {
        // from Self
        target->addBinding(&target->pos0, &pos0, sizeof(vec3));
        // from TARGET
        target->addBinding(&target->active, &target->alife, sizeof(bool));
        // from RIGGED
        target->addBinding(&target->radius, &rigged->radius, sizeof(float));
        target->addBinding(&target->height, &rigged->height, sizeof(float));
    }
    
    // COMCOM
    {
        comcom->addBinding(&comcom->active, &target->alife, sizeof(bool));
    }
    
    // TARCOM
    {
        // from Pad
        //tarcom->switchnext = pad->getButton(Pad::MECH_NEXT_BUTTON);
        //tarcom->switchprev = pad->getButton(Pad::MECH_PREV_BUTTON);
        // from TARGET
        tarcom->addBinding(&tarcom->active, &target->alife, sizeof(bool));
        // from TRACEABLE
        tarcom->addBinding(&tarcom->pos0, &traceable->pos, sizeof(vec3));
        tarcom->addBinding(&tarcom->ori0, &traceable->ori, sizeof(quat));
    }
    
    // WEPCOM
    {
        wepcom->addBinding(&wepcom->active, &target->alife, sizeof(bool));
    }
    
    // FORCOM
    {
        // from TARGET
        forcom->addBinding(&forcom->active, &target->alife, sizeof(bool));
        // from TRACEABLE
        forcom->addBinding(&forcom->ori, &traceable->ori, sizeof(quat));
        // from MOBILE
        forcom->addBinding(&forcom->twr, &mobile->twr, sizeof(vec3));
        // from CAMERA
        forcom->addBinding(&forcom->reticle, &camra->firstperson, sizeof(bool));
    }
    
    // NAVCOM
    {
        // from TARGET
        navcom->addBinding(&navcom->active, &target->alife, sizeof(bool));
        // from TRACEABLE
        navcom->addBinding(&navcom->pos0, &traceable->pos, sizeof(vec3));
        navcom->addBinding(&navcom->ori0, &traceable->ori, sizeof(quat));
    }
    
    // CONTROLLER
    {
        // from TARGET
        controller->addBinding(&controller->active, &target->alife, sizeof(bool));
        controller->addBinding(&controller->disturbedBy, &target->disturber, sizeof(OID));
        // from tarcom
        controller->addBinding(&controller->enemyNearby, &tarcom->nearbyEnemy, sizeof(OID));
        // from Mobile
        controller->addBinding(&controller->aimrange, &mobile->aimrange, sizeof(float));
        controller->addBinding(&controller->walkrange, &mobile->walkrange, sizeof(float));
    }
    
    // TRACEABLE: Rigid Body, Collisions etc.
    {
        // from MOBILE:
        traceable->addBinding(&traceable->ori, &mobile->ori0, sizeof(quat));
        traceable->addBinding(&traceable->jetthrottle, &mobile->jetthrottle, sizeof(float));
        traceable->addBinding(&traceable->throttle, &mobile->drivethrottle, sizeof(float));
    }
    
    // MOBILE
    {
        // from CONTROLLER
        mobile->addBinding(&mobile->aimtarget, &controller->aimtarget, sizeof(OID));
        mobile->addBinding(&mobile->walktarget, &controller->walktarget, sizeof(vec3));
        mobile->addBinding(&mobile->walktargetdist, &controller->walktargetdist, sizeof(float));
        mobile->addBinding(&mobile->firetarget, &controller->firetarget, sizeof(bool));
        // from TARGET
        mobile->addBinding(&mobile->active, &target->alife, sizeof(bool));
        // from traceable
        mobile->addBinding(&mobile->pos0, &traceable->pos, sizeof(vec3));
    }
    
    // RIGGED
    {
        // from MOBILE: Steering state.
        rigged->rotatorsFactors[rigged->YAW][1] = -1;
        rigged->addBinding(&rigged->rotators[rigged->YAW][1], &mobile->twr[1], sizeof(float));
        rigged->addBinding(&rigged->rotators[rigged->PITCH][0], &mobile->twr[0], sizeof(float));
        rigged->addBinding(&rigged->rotators[rigged->HEADPITCH][0], &mobile->twr[0], sizeof(float));
        rigged->addBinding(&rigged->jetting, &mobile->jetthrottle, sizeof(float));
        // from TRACEABLE: Physical movement state.
        rigged->addBinding(&rigged->pos0, &traceable->pos, sizeof(vec3));
        rigged->addBinding(&rigged->ori0, &traceable->ori, sizeof(quat));
        rigged->addBinding(&rigged->vel, &traceable->vel, sizeof(vec3));
        rigged->addBinding(&rigged->grounded, &traceable->grounded, sizeof(float));
    }
    
    // NAMEABLE
    {
        // from RIGGED
        int eye = rigged->jointpoints[rRigged::EYE];
        nameable->addBinding(&nameable->pos0, &rigged->pos0, sizeof(vec3));
        nameable->addBinding(&nameable->ori0, &rigged->ori0, sizeof(quat));
        nameable->addBinding(&nameable->pos1, &rigged->joints[eye].v, sizeof(vec3));
        nameable->addBinding(&nameable->ori1, &rigged->joints[eye].q, sizeof(quat));
        nameable->pos2[1] = 2.0f;
    }
    
    // CAMERA
    {
        // from RIGGED
        int eye = rigged->jointpoints[rRigged::EYE];
        camra->addBinding(&camra->pos0, &rigged->pos0, sizeof(vec3));
        camra->addBinding(&camra->ori0, &rigged->ori0, sizeof(quat));
        camra->addBinding(&camra->pos1, &rigged->joints[eye].v, sizeof(vec3));
        camra->addBinding(&camra->ori1, &rigged->joints[eye].q, sizeof(quat));
        // from MOBILE
        camra->addBinding(&camra->camerashake, &mobile->jetthrottle, sizeof(float));
    }
    
    // SOUNDSOURCE
    {
        // from TRACEABLE
        soundsource->addBinding(&soundsource->pos0, &traceable->pos, sizeof(vec3));
    }
    
    // LIGHTSOURCE
    {
        
    }
}


cMech::~cMech() {
    // FIXME: delete all components.
    if (sInstances == 1) {
        // Last one cleans up

    }
    sInstances--;
}


/*
void cMech::message(Message* message) {
    cout << "I (" << this->oid << ":" << this->name << ") just received: \"" << message->getText() << "\" from sender " << message->getSender() << "\n";
    forcom->message(message);
}
*/


void cMech::spawn() {
    //cout << "cMech::onSpawn()\n";
    if (1) {
        bool isHumanPlayer = target->hasTag(World::getInstance()->getGroup(PLR_HUMAN));
        if (isHumanPlayer) {
            soundsource->play();
        }
        
    } else {
        ALuint soundsource = traceable->sound;
        bool isHumanPlayer = target->hasTag(World::getInstance()->getGroup(PLR_HUMAN));
        bool isSoundEnabled = alIsSource(soundsource);

        if ( isHumanPlayer && isSoundEnabled ) {
            alSourcePlay(soundsource);
        }
    }
    //cout << "Mech spawned " << oid << "\n";
}


void cMech::camera() {
    camra->camera();
}


void cMech::listener() {
    float s = -0.1;
    float step = s * World::getInstance()->getTiming()->getSPF();
    float vel[] = {
        traceable->vel[0] * step,
        traceable->vel[1] * step,
        traceable->vel[2] * step
    };
    float pos[] = {
        traceable->pos[0], traceable->pos[1], traceable->pos[2]
    };
    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_VELOCITY, vel);
    vec3 fwd = {0, 0, +1};
    quat_apply(fwd, rigged->ori0, fwd);
    vec3 uwd = {0, -1, 0};
    quat_apply(uwd, rigged->ori0, uwd);
    float at_and_up[] = {
        fwd[0], fwd[1], fwd[2],
        uwd[0], uwd[1], uwd[2]
    };
    alListenerfv(AL_ORIENTATION, at_and_up);
}


void cMech::mountWeapon(const char* point, rWeapon *weapon, bool add) {    
    if (weapon == NULL) throw "Null weapon for mounting given.";

    weapon->weaponMount = rigged->getMountpoint(point);
    weapon->object = this;
    WeaponSystem::getInstance()->add(weapon);
    
    if (add) {
        weapons.push_back(weapon);
        wepcom->addControlledWeapon(weapon);
    }
    
    {
        // from CONTROLLED:
        weapon->addBinding(&weapon->target, &mobile->aimtarget, sizeof(OID));
        // from RIGGED:
        weapon->addBinding(&weapon->ori0, &traceable->ori, sizeof(quat));
        weapon->addBinding(&weapon->pos0, &traceable->pos, sizeof(vec3));
        weapon->addBinding(&weapon->ori1, &rigged->joints[weapon->weaponMount].q, sizeof(quat));
        weapon->addBinding(&weapon->pos1, &rigged->joints[weapon->weaponMount].v, sizeof(vec3));
    }
}


void cMech::animate(float spf) {
    // Read in.
    //vector_cpy(this->pos0, traceable->pos);
    //quat_cpy(this->ori0, traceable->ori);
    //this->radius = traceable->radius;

    /* Index of Component order
     * ------------------------
     * COLLIDER
     * TARGET
     * COMPUTERs
     * CONTROLLER
     * MOBILE
     * TRACEABLE
     * RIGGED
     * NAMEABLE
     * CAMERA
     * WEAPON
     * EXPLOSION
     * Pad
     */

    // COLLIDER
    {
        // from SELF:
        if (0) {
            vector_cpy(collider->pos0, this->pos0);
            quat_cpy(collider->ori0, this->ori0);
        }
        // from RIGGED:
        if (0) {
            collider->radius = rigged->radius;
            collider->ratio = 0.0f;
            collider->height = rigged->height;
        }
    }

    // TARGET
    {
        // from TARGET itself
        if (0) {
            target->active = target->alife;
        }
        // from RIGGED
        if (0) {
            target->radius = rigged->radius;
            target->height = rigged->height;
        }
    }

    // begin COMPUTERs -->

    // COMCOM
    {
        // from TARGET
        if (0) {
            comcom->active = target->alife;
        }
    }

    // TARCOM
    {
        // from Pad
        {
            tarcom->switchnext = pad->getButton(Pad::MECH_NEXT_BUTTON);
            tarcom->switchprev = pad->getButton(Pad::MECH_PREV_BUTTON);
        }
        // from TARGET
        if (0) {
            tarcom->active = target->alife;
        }
        // from TRACEABLE
        if (0) {
            vector_cpy(tarcom->pos0, traceable->pos);
            quat_cpy(tarcom->ori0, traceable->ori);
        }
    }

    // FORCOM
    {
        // from TARGET
        if (0) {
            forcom->active = target->alife;
        }
        // from traceable
        if (0) {
            quat_cpy(forcom->ori, traceable->ori);
        }
        // from MOBILE
        if (0) {
            vector_cpy(forcom->twr, mobile->twr);
        }
        // from CAMERA
        if (0) {
            forcom->reticle = camra->firstperson;
        }
    }

    // NAVCOM
    {
        // from TARGET
        if (0) {
            navcom->active = target->alife;
        }
        // from TRACEABLE
        if (0) {
            vector_cpy(navcom->pos0, traceable->pos);
            quat_cpy(navcom->ori0, traceable->ori);
        }
    }

    // CONTROLLER
    {
        // from TARGET
        if (0) {
            controller->active = target->alife;
            controller->disturbedBy = target->disturber;
        }
        // from TARCOM
        if (0) {
            controller->enemyNearby = tarcom->nearbyEnemy;
        }
        // from Mobile
        if (0) {
            controller->aimrange = mobile->aimrange;
            controller->walkrange = mobile->walkrange;
        }
    }

    // MOBILE
    {
        // from Pad
        {
            mobile->tower_lr = pad->getAxis(Pad::MECH_TURRET_LR_AXIS);
            mobile->tower_ud = pad->getAxis(Pad::MECH_TURRET_UD_AXIS);
            mobile->chassis_lr = pad->getAxis(Pad::MECH_CHASSIS_LR_AXIS);
            mobile->driveen = pad->getAxis(Pad::MECH_THROTTLE_AXIS);
            mobile->jeten = (pad->getButton(Pad::MECH_JET_BUTTON1) + pad->getButton(Pad::MECH_JET_BUTTON2));
        }
        // from CONTROLLER
        if (0) {
            mobile->aimtarget = controller->aimtarget;
            vector_cpy(mobile->walktarget, controller->walktarget);
            mobile->walktargetdist = controller->walktargetdist;
            mobile->firetarget = controller->firetarget;
        }
        // from TARGET
        if (0) {
            mobile->active = target->alife;
        }
        // from traceable
        if (0) {
            vector_cpy(mobile->pos0, traceable->pos);
        }
    }

    // TRACEABLE: Rigid Body, Collisions etc.
    {
        // from MOBILE:
        if (0) {
            quat_cpy(traceable->ori, mobile->ori0);
            traceable->jetthrottle = mobile->jetthrottle;
            traceable->throttle = mobile->drivethrottle;
        }
    }
    
    // SOUND - FIXME: Add Sound-Subsystem with sound-source component.
    if (0) {
        // from TRACEABLE
        if (traceable->sound != 0) {
            alSourcefv(traceable->sound, AL_POSITION, traceable->pos);
            //alSourcefv(traceable->sound, AL_VELOCITY, traceable->vel);
        }
    }

    // RIGGED
    {
        // from MOBILE: Steering state.
        if (0) {
            rigged->rotators[rigged->YAW][1] = -mobile->twr[1];
            rigged->rotators[rigged->PITCH][0] = mobile->twr[0];
            rigged->rotators[rigged->HEADPITCH][0] = mobile->twr[0];
            rigged->jetting = mobile->jetthrottle;
        }
        // from TRACEABLE: Physical movement state.
        if (0) {
            vector_cpy(rigged->pos0, traceable->pos);
            quat_cpy(rigged->ori0, traceable->ori);
            vector_cpy(rigged->vel, traceable->vel);
            rigged->grounded = traceable->grounded;
        }
        // from TARGET: Fixme tags
        {
            // Group-to-texture.
            int texture = 0;
            texture = target->hasTag(World::getInstance()->getGroup(FAC_RED)) ? 0 : texture;
            texture = target->hasTag(World::getInstance()->getGroup(FAC_BLUE)) ? 1 : texture;
            texture = target->hasTag(World::getInstance()->getGroup(FAC_GREEN)) ? 2 : texture;
            texture = target->hasTag(World::getInstance()->getGroup(FAC_YELLOW)) ? 3 : texture;
            rigged->basetexture3d = texture;
        }
    }

    // NAMEABLE
    {
        // from TARGET
        {
            nameable->color[0] = target->hasTag(World::getInstance()->getGroup(FAC_RED)) ? 1.0f : 0.0f;
            nameable->color[1] = target->hasTag(World::getInstance()->getGroup(FAC_GREEN)) ? 1.0f : 0.0f;
            nameable->color[2] = target->hasTag(World::getInstance()->getGroup(FAC_BLUE)) ? 1.0f : 0.0f;
            nameable->effect = !target->hasTag(World::getInstance()->getGroup(PLR_HUMAN)) && !target->hasTag(World::getInstance()->getGroup(HLT_DEAD));
        }
        // from RIGGED
        if (0) {
            vector_cpy(nameable->pos0, rigged->pos0);
            quat_cpy(nameable->ori0, rigged->ori0);
            int eye = rigged->jointpoints[rRigged::EYE];
            vector_cpy(nameable->pos1, rigged->joints[eye].v);
            nameable->pos1[1] += 2.0f;
            quat_cpy(nameable->ori1, rigged->joints[eye].q);
        }
    }

    // CAMERA
    {
        // from RIGGED
        if (0) {
            int eye = rigged->jointpoints[rRigged::EYE];
            vector_cpy(camra->pos0, rigged->pos0);
            quat_cpy(camra->ori0, rigged->ori0);
            vector_cpy(camra->pos1, rigged->joints[eye].v);
            quat_cpy(camra->ori1, rigged->joints[eye].q);
        }

        // from MOBILE
        if (0) {
            camra->camerashake = mobile->jetthrottle;
        }

        // from CONTROLLED
        {
            camra->cameraswitch = pad->getButton(Pad::MECH_CAMERA_BUTTON);
        }
    }

    // FIXME: Input binding.
    if (target->alife) {
        if (pad->getButton(Pad::MECH_FIRE_BUTTON1) || pad->getButton(Pad::MECH_FIRE_BUTTON2)) {
            wepcom->fire();
        }
    }

    // WEPCOM
    {
        // from TARGET
        if (0) {
            wepcom->active = target->alife;
        }
    }
    
    // WEAPON

    if (0)
    loopi(weapons.size()) {
        // from CONTROLLED:
        {
            weapons[i]->target = mobile->aimtarget;
        }
        // from RIGGED:
        {
            rWeapon* weapon = weapons[i];
            MD5Format::joint* joint = &rigged->joints[weapon->weaponMount];
            quat_cpy(weapon->ori0, traceable->ori);
            vector_cpy(weapon->pos0, traceable->pos);
            quat_cpy(weapon->ori1, joint->q);
            vector_cpy(weapon->pos1, joint->v);
        }
    }

    // EXPLOSION (WEAPON)
    if (0) {
        // from RIGGED:
        {
            rWeapon* weapon = explosion;
            MD5Format::joint* joint = &rigged->joints[weapon->weaponMount];
            quat_cpy(weapon->ori0, traceable->ori);
            vector_cpy(weapon->pos0, traceable->pos);
            quat_cpy(weapon->ori1, joint->q);
            vector_cpy(weapon->pos1, joint->v);
        }
    }
    
    // Pad
    if (pad) {
        // from MOBILE
        {
            pad->setAxis(Pad::MECH_CHASSIS_LR_AXIS, mobile->chassis_lr_tgt);
            pad->setAxis(Pad::MECH_THROTTLE_AXIS, mobile->drive_tgt);
            pad->setAxis(Pad::MECH_TURRET_LR_AXIS, mobile->tower_lr_tgt);
            pad->setAxis(Pad::MECH_TURRET_UD_AXIS, mobile->tower_ud_tgt);
            pad->setButton(Pad::MECH_FIRE_BUTTON1, mobile->firetarget_tgt);
        }
        if (!controller->enabled) pad->reset();
    }

    // Write back.
    vector_cpy(this->pos0, traceable->pos);
    quat_cpy(this->ori0, traceable->ori);
    this->radius = traceable->radius;
}

void cMech::transform() {
    // Everything in animation ?!
}

void cMech::drawSolid() {
    // Setup jumpjet light source - for player only so far. move to rLightsource?
    if (target->hasTag(World::getInstance()->getGroup(PLR_HUMAN))) {
        int light = GL_LIGHT1;
        if (mobile->jetthrottle > 0.001f) {
            float p[] = {traceable->pos[0], traceable->pos[1] + 1.2f, traceable->pos[2], 1.0f};
            //float zero[] = {0, 0, 0, 1};
            float s = mobile->jetthrottle;
            float a[] = {0.0f, 0.0f, 0.0f, 1.0f};
            float d[] = {0.9f * s, 0.9f * s, 0.4f * s, 1.0f};
            //GL::glPushMatrix();
            {
                //GL::glLoadIdentity();
                //GL::glTranslatef(traceable->pos[0], traceable->pos[1]+1, traceable->pos[2]);
                //glLightfv(light, GL_POSITION, zero);
                GL::glLightfv(light, GL_POSITION, p);
                GL::glLightfv(light, GL_AMBIENT, a);
                GL::glLightfv(light, GL_DIFFUSE, d);
                GL::glLightf(light, GL_CONSTANT_ATTENUATION, 1.0);
                GL::glLightf(light, GL_LINEAR_ATTENUATION, 0.001);
                GL::glLightf(light, GL_QUADRATIC_ATTENUATION, 0.005);
                GL::glEnable(light);
            }
            //GL::glPopMatrix();
        } else {
            GL::glDisable(light);
        }
    }
}

void cMech::drawEffect() {
    
}

void cMech::drawHUD() {
    GL::glPushAttrib(GL_ALL_ATTRIB_BITS /* more secure */);
    {
        GLS::glUseProgram_bkplaincolor();
        GL::glDisable(GL_CULL_FACE);
        GL::glLineWidth(2);

        GLS::glPushOrthoProjection();
        {
            GL::glPushMatrix();
            {
                GL::glLoadIdentity();

                float bk[] = {0.0, 0.2, 0.3, 0.2};
                //float bk[] = {0, 0, 0, 0.0};
                float w = 5;
                float h = 4;
                float sx = 1.0f / w;
                float sy = 1.0f / h;

                Component * displays[4][5] = {
                    { navcom, chatMember, NULL, NULL, comcom },
                    { NULL, NULL, NULL, NULL, NULL},
                    { NULL, NULL, NULL, NULL, NULL},
                    { tarcom, NULL, wepcom, NULL, target}
                };

                loopj(4) {

                    loopi(5) {
                        if (displays[j][i] == NULL) continue;
                        GL::glPushMatrix();
                        {
                            GL::glScalef(sx, sy, 1);
                            GL::glTranslatef(i, 3 - j, 0);
                            GL::glColor4fv(bk);
                            displays[j][i]->drawHUD();
                        }
                        GL::glPopMatrix();
                    }
                }

                if (true) {
                    GL::glPushMatrix();
                    {
                        GL::glTranslatef(0.25, 0.25, 0);
                        GL::glScalef(0.5, 0.5, 1);
                        GL::glTranslatef(0, 0, 0);
                        GL::glColor4fv(bk);
                        forcom->drawHUD();
                    }
                    GL::glPopMatrix();
                }

            }
            GL::glPopMatrix();
        }
        GLS::glPopProjection();
    }
    GL::glPopAttrib();
}

void cMech::damage(float* localpos, float damage, Entity* enactor) {
    if (!target->alife || damage == 0.0f) return;

    if (!target->damage(localpos, damage, enactor)) {
        logger->debug() << "cMech::damageByParticle(): DEAD\n";
        //explosion->fire();
        explosion->triggeren = true;
        explosion->trigger = true;
    }
    
    int body = rTarget::BODY;
    if (target->hp[body] <= 75) target->addTag(World::getInstance()->getGroup(HLT_WOUNDED));
    if (target->hp[body] <= 50) target->addTag(World::getInstance()->getGroup(HLT_SERIOUS));
    if (target->hp[body] <= 25) target->addTag(World::getInstance()->getGroup(HLT_CRITICAL));
    if (target->hp[body] <= 0) target->addTag(World::getInstance()->getGroup(HLT_DEAD));
}

float cMech::constrain(float* worldpos, float radius, float* localpos, Entity* enactor) {
    float maxdepth = 0.0f;
    if (true) {
        float depth = collider->constrain(worldpos, radius, localpos, enactor);
        if (depth > maxdepth) {
            maxdepth = depth;
        }
    }
    return maxdepth;
}

