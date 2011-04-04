// cMech.cpp

#include "cMech.h"

#include "cWorld.h"
#include "cController.h"
#include "cPad.h"

#include "rWeapon.h"
#include "rComcom.h"
#include "rNameable.h"
#include "rTraceable.h"
#include "rDamageable.h"
#include "rControlled.h"
#include "rGrouping.h"
#include "rRigged.h"
#include "rCamera.h"
#include "rCollider.h"
#include "rMobile.h"

#include "psi3d/macros.h"
#include "psi3d/snippetsgl.h"

#include "cSolid.h"

#include <cassert>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#define MECHDETAIL -1

//#include "psi3d/instfont.h"
//DEFINE_glprintf

#define EXPERIMENT true


// -------------------------------------------------------------------


int cMech::sInstances = 0;
std::map<int, long> cMech::sTextures;


cMech::cMech(float* pos, float* rot) {
    sInstances++;
    if (sInstances == 1) {
        if (1) {
            //registerRole(new rMobile((cObject*)NULL), FIELDOFS(mobile), ROLEPTR(cMech::mobile));
            //registerRole(new rRigged((cObject*)NULL), FIELDOFS(rigged), ROLEPTR(cMech::rigged));
            //registerRole(new rComputerised((cObject*)NULL), FIELDOFS(computerised), ROLEPTR(cMech::computerised));

            cout << "Generating Camoflage..." << endl;

            const int SIZE = 1<<(7+MECHDETAIL);
            unsigned char* texels = new unsigned char[SIZE * SIZE * SIZE * 3];

            for (int l = 0; l < 4; l++) {
                long t = 0;
                loopijk(SIZE, SIZE, SIZE) {
                    float color[16];
                    const float f = 0.25f*0.25f * 64.0f / SIZE;
                    float x = f*i, y = f*j, z = f*k;
                    switch(l) {
                        //case TEXTURE_WOOD: cSolid::camo_wood(x, y, z, color); break;
                        case TEXTURE_WOOD: cSolid::camo_rust(x, y, z, color); break;
                        case TEXTURE_URBAN: cSolid::camo_urban(x, y, z, color); break;
                        case TEXTURE_DESERT: cSolid::camo_desert(x, y, z, color); break;
                        case TEXTURE_SNOW: cSolid::camo_snow(x, y, z, color); break;
                        default:
                            cSolid::camo_rust(x, y, z, color);
                    }
                    texels[t++] = 255.0f * color[0];
                    texels[t++] = 255.0f * color[1];
                    texels[t++] = 255.0f * color[2];
                }
                unsigned int texname = SGL::glBindTexture3D(0, true, true, true, true, true, SIZE, SIZE, SIZE, texels);
                sTextures[l] = texname;
            }
            delete texels;
        }
    }

    collider = new rCollider(this);
    rigged = new rRigged(this);
    damageable = new rDamageable(this);
    controlled = new rControlled(this);
    nameable = new rNameable(this);
    traceable = new rTraceable(this);
    camera = new rCamera(this);
    mobile = new rMobile(this);

    comcom = new rComcom(this);
    tarcom = new rTarcom(this);
    wepcom = new rWepcom(this);
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

    do_moveFor(NULL);

    try {
        int mod = (9+rand())%8; // fr
        //int mod = (13+rand())%8; // le
        const char* fns[] = {
            //"/media/44EA-7693/workspaces/mm3d/soldier/soldier.md5mesh",
            "data/base/wanzers/frogger/frogger.md5mesh",
            "data/blendswap.com/flopsy/flopsy.md5mesh",
            "data/base/wanzers/gorilla/gorilla_ii.md5mesh",
            "data/opengameart.org/scorpion/scorpion.md5mesh",
            "data/base/wanzers/lemur/lemur.md5mesh",
            "data/base/tanks/bug/bug.md5mesh",
            "data/base/tanks/ant/ant.md5mesh",
            "data/base/wanzers/kibitz/kibitz.md5mesh",
            //"data/base/tanks/pod/pod.md5mesh"
        };
        rigged->loadModel(string(fns[mod]));
    } catch (const char* s) {
        cout << "CATCHING: " << s << endl;
        rigged->loadModel(string("data/base/wanzers/frogger/frogger.md5mesh"));
    }

    traceable->radius = 1.75f;
    traceable->cwm2 = 0.4f /*very bad cw*/ * traceable->radius * traceable->radius * M_PI /*m2*/;
    traceable->mass = 11000.0f; // TODO mass is qubic to size.
    traceable->mass_inv = 1.0f / traceable->mass;

    vector_cpy(this->pos, traceable->pos);
    quat_cpy(this->ori, traceable->ori);
    this->radius = traceable->radius;

    vector_cpy(collider->pos, this->pos);
    quat_cpy(collider->ori, this->ori);
    collider->radius = this->radius;

    explosion = new rWeaponExplosion(this);
    mountWeapon((char*) "CTorsor", explosion, false);
}

cMech::~cMech() {
    // FIXME: delete all components.
    if (sInstances == 1) {
        // Last one cleans up

    }
    sInstances--;
}

void cMech::message(cMessage* message) {
    cout << "I (" << this->oid << ":" << this->name << ") just received: \"" << message->getText() << "\" from sender " << message->getSender() << endl;
    forcom->message(message);
}

void cMech::onSpawn() {
    //cout << "cMech::onSpawn()\n";
    ALuint soundsource = traceable->sound;
    if (hasTag(HUMANPLAYER) && alIsSource(soundsource)) alSourcePlay(soundsource);
    //cout << "Mech spawned " << oid << "\n";
}

void cMech::multEyeMatrix() {
    camera->camera();
}

void cMech::setAsAudioListener() {
    float s = -0.1;
    float step = s * cWorld::instance->getTiming()->getSPF();
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
    vec3 fwd = { 0, 0, +1 };
    quat_apply(fwd, rigged->ori, fwd);
    vec3 uwd = { 0, -1, 0 };
    quat_apply(uwd, rigged->ori, uwd);
    float at_and_up[] = {
        fwd[0], fwd[1], fwd[2],
        uwd[0], uwd[1], uwd[2]
    };
    alListenerfv(AL_ORIENTATION, at_and_up);
}

void cMech::mountWeapon(char* point, rWeapon *weapon, bool add) {
    if (weapon == NULL) throw "Null weapon for mounting given.";
    weapon->weaponMount = rigged->getMountpoint(point);
    //weapon->weaponBasefv = rigged->getMountMatrix(point);
    weapon->object = this;
    if (add) {
        weapons.push_back(weapon);
#if 0
        int n = weapons.size();
        loopi(n) {
            weapons[i]->triggeren = false;
            weapons[i]->triggered = false;
            weapons[i]->triggereded = false;
        }
        weapon->triggeren = true;
        weapon->triggered = true;
        weapon->triggereded = true;
#endif
    }
}

#if 0
void cMech::fireCycleWeapons() {
    cout << "fireCycleWeapons()\n";
    int n = weapons.size();
    loopi(n) {
        cout << "W " << i << ", triggeren = " << weapons[i]->triggeren << ", triggered = " << weapons[i]->triggered << ", triggereded = " << weapons[i]->triggereded << endl;
        bool selfdone = weapons[i]->triggered && weapons[i]->triggereded;
        bool otherdone = weapons[(i+1)%n]->triggered;
        weapons[i]->triggeren ^= selfdone;
        weapons[i]->triggeren |= otherdone;
        weapons[i]->triggered ^= selfdone;
        weapons[i]->triggereded ^= selfdone;
        weapons[i]->target = target;
        weapons[i]->trigger = true;
    }
    loopi(n) {
        cout << "W'" << i << ", triggeren = " << weapons[i]->triggeren << ", triggered = " << weapons[i]->triggered << ", triggereded = " << weapons[i]->triggereded << endl;
    }
}
#else
void cMech::fireCycleWeapons() {
    if (weapons.size() == 0) return;
    currentWeapon %= weapons.size();
    fireWeapon(currentWeapon);
    currentWeapon++;
    currentWeapon %= weapons.size();
}
#endif

void cMech::fireAllWeapons() {

    loopi(weapons.size()) {
        weapons[i]->trigger = true;
    }
}

void cMech::fireWeapon(unsigned n) {
    if (n >= weapons.size()) return;
    weapons[n]->trigger = true;
}


void cMech::animate(float spf) {
    // Read in.
    vector_cpy(this->pos, traceable->pos);
    quat_cpy(this->ori, traceable->ori);
    this->radius = traceable->radius;

    /* Index of Component order
     * ------------------------
     * COLLIDER
     * DAMAGEABLE
     * COMPUTERs
     * CONTROLLED
     * MOBILE
     * TRACEABLE
     * RIGGED
     * NAMEABLE
     * CAMERA
     * WEAPON
     * EXPLOSION
     */

    // COLLIDER
    {
        // from SELF:
        {
            vector_cpy(collider->pos, this->pos);
            quat_cpy(collider->ori, this->ori);
            collider->radius = this->radius;
        }
        // from RIGGED:
        {
            collider->radius = rigged->radius;
            collider->ratio = 0.0f;
            collider->height = rigged->height;
        }

        collider->animate(spf);
    }

    // DAMAGEABLE
    {
        // from DAMAGEABLE
        {
            damageable->active = damageable->alife;
        }
        damageable->animate(spf);
    }

    // begin COMPUTERs -->

    // COMCOM
    {
        // from DAMAGEABLE
        {
            comcom->active = damageable->alife;
        }
        comcom->animate(spf);
    }

    // TARCOM
    {
        // from CONTROLLED
        {
            tarcom->switchnext = controlled->pad->getButton(cPad::MECH_NEXT_BUTTON);
            tarcom->switchprev = controlled->pad->getButton(cPad::MECH_PREV_BUTTON);
        }
        // from DAMAGEABLE
        {
            tarcom->active = damageable->alife;
        }
        // from TRACEABLE
        {
            vector_cpy(tarcom->pos, traceable->pos);
            quat_cpy(tarcom->ori, traceable->ori);
        }
        tarcom->animate(spf);
    }

    // WEPCOM
    {
        // from DAMAGEABLE
        {
            wepcom->active = damageable->alife;
        }
        wepcom->animate(spf);
    }

    // FORCOM
    {
        // from DAMAGEABLE
        {
            forcom->active = damageable->alife;
        }
        // from traceable
        {
            quat_cpy(forcom->ori, traceable->ori);
        }
        // from MOBILE
        {
            vector_cpy(forcom->twr, mobile->twr);
        }
        // from CAMERA
        {
            forcom->reticle = camera->firstperson;
        }
        forcom->animate(spf);
    }

    // NAVCOM
    {
        // from DAMAGEABLE
        {
            navcom->active = damageable->alife;
        }
        // from TRACEABLE
        {
            vector_cpy(navcom->pos, traceable->pos);
            quat_cpy(navcom->ori, traceable->ori);
        }
        navcom->animate(spf);
    }

    // <-- end COMPUTERs

    // CONTROLLED
    {
        // from Damageable
        {
            controlled->active = damageable->alife;
            controlled->controller->disturbedBy = damageable->disturber;
            //controlled->controller->disturbedBy = this->disturbedBy();
        }

        // FIXME: from anywhere
        {
            //controlled->controller->enemyNearby = this->enemyNearby();
            controlled->controller->enemyNearby = this->tarcom->nearbyEnemy;
        }

        controlled->animate(spf);
    }

    // MOBILE
    {
        // from CONTROLLED
        {
            mobile->tower_lr = controlled->pad->getAxis(cPad::MECH_TURRET_LR_AXIS);
            mobile->tower_ud = controlled->pad->getAxis(cPad::MECH_TURRET_UD_AXIS);
            mobile->chassis_lr = controlled->pad->getAxis(cPad::MECH_CHASSIS_LR_AXIS);
            mobile->driveen = controlled->pad->getAxis(cPad::MECH_THROTTLE_AXIS);
            mobile->jeten = (controlled->pad->getButton(cPad::MECH_JET_BUTTON1) + controlled->pad->getButton(cPad::MECH_JET_BUTTON2));
        }
        // from DAMAGEABLE
        {
            mobile->active = damageable->alife;
        }
        // from traceable
        {
            vector_cpy(mobile->pos, traceable->pos);
        }
        mobile->animate(spf);
    }

    // TRACEABLE: Rigid Body, Collisions etc.
    {
        // from MOBILE:
        {
            quat_cpy(traceable->ori, mobile->ori);
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
            vector_cpy(rigged->pos, traceable->pos);
            quat_cpy(rigged->ori, traceable->ori);
            vector_cpy(rigged->vel, traceable->vel);
            rigged->grounded = traceable->grounded;
        }
        // FIXME: from object tags
        {
            // Group-to-texture.
            int texture = 0;
            texture = hasTag(RED) ? 0 : texture;
            texture = hasTag(BLUE) ? 1 : texture;
            texture = hasTag(GREEN) ? 2 : texture;
            texture = hasTag(YELLOW) ? 3 : texture;
            rigged->basetexture3d = sTextures[texture];
        }

        rigged->animate(spf);
        rigged->transform();
    }

    // NAMEABLE
    {
        // from Object
        {
            nameable->color[0] = hasTag(RED) ? 1.0f : 0.0f;
            nameable->color[1] = hasTag(GREEN) ? 1.0f : 0.0f;
            nameable->color[2] = hasTag(BLUE) ? 1.0f : 0.0f;
            nameable->effect = !hasTag(HUMANPLAYER) && !hasTag(DEAD);
        }
        // from RIGGED
        {
            vector_cpy(nameable->pos0, rigged->pos);
            quat_cpy(nameable->ori0, rigged->ori);
            int eye = rigged->jointpoints[rRigged::EYE];
            vector_cpy(nameable->pos1, rigged->joints[eye].v);
            nameable->pos1[1] += 2.0f;
            quat_cpy(nameable->ori1, rigged->joints[eye].q);
        }

        nameable->animate(spf);
    }

    // CAMERA
    {
        // from RIGGED
        {
            int eye = rigged->jointpoints[rRigged::EYE];
            vector_cpy(camera->pos0, rigged->pos);
            quat_cpy(camera->ori0, rigged->ori);
            vector_cpy(camera->pos1, rigged->joints[eye].v);
            quat_cpy(camera->ori1, rigged->joints[eye].q);
        }
        
        // from MOBILE
        {
            camera->camerashake = mobile->jetthrottle;
        }
        
        // from CONTROLLED
        {
            camera->cameraswitch = controlled->pad->getButton(cPad::MECH_CAMERA_BUTTON);
        }
        
        camera->animate(spf);
    }

    // FIXME: This block needs to be factored out to fit the used component pattern.
    // FIXME: Weapons need to be chained in an independent style.
    if (damageable->alife) {
        if (controlled->pad->getButton(cPad::MECH_FIRE_BUTTON1) || controlled->pad->getButton(cPad::MECH_FIRE_BUTTON2)) {
            if (true) {
                fireCycleWeapons();
            } else {
                fireAllWeapons();
            }
        }
    }

    // WEAPON
    loopi(weapons.size()) {
        // from CONTROLLED:
        {
            weapons[i]->target = controlled->target;
        }
        // from RIGGED:
        {
            rWeapon* weapon = weapons[i];
            MD5Format::joint* joint = &rigged->joints[weapon->weaponMount];
            quat_cpy(weapon->weaponOri0, traceable->ori);
            vector_cpy(weapon->weaponPos0, traceable->pos);
            quat_cpy(weapon->weaponOri1, joint->q);
            vector_cpy(weapon->weaponPos1, joint->v);
        }
        
        weapons[i]->animate(spf);
        weapons[i]->transform();
    }

    // EXPLOSION (WEAPON)
    {
        // from RIGGED:
        {
            rWeapon* weapon = explosion;
            MD5Format::joint* joint = &rigged->joints[weapon->weaponMount];
            quat_cpy(weapon->weaponOri0, traceable->ori);
            vector_cpy(weapon->weaponPos0, traceable->pos);
            quat_cpy(weapon->weaponOri1, joint->q);
            vector_cpy(weapon->weaponPos1, joint->v);
        }
        explosion->animate(spf);
        explosion->transform();
    }

    // Write back.
    vector_cpy(this->pos, traceable->pos);
    quat_cpy(this->ori, traceable->ori);
    this->radius = traceable->radius;
}

void cMech::transform() {
    /*
    {
        rigged->transform();
    }
    {
        mobile->transform();
    }
    loopi(weapons.size()) {
        weapons[i]->transform();
    }
    {
        explosion->transform();
    }
    */
}

void cMech::drawSolid() {
    // Setup jumpjet light source - for player only so far. move to rLightsource?
    if (hasTag(HUMANPLAYER)) {
        int light = GL_LIGHT1;
        if (mobile->jetthrottle > 0.001f) {
            float p[] = {traceable->pos[0], traceable->pos[1]+1.2, traceable->pos[2], 1};
            //float zero[] = {0, 0, 0, 1};
            float s = mobile->jetthrottle;
            float a[] = {0.0,0.0,0.0,1};
            float d[] = {0.9 * s, 0.9 * s, 0.4 * s, 1};
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

    {
        rigged->drawSolid();
    }
    {
        mobile->drawSolid();
    }
    loopi(weapons.size()) {
        weapons[i]->drawSolid();
    }
    {
        explosion->drawSolid();
    }
}

void cMech::drawEffect() {
    {
        collider->drawEffect();
    }
    {
        rigged->drawEffect();
    }
    {
        mobile->drawEffect();
    }
    loopi(weapons.size()) {
        weapons[i]->drawEffect();
    }
    {
        explosion->drawEffect();
    }
    {
        nameable->drawEffect();
    }
}

void cMech::drawHUD() {
    //computerised->drawHUD();
    glPushAttrib(GL_ALL_ATTRIB_BITS /* more secure */);
    {
        SGL::glUseProgram_bkplaincolor();
        glDisable(GL_CULL_FACE);
        glLineWidth(2);

        SGL::glPushOrthoProjection();
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

                rComponent* displays[4][5] = {
                    { navcom, NULL, NULL, NULL, comcom},
                    { NULL, NULL, NULL, NULL, NULL},
                    { NULL, NULL, NULL, NULL, NULL},
                    { tarcom, NULL, wepcom, NULL, damageable}
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
        SGL::glPopProjection();
    }
    glPopAttrib();
}

void cMech::damageByParticle(float* localpos, float damage, cObject* enactor) {
    if (!damageable->alife || damage == 0.0f) return;

    int hitzone = rDamageable::BODY;
    if (localpos[1] < 1.1*rigged->height && damageable->hp[rDamageable::LEGS] > 0) hitzone = rDamageable::LEGS;
    else if (localpos[0] < -0.5 && damageable->hp[rDamageable::LEFT] > 0) hitzone = rDamageable::LEFT;
    else if (localpos[0] > +0.5 && damageable->hp[rDamageable::RIGHT] > 0) hitzone = rDamageable::RIGHT;

    if (!damageable->damage(hitzone, damage, enactor)) {
        cout << "cMech::damageByParticle(): DEAD\n";
        //explosion->fire();
        explosion->triggeren = true;
        explosion->trigger = true;
    }
    int body = rDamageable::BODY;
    if (damageable->hp[body] <= 75) addTag(WOUNDED);
    if (damageable->hp[body] <= 50) addTag(SERIOUS);
    if (damageable->hp[body] <= 25) addTag(CRITICAL);
    if (damageable->hp[body] <= 0) addTag(DEAD);
}

float cMech::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float maxdepth = 0.0f;
    {
        float depth = collider->constrainParticle(worldpos, radius, localpos, enactor);
        if (depth > maxdepth) {
            maxdepth = depth;
        }
    }
    return maxdepth;
}

float cMech::inDestinationRange() {
    float a = 0;
    float b = 6;
    float d = vector_distance(mobile->pos, controlled->destination);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inMeeleRange() {
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 16;
    float b = 24;
    float d = vector_distance(mobile->pos, target->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inWeaponRange() {
    // TODO inWeaponRange should depend on ready to fire weapons properties.
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 36 + 10;
    float b = 44 + 10;
    float d = vector_distance(mobile->pos, target->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inTargetRange() {
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 56;
    float b = 124;
    float d = vector_distance(mobile->pos, target->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

void cMech::do_moveTowards() {
    //cout << "do_moveTowards():\n";
    if (controlled->pad == NULL) return;

    // Determine Target (ie. Move- or Aim-Target if former is not available).
    float* target_pos = NULL;
    if (finitef(controlled->destination[0])) {
        target_pos = controlled->destination;
    } else if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) {
            target_pos = target->pos;
        } else {
            controlled->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori);
    float lr = rd[0];
    // Determine throttling according to angle.
    float thr = 1.0f * (1.0f - 0.6 * fabs(rd[0]));
    float throttle = -thr;
    //cout << "Throttle: " << throttle << "\n";

    controlled->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, lr);
    controlled->pad->setAxis(cPad::MECH_THROTTLE_AXIS, throttle);
}

void cMech::do_moveNear() {
    //cout << "do_moveNear():\n";
    if (controlled->pad == NULL) return;

    // Determine Target (ie. Move- or Aim-Target if former is not available).
    float* target_pos = NULL;
    if (finitef(controlled->destination[0])) {
        target_pos = controlled->destination;
    } else if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) {
            target_pos = target->pos;
        } else {
            controlled->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction.
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori);
    float lr = 2 * rd[0];
    // Determine distance.
    float d = vector_distance(mobile->pos, target_pos);
    // Throttle according to angle and distance.
    float thr = 1.0f * (1.0f - 0.7 * fabs(rd[0]));
    float f = (d - 23);
    f = fmin(+1.0f, fmax(-1.0f, f));
    f *= thr;
    float throttle = -f;
    //cout << "Throttle: " << throttle << "\n";

    controlled->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, lr);
    controlled->pad->setAxis(cPad::MECH_THROTTLE_AXIS, throttle);
}

void cMech::do_aimAt() {
    //cout "do_aimAt():\n";
    if (controlled->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) target_pos = target->pos;
        else return;
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori, mobile->ori1);
    float lr = 2 * rd[0];
    float ud = rd[1];

    controlled->pad->setAxis(cPad::MECH_TURRET_LR_AXIS, lr);
    controlled->pad->setAxis(cPad::MECH_TURRET_UD_AXIS, ud);
}

void cMech::do_fireAt() {
    //cout << "do_fireAt():\n";
    if (controlled->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) target_pos = target->pos;
        else return;
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori, mobile->ori1);
    //  Fire at random and only if angle small enough.
    bool fire = (rand() % 100 <= 40 && fabs(rd[0]) < 0.5);
    //cout "Fire: " << fire << " \n";

    controlled->pad->setButton(cPad::MECH_FIRE_BUTTON1, fire);
}

void cMech::do_idle() {
    if (controlled->pad == NULL) return;
    mobile->drivethrottle = 0;
    controlled->pad->reset();
}

void cMech::do_aimFor(OID target) {
    controlled->target = target;
}

void cMech::do_moveFor(float* dest) {
    if (dest != NULL) {
        vector_cpy(controlled->destination, dest);
    } else {
        // Set XYZ to Not-A-Number (NaN) for no location.
        // Note that NaN-ity can only be tested either by
        // isnanf(x), !finite(x) or by x!=x as NaN always != NaN.
        vector_set(controlled->destination, float_NAN, float_NAN, float_NAN);
        assert(controlled->destination[0] != controlled->destination[0]);
        //cout << "Destination is " << ((finitef(mDestination[0])) ? "finite" : "infinite" ) << "\n";
    }
}

