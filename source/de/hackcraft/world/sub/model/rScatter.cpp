/* 
 * File:   rScatter.cpp
 * Author: benben
 * 
 * Created on June 8, 2011, 10:33 PM
 */

#include "rScatter.h"

#include "de/hackcraft/psi3d/ctrl.h"
#include "de/hackcraft/psi3d/Particle.h"
#include "de/hackcraft/psi3d/Primitive.h"
#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/io/Texfile.h"

#include <cassert>

std::string rScatter::cname = "SCATTER";
unsigned int rScatter::cid = 8412;

int rScatter::sInstances = 0;
std::map<int, long> rScatter::sTextures;

rScatter::rScatter(Entity* obj, float radius, float density) {
    object = obj;

    sInstances++;
    if (sInstances == 1) {
        {
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = Texfile::loadTGA("data/base/landscape/plant/plant_desert.tga", &w, &h, &bpp);
            texname = GLS::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete[] texels;
            sTextures[0] = texname;
        }
    }

    vector_zero(pos0);
    quat_zero(ori0);

    int amount = density * 4 * radius * M_PI;
    for (int i = 0; i < amount; i++) {
        float x_ = 0 + (rand() % 1000) * 0.001 * 2 * radius - radius;
        float y_ = 0;
        float z_ = 0 + (rand() % 1000) * 0.001 * 2 * radius - radius;
        float r_ = (rand() % 360);
        Particle* p = new Particle();
        assert(p != NULL);
        vector_set(p->pos, x_, y_, z_);
        p->spawn = r_;
        decalParticles.push_back(p);
    }
}

rScatter::rScatter(const rScatter& orig) {
}

rScatter::~rScatter() {
}

void rScatter::drawEffect() {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glColor4f(0.3, 0.7, 0.3, 1);
        glBindTexture(GL_TEXTURE_2D, sTextures[0]);

        foreachNoInc(i, decalParticles) {
            Particle* p = *i++;
            glPushMatrix();
            {
                glTranslatef(p->pos[0], p->pos[1], p->pos[2]);
                glRotatef(p->spawn, 0, 1, 0);
                //glAxis(0.9);
                Primitive::glXYCenteredTextureSquare();
            }
            glPopMatrix();
        }
    }
    glPopAttrib();
}
