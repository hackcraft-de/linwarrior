#include "rWeaponExplosion.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include <cassert>


std::string rWeaponExplosion::cname = "EXPLOSION";
unsigned int rWeaponExplosion::cid = 4370;

rWeaponExplosion::rWeaponExplosion(Entity* obj) {
    object = obj;

    if (EXPLOSIONSOUND) {
        loadSource("data/org/freesound/explosion.wav", 1.0f, 10.0f);
    }
}

void rWeaponExplosion::fire() {
    if (!ready()) return;
    triggered = true;

    timeReloading = 3.8;
    timeFiring = 3.0f;
    // Shoot several shrapnels around.

    //loopi(11)
    if (0) {

        float* source = weaponPosef;

        Particle* s = new Particle();
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

void rWeaponExplosion::animate(float spf) {
    triggereded = triggered;
    if (trigger) fire();
    trigger = false;

    if (timeFiring != 0.0f) timeFiring -= spf;

    if (timeFiring < 0) {
        //timeFiring = 0.0f;
    } else if (timeFiring > 0) {
        float* source = weaponPosef;

        Particle* s = new Particle();
        assert(s != NULL);
        s->target = 0;
        s->fuel = 0.5 + 0.3 * 0.01 * (rand() % 100);
        s->spawn = 0.0f;
        s->timer = 0.0f;

        s->pos[0] = source[12] + (100.0f - (rand() % 200)) * 0.01;
        s->pos[1] = source[13] + (100.0f - (rand() % 200)) * 0.01;
        s->pos[2] = source[14] + (100.0f - (rand() % 200)) * 0.01;

        float nrm[3];
        float pos2[] = {0, 0, -1};

        pos2[0] = (100 - (rand() % 200)) * 0.01;
        pos2[1] = 0.4 + (100 - (rand() % 200)) * 0.01;
        pos2[2] = (100 - (rand() % 200)) * 0.01;

        matrix_apply2(source, pos2);
        vector_sub(nrm, pos2, s->pos);

        vector_scale(s->vel, nrm, 1);
        missileParticles.push_back(s);
    }

    // Fat smoke production.
    if (timeFiring < 0.0f) {
        if (timeFiring < -4.0f * 60.0f) timeFiring = 0.0f;

        float* source = weaponPosef;

        Particle* s = new Particle();
        assert(s != NULL);
        s->target = 0;
        s->fuel = 1.4 + 0.1 * 0.01 * (rand() % 100);
        s->spawn = 0.0f;
        s->timer = 0.0f;

        s->pos[0] = source[12] + (100.0f - (rand() % 200)) * 0.01;
        s->pos[1] = source[13] + (100.0f - (rand() % 200)) * 0.01;
        s->pos[2] = source[14] + (100.0f - (rand() % 200)) * 0.01;

        float nrm[3];
        float pos2[] = {0, 0, -1};

        pos2[0] = 0.1f * (100 - (rand() % 200)) * 0.01;
        pos2[1] = 1.2f + 0.1 * fabs((100 - (rand() % 200)) * 0.01);
        pos2[2] = 0.1f * (100 - (rand() % 200)) * 0.01;

        matrix_apply2(source, pos2);
        vector_sub(nrm, pos2, s->pos);

        vector_scale(s->vel, nrm, 1);
        castoffParticles.push_back(s);
    }

    // Fat smoke movement.

    foreachNoInc(i, castoffParticles) {
        Particle* s = *i++;

        const float alpha = 0.922f;
        const float beta = 0.99f;
        s->vel[0] *= alpha;
        s->vel[1] *= beta;
        s->vel[2] *= alpha;

        const float speed = 5;
        vector_muladd(s->pos, s->pos, s->vel, speed * spf);
        s->fuel -= spf;
        s->timer += spf;
        if (s->fuel < 0) {
            castoffParticles.remove(s);
            delete s;
        }
    }

    foreachNoInc(i, missileParticles) {
        Particle* s = *i++;

        // Update Smoke from Missile Trails.
        bool removed_smoke = false;

        foreachNoInc(j, s->trail) {
            Particle* smoke = *j++;
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
            Particle* smoke = new Particle();
            assert(smoke != NULL);
            vector_cpy(smoke->pos, s->pos);
            smoke->fuel = 1.2f;
            smoke->timer = 0.0f;
            s->trail.push_back(smoke);
        }

        const float alpha = 0.992f;
        loop3i(s->vel[i] *= alpha);

        const float speed = 5;
        vector_muladd(s->pos, s->pos, s->vel, speed * spf);
        s->fuel -= spf;
        s->timer += spf;

        // When fuel is empty just let the Missile and all
        // it's Smoke-Particles disappear.
        if (s->fuel < 0) {

            foreachNoInc(j, s->trail) {
                Particle* smoke = *j++;
                s->trail.remove(smoke);
            }
            missileParticles.remove(s);
            delete s;
        } else {
            float radius = 0.25;
            int roles = 0;
            float damage = 10;
            int damaged = this->damageByParticle(s->pos, radius, roles, damage);
            if (damaged) {
                missileParticles.remove(s);
                delete s;
            }
        }
    }

    timeReloading -= spf;
    if (timeReloading < 0) timeReloading = 0.0f;
};

void rWeaponExplosion::drawSolid() {
    return; // !!

    GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GLS::glUseProgram_fglitcolor();

        GL::glColor4f(0.7, 0.7, 0.7, 1.0);

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
        GL::glGetFloatv(GL_MODELVIEW_MATRIX, m);

        foreachNoInc(i, missileParticles) {
            Particle* s = *i++;

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

            GL::glPushMatrix();
            {
                GL::glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
                GL::glMultMatrixf(b);
                //glNormAxis();
                GL::glBegin(GL_TRIANGLE_FAN);
                GL::glNormal3f(0, 0, 1);
                GL::glVertex3f(0, 0, length);
                GL::glVertex3f(s1, c1, 0);
                GL::glVertex3f(s2, c2, 0);
                GL::glVertex3f(s3, c3, 0);
                GL::glVertex3f(s1, c1, 0);
                GL::glEnd();
            }
            GL::glPopMatrix();
        }

    }
    GL::glPopAttrib();
}

void rWeaponExplosion::drawEffect() {
    if (missileParticles.empty() && castoffParticles.empty()) return;

#if 0
    // Experiment with hardware Point-Sprites
    GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GLS::glUseProgram_fgaddtexture();
        GL::glBindTexture(GL_TEXTURE_2D, sTextures[0]);
        /*
        GL::glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        GL::glEnable(GL_POINT_SPRITE);
        glPointSize(90.0f);
         */

        //float quadratic[] =  { 0.0f, 0.0f, 0.01f };
        //float quadratic[] =  { 1.0f, 1.0f, 100.0f };
        float quadratic[] = {1.0f, 0.0f, 0.0f};
        glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);
        float maxSize = 0.0f;
        GL::glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);
        glPointSize(maxSize);
        logger->trace() << maxSize << "\n";
        glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, maxSize);
        glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
        glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 50.0f);
        GL::glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
        GL::glEnable(GL_POINT_SPRITE_ARB);

        /*
        GL::glEnable(GL_POINT_SPRITE_NV);
        GL::glTexEnvf(GL_POINT_SPRITE_NV,GL_COORD_REPLACE_NV,GL_TRUE);
        //glPointParameterf(GL_POINT_SPRITE_R_MODE_NV,GL_ZERO);
        glPointSize(100.0);
         */

        //glPointSize(9.0f);

        foreachNoInc(i, missileParticles) {
            Particle* s = *i++;

            GL::glBegin(GL_POINTS);

            foreachNoInc(j, s->trail) {
                Particle* smoke = *j++;
                float m = (0.0f + 0.7 * smoke->fuel);
                float a = 1.0f; //fmin(1.0f, smoke->fuel);
                GL::glColor4f(a*m, a * m * 0.8, a * m * 0.6, 0.9);
                GL::glVertex3f(smoke->pos[0], smoke->pos[1], smoke->pos[2]);
            }
            GL::glEnd();
        }
    }
    GL::glPopAttrib();

    return;
#endif

    GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GLS::glUseProgram_fgaddtexture();

        float n[16];
        GLS::glGetTransposeInverseRotationMatrix(n);

        // Draw Missile's trails.

        GL::glBindTexture(GL_TEXTURE_2D, sTextures[0]);

        foreachNoInc(i, missileParticles) {
            Particle* s = *i++;

            foreachNoInc(j, s->trail) {
                Particle* smoke = *j++;
                GL::glPushMatrix();
                {
                    GL::glTranslatef(smoke->pos[0], smoke->pos[1], smoke->pos[2]);
                    GL::glMultMatrixf(n);
                    float m = (0.0f + 0.7 * smoke->fuel);
                    float a = 1.0f; //fmin(1.0f, smoke->fuel);
                    GL::glColor4f(a*m, a * m * 0.8, a * m * 0.6, 0.1);
                    //float size = 0.1 + 0.3 * pow(1.0 + smoke->timer, 3.5);
                    //float size = 5.0f / (1.0f + 1.0f * smoke->fuel);
                    float size = 3.0f + 5.0f / (1.0f + 9.0f * smoke->fuel);
                    Primitive::glXYCenteredTextureSquare(size);
                }
                GL::glPopMatrix();
            }
        }

        GLS::glUseProgram_fgplaintexture();
        GL::glDisable(GL_CULL_FACE);
        //GL::glEnable(GL_ALPHA_TEST);

        foreachNoInc(i, castoffParticles) {
            Particle* smoke = *i++;
            GL::glPushMatrix();
            {
                GL::glTranslatef(smoke->pos[0], smoke->pos[1], smoke->pos[2]);
                GL::glMultMatrixf(n);
                float total = smoke->fuel + smoke->timer;
                float alpha = smoke->fuel / total;
                float beta = 1.0f - alpha;
                float grey = fmax(alpha, beta) * 0.7;
                GL::glColor4f(grey, grey, grey, 0.5 + cos(0.5 * M_PI * alpha));
                float size = 0.5f + 1.5f * (smoke->timer / total);
                Primitive::glXYCenteredTextureSquare(size);
            }
            GL::glPopMatrix();
        }

    }
    GL::glPopAttrib();
}
