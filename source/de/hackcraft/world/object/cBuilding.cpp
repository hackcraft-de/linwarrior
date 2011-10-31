// cBuilding.cpp

#include "cBuilding.h"

#include "de/hackcraft/world/comp/behavior/rDamageable.h"
#include "de/hackcraft/world/World.h"

#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include "de/hackcraft/proc/Noise.h"
#include "de/hackcraft/proc/Distortion.h"
#include "de/hackcraft/proc/Facade.h"

#include "de/hackcraft/io/Texfile.h"

#include <cassert>

#include <iostream>
using std::cout;
using std::endl;

#define BUILDINGDETAIL 0


int cBuilding::sInstances = 0;
std::map<int, long> cBuilding::sTextures;

cBuilding::cBuilding(int x, int y, int z, int rooms_x, int rooms_y, int rooms_z) {
    sInstances++;
    if (sInstances == 1) {
        unsigned int texname;

        {
            cout << "Generating Rooftops..." << endl;
            int w = 1 << (6 + BUILDINGDETAIL);
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float c4f[4];
                    Facade::getConcrete(1 * (float) i / (float) w, 1 * (float) j / (float) h, 0, c4f);
                    *p++ = c4f[2] * 255;
                    *p++ = c4f[1] * 255;
                    *p++ = c4f[0] * 255;
                }
            }
            texname = GLS::glBindTexture2D(0, true, false, true, true, w, h, bpp, texels);
            delete texels;
            sTextures[0] = texname;
        }

        {
            //glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
            cout << "Generating Facades..." << endl;

            loopk(17) {
                int w = 1 << (8 + BUILDINGDETAIL);
                int h = w;
                int bpp = 3;
                unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
                unsigned char* p = texels;

                loopj(h) {

                    loopi(w) {
                        float a = (0.0f + ((float) i / (float) w)) * 3.0f;
                        float b = (1.0f - ((float) j / (float) h)) * 3.0f;
                        int gx = a;
                        int gy = b;
                        float x = a - gx;
                        float y = b - gy;
                        float c[3];
                        Facade::getFacade(x, y, gx, gy, 0, c, 131 + k * 31);
                        *p++ = c[2] * 255;
                        *p++ = c[1] * 255;
                        *p++ = c[0] * 255;
                    }
                }

                texname = GLS::glBindTextureMipmap2D(0, true, true, true, true, w, h, texels);
                //texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
                sTextures[k + 1] = texname;

                if (0) {
                    try {
                        char numb[3] = { char('0' + (k / 10)), char('0' + (k % 10)), '\0' };
                        std::string fname = std::string("data/base/urban/facade/facade_") + std::string(numb) + std::string(".tga");
                        if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                            cout << "Could not save image" << endl;
                        }
                    } catch (char const* s) {
                        cout << "ERROR: " << s << endl;
                    }
                }
                delete texels;
            }
        }
    }

    damageable = new rDamageable;

    buildingRooms[0] = 3 * rooms_x;
    buildingRooms[1] = 3 * rooms_y;
    buildingRooms[2] = 3 * rooms_z;

    vector_set(this->pos0, x + buildingRooms[0]*0.5f, y, z + buildingRooms[2]*0.5f);
    quat_set(this->ori0, 0, 0, 0, 1);
    this->radius = sqrtf(0.25f * buildingRooms[0] * buildingRooms[0] + 0.25f * buildingRooms[1] * buildingRooms[1] + 0.25f * buildingRooms[2] * buildingRooms[2]);
}

void cBuilding::damage(float* localpos, float damage, Entity* enactor) {
    if (!damageable->alife) return;

    if (damage > 0) {
        int body = rDamageable::BODY;
        damageable->hp[body] -= damage;
        if (damageable->hp[body] < 0) damageable->hp[body] = 0;
        if (damageable->hp[body] <= 0) {
            damageable->alife = false;
            cout << "cBuilding::damageByParticle(): DEAD\n";
            explosionObject.trigger = true;
        }
        if (damageable->hp[body] <= 75) addTag(World::instance->getGroup(HLT_WOUNDED));
        if (damageable->hp[body] <= 50) addTag(World::instance->getGroup(HLT_SERIOUS));
        if (damageable->hp[body] <= 25) addTag(World::instance->getGroup(HLT_CRITICAL));
        if (damageable->hp[body] <= 0) addTag(World::instance->getGroup(HLT_DEAD));
    }
}

float cBuilding::constrain(float* worldpos, float radius, float* localpos, Entity* enactor) {
    float localpos_[3];

    {
        float ori_inv[4];
        quat_cpy(ori_inv, this->ori0);
        quat_conj(ori_inv);

        vector_cpy(localpos_, worldpos);
        vector_sub(localpos_, localpos_, this->pos0);
        quat_apply(localpos_, ori_inv, localpos_);
    }

    float* w = this->buildingRooms;
    float mins[3] = {-w[0]*0.5f - radius, -0.0f - radius, -w[2]*0.5f - radius};
    float maxs[3] = {+w[0]*0.5f + radius, +w[1] + radius, +w[2]*0.5f + radius};
    float localprj[3];

    //cout << endl;
    //cout << mins[0] << " " << mins[1] << " " << mins[2] << endl;
    //cout << localpos[0] << " " << localpos[1] << " " << localpos[2] << endl;
    //cout << maxs[0] << " " << maxs[1] << " " << maxs[2] << endl;

    float depth = 0;
    depth = Particle::constraintParticleByBox(localpos_, mins, maxs, localprj);

    //cout << depth << endl;

    if (depth <= 0) return 0;

    if (localpos != NULL) vector_cpy(localpos, localprj);

    {
        quat_apply(worldpos, this->ori0, localprj);
        vector_add(worldpos, worldpos, this->pos0);
    }

    return depth;
}

void cBuilding::spawn() {
    explosionObject.object = this;
}

void cBuilding::animate(float spf) {
    {
        float* p = this->pos0;
        float* w = buildingRooms;
        vector_set(explosionObject.pos0, p[0], p[1]+0.125f * 3.5f * w[1], p[2]);
        explosionObject.animate(spf);
    }
}

void cBuilding::transform() {
    explosionObject.transform();
}

void cBuilding::drawSolid() {
    explosionObject.drawSolid();

    float* p = this->pos0;
    float r[] = {0, 0, 0};
    float* w = buildingRooms;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        GLS::glUseProgram_fglittexture();

        long roof = sTextures[0];
        long wall = 0;
        if (this->damageable->hp[rDamageable::BODY] <= 0) {
            glColor3f(0.6f, 0.5f, 0.4f);
        } else if (this->damageable->hp[rDamageable::BODY] <= 50) {
            glColor3f(0.6f, 0.7f, 0.8f);
        } else {
            glColor3f(0.7f, 0.8f, 0.9f);
        }

        wall = sTextures[1 + (((int) (p[0]*1.234567 + p[2]*7.654321)) % (sTextures.size() - 1))];

        float hp = this->damageable->hp[rDamageable::BODY] * 0.01;
        hp = hp < 0 ? 0 : hp;
        float d = 0.5f + hp * 0.5f;
        glColor3f(d, d, d);

        glPushMatrix();
        {
            if (r[1] < -0.00001f || r[1] > 0.00001f) glRotatef(r[1] * 0.017453f, 0, 1, 0);
            glTranslatef(p[0] - w[0]*0.5f, p[1], p[2] - w[2]*0.5f);
            //cPrimitives::glBlockFlat(w[0], w[1], w[2], wall, wall, wall, wall, roof, roof);
            const float n2 = 0.707107f; // 1.0f / sqrtf(2);
            const float n3 = 0.577350f; // 1.0f / sqrtf(3);
            glBindTexture(GL_TEXTURE_2D, roof);
            glBegin(GL_QUADS);
            {
                glNormal3f(0, 1, 0);

                glTexCoord2f(0, 0);
                glVertex3f(0, w[1], 0);

                glTexCoord2f(0, w[2]);
                glVertex3f(0, w[1], w[2]);

                glTexCoord2f(w[0], w[2]);
                glVertex3f(w[0], w[1], w[2]);

                glTexCoord2f(w[0], 0);
                glVertex3f(w[0], w[1], 0);
            }
            glEnd();
            glBindTexture(GL_TEXTURE_2D, wall);
            glBegin(GL_TRIANGLE_STRIP);
            {
                float tx = 0;
                //float f = 1.0f / 9.0f * 2.0f;
                float f = 1.0f / 9.0f;
                float ty = w[1] * f;

                glNormal3f(-n2, 0, -n2);
                glTexCoord2f(tx, 0);
                glVertex3f(0, 0, 0);

                glNormal3f(-n3, n3, -n3);
                glTexCoord2f(tx, ty);
                glVertex3f(0, w[1], 0);

                tx += w[0] * f;

                glNormal3f(+n2, 0, -n2);
                glTexCoord2f(tx, 0);
                glVertex3f(w[0], 0, 0);

                glNormal3f(+n3, n3, -n3);
                glTexCoord2f(tx, ty);
                glVertex3f(w[0], w[1], 0);

                tx += w[2] * f;

                glNormal3f(+n2, 0, +n2);
                glTexCoord2f(tx, 0);
                glVertex3f(w[0], 0, w[2]);

                glNormal3f(+n3, n3, +n3);
                glTexCoord2f(tx, ty);
                glVertex3f(w[0], w[1], w[2]);

                tx += w[0] * f;

                glNormal3f(-n2, 0, +n2);
                glTexCoord2f(tx, 0);
                glVertex3f(0, 0, w[2]);

                glNormal3f(-n3, n3, +n3);
                glTexCoord2f(tx, ty);
                glVertex3f(0, w[1], w[2]);

                tx += w[2] * f;

                glNormal3f(-n2, 0, -n2);
                glTexCoord2f(tx, 0);
                glVertex3f(0, 0, 0);

                glNormal3f(-n3, n3, -n3);
                glTexCoord2f(tx, ty);
                glVertex3f(0, w[1], 0);
            }
            glEnd();

            if (!true) Primitive::glAxis();
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void cBuilding::drawEffect() {
    explosionObject.drawEffect();
}


// ###############################################

#include "de/hackcraft/proc/Surface.h"
#include "de/hackcraft/proc/Road.h"
#include "de/hackcraft/world/World.h"


int cTile::sInstances = 0;
std::map<int, long> cTile::sTextures;

cTile::cTile(int x, int y, int z, int kind) {
    sInstances++;
    if (sInstances == 1) {
        cout << "Generating Roads..." << endl;
        std::string basepath = std::string("data/base/roads/");
        bool save = false;

        unsigned int texname;
        int w_ = 1 << (8 + BUILDINGDETAIL);

        // NEWS-Road
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicCrossRoad(1 * (float) i / (float) w, 1 * (float) j / (float) h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNEWS.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_NEWS] = texname;
        }
        // TN-Road
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicTNRoad(1 * (float) i / (float) w, 1 * (float) j / (float) h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadTN.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_TN] = texname;
        }
        // TS-Road
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicTSRoad(1 * (float) i / (float) w, 1 * (float) j / (float) h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadTS.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_TS] = texname;
        }
        // TE-Road
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicTERoad(1 * (float) i / (float) w, 1 * (float) j / (float) h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadTE.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_TE] = texname;
        }
        // TW-Road
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicTWRoad(1 * (float) i / (float) w, 1 * (float) j / (float) h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadTW.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_TW] = texname;
        }
        // NS road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicRoad(1 * (float) i / (float) w, 1 * (float) j / (float) h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNS.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_NS] = texname;
        }
        // EW road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicRoad(1 * (float) j / (float) h, 1 * (float) i / (float) w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadEW.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_EW] = texname;
        }
        // NE road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicNERoad(1 * (float) i / (float) h, 1 * (float) j / (float) w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNE.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_NE] = texname;
        }
        // SE road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicSERoad(1 * (float) i / (float) h, 1 * (float) j / (float) w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadSE.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }
        // SW road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicSWRoad(1 * (float) i / (float) h, 1 * (float) j / (float) w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadSW.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }
        // NW road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Road::getBasicNWRoad(1 * (float) i / (float) h, 1 * (float) j / (float) w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNW.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }
        // PLAZA road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
            unsigned char* p = texels;

            loopj(h) {

                loopi(w) {
                    float color[16];
                    Surface::stone_plates(1 * (float) i / (float) h, 1 * (float) j / (float) w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadPLAZA.tga");
            if (save) {
                if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }

    } // if sInstances == 1

    //    addRole(COLLIDEABLE); // !!
    vector_set(this->pos0, x + 4.5f, y, z + 4.5f);

    tileKind = kind;
}

void cTile::drawSolid() {
    float* p = this->pos0;
    float r[] = {0, 0, 0};

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        GLS::glUseProgram_fglittexture();

        long ground = sTextures[tileKind];
        glColor4f(1, 1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, ground);

        glPushMatrix();
        {
            float x = p[0] - 4.50f;
            float y = p[1] + 0.08f;
            float z = p[2] - 4.50f;
            if (r[1] < -0.01f || r[1] > 0.01f) glRotatef(r[1], 0, 1, 0);
            glTranslatef(x, y, z);
            GLfloat p[][3] = {
                {0, 0, 0},
                {9, 0, 0},
                {0, 0, 9},
                {9, 0, 9}
            };
            const float a = 0.01;
            const float b = 0.99;
            GLfloat t[][2] = {
                {a, a},
                {b, a},
                {b, b},
                {a, b},
            };
            glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);
            Primitive::glVertex3fv2fv(p[1], t[0]);
            Primitive::glVertex3fv2fv(p[0], t[1]);
            Primitive::glVertex3fv2fv(p[2], t[2]);
            Primitive::glVertex3fv2fv(p[3], t[3]);
            glEnd();
        }
        glPopMatrix();
    }
    glPopAttrib();
}


// ###############################################

