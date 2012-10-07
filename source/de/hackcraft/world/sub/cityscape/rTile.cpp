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


Logger* rTile::logger = Logger::getLogger("de.hackcraft.world.sub.cityscape.rTile");

int rTile::sInstances = 0;

std::map<int, long> rTile::sTextures;


rTile::rTile(int x, int y, int z, int kind) {
    
    sInstances++;
    
    if (sInstances == 1) {

        logger->info() << "Generating Roads..." << "\n";

        std::string basepath = std::string("data/base/roads/");
        bool save = false;
        int resolution = 1 << (8 + ROADTILEDETAIL);
        unsigned char seed = 131;
        
        uploadTextures(resolution, save, basepath, seed);
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


unsigned int rTile::loadMaterial() {
    
    static bool fail = false;
    static GL::GLenum prog = 0;

    if (prog == 0 && !fail) {
        
        char* vtx = Filesystem::loadTextFile("data/base/material/base.vert");
        if (vtx) {
            logger->debug() << "--- Vertex-Program Begin ---\n" << vtx << "\n--- Vertex-Program End ---\n";
        }
        
        char* fgm = Filesystem::loadTextFile("data/base/material/base2d.frag");
        if (fgm) {
            logger->debug() << "--- Fragment-Program Begin ---\n" << fgm << "\n--- Fragment-Program End ---\n";
        }
        
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


void rTile::uploadTextures(int resolution, bool save, std::string basepath, unsigned char seed) {
    
    uploadTexture(KIND_ROAD_EW, resolution, save, basepath, "roadEW", seed);
    uploadTexture(KIND_ROAD_NS, resolution, save, basepath, "roadNS", seed);
    uploadTexture(KIND_ROAD_NEWS, resolution, save, basepath, "roadNEWS", seed);
    uploadTexture(KIND_ROAD_TN, resolution, save, basepath, "roadTN", seed);
    uploadTexture(KIND_ROAD_TE, resolution, save, basepath, "roadTE", seed);
    uploadTexture(KIND_ROAD_TW, resolution, save, basepath, "roadTW", seed);
    uploadTexture(KIND_ROAD_TS, resolution, save, basepath, "roadTS", seed);
    uploadTexture(KIND_ROAD_NE, resolution, save, basepath, "roadNE", seed);
    uploadTexture(KIND_ROAD_NW, resolution, save, basepath, "roadNW", seed);
    uploadTexture(KIND_ROAD_SE, resolution, save, basepath, "roadSE", seed);
    uploadTexture(KIND_ROAD_SW, resolution, save, basepath, "roadSW", seed);
}


void rTile::uploadTexture(int kind, int resolution, bool save, std::string basepath, std::string name, unsigned char seed) {
    
    int w = resolution;
    int h = w;
    int bpp = 3;
    unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
    unsigned char* p = texels;

    loopj(h) {

        loopi(w) {
            float color[16];
            float x = 1 * (float) i / (float) w;
            float y = 1 * (float) j / (float) h;
            getTexelOfKind(kind, x, y, color, seed);
            *p++ = color[2] * 255;
            *p++ = color[1] * 255;
            *p++ = color[0] * 255;
        }
    }
    
    //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
    unsigned int texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
    sTextures[kind] = texname;
    
    if (save) {
        
        std::string fname = basepath + name + std::string(".tga");
        if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
            logger->error() << "Could not save image: " << fname << "\n";
        }
    }
    
    delete[] texels;
}


void rTile::getTexelOfKind(int kind, float x, float y, float* color, unsigned char seed) {
    
    switch (kind) {
        
        case KIND_ROAD_EW: 
            Road::getBasicRoad(y, x, 0, color, seed);
            break;
            
        case KIND_ROAD_NS: 
            Road::getBasicRoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_NEWS: 
            Road::getBasicCrossRoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_TN: 
            Road::getBasicTNRoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_TE: 
            Road::getBasicTERoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_TW: 
            Road::getBasicTWRoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_TS: 
            Road::getBasicTSRoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_NE: 
            Road::getBasicNERoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_NW: 
            Road::getBasicNWRoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_SE: 
            Road::getBasicSERoad(x, y, 0, color, seed);
            break;
            
        case KIND_ROAD_SW: 
            Road::getBasicSWRoad(x, y, 0, color, seed);
            break;
        
        default:
            Road::getBasicCrossRoad(x, y, 0, color, seed);
            break;
    }
}

