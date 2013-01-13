#include "rWeapon.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/openal/AL.h"

#include "de/hackcraft/proc/Noise.h"
#include "de/hackcraft/proc/Distortion.h"

#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/weapon/rTarget.h"


Logger* rWeapon::logger = Logger::getLogger("de.hackcraft.world.sub.weapon.rWeapon");

int rWeapon::sInstances = 0;
std::map<int, long> rWeapon::sTextures;


rWeapon::rWeapon(Entity* obj) {
    
    object = obj;

    sInstances++;
    if (sInstances == 1) {
        // Haze Texture
        if (1) {
            logger->info() << "Generating Haze..." << "\n";
            int w = 1 << 7;
            int h = w;
            int bpp = 4;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopij(h, w) {

                float dx = (((float) j / (float) w) - 0.5f) * 2.0f;
                float dy = (((float) i / (float) h) - 0.5f) * 2.0f;
                float r = sqrtf(dx * dx + dy * dy);
                float r_ = fmin(1.0f, r);
                float a = 1.0f - pow(r_, 1.7);

                const float f = 2 * 2.0f;
                float v3f[] = {f * (float) j / (float) w, 0, f * (float) i / (float) h};
                float c = Distortion::sig(Noise::simplex3(43 + v3f[0], 23 + v3f[1], 11 + v3f[2])*4);
                rgba c4f = {c, c, c, c};
                *p++ = 255 * c4f[2];
                *p++ = 255 * c4f[1];
                *p++ = 255 * c4f[0];
                *p++ = 255 * c4f[3] * a;
            }
            bool soft = true;
            bool repeat = true;
            unsigned int texname;
            texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
            sTextures[0] = texname;
            delete[] texels;
        }
    }

    target = 0;
    triggeren = true;
    trigger = false;
    triggered = false;
    triggereded = false;
    
    matrix_identity(weaponPosef);

    quat_zero(ori1);
    vector_zero(pos1);
    quat_zero(ori0);
    vector_zero(pos0);
    weaponMount = 0;

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


void rWeapon::loadSource(const char* wavFilename, float pitch, float gain) {
    
    // TODO: Use Filesystem-Class to load file image to memory, for now:
    std::stringstream fname;
    fname << "data" << wavFilename;
    
    //ALuint buffer = alutCreateBufferHelloWorld();
    ALuint buffer = alutCreateBufferFromFile(fname.str().c_str());
    alGenSources(1, &soundSource);
    alSourcei(soundSource, AL_BUFFER, buffer);
    alSourcef(soundSource, AL_PITCH, pitch);
    alSourcef(soundSource, AL_GAIN, gain);
    alSourcei(soundSource, AL_LOOPING, AL_FALSE);
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
    
    // Current Pose-Matrix and Sound-Source-Position update.
    GL::glPushMatrix();
    {
        GL::glLoadIdentity();
        GL::glTranslatef(pos0[0], pos0[1], pos0[2]);
        GLS::glRotateq(ori0);
        GL::glTranslatef(pos1[0], pos1[1], pos1[2]);
        GLS::glRotateq(ori1);
        // FIXME: All weapon's forward are inverted, therefore rotate.
        GL::glRotatef(180, 0, 1, 0);
        GL::glGetFloatv(GL_MODELVIEW_MATRIX, weaponPosef);
    }
    GL::glPopMatrix();
    
    if (alIsSource(soundSource)) {
        alSourcefv(soundSource, AL_POSITION, &weaponPosef[12]);
    }
}


void rWeapon::transformTo() {
    GL::glMultMatrixf(weaponPosef);
}

int rWeapon::damageByParticle(float* worldpos, float radius, int roles, float damage) {
    
    roles = 0; //(1UL << cObject::DAMAGEABLE) | (1UL << cObject::COLLIDEABLE);
    float scaled_damage = damage * weaponScale;
    int damaged = 0;
    float maxrange = radius + 25;
    float worldpos_[3];
    
    vector_cpy(worldpos_, worldpos);

    // Find targets that may be affected by the particle.
    std::list<rTarget*>* range = WeaponSystem::getInstance()->filterByRange(object, worldpos, 0.0f, maxrange, -1, NULL);
    
    if (!range->empty()) {

        for(rTarget* target: *range) {
            float localpos_[3];
            float depth = target->object->constrain(worldpos_, radius, localpos_, NULL);
            //cout << object->nameable->name << " depth: " << depth << "\n";
            if (depth == 0) continue;
            damaged++;
            target->object->damage(localpos_, scaled_damage, object);
            break;
        }
    }
    
    delete range;
    
    // If no object was hit then collide with the rest of the world.
    if (damaged == 0) {
        float depth = World::getInstance()->constrainParticle(object, worldpos_, radius);
        if (depth != 0) {
            damaged++;
        }
    }

    return damaged;
}

