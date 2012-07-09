#include "rBuilding.h"

#include "de/hackcraft/io/Texfile.h"
#include "de/hackcraft/io/Filesystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include "de/hackcraft/proc/Distortion.h"
#include "de/hackcraft/proc/Facade.h"
#include "de/hackcraft/proc/Noise.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/weapon/rTarget.h"

#include <cassert>

#include <sstream>

#define BUILDINGDETAIL 0

unsigned int rBuilding::loadMaterial() {
    static bool fail = false;
    static GLenum prog = 0;

    if (prog == 0 && !fail) {
        char* vtx = Filesystem::loadTextFile("data/base/material/base.vert");
        if (vtx) logger->debug() << "--- Vertex-Program Begin ---\n" << vtx << "\n--- Vertex-Program End ---\n";
        char* fgm = Filesystem::loadTextFile("data/base/material/base2d.frag");
        if (fgm) logger->debug() << "--- Fragment-Program Begin ---\n" << fgm << "\n--- Fragment-Program End ---\n";
        fail = (vtx == NULL || fgm == NULL) || (vtx[0] == 0 && fgm[0] == 0);
        if (!fail) {
            std::stringstream str;
            prog = GLS::glCompileProgram(vtx, fgm, str);
            logger->error() << str.str() << "\n";
        }
        delete[] vtx;
        delete[] fgm;
    }

    if (fail) return 0;
    return prog;
}

Logger* rBuilding::logger = Logger::getLogger("de.hackcraft.world.sub.cityscape.rBuilding");

int rBuilding::sInstances = 0;
std::map<int, long> rBuilding::sTextures;

rBuilding::rBuilding(int x, int y, int z, int rooms_x, int rooms_y, int rooms_z) {
    sInstances++;
    if (sInstances == 1) {
        unsigned int texname;

        {
            logger->info() << "Generating Rooftops..." << "\n";
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
            delete[] texels;
            sTextures[0] = texname;
        }

        {
            //glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
            logger->info() << "Generating Facades..." << "\n";

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
                        std::string fname = std::string("data/base/cityscape/facade/facade_") + std::string(numb) + std::string(".tga");
                        if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                            logger->error() << "Could not save image" << "\n";
                        }
                    } catch (char const* s) {
                        logger->error() << "ERROR: " << s << "\n";
                    }
                }
                delete[] texels;
            }
        }
    }

    buildingRooms[0] = 3 * rooms_x;
    buildingRooms[1] = 3 * rooms_y;
    buildingRooms[2] = 3 * rooms_z;

    vector_set(this->pos0, x + buildingRooms[0]*0.5f, y, z + buildingRooms[2]*0.5f);
    quat_set(this->ori0, 0, 0, 0, 1);
    this->radius = sqrtf(0.25f * buildingRooms[0] * buildingRooms[0] + 0.25f * buildingRooms[1] * buildingRooms[1] + 0.25f * buildingRooms[2] * buildingRooms[2]);
    
    this->id = (OID) this;
}

float rBuilding::constrain(float* worldpos, float radius, float* localpos, Entity* enactor) {
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

    //cout << "\n";
    //cout << mins[0] << " " << mins[1] << " " << mins[2] << "\n";
    //cout << localpos[0] << " " << localpos[1] << " " << localpos[2] << "\n";
    //cout << maxs[0] << " " << maxs[1] << " " << maxs[2] << "\n";

    float depth = 0;
    depth = Particle::constraintParticleByBox(localpos_, mins, maxs, localprj);

    //cout << depth << "\n";

    if (depth <= 0) return 0;

    if (localpos != NULL) vector_cpy(localpos, localprj);

    {
        quat_apply(worldpos, this->ori0, localprj);
        vector_add(worldpos, worldpos, this->pos0);
    }

    return depth;
}

void rBuilding::drawSolid() {
    float* p = this->pos0;
    float r[] = {0, 0, 0};
    float* w = buildingRooms;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        GLS::glUseProgram_fglittexture();
        glUseProgramObjectARB(loadMaterial());

        long roof = sTextures[0];
        long wall = sTextures[1 + (((int) (p[0]*1.234567 + p[2]*7.654321)) % (sTextures.size() - 1))];

        glColor3f(1.0f, 1.0f, 1.0f);

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
        glUseProgramObjectARB(0);
    }
    glPopAttrib();
}

