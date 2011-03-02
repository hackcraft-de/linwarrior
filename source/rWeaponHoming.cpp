#include "rWeaponHoming.h"

#include "cWorld.h"

#include "psi3d/snippetsgl.h"

#include <cassert>

rWeaponHoming::rWeaponHoming(cObject* obj) {
    role = "HOMING";
    object = obj;
    
    clipSize = 4;
    depotSize = 7;
    remainingAmmo = clipSize;
    remainingClips = depotSize;

    if (WEAPONSOUND) {
        ALuint buffer = alutCreateBufferFromFile("data/freesound.org/homingmissile.wav");
        alGenSources(1, &soundSource);
        alSourcei(soundSource, AL_BUFFER, buffer);
        alSourcef(soundSource, AL_PITCH, 1.0f);
        alSourcef(soundSource, AL_GAIN, 1.0f);
        alSourcei(soundSource, AL_LOOPING, AL_FALSE);
    }
}

void rWeaponHoming::fire(OID target) {
    if (!ready()) return;

    if (remainingAmmo > 0) {
        remainingAmmo--;
        if (remainingAmmo == 0 && remainingClips != 0) {
            timeReloading = 2.0;
        } else {
            timeReloading = 0.8;
        }
    }

    if (target == 0 && object != NULL) {
        target = object->enemyNearby();
    }

    float* source = weaponPosef;

    cParticle* s = new cParticle();
    assert(s != NULL);
    s->target = target;
    s->fuel = 4;
    s->spawn = 0.0f;
    s->timer = 0.0f;

    float* pos = &source[12];
    vector_cpy(s->pos, pos);

    float nrm[3];
    float pos2[] = {0, 0, -1};

    // Variation of direction.
    //pos2[0] += (100-(rand()%200)) * 0.005;
    //pos2[1] += (100-(rand()%200)) * 0.005 + 0.1;
    //pos2[2] += (100-(rand()%200)) * 0.01;

    matrix_apply2(source, pos2);
    vector_sub(nrm, pos2, s->pos);

    vector_scale(s->vel, nrm, 1);
    missileParticles.push_back(s);

    playSource();
}

void rWeaponHoming::animate(float spf) {

    foreachNoInc(i, missileParticles) {
        cParticle* s = *i++;

        // Update Particle from Missile Trails.
        bool removed_smoke = false;

        foreachNoInc(j, s->trail) {
            cParticle* smoke = *j++;
            smoke->fuel -= spf;
            smoke->timer += spf;
            if (smoke->fuel < 0) {
                s->trail.remove(smoke);
                removed_smoke = true;
            }
        }

        const int MAX_SMOKE = 50;
        // Add new Smoke-Particle whenever an old Smoke-Particel disappears
        // or at specified intervals.
        s->spawn -= spf;
        if (((int) s->trail.size() < MAX_SMOKE) && (s->spawn <= 0 || removed_smoke)) {
            s->spawn = 0.04;
            cParticle* smoke = new cParticle();
            assert(smoke != NULL);
            vector_cpy(smoke->pos, s->pos);
            smoke->fuel = 0.9f;
            smoke->timer = 0.0f;
            s->trail.push_back(smoke);
        }

        // Now update the Missile itself
        if (s->target) {
            cObject *targetobject = cWorld::instance->getObject(s->target);
            if (targetobject && true) {
                float tgt[] = {0, 0, 0};
                vector_cpy(tgt, targetobject->traceable->pos);
                float dir[] = {tgt[0] - s->pos[0], tgt[1] - s->pos[1], tgt[2] - s->pos[2]};
                vector_norm(dir, dir);
                vector_norm(s->vel, s->vel);
                const float alpha = 0.96;
                loop3i(s->vel[i] = (s->vel[i] * alpha + dir[i]*(1 - alpha)));
            }
        }
        vector_norm(s->vel, s->vel);
        const float speed = 30;
        vector_muladd(s->pos, s->pos, s->vel, speed * spf);
        s->fuel -= spf;
        s->timer += spf;

        // When fuel is empty just let the Missile and all
        // it's Smoke-Particles disappear.
        if (s->fuel < 0) {

            foreachNoInc(j, s->trail) {
                cParticle* smoke = *j++;
                s->trail.remove(smoke);
            }
            missileParticles.remove(s);
            delete s;
        } else {
            float radius = 0.25;
            int roles = 0;
            float damage = 16;
            int damaged = this->damageByParticle(s->pos, radius, roles, damage);
            if (damaged) {
                missileParticles.remove(s);
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
};

void rWeaponHoming::drawSolid() {
    // Missile barrels
    if (drawWeapon) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            SGL::glUseProgram_fglitcolor();

            glPushMatrix();
            {
                transformTo();
                glRotatef(-90, 1, 0, 0);

                // Scale
                float scale = weaponScale;
                glScalef(scale, scale, scale);

                glColor4f(0.3, 0.3, 0.3, 1.0);
                glPushMatrix();
                {
                    glTranslatef(0, 0.75, 0);
                    glScalef(0.065, 0.75, 0.065);
                    cPrimitives::glCenterUnitCylinder(7);

                }
                glPopMatrix();

                glColor4f(0.3, 0.3, 0.2, 1.0);
                glPushMatrix();
                {
                    glScalef(0.1, 0.14, 0.12);
                    cPrimitives::glCenterUnitBlock();
                }
                glPopMatrix();

                glColor4f(0.1, 0.1, 0.1, 1.0);
                if (this->ready() == 0 && remainingAmmo != 0 && object != NULL) {
                    glRotatef(object->seconds * 1 * 90, 0, 1, 0);
                }
                int n = 4;

                loopi(n) {
                    glPushMatrix();
                    {
                        glRotatef(i * 360 / n, 0, 1, 0);
                        glTranslatef(0.075, 0.8, 0);
                        glScalef(0.06, 0.8, 0.06);
                        cPrimitives::glCenterUnitCylinder(7);
                    }
                    glPopMatrix();
                }
            }
            glPopMatrix();
        }
        glPopAttrib();
    }

    // Missiles themselves:
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        SGL::glUseProgram_fglitcolor();

        const float size = 0.8;
        const float length = size * 1.4;
        const float radius = size * 0.2;
        const float s1 = radius * sin(0 * PI_OVER_180);
        const float c1 = radius * cos(0 * PI_OVER_180);
        const float s2 = radius * sin(120 * PI_OVER_180);
        const float c2 = radius * cos(120 * PI_OVER_180);
        const float s3 = radius * sin(240 * PI_OVER_180);
        const float c3 = radius * cos(240 * PI_OVER_180);

        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        glColor4f(0.7, 0.7, 0.7, 1.0);

        foreachNoInc(i, missileParticles) {
            cParticle* s = *i++;

            float r[3];
            vector_cross(r, s->vel, (&m[4]));
            float u[3];
            vector_cross(u, s->vel, r);
            float b[16];
            memset(b, 0, sizeof (float) *16);
            float* r_vec = &b[0];
            float* u_vec = &b[4];
            float* d_vec = &b[8];
            vector_cpy(r_vec, r);
            vector_cpy(u_vec, u);
            vector_cpy(d_vec, s->vel);
            b[15] = 1;
            //matrix_print(b);

            glPushMatrix();
            {
                glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
                glMultMatrixf(b);
                //glNormAxis();
                glBegin(GL_TRIANGLE_FAN);
                glNormal3f(0,0,1);
                glVertex3f(0, 0, length);
                glVertex3f(s1, c1, 0);
                glVertex3f(s2, c2, 0);
                glVertex3f(s3, c3, 0);
                glVertex3f(s1, c1, 0);
                glEnd();
            }
            glPopMatrix();
        }

    }
    glPopAttrib();
}

void rWeaponHoming::drawEffect() {
    if (missileParticles.empty()) return;
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        SGL::glUseProgram_fgaddcolor();

        float n[16];
        SGL::glGetTransposeInverseRotationMatrix(n);

        // Draw Missile's flare.

        foreachNoInc(i, missileParticles) {
            cParticle* s = *i++;

            glPushMatrix();
            {
                glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
                glMultMatrixf(n);
                float size = 2.5 + 2.5 * 0.01f * (rand() % 100);
                glColor4f(0.8, 0.5, 0.1, 0.99f);
                cPrimitives::glDisk(9 + WEAPONDETAIL, size * 0.1f);
                size = 2.5 + 2.5 * 0.01f * (rand() % 100);
                glColor4f(0.9, 0.9, 0.9, 0.99f);
                cPrimitives::glDisk(7 + WEAPONDETAIL, size * 0.07f);
            }
            glPopMatrix();
        } // missiles

        // Draw Missile's trails.
        SGL::glUseProgram_fgaddtexture();
        
        glBindTexture(GL_TEXTURE_2D, sTextures[0]);

        foreachNoInc(i, missileParticles) {
            cParticle* s = *i++;

            foreachNoInc(j, s->trail) {
                cParticle* smoke = *j++;
                glPushMatrix();
                {
                    glTranslatef(smoke->pos[0], smoke->pos[1], smoke->pos[2]);
                    glMultMatrixf(n);
                    float m = 2 * smoke->fuel;
                    glColor4f(m, m * 0.8, m * 0.6, 0.05 + smoke->fuel);
                    float size = 0.1 + 0.2 * pow(1.0 + smoke->timer, 2.0);
                    cPrimitives::glDisk(5 + WEAPONDETAIL, size);
                }
                glPopMatrix();
            } // trails
        } // missiles

    }
    glPopAttrib();
}

void rWeaponHoming::drawHUD() {
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
    glColor4f(0.3, 1, 0.3, b);
    glVertex3f(0.4, 0.4, 0.0);
    glVertex3f(0.6, 0.6, 0.0);
    glVertex3f(0.6, 0.4, 0.0);
    glVertex3f(0.4, 0.6, 0.0);
    glVertex3f(0.5, 0.4, 0.0);
    glVertex3f(0.5, 0.6, 0.0);
    glVertex3f(0.4, 0.5, 0.0);
    glVertex3f(0.6, 0.5, 0.0);
    glEnd();
}

