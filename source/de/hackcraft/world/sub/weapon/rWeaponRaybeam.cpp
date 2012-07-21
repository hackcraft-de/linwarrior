#include "rWeaponRaybeam.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"

std::string rWeaponRaybeam::cname = "RAYBEAM";
unsigned int rWeaponRaybeam::cid = 5357;

rWeaponRaybeam::rWeaponRaybeam(Entity* obj) {
    object = obj;

    clipSize = 12;
    depotSize = 1;
    remainingAmmo = clipSize;
    remainingClips = depotSize;

    if (WEAPONSOUND) {
        ALuint buffer = alutCreateBufferFromFile("data/org/freesound/raybeam.wav");
        alGenSources(1, &soundSource);
        alSourcei(soundSource, AL_BUFFER, buffer);
        alSourcef(soundSource, AL_PITCH, 1.0f);
        alSourcef(soundSource, AL_GAIN, 1.0f);
        alSourcei(soundSource, AL_LOOPING, AL_FALSE);
    }
};

void rWeaponRaybeam::fire() {
    if (!ready()) return;
    triggered = true;

    if (remainingAmmo > 0) {
        remainingAmmo--;
        if (remainingAmmo == 0 && remainingClips != 0) {
            timeReloading = 2.0;
        } else {
            timeReloading = 2.0;
            timeFiring = 0.7;
        }
    }

    playSourceIfNotPlaying();
}

void rWeaponRaybeam::animate(float spf) {
    triggereded = triggered;
    if (trigger) fire();
    trigger = false;

    timeReloading -= spf;
    if (timeReloading < 0) {
        timeReloading = 0.0f;
        if (remainingAmmo == 0 && remainingClips != 0) {
            remainingAmmo = clipSize;
            if (remainingClips > 0) remainingClips--;
        }
    }

    timeFiring -= spf;
    if (timeFiring < 0) {
        timeFiring = 0.0f;
        return;
    }

    float* source = weaponPosef;

    // Vector that points to the Mountpoint relative to the eye.
    float* mpnt = &source[12];
    //vector_print(mpnt);

    // Mountpoint-Forward Vector relative to the eye.
    float* source_8 = &source[8];
    float mfwd[3];
    vector_cpy(mfwd, source_8);
    //vector_print(mfwd);
    vector_norm(mfwd, mfwd);

    // Interpolate single points towards the "tip" of the ray.
    float offset = 80 * (1.0f - (timeFiring / 0.7));
    float worldpos[] = {0, 0, 0};
    vector_scale(worldpos, mfwd, -offset);
    vector_add(worldpos, worldpos, mpnt);
    //vector_print(worldpos);

    // Collide and Damage with single point on ray.
    {
        float radius = 0.5;
        int roles = 0;
        float damage = 25;
        int damaged = damageByParticle(worldpos, radius, roles, damage);
        // Stop ray on collision.
        if (damaged > 0) timeFiring = 0;
    }
}

void rWeaponRaybeam::drawSolid() {
    if (drawWeapon) {
        GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GLS::glUseProgram_fglitcolor();

            GL::glPushMatrix();
            {
                transformTo();
                GL::glRotatef(-90, 1, 0, 0);

                // Scale
                float scale = weaponScale;
                GL::glScalef(scale, scale, scale);

                if (this->ready() == 0 && remainingAmmo != 0) {
                }

                GL::glColor4f(0.3, 0.2, 0.3, 1.0);
                GL::glPushMatrix();
                {
                    GL::glScalef(0.1, 0.14, 0.12);
                    Primitive::glCenterUnitBlock();
                }
                GL::glPopMatrix();

                GL::glColor4f(0.1, 0.1, 0.1, 1.0);
                GL::glPushMatrix();
                {
                    GL::glTranslatef(0.0, 0.7, 0);
                    GL::glScalef(0.03, 0.7, 0.03);
                    Primitive::glCenterUnitCylinder(7);
                    GL::glTranslatef(0.0, 1.0, 0);
                    GL::glScalef(2.0, 0.1, 2.0);
                    GL::glColor4f(0.4, 0.4, 0.4, 1.0);
                    Primitive::glCenterUnitCylinder(7);
                }
                GL::glPopMatrix();
            }
            GL::glPopMatrix();
        }
        GL::glPopAttrib();
    }
}

void rWeaponRaybeam::drawEffect() {
    if (timeFiring == 0) return;

    const float len = 10;

    struct appearance {
        float width, length;
        rgba icolor;
        rgba ocolor;
    } app[] = {
        {
            0.10, 8 * len,
            {0.3, 0.3, 0.3, 1},
            {0.3, 0.0, 0.0, 1},
        },
        {
            0.40, 8 * len,
            {0.5, 0.5, 0.5, 1},
            {0.3, 0.0, 0.0, 1},
        },
        {
            0.80, 8 * len,
            {0.9, 0.9, 0.9, 1},
            {0.3, 0.0, 0.0, 1},
        },
        {
            0.85, 8 * len,
            {1.0, 1.0, 1.0, 1},
            {0.4, 0.0, 0.0, 1},
        },
        {
            0.90, 8.2 * len,
            {1.0, 1.0, 1.0, 1},
            {0.7, 0.0, 0.0, 1},
        },
        {
            0.60, 8.4 * len,
            {0.9, 0.9, 0.9, 1},
            {0.3, 0.0, 0.0, 1},
        },
        {
            0.30, 8.5 * len,
            {0.7, 0.7, 0.7, 1},
            {0.3, 0.0, 0.0, 1},
        }
    };

    unsigned int lifeindex = (int) ((0.7 - timeFiring) * 10);
    if (lifeindex > sizeof (app) / sizeof (appearance) - 1) lifeindex = sizeof (app) / sizeof (appearance) - 1;
    if (lifeindex < 0) lifeindex = 0;

    GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GLS::glUseProgram_fgaddcolor();

        GL::glPushMatrix();
        {
            transformTo();

            float m[16];
            GL::glGetFloatv(GL_MODELVIEW_MATRIX, m);
            float* mpnt = &m[12]; // Vector that points to the Mountpoint relative to the eye.
            //vector_print(mpnt);
            float* mfwd = &m[8]; // Mountpoint-Forward Vector relative to the eye.
            //vector_print(mfwd);

            float right[3];
            vector_cross(right, mpnt, mfwd);
            vector_norm(right, right);
            //vector_print(right);

            //if (mpnt[2] > 0 || mfwd[2] < 0) right[1] = -right[1]; // Correction
            //if (mpnt[1] > 0 || mfwd[1] < 0) right[1] = -right[1]; // Correction

            const float w = app[lifeindex].width;
            right[0] *= -w;
            right[1] *= -w;
            right[2] *= -w;
            GL::glBegin(GL_TRIANGLE_STRIP);
            GL::glColor4fv(app[lifeindex].ocolor);
            GL::glVertex3f(-right[0], -right[1], -right[2] - 2 * w);
            GL::glVertex3f(-right[0], -right[1], -right[2] - app[lifeindex].length);
            GL::glColor4fv(app[lifeindex].icolor);
            GL::glVertex3f(0, 0.0, 0);
            GL::glVertex3f(0, 0.0, -app[lifeindex].length);
            GL::glColor4fv(app[lifeindex].ocolor);
            GL::glVertex3f(+right[0], +right[1], +right[2] - 2 * w);
            GL::glVertex3f(+right[0], +right[1], +right[2] - app[lifeindex].length);
            GL::glEnd();

        }
        GL::glPopMatrix();
    }
    GL::glPopAttrib();
}

void rWeaponRaybeam::drawHUD() {
    float a = 0.9;
    float b = 0.6;

    // Ammo
    float i = 0.1;
    float j = 0.9;
    float r = i + (remainingAmmo / (float) clipSize) * (j - i);
    GL::glBegin(GL_QUADS);
    GL::glColor4f(1, 1, 0, b);
    GL::glVertex3f(i, j, 0.0);
    GL::glVertex3f(i, i, 0.0);
    GL::glColor4f(1, 0, 0, b);
    GL::glVertex3f(r, i, 0.0);
    GL::glVertex3f(r, j, 0.0);
    GL::glEnd();

    // Clips
    float l = 0.1;
    float h = 0.15;
    r = i + (remainingClips / (float) depotSize) * (j - i);
    GL::glBegin(GL_QUADS);
    GL::glColor4f(0, 0, 0, a);
    GL::glVertex3f(l, i + h, 0.0);
    GL::glVertex3f(l, i, 0.0);
    GL::glColor4f(1, 1, 1, a);
    GL::glVertex3f(r, i, 0.0);
    GL::glVertex3f(r, i + h, 0.0);
    GL::glEnd();

    GL::glBegin(GL_LINES);
    GL::glColor4f(1, 0.3, 0.3, b);
    GL::glVertex3f(0.25, 0.5, 0.0);
    GL::glColor4f(1, 0.6, 0.6, a);
    GL::glVertex3f(0.75, 0.5, 0.0);
    GL::glEnd();
}
