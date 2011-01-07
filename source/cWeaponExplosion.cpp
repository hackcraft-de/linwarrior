#include "cWeaponExplosion.h"

#include "psi3d/snippetsgl.h"

#include <cassert>

cWeaponExplosion::cWeaponExplosion() {
    if (EXPLOSIONSOUND) {
        unsigned buffer;
        //buffer = alutCreateBufferHelloWorld();
        buffer = alutCreateBufferFromFile("data/freesound/explosion.wav");
        alGenSources(1, &soundSource);
        //if (alGetError() != AL_FALSE || !alIsSource(soundSource)) throw "sorry, could not create audio source for explosion";
        alSourcei(soundSource, AL_BUFFER, buffer);
        alSourcef(soundSource, AL_PITCH, 1.0f);
        alSourcef(soundSource, AL_GAIN, 10.0f);
        alSourcei(soundSource, AL_LOOPING, AL_FALSE);
    }
}

void cWeaponExplosion::fire(OID target) {
    if (!ready()) return;
    timeReloading = 3.8;

    // Shoot several shrapnels around.

    loopi(11) {

        float* source = weaponPosef;

        cParticle* s = new cParticle();
        assert(s != NULL);
        s->target = target;
        s->fuel = 5;
        s->spawn = 0.0f;
        s->timer = 0.0f;

        s->pos[0] = source[12];
        s->pos[1] = source[13];
        s->pos[2] = source[14];

        float nrm[3];
        float pos2[] = {0, 0, -1};

        pos2[0] = (100 - (rand() % 200)) * 0.01;
        pos2[1] = 0.4 + (100 - (rand() % 200)) * 0.01;
        pos2[2] = (100 - (rand() % 200)) * 0.01;

        matrix_apply2(source, pos2);
        vector_sub(nrm, pos2, s->pos);

        vector_scale(s->vel, nrm, 1);
        missileParticles.push_back(s);
    } // for

    playSourceIfNotPlaying();
}

void cWeaponExplosion::animate(float spf) {

    foreachNoInc(i, missileParticles) {
        cParticle* s = *i++;

        // Update Smoke from Missile Trails.
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

        const int MAX_SMOKE = 100;
        // Add new Smoke-Particle whenever an old Smoke-Particel disappears
        // or at specified intervals.
        s->spawn -= spf;
        if (((int) s->trail.size() < MAX_SMOKE) && (s->spawn <= 0 || removed_smoke)) {
            s->spawn = 0.12;
            cParticle* smoke = new cParticle();
            assert(smoke != NULL);
            smoke->pos = s->pos;
            //vector_cpy(smoke->pos, s->pos);
            smoke->fuel = 1.2f;
            smoke->timer = 0.0f;
            s->trail.push_back(smoke);
        }

        const float alpha = 0.992f;
        loop3i(s->vel[i] *= alpha);

        const float speed = 5;
        s->pos[0] += speed * s->vel[0] * spf;
        s->pos[1] += speed * s->vel[1] * spf;
        s->pos[2] += speed * s->vel[2] * spf;
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
            float damage = 10;
            int damaged = this->damageByParticle(s->pos.data(), radius, roles, damage);
            if (damaged) {
                missileParticles.remove(s);
                delete s;
            }
        }
    }

    timeReloading -= spf;
    if (timeReloading < 0) timeReloading = 0.0f;
};

void cWeaponExplosion::drawSolid() {
    return; // !!

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {

        glEnable(GL_NORMALIZE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        glColor4f(0.7, 0.7, 0.7, 1.0);

        const float size = 0.8 * this->weaponScale;
        const float length = size * 0.4;
        const float radius = size * 0.8;
        const float s1 = radius * sin(0 * PI_OVER_180);
        const float c1 = radius * cos(0 * PI_OVER_180);
        const float s2 = radius * sin(120 * PI_OVER_180);
        const float c2 = radius * cos(120 * PI_OVER_180);
        const float s3 = radius * sin(240 * PI_OVER_180);
        const float c3 = radius * cos(240 * PI_OVER_180);

        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);

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

void cWeaponExplosion::drawEffect() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDepthMask(GL_FALSE);

        float n[16];
        SGL::glGetTransposeInverseRotationMatrix(n);

        /*
        // Draw Missile's flare.
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        foreachNoInc(i, mMissiles) {
            Particle* s = *i++;

            glPushMatrix();
            {
                glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
                glMultMatrixf(n);
                float size = 2.5 + 2.5 * 0.01f * (rand() % 100);
                glColor4f(0.8, 0.5, 0.1, 0.99f);
                cPrimitives::(9 + DETAIL, size * 0.1f);
                size = 2.5 + 2.5 * 0.01f * (rand() % 100);
                glColor4f(0.9, 0.9, 0.9, 0.99f);
                cPrimitives::(7 + DETAIL, size * 0.07f);
            }
            glPopMatrix();
        }
         */

        // Draw Missile's trails.
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_TEXTURE_2D);
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
                    glColor4f(m, m * 0.8, m * 0.6, 0.07 + smoke->fuel);
                    float size = 0.1 + 0.3 * pow(1.0 + smoke->timer, 3.5);
                    cPrimitives::glDisk(5 + WEAPONDETAIL, size);
                }
                glPopMatrix();
            }
        }

    }
    glPopAttrib();
}
