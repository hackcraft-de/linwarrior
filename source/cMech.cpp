// cMech.cpp

#include "cObject.h"

#include "cMech.h"
#include "cController.h"
#include "cPad.h"
#include "psi3d/macros.h"
#include "psi3d/snippetsgl.h"

#include "cSolid.h"

#include <cassert>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#define MECHDETAIL -1

DEFINE_glprintf

#define DRAWJOINTS !true


cMech::rComputerised::rComputerised(cObject* o) {
    comcom = new cComcom(o);
    assert(comcom != NULL);

    tarcom = new cTarcom(o);
    assert(tarcom != NULL);

    syscom = new cSyscom(o);
    assert(syscom != NULL);

    wepcom = new cWepcom(o);
    assert(wepcom != NULL);

    forcom = new cForcom((cMech*) o);
    assert(forcom != NULL);

    navcom = new cNavcom(o);
    assert(navcom != NULL);
};

cMech::rComputerised::~rComputerised() {
    delete comcom;
    delete tarcom;
    delete syscom;
    delete wepcom;
    delete forcom;
    delete navcom;
}

void cMech::rComputerised::process(float dt) {
    comcom->process(dt);
    tarcom->process(dt);
    syscom->process(dt);
    wepcom->process(dt);
    forcom->process(dt);
    navcom->process(dt);
}

int cMech::sInstances = 0;
std::map<int, long> cMech::sTextures;


cMech::cMech(float* pos, float* rot) {
    sInstances++;
    if (sInstances == 1) {
        if (1) {
            cout << "Generating Camoflage..." << endl;

            const int SIZE = 1<<(7+MECHDETAIL);
            unsigned char texels[SIZE * SIZE * SIZE * 3];

            for (int l = 0; l < 4; l++) {
                long t = 0;
                loopijk(SIZE, SIZE, SIZE) {
                    float color[16];
                    const float f = 0.25f*0.25f;
                    float x = f*i, y = f*j, z = f*k;
                    switch(l) {
                        case TEXTURE_WOOD: cSolid::camo_wood(x, y, z, color); break;
                        case TEXTURE_URBAN: cSolid::camo_urban(x, y, z, color); break;
                        case TEXTURE_DESERT: cSolid::camo_desert(x, y, z, color); break;
                        case TEXTURE_SNOW: cSolid::camo_snow(x, y, z, color); break;
                        default:
                            cSolid::camo_rust(x, y, z, color);
                    }
                    //cSolid::camo_snow(x, y, z, color3f);
                    //cSolid::camo_desert(x, y, z, color3f);
                    //cSolid::camo_wood(x, y, z, color3f);
                    //cSolid::camo_urban(x, y, z, color3f);
                    //cSolid::animal_gecko(x, y, z, color3f);
                    //cSolid::metal_rust(x, y, z, color3f);
                    //cSolid::metal_damast(x, y, z, color3f);
                    //cSolid::flesh_organix(x, y, z, color3f);
                    //cSolid::camo_rust(x, y, z, color3f);
                    //cSolid::metal_sheet(x, y, z, color3f);
                    texels[t++] = 255.0f * color[0];
                    texels[t++] = 255.0f * color[1];
                    texels[t++] = 255.0f * color[2];
                }
                unsigned int texname = SGL::glBindTexture3D(0, true, true, true, true, true, SIZE, SIZE, SIZE, texels);
                sTextures[l] = texname;
            }
        }
    }

    rigged = new rRigged;
    collideable = new rCollideable;
    damageable = new rDamageable;
    misc = new rMisc;
    computerised = new rComputerised(this);

    entity = new rEntity();
    socialised = new rSocialised();

    addRole(COLLIDEABLE);
    addRole(DAMAGEABLE);
    addRole(SOCIALISED, socialised);
    addRole(ENTITY, entity);
    addRole(MECH);

    if (pos != NULL) vector_cpy(traceable->pos, pos);
    vector_cpy(traceable->old, traceable->pos);
    vector_set(traceable->vel, 0, 0, 0);
    vector_set(misc->twr, 0, 0, 0);
    vector_set(misc->bse, 0, 0, 0);
    if (rot != NULL) vector_scale(misc->bse, rot, 0.017453);

    float axis[] = {0, 1, 0};
    quat_rotaxis(traceable->ori, misc->bse[Y], axis);

    misc->currentWeapon = 0;
    misc->jetpower = 0;
    misc->throttle = 0;
    misc->camerastate = 1;

    // Mech Speaker
    if (0) {
        try {
            ALuint buffer;
            //buffer = alutCreateBufferHelloWorld();
            buffer = alutCreateBufferFromFile("data/base/device/pow.wav");
            if (buffer == AL_NONE) throw "could not load pow.wav";
            ALuint* soundsource = &traceable->sound;
            alGenSources(1, soundsource);
            if (alGetError() == AL_NO_ERROR && alIsSource(*soundsource)) {
                alSourcei(*soundsource, AL_BUFFER, buffer);
                alSourcef(*soundsource, AL_PITCH, 1.0f);
                alSourcef(*soundsource, AL_GAIN, 1.0f);
                alSourcefv(*soundsource, AL_POSITION, traceable->pos.data());
                alSourcefv(*soundsource, AL_VELOCITY, traceable->vel.data());
                alSourcei(*soundsource, AL_LOOPING, AL_FALSE);
            }
        } catch (...) {
            cout << "Sorry, no mech sound possible.";
        }
    }

    entity->controller = new cController(this, true);
    assert(entity->controller != NULL);

    do_moveFor(NULL);

    MD5Format::model* model = NULL;
    MD5Format::joint* modeljoints = NULL;
    try {
        if (true) {
            model = MD5Format::mapMD5Mesh("data/base/wanzers/gorilla/gorilla_ii.md5mesh");
        } else {
            model = MD5Format::mapMD5Mesh("data/base/wanzers/lemur/lemur.md5mesh");
        }
        assert(model != NULL);

        //cout << MD5Format::getModelStats(model) << endl;

        MD5Format::joint* joints = MD5Format::getJoints(model);

        // Allocate space for animated global joints.
        modeljoints = new MD5Format::joint[model->numJoints];
        memcpy(modeljoints, joints, sizeof (MD5Format::joint) * model->numJoints);

        // Make joints local for later animation and transformation to global.
        MD5Format::toLocalJoints(model->numJoints, joints, joints);

        // Map enums to actual joint index.
        std::map<int, int>& jointpoints = rigged->jointpoints;

        loopi(rigged->MAX_JOINTPOINTS) {
            jointpoints[i] = MD5Format::findJoint(model, rigged->getJointname(i).c_str());
        }

        rigged->model = model;
        rigged->joints = modeljoints;
    } catch (const char* s) {
        cout << "CATCHING: " << s << endl;
        delete model;
        delete modeljoints;
    }

    //rigged->scale = 1.0f;
    //rigged->scale = 0.8f;
    //rigged->scale = 0.15;
    //rigged->scale = 0.05;
    //rigged->scale = 0.3f;
    //rigged->scale = 1.7f;
    rigged->scale = 0.33f;

    traceable->radius = 3.45f * rigged->scale;

    traceable->cwm2 = 1.33 /*very bad cw*/ * traceable->radius * traceable->radius * M_PI * rigged->scale /*m2*/;

    traceable->mass = 11000 * rigged->scale * 3.33f; // TODO mass is qubic to size.
    traceable->mass_inv = 1.0f / traceable->mass;
}

cMech::~cMech() {
    delete computerised;
    delete rigged;
    if (sInstances == 1) {
        // Last one cleans up

    }
    sInstances--;
}

void cMech::onMessage(void* message) {
    cWorld::rMessage* m = (cWorld::rMessage*) message;
    cout << "I (" << this->base->oid << ":" << this->nameable->name << ") just received: \"" << m->mText << "\" from group " << m->mGroup << endl;
}

void cMech::onSpawn() {
    //cout << "cMech::onSpawn()\n";
    this->mountWeapon((char*) "CTorsor", &misc->explosion, false);
    ALuint soundsource = traceable->sound;
    if (hasRole(HUMANPLAYER) && alIsSource(soundsource)) alSourcePlay(soundsource);
    //cout << "Mech spawned " << base->oid << "\n";
}

void cMech::multEyeMatrix() {
    float camera[16];
    float rot_inv[16];
    float pos_inv[16];

    glPushMatrix();
    {
        // Get Inverse Components of Head Matrix
        glLoadMatrixf(rigged->HDMount);
        SGL::glGetTransposeInverseRotationMatrix(rot_inv);
        SGL::glGetInverseTranslationMatrix(pos_inv);
        // Compose Camera Matrix from inverse components
        glLoadIdentity();
        glMultMatrixf(rot_inv);
        glMultMatrixf(pos_inv);
        glGetFloatv(GL_MODELVIEW_MATRIX, camera);
        //matrix_print(camera);
    }
    glPopMatrix();

    if (abs(misc->camerastate) == 1) { // 1st
        glMultMatrixf(camera);
    } else if (abs(misc->camerastate) == 2) { // 3rd
        glTranslatef(0, 0, -5);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(camera);
    } else if (abs(misc->camerastate) == 3) { // 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(camera);
    } else if (abs(misc->camerastate) == 4) { // Reverse 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glMultMatrixf(camera);
    } else if (abs(misc->camerastate) == 5) { // Map Near
        glTranslatef(0, 0, -50);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } else if (abs(misc->camerastate) == 6) { // Map Far
        glTranslatef(0, 0, -100);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } // if camerastate
}

void cMech::setAsAudioListener() {
    float s = -0.1;
    float vel[] = {
        traceable->vel[0] * cWorld::instance->mSPF * s,
        traceable->vel[1] * cWorld::instance->mSPF * s,
        traceable->vel[2] * cWorld::instance->mSPF * s
    };
    float pos[] = {
        traceable->pos[0], traceable->pos[1], traceable->pos[2]
    };
    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_VELOCITY, vel);
    float* head = rigged->HDMount;
    float at_and_up[] = {
        head[8], head[9], head[10],
        0, -1, 0
    };
    alListenerfv(AL_ORIENTATION, at_and_up);
}

void cMech::ChassisLR(float radians) {
    const float limit = 0.01 * M_PI;
    float e = radians;
    if (e > +limit) e = +limit;
    if (e < -limit) e = -limit;
    misc->bse[Y] += e;
    const float fullcircle = 2 * M_PI;
    misc->bse[Y] = fmod(misc->bse[Y], fullcircle);

    float axis[] = {0, 1, 0};
    quat_rotaxis(traceable->ori, misc->bse[Y], axis);
}

void cMech::ChassisUD(float radians) {
    float e = radians;
    const float limit = 0.005 * M_PI;
    if (e > +limit) e = +limit;
    if (e < -limit) e = -limit;
    misc->bse[X] += e;
    const float fullcircle = 2 * M_PI;
    misc->bse[X] = fmod(misc->bse[X], fullcircle);

    float axis[] = {1, 0, 0};
    quat_rotaxis(traceable->ori, misc->bse[X], axis);
}

float cMech::TowerLR(float radians) {
    float e = radians;
    const float steplimit = 0.01 * M_PI;
    if (e > +steplimit) e = +steplimit;
    if (e < -steplimit) e = -steplimit;
    misc->twr[Y] += e;
    if (this->hasRole(IMMOBILE)) {
        const float fullcircle = 2 * M_PI; // 360
        misc->twr[Y] = fmod(misc->twr[Y], fullcircle);
        return 0.0f;
    } else {
        const float limit = 0.4 * M_PI;
        float excess = copysign( (fabs(misc->twr[Y]) > limit ? fabs(misc->twr[Y]) - limit : 0), misc->twr[Y]);
        if (misc->twr[Y] > +limit) misc->twr[Y] = +limit;
        if (misc->twr[Y] < -limit) misc->twr[Y] = -limit;
        return excess;
    }
}

void cMech::TowerUD(float radians) {
    float e = radians;
    const float steplimit = 0.01 * M_PI;
    if (e > +steplimit) e = +steplimit;
    if (e < -steplimit) e = -steplimit;
    misc->twr[X] += e;
    const float limit = 0.4 * M_PI;
    if (misc->twr[X] > +limit) misc->twr[X] = +limit;
    if (misc->twr[X] < -limit) misc->twr[X] = -limit;
}

void cMech::fireAllWeapons() {

    loopi(misc->weapons.size()) {
        misc->weapons[i]->fire(entity->target);
    }
}

void cMech::fireWeapon(unsigned n) {
    if (n >= misc->weapons.size()) return;
    misc->weapons[n]->fire(entity->target);
}

void cMech::mountWeapon(char* point, cWeapon *weapon, bool add) {
    if (weapon == NULL) throw "Null weapon for mounting given.";

    if (strcmp(point, "LTorsor") == 0) weapon->weaponBasefv = rigged->LSMount;
    else if (strcmp(point, "LUpArm") == 0) weapon->weaponBasefv = rigged->LSMount;
    else if (strcmp(point, "LLoArm") == 0) weapon->weaponBasefv = rigged->LAMount;
    else if (strcmp(point, "RTorsor") == 0) weapon->weaponBasefv = rigged->RSMount;
    else if (strcmp(point, "RUpArm") == 0) weapon->weaponBasefv = rigged->RSMount;
    else if (strcmp(point, "RLoArm") == 0) weapon->weaponBasefv = rigged->RAMount;
    else if (point[0] == 'L') weapon->weaponBasefv = rigged->LSMount;
    else if (point[0] == 'R') weapon->weaponBasefv = rigged->RSMount;
    else weapon->weaponBasefv = rigged->BKMount;

    weapon->weaponOwner = this;
    weapon->weaponScale = this->rigged->scale * 2.5f;
    if (add) misc->weapons.push_back(weapon);
}

void cMech::animatePhysics(float spf) {

    // Accumulate steering forces
    {
        // Jump Jet accelleration
        {
            const float jetforce = 130000.0f * (0.01f * misc->jetpower);
            float driveforce = -20000.0f * (0.01f * misc->throttle) * (0.01f * misc->jetpower);
            float turbo = 1;
            float fwd[] = {0, jetforce, turbo*driveforce};
            quat_apply(fwd, traceable->ori, fwd);
            vector_add(traceable->fce, traceable->fce, fwd);
        }

        // Engine acceleration
        {
            float driveforce = -150000.0 * (0.01f * misc->throttle);
            if (traceable->friction <= 0.01f) driveforce = 0.0f;
            float turbo = 1.0f;
            float fwd[] = {0, 0, turbo*driveforce};
            quat_apply(fwd, traceable->ori, fwd);
            vector_add(traceable->fce, traceable->fce, fwd);
        }
    }

    // Accumulate environmental forces
    traceable->applyGravityForce(cWorld::instance->mGravity.data());
    traceable->applyFrictionForce(spf);
    traceable->applyAirdragForce(cWorld::instance->mAirdensity);

    // Integrate position and estimate current velocity...
    traceable->stepVerlet(1.0f / spf, spf*spf, 0.001f);


    // Constraint position...
    {
        float center[3];
        vector_cpy(center, traceable->pos);
        float radius = traceable->radius;
        radius *= 1.5f;
        center[1] += radius;
        float depth = 0;
        depth = constrainParticleToWorld(center, radius);
        if (depth > 0.0f) {
            // There was a collision.
            // Set current position to corrected position.
            vector_cpy(traceable->pos, center);
            traceable->pos[1] -= radius;

            float delta[3];
            vector_set3i(delta, traceable->pos[i] - traceable->old[i]);
            float dist = vector_mag(delta);
            bool infinite = !finitef(traceable->pos[0]) || !finitef(traceable->pos[1]) || !finitef(traceable->pos[2]);
            if (infinite) {
                // The new position is not feasible.
                // (Re-)Set position to a position higher than the old position.
                traceable->old[1] += 0.3f;
                vector_cpy(traceable->pos, traceable->old);
                vector_set(traceable->vel, 0,0,0);
            } else if (dist > 1.3f) {
                // Way too fast.
                // Reset position.
                vector_cpy(traceable->old, traceable->pos);
            } else if (dist > 1.0f) {
                // Just too fast.
                // Rescale (normalize) movement.
                vector_scale(delta, delta, 1.0f / dist);
                vector_add(traceable->pos, traceable->old, delta);
                vector_cpy(traceable->vel, delta);
            }

        }

        // Set friction for next frame - if there was a collision.
        traceable->friction = (depth > 0) ? cWorld::instance->mGndfriction : 0.0f;
    }
}

void cMech::poseRunning(float spf) {
    // Animate legs according to real velocity.
    float v[3];
    float ori_inv[4];
    quat_cpy(ori_inv, traceable->ori);
    quat_conj(ori_inv);
    quat_apply(v, ori_inv, traceable->vel);

    float vel = copysign( sqrt(v[X] * v[X] + v[Z] * v[Z]), v[Z] ) * -0.16f;
    if (vel > 2.8) vel = 2.8;
    if (vel < -1.0) vel = -1.0;

    // This is full of hand crafted magic numbers and magic code don't ask...
    const float e = 0.017453;
    float o = e * -60;
    float t = e * base->seconds * 180;

    float l1 = 30 * (+sin(t)* 1) * vel;
    float l2 = 21 * (+sin(copysign(t, vel) + o) + 1) * fabs(vel);

    float r1 = 30 * (-sin(t)* 1) * vel;
    float r2 = 21 * (-sin(copysign(t, vel) + o) + 1) * fabs(vel);

    std::map<int, std::map<int, float> > &rotators = rigged->rotators;
    rotators[rigged->LEFTLEG][X] = -l1;
    rotators[rigged->LEFTCALF][X] = -l2;
    rotators[rigged->RIGHTLEG][X] = -r1;
    rotators[rigged->RIGHTCALF][X] = -r2;
}

void cMech::poseJumping(float spf) {
    // Hanging Legs
    const float a = -50;
    const float b = -130;
    const float s = 30;
    const float f = b / a;

    std::map<int, std::map<int, float> > &rotators = rigged->rotators;
    if (rotators[rigged->LEFTLEG][X] > -a) rotators[rigged->LEFTLEG][X] -= s * spf;
    if (rotators[rigged->LEFTLEG][X] < -a + 1) rotators[rigged->LEFTLEG][X] += s * spf;

    if (rotators[rigged->LEFTCALF][X] > +b) rotators[rigged->LEFTCALF][X] -= f * s * spf;
    if (rotators[rigged->LEFTCALF][X] < +b + 1) rotators[rigged->LEFTCALF][X] += f * s * spf;

    if (rotators[rigged->RIGHTLEG][X] > -a) rotators[rigged->RIGHTLEG][X] -= s * spf;
    if (rotators[rigged->RIGHTLEG][X] < -a + 1) rotators[rigged->RIGHTLEG][X] += s * spf;

    if (rotators[rigged->RIGHTCALF][X] > +b) rotators[rigged->RIGHTCALF][X] -= f * s * spf;
    if (rotators[rigged->RIGHTCALF][X] < +b + 1) rotators[rigged->RIGHTCALF][X] += f * s * spf;
}

void cMech::animate(float spf) {
    // Update time since spawn
    base->seconds += spf;

    int body = rDamageable::BODY;
    if (damageable->hp[body] <= 75) addRole(WOUNDED);
    if (damageable->hp[body] <= 50) addRole(SERIOUS);
    if (damageable->hp[body] <= 25) addRole(CRITICAL);
    //if (damageable->mHp[body] <= 0) this->addRole(DEAD);

    if (hasRole(DEAD)) {
        remRole(COLLIDEABLE);
        remRole(DAMAGEABLE);
        // Stop movement when dead
        misc->throttle = 0;
        misc->jetpower = 0;
        // If already exploded then frag.
        // Animate Own Explosion (if fired)
        misc->explosion.animate(spf);
        if (misc->explosion.ready()) {
            ChassisLR(0);
            TowerLR(0);
            TowerUD(0);
            //cWorld::instance->fragObject(this);
        }
        if (entity->pad) entity->pad->reset();
    }

    // Process computers and Pad-input when not dead.
    if (!hasRole(DEAD)) {
        assert(entity->pad != NULL);
        //cout << mPad->toString();

        // Let all computers do their work.
        if (computerised) computerised->process(spf);

        // Let AI-Controller set Pad-input.
        if (entity->controller && entity->controller->controllerEnabled) {
            if (entity->pad) entity->pad->reset();
            entity->controller->process();
        }

        float excess = this->TowerLR(0.25f * M_PI * spf * -entity->pad->getAxis(cPad::MECH_TURRET_LR_AXIS));
        this->TowerUD(50 * spf * entity->pad->getAxis(cPad::MECH_TURRET_UD_AXIS) * 0.017453f);

        if (!this->hasRole(IMMOBILE)) {
            // Steer left right
            this->ChassisLR(excess + 0.25f * M_PI * spf * -entity->pad->getAxis(cPad::MECH_CHASSIS_LR_AXIS));
            // Speed
            float tdelta = -entity->pad->getAxis(cPad::MECH_THROTTLE_AXIS);
            //cout << "tdelta: " << tdelta << "  speed: " << mSpeed << "\n";
            misc->throttle = fmax(-200.0f, tdelta * 400.0f);
        }

        if (entity->pad->getButton(cPad::MECH_NEXT_BUTTON) && entity->pad->getButtonEvent(cPad::MECH_NEXT_BUTTON)) {
            this->computerised->tarcom->nextTarget();
        } else if (entity->pad->getButton(cPad::MECH_PREV_BUTTON) && entity->pad->getButtonEvent(cPad::MECH_PREV_BUTTON)) {
            this->computerised->tarcom->prevTarget();
        }

        if (entity->pad->getButton(cPad::MECH_FIRE_BUTTON)) {
            if (true) {
                misc->currentWeapon %= misc->weapons.size();
                fireWeapon(misc->currentWeapon);
                misc->currentWeapon++;
                misc->currentWeapon %= misc->weapons.size();
            } else {
                fireAllWeapons();
            }
        }

        if (entity->pad->getButton(cPad::MECH_JET_BUTTON)) {
            misc->jetpower += (100 - misc->jetpower) * 0.1;
        } else {
            misc->jetpower -= (misc->jetpower * 0.05);
        }

        if (entity->pad->getButton(cPad::MECH_CAMERA_BUTTON)) {
            // Only if Camera State is not transitional
            // then switch to next perspective on button press.
            if (misc->camerastate > 0) {
                misc->camerastate = (1 + (misc->camerastate % MAX_CAMERAMODES));
                misc->camerastate *= -1; // Set transitional.
            }
        } else {
            // Set Camera State as fixed.
            misc->camerastate = abs(misc->camerastate);
        }
    }

    // Rigid Body, Collisions etc.
    animatePhysics(spf);

    vector_scale(rigged->rotators[rigged->TORSOR], misc->twr, 1.0f / 0.017453f);

    // Show fitting animation pose based on state and physics.
    //if (mJumpstate == GROUNDED) {
    if (traceable->friction > 0.01) {
        poseRunning(spf);
    } else {
        poseJumping(spf);
    }

    // Animate Weapons

    loopi(misc->weapons.size()) {
        misc->weapons[i]->animate(spf);
    }

    alSourcefv(traceable->sound, AL_POSITION, traceable->pos.data());
    //alSourcefv(traceable->sound, AL_VELOCITY, traceable->vel.data());
}

void cMech::transform() {
    if (true) {
        MD5Format::model* model = rigged->model;
        MD5Format::joint* manipulators = new MD5Format::joint[model->numJoints];
        assert(manipulators != NULL);

        loopi(model->numJoints) {
            quat_set(manipulators[i].q, 0, 0, 0, -1);
            vector_set(manipulators[i].v, 0, 0, 0);
        }

        if (true) { // set joint manipulators
            float xaxis[] = {-1, 0, 0};
            float yaxis[] = {0, -1, 0};
            //float zaxis[] = {0, 0, -1};

            std::map<int, int>& jointpoints = rigged->jointpoints;

            std::map<int, std::map<int, float> > &rotators = rigged->rotators;

            int spine_idx = jointpoints[rigged->SPINE];
            int torsor_idx = jointpoints[rigged->TORSOR];
            if (spine_idx < 0) spine_idx = torsor_idx;

            if (0) {
                float qy[4];
                quat_rotaxis(qy, +1 * M_PI, yaxis);
                quat_cpy(manipulators[0].q, qy);
            }

            if (spine_idx == torsor_idx && torsor_idx >= 0) {
                float qy[4];
                quat_rotaxis(qy, -misc->twr[1], yaxis);
                float qx[4];
                quat_rotaxis(qx, +misc->twr[0], xaxis);
                float q[4];
                quat_mul(q, qy, qx);
                quat_cpy(manipulators[torsor_idx].q, q);
            } else {
                if (spine_idx >= 0) {
                    float qy[4];
                    quat_rotaxis(qy, -misc->twr[1], yaxis);
                    quat_cpy(manipulators[spine_idx].q, qy);
                }
                if (torsor_idx >= 0) {
                    float qx[4];
                    quat_rotaxis(qx, +misc->twr[0], xaxis);
                    quat_cpy(manipulators[torsor_idx].q, qx);
                }
            }

            if (true) {
                float qx[4];
                quat_rotaxis(qx, +misc->twr[0], xaxis);
                int jidx = jointpoints[rigged->HEAD];
                quat_cpy(manipulators[jidx].q, qx);
            }

            bool left = true;
            if (left) {
                float qx[4];
                quat_rotaxis(qx, rotators[rigged->LEFTLEG][0]*0.017453, xaxis);
                int jidx = jointpoints[rigged->LEFTLEG];
                if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
            }
            if (left) {
                float qx[4];
                quat_rotaxis(qx, rotators[rigged->LEFTCALF][0]*0.017453, xaxis);
                int jidx = jointpoints[rigged->LEFTCALF];
                if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
            }
            bool right = true;
            if (right) {
                float qx[4];
                quat_rotaxis(qx, rotators[rigged->RIGHTLEG][0]*0.017453, xaxis);
                int jidx = jointpoints[rigged->RIGHTLEG];
                if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
            }
            if (right) {
                float qx[4];
                quat_rotaxis(qx, rotators[rigged->RIGHTCALF][0]*0.017453, xaxis);
                int jidx = jointpoints[rigged->RIGHTCALF];
                if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
            }
        } // set joint manipulators

        // Transform local skeleton using manipulators to global skeleton.
        MD5Format::joint* joints = MD5Format::getJoints(model);
        MD5Format::joint* joints_ = rigged->joints;
        MD5Format::toGlobalJoints(model->numJoints, joints, joints_, manipulators);
        delete manipulators;

        // Create mountpoint matrices. Later only quaternions and points.
        float* matrices[] = {
            rigged->HDMount,
            rigged->CTMount, rigged->BKMount,
            rigged->LSMount, rigged->RSMount,
            rigged->LAMount, rigged->RAMount
        };

        int mounts[] = {
            rigged->EYE,
            rigged->CTMOUNT, rigged->BKMOUNT,
            rigged->LSMOUNT, rigged->RSMOUNT,
            rigged->LAMOUNT, rigged->RAMOUNT,
        };

        loopi(7) {
            int jidx = rigged->jointpoints[mounts[i]];
            if (jidx < 0) continue;

            glPushMatrix();
            {
                glLoadIdentity();

                const float f = rigged->scale;
                glTranslatef(traceable->pos[0], traceable->pos[1], traceable->pos[2]);
                SGL::glRotateq(this->traceable->ori.data());
                glRotatef(180, 0, 1, 0);
                // Swap axes (it's actually a rotation around x)
                float m[] = {
                    1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1
                };
                glMultMatrixf(m);
                glScalef(f, f, f);

                glTranslatef(joints_[jidx].v[0], joints_[jidx].v[1], joints_[jidx].v[2]);
                SGL::glRotateq(joints_[jidx].q);
                glScalef(1.0f / f, 1.0f / f, 1.0f / f);
                glRotatef(180, 0, 1, 0); // Hack, need to invert weapons and camera.
                if (matrices[i] != NULL) glGetFloatv(GL_MODELVIEW_MATRIX, matrices[i]);
            }
            glPopMatrix();
        }
    }

    loopi(misc->weapons.size()) {
        misc->weapons[i]->transform();
    }

    misc->explosion.transform();
}

void cMech::drawSolid() {
    if (rigged->model != NULL) {
        MD5Format::model* model = rigged->model;
        MD5Format::joint* joints_ = rigged->joints;
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            glColor3f(1, 1, 1);
            glFrontFace(GL_CW);
            glDisable(GL_LIGHTING);
            int texture = 0;
            texture = hasRole(RED) ? 0 : texture;
            texture = hasRole(BLUE) ? 1 : texture;
            texture = hasRole(GREEN) ? 2 : texture;
            texture = hasRole(YELLOW) ? 3 : texture;
            glBindTexture(GL_TEXTURE_3D, sTextures[texture]);
            glPushMatrix();
            {
                glTranslatef(traceable->pos[0], traceable->pos[1], traceable->pos[2]);
                SGL::glRotateq(this->traceable->ori.data());
                // The model was modelled facing to the camera.
                glRotatef(180, 0, 1, 0);
                // Swap axes (it's actually a rotation around x)
                float m[] = {
                    1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1
                };
                glMultMatrixf(m);
                float rscale = rigged->scale;
                glScalef(rscale, rscale, rscale);

                if (DRAWJOINTS) { // draw skeleton
                    glDisable(GL_TEXTURE_2D);

                    loopi(model->numJoints) {
                        glPushMatrix();
                        {
                            glTranslatef(joints_[i].v[0], joints_[i].v[1], joints_[i].v[2]);
                            SGL::glRotateq(joints_[i].q);
                            cPrimitives::glAxis(0.7);
                        }
                        glPopMatrix();
                        glColor3f(0.5, 0.5, 0.5);
                        glBegin(GL_LINES);
                        {
                            int j = joints_[i].parent;
                            if (j >= 0) {
                                glVertex3fv(joints_[i].v);
                                glVertex3fv(joints_[j].v);
                            }
                        }
                        glEnd();
                    }
                } // draw skeleton

                if (true) { // draw models meshes
                    //glEnable(GL_TEXTURE_2D);
                    if (ENABLE_TEXTURE_3D) glEnable(GL_TEXTURE_3D);

                    // Generate base vertices for 3d-tex-coords.
                    if (rigged->baseverts.empty()) {
                        MD5Format::mesh* msh = MD5Format::getFirstMesh(model);
                        MD5Format::joint* staticjoints = MD5Format::getJoints(rigged->model);

                        loopi(model->numMeshes) {
                            float* baseverts = new float[msh->numverts * 3];
                            MD5Format::animatedMeshVertices(msh, staticjoints, baseverts);
                            rigged->baseverts[i] = baseverts;
                            msh = MD5Format::getNextMesh(msh);
                        }
                    }

                    MD5Format::mesh* msh = MD5Format::getFirstMesh(model);

                    loopi(model->numMeshes) {
                        //cout << curr->numverts << " " << curr->numtris << " " << curr->numweights << endl;
                        float* vtx = new float[msh->numverts * 3];
                        MD5Format::animatedMeshVertices(msh, joints_, vtx);
                        MD5Format::tri* tris = MD5Format::getTris(msh);
                        //MD5Format::vert* verts = MD5Format::getVerts(msh);
                        // For 3d texturing.
                        float* vox = rigged->baseverts[i];
                        //
                        glColor4f(0.8, 0.9, 1, 1);
                        glBegin(GL_TRIANGLES);
                        const float s = 0.25f;
                        for (int j = 0; j < msh->numtris; j++) {
                            int k = tris[j].a;
                            //glTexCoord2fv(verts[k].tmap);
                            //glTexCoord3fv(&vox[3 * k]);
                            glTexCoord3f(vox[3 * k+0]*s, vox[3 * k+1]*s, vox[3 * k+2]*s);
                            glVertex3fv(&vtx[3 * k]);
                            k = tris[j].b;
                            //glTexCoord2fv(verts[k].tmap);
                            //glTexCoord3fv(&vox[3 * k]);
                            glTexCoord3f(vox[3 * k+0]*s, vox[3 * k+1]*s, vox[3 * k+2]*s);
                            glVertex3fv(&vtx[3 * k]);
                            k = tris[j].c;
                            //glTexCoord2fv(verts[k].tmap);
                            //glTexCoord3fv(&vox[3 * k]);
                            glTexCoord3f(vox[3 * k+0]*s, vox[3 * k+1]*s, vox[3 * k+2]*s);
                            glVertex3fv(&vtx[3 * k]);
                        }
                        glEnd();
                        delete vtx;
                        msh = MD5Format::getNextMesh(msh);
                    }
                } // draw models meshes

            }
            glPopMatrix();
        }
        glPopAttrib();
    }

    loopi(misc->weapons.size()) {
        misc->weapons[i]->drawSolid();
    }

    misc->explosion.drawSolid();
}

void cMech::drawEffect() {
    // Draw name above head.
    if (!hasRole(HUMANPLAYER)) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDepthMask(GL_FALSE);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            if (hasRole(RED)) glColor4f(1, 0, 0, 0.4);
            else if (hasRole(GREEN)) glColor4f(0, 1, 0, 0.4);
            else if (hasRole(BLUE)) glColor4f(0, 0, 1, 0.4);
            else glColor4f(1, 1, 0, 0.4);
            glPushMatrix();
            {
                glTranslatef(traceable->pos[0], traceable->pos[1], traceable->pos[2]);
                SGL::glRotateq(this->traceable->ori.data());
                glTranslatef(0, +0.1, 0);
                glRotatef(90, 1, 0, 0);
                cPrimitives::glDisk(7, 1.3f);
            }
            glPopMatrix();
            glColor4f(0.95, 0.95, 1, 1);
            glPushMatrix();
            {
                glMultMatrixf(rigged->HDMount);
                glTranslatef(0, +0.9, 0);
                float s = 0.65;
                glScalef(s, s, s);
                float n[16];
                SGL::glGetTransposeInverseRotationMatrix(n);
                glMultMatrixf(n);
                int l = nameable->name.length();
                glTranslatef(-l * 0.5f, 0, 0);
                glprintf(nameable->name.c_str());
            }
            glPopMatrix();
        }
        glPopAttrib();
    }

    // Draw jumpjet exaust if jet is somewhat on.
    if (this->misc->jetpower > 30.0f) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            std::map<int, int>& jointpoints = rigged->jointpoints;
            int jet[5];
            jet[0] = jointpoints[rigged->JET0];
            jet[1] = jointpoints[rigged->JET1];
            jet[2] = jointpoints[rigged->JET2];
            jet[3] = jointpoints[rigged->JET3];
            jet[4] = jointpoints[rigged->JET4];

            loopi(5) {
                if (jet[i] >= 0) {
                    float* v = rigged->joints[jet[i]].v;
                    glPushMatrix();
                    {
                        float f = misc->jetpower * 0.003 * 5;

                        glTranslatef(traceable->pos[0], traceable->pos[1], traceable->pos[2]);
                        SGL::glRotateq(this->traceable->ori.data());
                        // The model was modelled facing to the camera.
                        glRotatef(180, 0, 1, 0);
                        // Swap axes (it's actually a rotation around x)
                        float m[] = {
                            1, 0, 0, 0,
                            0, 0, -1, 0,
                            0, 1, 0, 0,
                            0, 0, 0, 1
                        };
                        glMultMatrixf(m);
                        float rscale = rigged->scale;
                        glScalef(rscale, rscale, rscale);

                        glTranslatef(v[0], v[1], v[2]);
                        float n[16];
                        SGL::glGetTransposeInverseRotationMatrix(n);
                        glMultMatrixf(n);
                        glColor4f(1, 1, 0.3, 0.6);
                        cPrimitives::glDisk(7, f + 0.0003 * (rand() % 100));
                        glColor4f(1, 0.5, 0.3, 0.7);
                        cPrimitives::glDisk(7, f * 0.6 + 0.001 * (rand() % 100));
                        glColor4f(1, 1, 1, 0.8);
                        cPrimitives::glDisk(7, f * 0.3 + 0.001 * (rand() % 100));
                    }
                    glPopMatrix();
                } // if
            } // loopi
        }
        glPopAttrib();
    }

    // Draw weapon's effects.

    loopi(misc->weapons.size()) {
        misc->weapons[i]->drawEffect();
    }
    // Draw effect of own explosion (yes, it is only when exploding).
    misc->explosion.drawEffect();
}

void cMech::drawHUD() {
    glPushAttrib(GL_ALL_ATTRIB_BITS /* more secure */);
    {
        glLineWidth(2);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

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

                cComputer * displays[4][5] = {
                    { computerised->navcom, NULL, NULL, NULL, computerised->comcom},
                    { NULL, NULL, NULL, NULL, NULL},
                    { NULL, NULL, NULL, NULL, NULL},
                    { computerised->tarcom, NULL, computerised->wepcom, NULL, computerised->syscom}
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
                        computerised->forcom->drawHUD();
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
    int hitzone = rDamageable::BODY;
    if (localpos[1] < 1.3 && this->damageable->hp[rDamageable::LEGS] > 0) hitzone = rDamageable::LEGS;
    else if (localpos[0] < -0.5 && this->damageable->hp[rDamageable::LEFT] > 0) hitzone = rDamageable::LEFT;
    else if (localpos[0] > +0.5 && this->damageable->hp[rDamageable::RIGHT] > 0) hitzone = rDamageable::RIGHT;

    if (damage != 0.0f) {
        if (enactor != NULL) entity->disturber = enactor->base->oid;
        this->damageable->hp[hitzone] -= damage;
        if (this->damageable->hp[hitzone] < 0) this->damageable->hp[hitzone] = 0;
        if (this->damageable->hp[rDamageable::BODY] <= 0 && !this->hasRole(DEAD)) {
            this->remRole(DAMAGEABLE);
            this->remRole(COLLIDEABLE);
            this->addRole(DEAD);
            cout << "cMech::damageByParticle(): DEAD\n";
            misc->explosion.fire(0);
        }
    }
}

float cMech::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float localpos_[3];
    { // Transform to local.
        float ori_inv[4];
        quat_cpy(ori_inv, traceable->ori);
        quat_conj(ori_inv);

        vector_cpy(localpos_, worldpos);
        vector_sub(localpos_, localpos_, traceable->pos);
        quat_apply(localpos_, ori_inv, localpos_);
    }

    float base[3] = {0, -0.0 - radius, 0};
    float radius_ = traceable->radius + radius;
    float height = traceable->radius * 3 + 2 * radius;
    float localprj[3] = { 0, 0, 0 };

    float depth = 0;
    depth = cParticle::constraintParticleByCylinder(localpos_, base, radius_, height, localprj);
    //depth = cParticle::constraintParticleByCone(localpos_, base, radius_, height, localprj);

    if (depth <= 0) return 0;

    if (localpos != NULL) vector_cpy(localpos, localprj);

    { // Transform to global.
        quat_apply(worldpos, traceable->ori, localprj);
        vector_add(worldpos, worldpos, traceable->pos);
    }

    return depth;
}

float cMech::constrainParticleToWorld(float* worldpos, float radius) {
    float depth = 0;
    float maxrange = 25;
    bool groundplane = !true;
    if (groundplane)
    if (worldpos[1] - radius < 0.0f) {
        worldpos[1] = 0.0f + radius;
        depth += -(worldpos[1] - radius) + 0.000001f;
    }
    std::list<cObject*>* range = cWorld::instance->filterByRange(this, worldpos, 0.0f, maxrange, -1, NULL);
    if (!range->empty()) {

        foreach(i, *range) {
            cObject* object = *i;
            depth += object->constrainParticle(worldpos, radius, NULL, this);
        }
    }
    delete range;
    return depth;
}

OID cMech::enemyNearby() {
    OID result = 0;
    std::set<OID> test;
    test.insert(DAMAGEABLE);
    // Filter objects by distance
    std::list<cObject*>* scan = cWorld::instance->filterByRange(this, this->traceable->pos.data(), 0.0f, 50.0f, -1, NULL);
    if (!scan->empty()) {
        // Find all objects belonging to any enemy party/role.
        std::list<cObject*>* roles = cWorld::instance->filterByRole(this, &this->socialised->enemies, false, -1, scan);
        if (!roles->empty()) {
            // Only those objects which are damageable->
            std::list<cObject*>* damages = cWorld::instance->filterByRole(this, &test, false, 1, roles);
            //cout << cWorld::instance->getNames(damages) << endl;
            if (!damages->empty()) result = damages->front()->base->oid;
            delete damages;
        }
        delete roles;
    }
    delete scan;
    return result;
}

OID cMech::disturbedBy() {
    OID disturb = entity->disturber;
    entity->disturber = 0;
    return disturb;
}

float cMech::inDestinationRange() {
    float a = 0;
    float b = 6;
    float d = vector_distance(traceable->pos, entity->destination);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inMeeleRange() {
    cObject* target = cWorld::instance->mIndex[entity->target];
    if (target == NULL) return 0.0f;
    float a = 16;
    float b = 24;
    float d = vector_distance(traceable->pos, target->traceable->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inWeaponRange() {
    // TODO inWeaponRange should depend on ready to fire weapons properties.
    cObject* target = cWorld::instance->mIndex[entity->target];
    if (target == NULL) return 0.0f;
    float a = 36 + 10;
    float b = 44 + 10;
    float d = vector_distance(traceable->pos, target->traceable->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inTargetRange() {
    cObject* target = cWorld::instance->mIndex[entity->target];
    if (target == NULL) return 0.0f;
    float a = 56;
    float b = 124;
    float d = vector_distance(traceable->pos, target->traceable->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

void cMech::do_moveTowards() {
    //cout << "do_moveTowards():\n";
    if (entity->pad == NULL) return;

    // Determine Target (ie. Move- or Aim-Target if former is not available).
    float* target_pos = NULL;
    if (finitef(entity->destination[0])) {
        target_pos = entity->destination.data();
    } else if (entity->target != 0) {
        cObject* target = cWorld::instance->mIndex[entity->target];
        if (target != NULL) {
            target_pos = target->traceable->pos.data();
        } else {
            entity->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction
    float rd[2];
    rotationTo(rd, traceable->pos.data(), target_pos, this->traceable->ori.data());
    entity->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, rd[X]);

    //float thr = 20.0f * (1.0f - fabs(rd[X]) / 360.0f);
    float thr = 1.0f * (1.0f - 0.6 * fabs(rd[X]));
    //cout << "Throttle: " << thr << "\n";
    //mPad->setAxis(cPad::MECH_THROTTLE_AXIS, (char) - thr);
    entity->pad->setAxis(cPad::MECH_THROTTLE_AXIS, -thr);
}

void cMech::do_moveNear() {
    //cout << "do_moveNear():\n";
    if (entity->pad == NULL) return;

    // Determine Target (ie. Move- or Aim-Target if former is not available).
    float* target_pos = NULL;
    if (finitef(entity->destination[0])) {
        target_pos = entity->destination.data();
    } else if (entity->target != 0) {
        cObject* target = cWorld::instance->mIndex[entity->target];
        if (target != NULL) {
            target_pos = target->traceable->pos.data();
        } else {
            entity->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction
    float rd[2];
    rotationTo(rd, traceable->pos.data(), target_pos, this->traceable->ori.data());
    entity->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, 2 * rd[X]);

    float d = vector_distance(this->traceable->pos, target_pos);

    float thr = 1.0f * (1.0f - 0.7 * fabs(rd[X]));

    float f = (d - 23);
    if (f < -1) f = -1;
    if (f > +1) f = +1;

    f *= thr;

    entity->pad->setAxis(cPad::MECH_THROTTLE_AXIS, -f);
    //cout << "Throttle: " << thr << "\n";
    //mPad->setAxis(cPad::MECH_THROTTLE_AXIS, (char) - thr);
}

void cMech::do_aimAt() {
    //cout "do_aimAt():\n";
    if (entity->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (entity->target != 0) {
        cObject* target = cWorld::instance->mIndex[entity->target];
        if (target != NULL) target_pos = target->traceable->pos.data();
        else return;
    } else return;

    // Determine nearest rotation direction
    float tower_ori[4];
    quat_set(tower_ori, 0, sin(0.5 * this->misc->twr[Y]), 0, cos(0.5 * this->misc->twr[Y]));

    float tower_ori_v[4];
    quat_set(tower_ori_v, sin(0.5 * this->misc->twr[X]), 0, 0, cos(0.5 * this->misc->twr[X]));

    quat_mul(tower_ori, tower_ori, tower_ori_v);

    float rd[2];
    rotationTo(rd, traceable->pos.data(), target_pos, this->traceable->ori.data(), tower_ori);
    entity->pad->setAxis(cPad::MECH_TURRET_LR_AXIS, 2 * rd[X]);
    entity->pad->setAxis(cPad::MECH_TURRET_UD_AXIS, rd[Y]);
}

void cMech::do_fireAt() {
    //cout << "do_fireAt():\n";
    if (entity->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (entity->target != 0) {
        cObject* target = cWorld::instance->mIndex[entity->target];
        if (target != NULL) target_pos = target->traceable->pos.data();
        else return;
    } else return;

    // Determine nearest rotation direction
    float tower_ori[4];
    quat_set(tower_ori, 0, sin(0.5 * this->misc->twr[Y]), 0, cos(0.5 * this->misc->twr[Y]));
    float rd[2];
    rotationTo(rd, traceable->pos.data(), target_pos, this->traceable->ori.data(), tower_ori);
    // Fire at random and only if angle small enough.
    if (rand() % 100 <= 40 && fabs(rd[X]) < 0.5) {
        entity->pad->setButton(cPad::MECH_FIRE_BUTTON, true);
        //cout "fire\n";
    }
}

void cMech::do_idle() {
    if (entity->pad == NULL) return;
    misc->throttle = 0;
    entity->pad->reset();
}

void cMech::do_aimFor(OID target) {
    entity->target = target;
}

void cMech::do_moveFor(float* dest) {
    if (dest != NULL) {
        vector_cpy(entity->destination, dest);
    } else {
        // Set XYZ to Not-A-Number (NaN) for no location.
        // Note that NaN-ity can only be tested either by
        // isnanf(x), !finite(x) or by x!=x as NaN always != NaN.
        entity->destination[0] = entity->destination[1] = entity->destination[2] = float_NAN;
        assert(entity->destination[0] != entity->destination[0]);
        //cout << "Destination is " << ((finitef(mDestination[0])) ? "finite" : "infinite" ) << "\n";
    }
}

