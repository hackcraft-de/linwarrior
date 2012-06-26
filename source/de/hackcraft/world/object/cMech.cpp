// cMech.cpp

#include "cMech.h"

#include "de/hackcraft/io/Pad.h"

#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/camera/rCamera.h"

#include "de/hackcraft/world/sub/computer/rComcom.h"
#include "de/hackcraft/world/sub/computer/rController.h"
#include "de/hackcraft/world/sub/computer/rNavcom.h"

#include "de/hackcraft/world/sub/model/rRigged.h"
#include "de/hackcraft/world/sub/model/ModelSystem.h"

#include "de/hackcraft/world/sub/mobile/rMobile.h"
#include "de/hackcraft/world/sub/mobile/rForcom.h"

#include "de/hackcraft/world/sub/physics/PhysicsSystem.h"

#include "de/hackcraft/world/sub/weapon/WeaponSystem.h"

#include <cassert>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <map>
using std::map;

#define EXPERIMENT true

#include "de/hackcraft/world/sub/model/ModelSystem.h"
// -------------------------------------------------------------------

#include <typeinfo>

int cMech::sInstances = 0;
std::map<int, long> cMech::sTextures;

cMech::cMech(Propmap* props) {
    
    Propmap& cnf = *props;
    
    int n;
    float x, y, z;
    
    cout << cnf.getProperty("mech.model", "no model") << "\n";
    
    cout << cnf.getProperty("mech.pos", "no position") << "\n";
    n = sscanf(cnf.getProperty("mech.pos", "0 0 0").c_str(), " ( %f , %f , %f ) ", &x, &y, &z);
    vec3 pos;
    vector_set(pos, x, y, z);
    
    cout << cnf.getProperty("mech.rot", "no rotation") << "\n";
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
        
        stringstream sstr;
        sstr << "mech." << mpts[i];
        string s = sstr.str();
        
        if (!cnf.contains(s)) continue;
        
        string wpn = cnf.getProperty(s, std::string(""));
        
        if (wpn.compare("Plasma") == 0) {
            cout << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponPlasmagun());
            
        } else if (wpn.compare("Homing") == 0) {
            cout << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponHoming());
            
        } else if (wpn.compare("Raybeam") == 0) {
            cout << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponRaybeam());
            
        } else if (wpn.compare("Machinegun") == 0) {
            cout << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponMachinegun());
            
        } else if (wpn.compare("Explosion") == 0) {
            cout << wpn << " selected for " << mpts[i] << "\n";
            mountWeapon(mpts[i], new rWeaponExplosion());
            
        } else {
            cout << "No weapon selected for " << mpts[i] << "\n";
        }
    }

    name = cnf.getProperty("mech.name", "unnamed");
    nameable->name = cnf.getProperty("mech.displayname", "???");
    controller->enabled = (cnf.getProperty("mech.ai", "false").compare("true") == 0);
    mobile->immobile = (cnf.getProperty("mech.immobile", "false").compare("true") == 0);
}

cMech::cMech(float* pos, float* rot, string modelName) {
    init(pos, rot, modelName);
}

void cMech::init(float* pos, float* rot, string modelName) {
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
    //PhysicsSystem::getInstance()->add(collider);
    //PhysicsSystem::getInstance()->add(traceable);

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
    camra = new rCamera(this);
    mobile = new rMobile(this);

    comcom = new rComcom(this);
    forcom = new rForcom(this);
    navcom = new rNavcom(this);

    if (rot != NULL) vector_scale(mobile->bse, rot, PI_OVER_180);

    if (pos != NULL) vector_cpy(traceable->pos, pos);
    vector_cpy(traceable->old, traceable->pos);
    float axis[] = {0, 1, 0};
    quat_rotaxis(traceable->ori, mobile->bse[1], axis);

    // Mech Speaker
    if (0) {
        try {
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
        } catch (...) {
            cout << "Sorry, no mech sound possible.";
        }
    }

    try {                
        rigged->loadModel(rigged->resolveFilename(modelName));
    } catch (const char* s) {
        cout << "CATCHING: " << s << endl;
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
}

cMech::~cMech() {
    // FIXME: delete all components.
    if (sInstances == 1) {
        // Last one cleans up

    }
    sInstances--;
}

void cMech::message(Message* message) {
    cout << "I (" << this->oid << ":" << this->name << ") just received: \"" << message->getText() << "\" from sender " << message->getSender() << endl;
    forcom->message(message);
}

void cMech::spawn() {
    //cout << "cMech::onSpawn()\n";
    ALuint soundsource = traceable->sound;
    bool isHumanPlayer = target->hasTag(World::getInstance()->getGroup(PLR_HUMAN));
    bool isSoundEnabled = alIsSource(soundsource);
    
    if ( isHumanPlayer && isSoundEnabled ) {
        alSourcePlay(soundsource);
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
}


void cMech::animate(float spf) {
    // Read in.
    vector_cpy(this->pos0, traceable->pos);
    quat_cpy(this->ori0, traceable->ori);
    this->radius = traceable->radius;

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
        collider->prebind();
        collider->animate(spf);
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
        target->prebind();
    }

    // begin COMPUTERs -->

    // COMCOM
    {
        // from TARGET
        if (0) {
            comcom->active = target->alife;
        }
        comcom->prebind();
        comcom->animate(spf);
    }

    // TARCOM
    {
        // from Pad
        if (1) {
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
        tarcom->prebind();
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
        forcom->prebind();
        forcom->animate(spf);
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
        navcom->prebind();
        navcom->animate(spf);
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
        controller->prebind();
        controller->animate(spf);
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
        {
            mobile->aimtarget = controller->aimtarget;
            vector_cpy(mobile->walktarget, controller->walktarget);
            mobile->walktargetdist = controller->walktargetdist;
            mobile->firetarget = controller->firetarget;
        }
        // from TARGET
        {
            mobile->active = target->alife;
        }
        // from traceable
        {
            vector_cpy(mobile->pos0, traceable->pos);
        }
        mobile->prebind();
        mobile->animate(spf);
    }

    // TRACEABLE: Rigid Body, Collisions etc.
    {
        // from MOBILE:
        {
            quat_cpy(traceable->ori, mobile->ori0);
            traceable->jetthrottle = mobile->jetthrottle;
            traceable->throttle = mobile->drivethrottle;
        }

        traceable->animate(spf);

        // FIXME: move to rSoundsource to be able to add loading/streaming
        // and to mount it somewhere.
        if (traceable->sound != 0) {
            alSourcefv(traceable->sound, AL_POSITION, traceable->pos);
            //alSourcefv(traceable->sound, AL_VELOCITY, traceable->vel);
        }
    }

    // RIGGED
    {
        // from MOBILE: Steering state.
        {
            rigged->rotators[rigged->YAW][1] = -mobile->twr[1];
            rigged->rotators[rigged->PITCH][0] = mobile->twr[0];
            rigged->rotators[rigged->HEADPITCH][0] = mobile->twr[0];
            rigged->jetting = mobile->jetthrottle;
        }

        // from TRACEABLE: Physical movement state.
        {
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
        {
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
        {
            int eye = rigged->jointpoints[rRigged::EYE];
            vector_cpy(camra->pos0, rigged->pos0);
            quat_cpy(camra->ori0, rigged->ori0);
            vector_cpy(camra->pos1, rigged->joints[eye].v);
            quat_cpy(camra->ori1, rigged->joints[eye].q);
        }

        // from MOBILE
        {
            camra->camerashake = mobile->jetthrottle;
        }

        // from CONTROLLED
        {
            camra->cameraswitch = pad->getButton(Pad::MECH_CAMERA_BUTTON);
        }

        camra->animate(spf);
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
        wepcom->prebind();
    }
    
    // WEAPON

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
    {
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

    for (auto i = components.begin(); i != components.end(); i++) {
        (*i)->prebind();
        (*i)->animate(spf);
        (*i)->postbind();
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
            //glPushMatrix();
            {
                //glLoadIdentity();
                //glTranslatef(traceable->pos[0], traceable->pos[1]+1, traceable->pos[2]);
                //glLightfv(light, GL_POSITION, zero);
                glLightfv(light, GL_POSITION, p);
                glLightfv(light, GL_AMBIENT, a);
                glLightfv(light, GL_DIFFUSE, d);
                glLightf(light, GL_CONSTANT_ATTENUATION, 1.0);
                glLightf(light, GL_LINEAR_ATTENUATION, 0.001);
                glLightf(light, GL_QUADRATIC_ATTENUATION, 0.005);
                glEnable(light);
            }
            //glPopMatrix();
        } else {
            glDisable(light);
        }
    }
}

void cMech::drawEffect() {
    
}

void cMech::drawHUD() {
    glPushAttrib(GL_ALL_ATTRIB_BITS /* more secure */);
    {
        GLS::glUseProgram_bkplaincolor();
        glDisable(GL_CULL_FACE);
        glLineWidth(2);

        GLS::glPushOrthoProjection();
        {
            glPushMatrix();
            {
                glLoadIdentity();

                float bk[] = {0.0, 0.2, 0.3, 0.2};
                //float bk[] = {0, 0, 0, 0.0};
                float w = 5;
                float h = 4;
                float sx = 1.0f / w;
                float sy = 1.0f / h;

                Component * displays[4][5] = {
                    { navcom, NULL, NULL, NULL, comcom},
                    { NULL, NULL, NULL, NULL, NULL},
                    { NULL, NULL, NULL, NULL, NULL},
                    { tarcom, NULL, wepcom, NULL, target}
                };

                loopj(4) {

                    loopi(5) {
                        if (displays[j][i] == NULL) continue;
                        glPushMatrix();
                        {
                            glScalef(sx, sy, 1);
                            glTranslatef(i, 3 - j, 0);
                            glColor4fv(bk);
                            displays[j][i]->drawHUD();
                        }
                        glPopMatrix();
                    }
                }

                if (true) {
                    glPushMatrix();
                    {
                        glTranslatef(0.25, 0.25, 0);
                        glScalef(0.5, 0.5, 1);
                        glTranslatef(0, 0, 0);
                        glColor4fv(bk);
                        forcom->drawHUD();
                    }
                    glPopMatrix();
                }

            }
            glPopMatrix();
        }
        GLS::glPopProjection();
    }
    glPopAttrib();
}

void cMech::damage(float* localpos, float damage, Entity* enactor) {
    if (!target->alife || damage == 0.0f) return;

    if (!target->damage(localpos, damage, enactor)) {
        cout << "cMech::damageByParticle(): DEAD\n";
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

