#include "rWeaponSparkgun.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include <cassert>

std::string rWeaponSparkgun::cname = "SPARKGUN";
unsigned int rWeaponSparkgun::cid = 5712;

rWeaponSparkgun::rWeaponSparkgun(Entity* obj) {
    object = obj;

    clipSize = 24;
    depotSize = 3;
    remainingAmmo = clipSize;
    remainingClips = depotSize;

    if (WEAPONSOUND) {
        ALuint buffer = alutCreateBufferFromFile("data/org/freesound/sparkgun.wav");
        alGenSources(1, &soundSource);
        alSourcei(soundSource, AL_BUFFER, buffer);
        alSourcef(soundSource, AL_PITCH, 1.0f);
        alSourcef(soundSource, AL_GAIN, 1.0f);
        alSourcei(soundSource, AL_LOOPING, AL_FALSE);
    }
}

void rWeaponSparkgun::fire() {
    if (!ready()) return;
    triggered = true;

    if (remainingAmmo > 0) {
        remainingAmmo--;
        if (remainingAmmo == 0 && remainingClips != 0) {
            timeReloading = 7.1;
        } else {
            timeReloading = 1.9;
        }
    }

    float* source = weaponPosef;

    Particle* s = new Particle();
    assert(s != NULL);
    s->fuel = 1;

    float* source_12 = &source[12];
    vector_cpy(s->pos, source_12)

            float nrm[3];
    float pos2[] = {0, 0, -1};
    matrix_apply2(source, pos2);
    vector_sub(nrm, pos2, s->pos);

    vector_scale(s->vel, nrm, 30);
    shrapnelParticles.push_back(s);

    playSourceIfNotPlaying();
}

void rWeaponSparkgun::animate(float spf) {
    triggereded = triggered;
    if (trigger) fire();
    trigger = false;

    foreachNoInc(i, shrapnelParticles) {
        Particle* s = *i++;
        vector_muladd(s->pos, s->pos, s->vel, spf);
        s->fuel -= spf;
        if (s->fuel < 0) {
            shrapnelParticles.remove(s);
            delete s;
        } else {
            float radius = 0.4;
            int roles = 0;
            float damage = 11;
            int damaged = this->damageByParticle(s->pos, radius, roles, damage);
            if (damaged) {
                shrapnelParticles.remove(s);
                delete s;
            }
        }
    }

    timeReloading -= spf;
    if (timeReloading < 0) {
        timeReloading = 0.0f;
        if (remainingAmmo == 0 && remainingClips != 0) {
            remainingAmmo = clipSize;
            if (remainingClips > 0) remainingClips--;
        }
    }
}

void rWeaponSparkgun::drawSolid() {
    if (drawWeapon) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GLS::glUseProgram_fglitcolor();

            glPushMatrix();
            {
                transformTo();
                glRotatef(-90, 1, 0, 0);

                // Scale
                float scale = weaponScale;
                glScalef(scale, scale, scale);

                if (this->ready() == 0 && remainingAmmo != 0) {
                }

                glColor4f(0.3, 0.2, 0.2, 1.0);
                glPushMatrix();
                {
                    glScalef(0.1, 0.14, 0.12);
                    Primitive::glCenterUnitBlock();
                }
                glPopMatrix();

                glColor4f(0.1, 0.1, 0.1, 1.0);
                glPushMatrix();
                {
                    glTranslatef(0.0, 0.7, 0);
                    glScalef(0.1, 0.7, 0.1);
                    Primitive::glCenterUnitCylinder(7);
                }
                glPopMatrix();
            }
            glPopMatrix();
        }
        glPopAttrib();
    }
}

void rWeaponSparkgun::drawEffect() {
    if (shrapnelParticles.empty()) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GLS::glUseProgram_fgaddcolor();

        float n[16];
        GLS::glGetTransposeInverseRotationMatrix(n);

        foreachNoInc(i, shrapnelParticles) {
            Particle* s = *i++;
            glPushMatrix();
            {
                glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
                glMultMatrixf(n);
                glColor4f(0.6, 0.1, 0.1, 0.99f);
                Primitive::glDisk(9 + WEAPONDETAIL, 5 * 0.1f + 0.006 * (rand() % 100));
                glColor4f(0.9, 0.9, 0.4, 0.99f);
                Primitive::glDisk(7 + WEAPONDETAIL, 5 * 0.07f + 0.006 * (rand() % 100));
            }
            glPopMatrix();
        }

    }
    glPopAttrib();
}

void rWeaponSparkgun::drawHUD() {
    float a = 0.9;
    float b = 0.6;

    // Ammo
    float i = 0.1;
    float j = 0.9;
    float r = i + (remainingAmmo / (float) clipSize) * (j - i);
    glBegin(GL_QUADS);
    glColor4f(1, 1, 0, b);
    glVertex3f(i, j, 0.0);
    glVertex3f(i, i, 0.0);
    glColor4f(1, 0, 0, b);
    glVertex3f(r, i, 0.0);
    glVertex3f(r, j, 0.0);
    glEnd();

    // Clips
    float l = 0.1;
    float h = 0.15;
    r = i + (remainingClips / (float) depotSize) * (j - i);
    glBegin(GL_QUADS);
    glColor4f(0, 0, 0, a);
    glVertex3f(l, i + h, 0.0);
    glVertex3f(l, i, 0.0);
    glColor4f(1, 1, 1, a);
    glVertex3f(r, i, 0.0);
    glVertex3f(r, i + h, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glColor4f(1, 1, 0, b);
    glEnd();
}

