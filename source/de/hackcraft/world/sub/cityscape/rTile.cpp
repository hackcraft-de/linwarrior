#include "rTile.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/io/Filesystem.h"
#include "de/hackcraft/io/Texfile.h"

#include "de/hackcraft/proc/Road.h"
#include "de/hackcraft/proc/Surface.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include "de/hackcraft/world/World.h"

#include <sstream>


#define ROADTILEDETAIL 0

unsigned int rTile::loadMaterial() {
    static bool fail = false;
    static GL::GLenum prog = 0;

    if (prog == 0 && !fail) {
        char* vtx = Filesystem::loadTextFile("data/base/material/base.vert");
        if (vtx) logger->debug() << "--- Vertex-Program Begin ---\n" << vtx << "\n--- Vertex-Program End ---\n";
        char* fgm = Filesystem::loadTextFile("data/base/material/base2d.frag");
        if (fgm) logger->debug() << "--- Fragment-Program Begin ---\n" << fgm << "\n--- Fragment-Program End ---\n";
        fail = (vtx == NULL || fgm == NULL) || (vtx[0] == 0 && fgm[0] == 0);
        if (!fail) {
            std::stringstream str;
            prog = GLS::glCompileProgram(vtx, fgm, str);
            logger->error() << str << "\n";
        }
        delete[] vtx;
        delete[] fgm;
    }

    if (fail) return 0;
    return prog;
}

Logger* rTile::logger = Logger::getLogger("de.hackcraft.world.sub.model.rTile");

int rTile::sInstances = 0;
std::map<int, long> rTile::sTextures;

rTile::rTile(int x, int y, int z, int kind) {
    sInstances++;
    if (sInstances == 1) {
        logger->info() << "Generating Roads..." << "\n";
        std::string basepath = std::string("data/base/roads/");
        bool save = false;

        unsigned int texname;
        int w_ = 1 << (8 + ROADTILEDETAIL);

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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
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
                    logger->error() << "Could not save image: " << fname << "\n";
                }
            }
            delete[] texels;
            sTextures[KIND_ROAD_SE] = texname;
        }

    } // if sInstances == 1

    //    addRole(COLLIDEABLE); // !!
    vector_set(this->pos0, x + 4.5f, y, z + 4.5f);

    tileKind = kind;
    id = (OID) this;
}

void rTile::drawSolid() {
    float* p = this->pos0;
    float r[] = {0, 0, 0};

    GL::glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        GLS::glUseProgram_fglittexture();
        GL::glUseProgramObjectARB(loadMaterial());

        long ground = sTextures[tileKind];
        GL::glColor4f(1, 1, 1, 1);
        GL::glBindTexture(GL_TEXTURE_2D, ground);

        GL::glPushMatrix();
        {
            float x = p[0] - 4.50f;
            float y = p[1] + 0.08f;
            float z = p[2] - 4.50f;
            if (r[1] < -0.01f || r[1] > 0.01f) GL::glRotatef(r[1], 0, 1, 0);
            GL::glTranslatef(x, y, z);
            GL::GLfloat p[][3] = {
                {0, 0, 0},
                {9, 0, 0},
                {0, 0, 9},
                {9, 0, 9}
            };
            const float a = 0.01;
            const float b = 0.99;
            GL::GLfloat t[][2] = {
                {a, a},
                {b, a},
                {b, b},
                {a, b},
            };
            GL::glBegin(GL_QUADS);
            GL::glNormal3f(0, 1, 0);
            Primitive::glVertex3fv2fv(p[1], t[0]);
            Primitive::glVertex3fv2fv(p[0], t[1]);
            Primitive::glVertex3fv2fv(p[2], t[2]);
            Primitive::glVertex3fv2fv(p[3], t[3]);
            GL::glEnd();
        }
        GL::glPopMatrix();
        GL::glUseProgramObjectARB(0);
    }
    GL::glPopAttrib();
}

