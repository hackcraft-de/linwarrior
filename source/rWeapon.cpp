#include "rWeapon.h"

#include "cWorld.h"
#include "psi3d/macros.h"
#include "psi3d/noise.h"
#include "psi3d/snippetsgl.h"

#include <iostream>
using std::cout;
using std::endl;


int rWeapon::sInstances = 0;
std::map<int,long> rWeapon::sTextures;

rWeapon::rWeapon(cObject* obj) {
    role = "WEAPON";
    object = obj;

    sInstances++;
    if (sInstances == 1) {
        // Haze Texture
        if (1) {
            cout << "Generating Haze..." << endl;
            int w = 1 << 7;
            int h = w;
            int bpp = 4;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopij(h,w) {

                float dx = (((float)j / (float)w) - 0.5f) * 2.0f;
                float dy = (((float)i / (float)h) - 0.5f) * 2.0f;
                float r = sqrtf(dx*dx+dy*dy);
                float r_ = fmin(1.0f, r);
                float a = 1.0f-pow(r_, 1.7);

                const float f = 2*2.0f;
                float v3f[] = { f * (float)j / (float)w, 0, f * (float)i / (float)h };
                float c = cDistortion::sig(cNoise::simplex3(43+v3f[0],23+v3f[1],11+v3f[2])*4);
                rgba c4f = { c, c, c, c };
                *p++ = 255*c4f[2];
                *p++ = 255*c4f[1];
                *p++ = 255*c4f[0];
                *p++ = 255*c4f[3]*a;
            }
            bool soft = true;
            bool repeat = true;
            unsigned int texname;
            texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
            sTextures[0] = texname;
            delete texels;
        }
    }

    quat_zero(weaponOri1);
    vector_zero(weaponPos1);
    quat_zero(weaponOri0);
    vector_zero(weaponPos0);
    weaponMount = 0;

    weaponBasefv = NULL;
    weaponScale = 1.0f;
    timeReloading = 0.0f;
    timeReadying = 0.0f;
    timeFiring = 0.0f;
    clipSize = 8;
    depotSize = 5;
    remainingAmmo = clipSize;
    remainingClips = depotSize;

    soundSource = -1;
    drawWeapon = false;
}

void rWeapon::playSource() {
    if (alIsSource(soundSource)) {
        alSourcePlay(soundSource);
    }
}

void rWeapon::playSourceIfNotPlaying() {
    if (alIsSource(soundSource)) {
        ALint playing;
        alGetSourcei(soundSource, AL_SOURCE_STATE, &playing);
        if (playing != AL_PLAYING)
            alSourcePlay(soundSource);
    }
}

void rWeapon::transform() {
    { // Current Pose-Matrix and Sound-Source-Position update.
        glPushMatrix();
        {
            glLoadIdentity();
            if (weaponBasefv != NULL) {
                glMultMatrixf(weaponBasefv);
            } else {
                glTranslatef(weaponPos0[0], weaponPos0[1], weaponPos0[2]);
                SGL::glRotateq(weaponOri0);
                glTranslatef(weaponPos1[0], weaponPos1[1], weaponPos1[2]);
                SGL::glRotateq(weaponOri1);
                // FIXME: All weapon's forward are inverted, therefore rotate.
                glRotatef(180, 0,1,0);
            }
            glGetFloatv(GL_MODELVIEW_MATRIX, weaponPosef);
        }
        glPopMatrix();
        if (alIsSource(soundSource)) {
            alSourcefv(soundSource, AL_POSITION, &weaponPosef[12]);
        }
    }
}

void rWeapon::transformTo() {
    glMultMatrixf(weaponPosef);
}

int rWeapon::damageByParticle(float* worldpos, float radius, int roles, float damage) {
    roles = 0;//(1UL << cObject::DAMAGEABLE) | (1UL << cObject::COLLIDEABLE);
    float scaled_damage = damage * weaponScale;
    int damaged = 0;
    float maxrange = 25;
    float worldpos_[3];
    vector_cpy(worldpos_, worldpos);
    std::list<cObject*>* range = cWorld::instance->filterByRange(object, worldpos, 0.0f, maxrange, -1, NULL);
    //cout << "damageByParticle: "  << cWorld::instance->getNames(range) << endl;
    if (!range->empty()) {

        foreach(i, *range) {
            cObject* object = *i;
            float localpos_[3];
            float depth = object->constrainParticle(worldpos_, radius, localpos_, NULL);
            //cout << object->nameable->name << " depth: " << depth << endl;
            if (depth == 0) continue;
            damaged++;
            object->damageByParticle(localpos_, scaled_damage, object);
            break;
        }
    }
    delete range;

    return damaged;
}


