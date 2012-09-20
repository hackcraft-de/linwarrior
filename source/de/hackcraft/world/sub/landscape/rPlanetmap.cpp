#include "rPlanetmap.h"

#include "de/hackcraft/io/Filesystem.h"
#include "de/hackcraft/io/Texfile.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/proc/Distortion.h"
#include "de/hackcraft/proc/Landscape.h"
#include "de/hackcraft/proc/Noise.h"
#include "de/hackcraft/proc/Solid.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include "de/hackcraft/util/Propmap.h"

#include "de/hackcraft/world/sub/model/rTree.h"

#include <cassert>
#include <ostream>
using namespace std;

#define GROUNDDETAIL 0
#define TWODIMTEX true

Logger* rPlanetmap::logger = Logger::getLogger("de.hackcraft.world.sub.landscape.rPlanetmap");

int rPlanetmap::sInstances = 0;

std::vector<long> rPlanetmap::sGrounds;
std::vector<long> rPlanetmap::sGrasses;
std::vector<rPlanetmap::Growth*> rPlanetmap::sGrowth;

const float rPlanetmap::oneOver256 = 0.003906f;


inline float rPlanetmap::sMod::getModifiedHeight(float x, float z, float h) {

    switch (type) {
        case MODTYPE_FLATSQUARE:
        {
            float x_ = x - pos[0];
            float z_ = z - pos[2];
            const float inv_range = 1.0f / range;
            if (-range < x_ && x_ < +range && -range < z_ && z_ < +range) {
                float dx = fabs(x_);
                float dz = fabs(z_);
                float dy = 1.0f - fmax(dx, dz) * inv_range;
                dy = (dy < 0.25f) ? (dy * 4.0f) : 1.0f;
                h *= (1.0f - dy);
                h += height * dy;
            }
            return h;
        }
        break;
        
        case MODTYPE_SMOOTHSQUARE:
        {
            float x_ = x - pos[0];
            float z_ = z - pos[2];
            if (-range < x_ && x_ < +range && -range < z_ && z_ < +range) {
                float dx = fabs(x_);
                float dz = fabs(z_);
                float d = fmax(dx, dz) / range;
                //float alpha = 1.0f - pow(d, 8);
                float alpha = 1.0f - fmin(Distortion::sig((d - 0.8f) * 20.0f) * 1.05f, 1.0f);
                h += alpha * (height - h);
            }
            return h;
        }
        break;
        
        case MODTYPE_SMOOTHROUND:
        {
            float x_ = x - pos[0];
            float z_ = z - pos[2];
            float d2 = x_ * x_ + z_*z_;
            if (d2 < range * range) {
                float d = sqrt(d2) / range;
                float alpha = 1.0f - fmin(Distortion::sig((d - 0.8f) * 20.0f) * 1.05f, 1.0f);
                h += alpha * (height - h);
            }
            return h;
        }
        break;
        
        case MODTYPE_CRATER:
        {
            float x_ = x - pos[0];
            float z_ = z - pos[2];
            float d2 = x_ * x_ + z_*z_;
            if (d2 < range * range) {
                float d = sqrt(d2) / range;
                float dy = height + 0.2f * range * sin(pow(d, 8.0) * M_PI);
                float alpha = 1.0f - fmin(Distortion::sig((d - 0.85f) * 30.0f) * 1.05f, 1.0f);
                h += alpha * (dy - h);
            }
            return h;
        }
        break;
        default: return h;
    }
}


rPlanetmap::rPlanetmap() {
    Propmap properties;
    properties.put("landscape.type", "grass");
    properties.put("landscape.vegetation", "0.7");
    properties.put("landscape.ground", "grass");
    init(&properties);
}


rPlanetmap::rPlanetmap(Propmap* properties) {
    init(properties);
}


void rPlanetmap::init(Propmap* properties) {
    sInstances++;
    if (sInstances == 1) {
        {
            logger->info() << "Generating solid ground...\n";
            unsigned char seed = 131;
            const int size = 1 << (6 + GROUNDDETAIL);
            unsigned char* texels = new unsigned char[size * size * size * 3];
            int samples = 5;
            float amt = 1.0f / (float) (samples);
            for (int l = 0; l < samples; l++) {
                unsigned char* texel = texels;

                loopijk(size, size, size) {
                    float f = 256.0f / (float) size;
                    float i_ = (rand()&127) * 0.0078f;
                    float j_ = (rand()&127) * 0.0078f;
                    float k_ = (rand()&127) * 0.0078f;

                    float color[16];
                    //cSolid::planet_grain((i+i_), (j+j_)*f,(k+k_)*f, color, seed);
                    //cSolid::camo_desert((i+i_), (j+j_)*f,(k+k_)*f, color, seed);
                    Solid::camo_snow((i + i_) * f, (j + j_) * f, (k + k_) * f, color, seed);

                    const float a = 0.475f;
                    const float b = 1.0f - a;
                    if (l == 0) {
                        *texel++ = 255 * (a + b * color[0]) * amt;
                        *texel++ = 255 * (a + b * color[1]) * amt;
                        *texel++ = 255 * (a + b * color[2]) * amt;
                    } else {
                        *texel++ += 255 * (a + b * color[0]) * amt;
                        *texel++ += 255 * (a + b * color[1]) * amt;
                        *texel++ += 255 * (a + b * color[2]) * amt;
                    }
                }
            }
            unsigned int texname = GLS::glBindTexture3D(0, true, true, true, true, true, size, size, size, texels);
            sGrounds.push_back(texname);
            delete[] texels;
        }
        
        for (int i = 0; i < 4; i++) {
            string basepath = string("data/base/landscape/ground/");
            string names[] = { 
                string("ground_grass"), 
                string("ground_rock"), 
                string("ground_stones"), 
                string("ground_snow") 
            };
            string name = string(basepath).append(names[i]).append(".tga");
            logger->debug() << "Loading [" << name << "] ...\n";
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = Texfile::loadTGA(name.c_str(), &w, &h, &bpp);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            delete[] texels;
            sGrounds.push_back(texname);
        }

        {
            string basepath = string("data/base/landscape/grass/");
            string name = string(basepath).append("grass_strip.tga");
            logger->debug() << "Loading [" << name << "] ...\n";
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = Texfile::loadTGA(name.c_str(), &w, &h, &bpp);
            texname = GLS::glBindTexture2D(0, true, true, true, false, w, h, bpp, texels);
            delete[] texels;
            sGrasses.push_back(texname);
        }

        string basepath = string("data/base/landscape/");
        string filenames[] = {
            string("blossom/blossom_desert.tga"),
            string("grass/grass_bold.tga"),
            string("plant/plant_long.tga"),
            string("plant/plant_green.tga"),
            string("plant/plant_desert.tga"),
            string("plant/plant_melissa.tga"),
            string("plant/plant_bluish.tga"),
            string("plant/plant_bamboo.tga"),
            string("plant/plant_palm.tga"),
            string("leaf/leaf_farn.tga"),
            string("leaf/leaf_pumpkin.tga"),
            string("foliage/foliage_twigs.tga"),
            string("foliage/foliage_pine.tga"),
            string("foliage/foliage_strange.tga")
        };
        float sizes[] = {
            0.4,
            1.1,
            1.9,
            1.6,
            1.0,
            0.7,
            0.8,
            1.2,
            2.7,
            1.9,
            1.3,
            1.4,
            1.5,
            1.1
        };
        Growth::Rendertype render[] = {
            Growth::BILLBOARD,
            Growth::TRIANGLE,
            Growth::BILLBOARD,
            Growth::BILLBOARD,
            Growth::BILLBOARD,
            Growth::BILLBOARD,
            Growth::BILLBOARD,
            Growth::BILLBOARD,
            Growth::CROSS,
            Growth::LEAFS,
            Growth::LEAFS,
            Growth::STAR,
            Growth::STAR,
            Growth::STAR
        };

        loopi(14) {
            string name = string(basepath).append(filenames[i]);
            logger->debug() << "Loading [" << name << "] ...\n";
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = Texfile::loadTGA(name.c_str(), &w, &h, &bpp);
            texname = GLS::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete[] texels;
            sGrowth.push_back(new Growth(texname, 2.0f * sizes[i], render[i]));
        }
    }

    //this->radius = 10;
    //vector_set(this->pos0, float_NAN, float_NAN, float_NAN);

    //name = "PLANETMAP";
    tree = new rTree(NULL, NULL, NULL, 0, 0, 0);
    
    landtype = 0;
    if (properties->contains("landscape.type")) {
        std::string value = properties->getProperty("landscape.type", "grass");
        if (value.compare("grass") == 0) {
            landtype = 4;
        } else if (value.compare("desert") == 0) {
            landtype = 3;
        } else if (value.compare("mars") == 0) {
            landtype = 2;
        } else if (value.compare("lava") == 0) {
            landtype = 1;
        } else if (value.compare("snow") == 0) {
            landtype = 0;
        }
    }
    
    vegetation = 0.7;
    if (properties->contains("landscape.vegetation")) {
        std::string value = properties->getProperty("landscape.vegetation", "1");
        double v = atof(value.c_str());
        vegetation = fminf(3.0f, fmaxf(0.0f, v));
    }
    
    groundtype = 0;
    if (properties->contains("landscape.ground")) {
        std::string value = properties->getProperty("landscape.ground", "grass");
        if (value.compare("grass") == 0) {
            groundtype = 1;
        } else if (value.compare("rock") == 0) {
            groundtype = 2;
        } else if (value.compare("stones") == 0) {
            groundtype = 3;
        } else if (value.compare("snow") == 0) {
            groundtype = 4;
        }
    }
}


void rPlanetmap::invalidateCache() {
    const long TILESIZE = PLANETMAP_TILESIZE;
    for (maptype<unsigned long, sPatch*>::iterator i = patches.begin(); i != patches.end(); ++i) {
        sPatch* patch = (*i).second;
        OID* ptr = patch->heightcolor;
        const unsigned long totaltilesize = (1UL << (TILESIZE + TILESIZE));
        for (unsigned long i = 0; i < totaltilesize; i++) {
            float* height = (float*) ptr;
            *height = float_NAN;
            ptr++;
        }
    }
}


void rPlanetmap::getHeight(float x, float z, float* const color) {
    float h = 0;
    float y = 0;

    if (1) {
        float grass[16] = {0, 0, 0,};
        float snow[16] = {0, 0, 0,};
        float stone[16] = {0, 0, 0,};

        // Blending between types.
        const int seed1 = 121;
        const float p1 = 1.0f / 301.0f;
        float o1 = Noise::simplex3(7 + x*p1, 15 + y*p1, 19 + z*p1, seed1);
        o1 = Distortion::sig(o1 * 28.0f);

        float h1 = 0;
        float h0 = 0;
        
        switch (landtype) {
            
            case 0: {
                // Snowset
                if (o1 > 0.005f) {
                    Landscape::land_snow(x, y, z, snow);
                    h1 = 1 * snow[Landscape::BUMP];
                }
                if (o1 < 0.995f) {
                    Landscape::land_snow(x, y, z, grass);
                    h0 = 10 * grass[Landscape::BUMP];
                }
            } break;
            
            case 1: {
                // Lavaset
                if (o1 > 0.005f) {
                    Landscape::mashup_lava(x, y, z, snow);
                    h1 = 1 * snow[Landscape::BUMP];
                }
                if (o1 < 0.995f) {
                    Landscape::land_lava(x, y, z, grass);
                    h0 = 10 * grass[Landscape::BUMP];
                }
            } break;
            
            case 2: {
                // Duneset Red
                if (o1 > 0.005f) {
                    Landscape::land_dunes_red(x, y, z, snow);
                    h1 = 1 * snow[Landscape::BUMP];
                }
                if (o1 < 0.995f) {
                    Landscape::land_rockies(x * 0.5f, y * 0.5f, z * 0.5f, grass);
                    h0 = 25 * grass[Landscape::BUMP];
                }
            } break;
            
            case 3: {
                // Duneset
                if (o1 > 0.005f) {
                    Landscape::land_dunes(x, y, z, snow);
                    h1 = 5.0f * snow[Landscape::BUMP];
                }
                if (o1 < 0.995f) {
                    Landscape::land_dunes(x * 0.5f, y * 0.5f, z * 0.5f, grass);
                    h0 = 18.0f * grass[Landscape::BUMP];
                }
            } break;
            
            case 4: {
                // Grassset
                if (o1 > 0.005f) {
                    Landscape::land_grass(x, y, z, snow);
                    h1 = 5.0f * snow[Landscape::BUMP];
                }
                if (o1 < 0.995f) {
                    Landscape::land_grass(x, y, z, grass);
                    //cLandscape::land_snow(x, y, z, grass);
                    h0 = 40 * grass[Landscape::BUMP];
                }
            } break;
            
            default: {
            } break;
        }

        // Original 1:1 mix
        //h = (1.0f - o1) * h0 + o1 * h1 - 9.0f;

        // (1+0.5):1 mix
        h = (1.0f - o1) * (h0 + 0.5 * h1) + o1 * h1 - 9.0f;

        float r = (1.0f - o1) * grass[0] + o1 * snow[0];
        float g = (1.0f - o1) * grass[1] + o1 * snow[1];
        float b = (1.0f - o1) * grass[2] + o1 * snow[2];

        Landscape::decoration_stones(x, y, z, stone);
        float hs = 7.0f * stone[Landscape::BUMP];

        if (hs > 0.01) {
            r = 1.0f - stone[0];
            g = 1.0f - stone[1];
            b = 1.0f - stone[2];
            h += hs;
        }

        color[0] = r;
        color[1] = g;
        color[2] = b;

        // Large Scale Hills.
        const int seed2 = 97;
        const float p2 = 1.0f / 1001.0f;
        float o2 = Noise::simplex3(73 + x*p2, 21 + y*p2, 39 + z*p2, seed2);
        h += Distortion::sig(o2 * 4 - 1.4f) * 30;
    }

    if (0) {
        float f = 0.005;
        float f2 = 0.5;
        float cont[16];
        Landscape::experimental_continents(x*f, y*f, z*f, cont);
        float hx = 15.0f / f2 * (cont[Landscape::BUMP] - 0.5f);
        //if (hx > 0.0) h += 2.0f*hx;
        //else h = hx;
        //h = hx+h;
        h = hx;
        color[0] = cont[0];
        color[1] = cont[1];
        color[2] = cont[2];
    }

    for (sMod* mod: mods) {
        h = mod->getModifiedHeight(x, z, h);
    }

    color[Landscape::BUMP] = h;
} // getHeight


void rPlanetmap::getCachedHeight(float x, float z, float* const color) {
    // nan?
    if (x != x || z != z) {
        color[0] = 1;
        color[1] = 0;
        color[2] = 0;
        color[3] = 0;
    }
#if 0
    getHeight(x, z, color);
    return color[Landscape::BUMP];
#else
    const unsigned CACHESIZE = PLANETMAP_CACHESIZE;
    const long TILESIZE = PLANETMAP_TILESIZE;
    const long PATCHSIZE = PLANETMAP_TILESIZE - PLANETMAP_DIVISIONS;
    const long tilesize_ = (1L << TILESIZE) - 1;
    const long detailfactor = (1L << (TILESIZE - PATCHSIZE));
    long tx = ((long) x) >> PATCHSIZE;
    long tz = ((long) z) >> PATCHSIZE;
    // Key needs to be unique, that can't be quaranteed with
    // key = (23473 * tx) ^ (23497 * tz) which seems to be ok but may fail!
    // The used formula quarantees uniqueness just within limited value range.
    tx &= 0xFFFF;
    tz &= 0xFFFF;
    unsigned long key = (tx << 16) | tz;

    sPatch* patch = patches[key]; // This may already add a new key.
    if (patch == NULL) {
        bool recycled = false;
        if (patches.size() > CACHESIZE) {
            recycled = true;
            unsigned long mintouches = -1;
            sPatch* minpatch = NULL;
            maptype<unsigned long, sPatch*>::iterator mini;
            maptype<unsigned long, sPatch*>::iterator i;
            for (i = patches.begin(); i != patches.end(); ++i) {
                sPatch* patch_ = i->second;
                if (patch_ == NULL) continue;
                if (patch_->touches < mintouches) {
                    mintouches = patch_->touches;
                    minpatch = patch_;
                    mini = i;
                }
            }
            patches.erase(mini);
            patch = minpatch;
        } else {
            patch = new sPatch();
            //assert(patch != NULL);
        }
        patch->touches = 0;
        patch->key = key;
        patches[patch->key] = patch;
        OID* ptr = patch->heightcolor;

        const unsigned long totaltilesize = (1UL << (TILESIZE + TILESIZE));
        for (unsigned long i = 0; i < totaltilesize; i++) {
            float* height = (float*) ptr;
            *height = float_NAN;
            ptr++;
        }

        ptr = patch->heightcolor;
        logger->trace() << "Patch=" << (recycled ? "recycled" : "new") << ", total=" << patches.size() << ", key=" << key << ", ptr=" << ptr << "\n";
    }
    patch->touches++;

    unsigned long fx = ((unsigned long) (fabs(x) * detailfactor));
    unsigned long fz = ((unsigned long) (fabs(z) * detailfactor));
    fx &= tilesize_;
    fz &= tilesize_;

    OID* h = &(patch->heightcolor[((fx << TILESIZE) | fz)]);
    unsigned char* bytes = (unsigned char*) h;
    float* height = (float*) &bytes[0];

    char* normal = (char*) &(patch->normal[((fx << TILESIZE) | fz)]);

    const float logbase = 255.0f / log(3);
    const float logbaseinv = 1.0f / logbase;

    if (*height != *height) {
        getHeight(x, z, color);
        float h00 = color[3]; // *
        *height = color[3];
        bytes[4] = log(1.0f + color[0]) * logbase;
        bytes[5] = log(1.0f + color[1]) * logbase;
        bytes[6] = log(1.0f + color[2]) * logbase;
        bytes[7] = 1;

        // * Experimental gradient caching:
        float e = 0.025f;
        getHeight(x + e, z, color);
        float h10 = color[3]; // *
        getHeight(x, z + e, color);
        float h01 = color[3]; // *
        float xd = (h10 - h00);
        float zd = (h01 - h00);
        //zv = { 0, zd, e }.
        //xv = { e, xd, 0 }.
        //n = zv X xv.
        float n[] = {-e * xd, e * e, -zd * e};
        vector_norm(n, n);
        normal[0] = n[0] * 126;
        normal[1] = n[1] * 126;
        normal[2] = n[2] * 126;
        normal[3] = 0;
    } else {
        float* height = (float*) &bytes[0];
        color[3] = *height;
        color[0] = float(exp(bytes[4] * logbaseinv)) - 1.0f;
        color[1] = float(exp(bytes[5] * logbaseinv)) - 1.0f;
        color[2] = float(exp(bytes[6] * logbaseinv)) - 1.0f;
    }
    color[4] = normal[0] * 0.007936f; // div by 126.
    color[5] = normal[1] * 0.007936f;
    color[6] = normal[2] * 0.007936f;
#endif
} // getCachedHeight


float rPlanetmap::constrain(float* worldpos, float radius, float* localpos, Entity* enactor) {
    //if (enactor == NULL) return 0.0;
    float localpos_[3];
    vector_cpy(localpos_, worldpos);

    //assert(finitef(localpos_[0]));
    //assert(finitef(localpos_[1]));
    //assert(finitef(localpos_[2]));

    const int iterations = 24;
    const int samples = 16;
    float relax = 0.1;
    float maxdelta = 0;
    float color[16];
    int runs = 0;

    loopj(iterations) {
        runs++;
        //relax = 0.05 + 0.1 * (iterations - j) / (float)(iterations);
        relax = 0.01;
        //const float a = 2.0 * M_PI / (float) samples;
        //const float b = 0.5 * M_PI / (float) samples;
        float nearest2 = radius*radius;
        float nearestV[] = {0, 0, 0};

        loopi(samples) {
            float x_ = localpos_[0];
            float y_ = localpos_[1];
            float z_ = localpos_[2];
            float alpha = 0;
            float beta = 0;
            if (i != 0) {
                //alpha = i * a;
                //beta = i * b;
                alpha = (rand() % 62831)*0.0001f; // 2PI 0-360
                beta = (rand() % 15707)*0.0001f; // 0.5PI 0-90
            }
            x_ += sin(alpha) * sin(beta) * radius;
            y_ += -cos(beta) * radius;
            z_ += cos(alpha) * sin(beta) * radius;

            // TODO: When already deep in ground there needs to be special handling.

            getCachedHeight(x_, z_, color);
            float h00 = color[Landscape::BUMP];
            float delta = y_ - h00;
            if (delta > 0) continue;
            if (h00 > localpos_[1]) {
                h00 = localpos_[1];
            }
            float v[] = {localpos_[0] - x_, localpos_[1] - h00, localpos_[2] - z_};
            float dist2 = vector_dot(v, v);
            if (dist2 < nearest2) {
                nearest2 = dist2;
                vector_cpy(nearestV, v);
            }
        }
        float nearest = sqrtf(nearest2);
        float delta = radius - nearest;
        if (delta > 0.01) {
            //vector_print(near);
            //cout << radius << " " << nearest << " " << delta << "\n";

            vector_scale(nearestV, nearestV, 1.0f / (nearest + 0.000001f));
            vector_scale(nearestV, nearestV, delta * relax);
            vector_add(localpos_, localpos_, nearestV);
        } else if (j > 2) {
            break;
        }
        maxdelta = fmax(maxdelta, delta);
    }
    //cout << "it " << runs << "\n";

    if (maxdelta > 0) {
        vector_cpy(worldpos, localpos_);
        if (localpos != NULL) vector_cpy(localpos, localpos_);
    }
    return maxdelta;
}


void rPlanetmap::animate(float spf) {
    if (tree) tree->seconds += spf;
}


unsigned int rPlanetmap::loadMaterial() {
    static bool fail = false;
    static GL::GLenum prog = 0;

    if (prog == 0 && !fail) {
        char* vtx = Filesystem::loadTextFile("data/base/material/base.vert");
        if (vtx) logger->debug() << "--- Vertex-Program Begin ---\n" << vtx << "\n--- Vertex-Program End ---\n";
        char* fgm = Filesystem::loadTextFile("data/base/material/base2d.frag");
        if (fgm) logger->debug() << "--- Fragment-Program Begin ---\n" << fgm << "\n--- Fragment-Program End ---\n";
        fail = (vtx == NULL || fgm == NULL) || (vtx[0] == 0 && fgm[0] == 0);
        if (!fail) {
            stringstream str;
            prog = GLS::glCompileProgram(vtx, fgm, str);
            logger->debug() << str.str() << "\n";
        }
        delete[] vtx;
        delete[] fgm;
    }

    if (fail) return 0;
    return prog;
}


void rPlanetmap::drawSolid() {

    // Get current Camera-Matrix.
    float m[16];
    GL::glGetFloatv(GL_MODELVIEW_MATRIX, m);
    // Position from 4th Col.
    float p[3];
    loop3i(p[i] = m[12 + i]);
    // Reset Position on 4th Col.
    loop3i(m[12 + i] = 0);
    // Apply transpose-inverse to position.
    float n[16];
    memcpy(n, m, sizeof (float) *16);
    matrix_transpose(m);
    matrix_apply2(m, p);

    // nan position?
    if (p[0] != p[0] || p[2] != p[2]) return;

    // Setup FOV for culling to be a forward cone set back a bit.
    //vec3 fwd = {m[4 * 2 + 0], m[4 * 2 + 1], m[4 * 2 + 2]};
    //float back = 16;
    //vec3 bk = {back * fwd[0], back * fwd[1], back * fwd[2]};

    int detail = 5;

    float color[16];
    getCachedHeight(-p[0], -p[2], color);
    float relheight = p[1] + color[Landscape::BUMP];

    int hscale = -relheight * 0.05f;
    hscale = (hscale < 2) ? hscale : 1;
    if (0) detail -= hscale;

    GL::glPushMatrix();
    {
        GL::glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GL::glUseProgramObjectARB(loadMaterial());
            
            GL::glEnable(GL_LIGHTING);
            GL::glEnable(GL_FOG);
            GL::glEnable(GL_COLOR_MATERIAL);
            //GL::glEnable(GL_NORMALIZE);
            
            float ts = 1;
            
            if(TWODIMTEX) {
                ts = 0.13f;
                GL::glDisable(GL_TEXTURE_3D);
                GL::glEnable(GL_TEXTURE_2D);
                GL::glBindTexture(GL_TEXTURE_2D, sGrounds[groundtype]);
            } else {
                ts = 0.3f;
                GL::glDisable(GL_TEXTURE_2D);
                GL::glEnable(GL_TEXTURE_3D);
                GL::glBindTexture(GL_TEXTURE_3D, sGrounds[0]);
            }

            // Levels of detail rings.
            int maxLevels = 3;
            
            // Half edge size of the area to be filled with polygon tiles.
            float ringRadius = 32 * (1 << 3);
            
            // Size of a single polygon tile.
            float tileSize = ringRadius / 32.0f;
            
            const float ringShrink = 0.5f;
            const float tileShrink = 0.5f;

            // From outer to inner ring
            for (int k = 1; k <= maxLevels; k++) {

                GL::glColor4f(0, 0, 1, 1);

                float ringInnerRadius = (ringShrink * ringRadius) - 4 * tileSize;
                if (k == maxLevels) ringInnerRadius = 0;

                // Ensure that the tiles start on the grid.
                float x = -((int) (p[0] / tileSize)) * tileSize;
                float z = -((int) (p[2] / tileSize)) * tileSize;
                
                //cout << x << " " << z << "\n";
                
                for (float i = x - ringRadius; i < x + ringRadius; i += tileSize) {
                    
                    GL::glBegin(GL_TRIANGLE_STRIP);
                    
                    bool skip = false;
                    
                    for (float j = z - ringRadius; j <= z + ringRadius; j += tileSize) {

                        float alpha_i_z = fmin(1, 0.3f * fmax(0, abs(j + p[2]) - ringInnerRadius));
                        float alpha_i_x = fmin(1, 0.3f * fmax(0, abs(i + tileSize + p[0]) - ringInnerRadius));
                        float alpha_i = fmax(alpha_i_z, alpha_i_x);

                        float alpha_o_z = fmin(1, 0.16f * fmax(0, ringRadius - abs(j + p[2])));
                        float alpha_o_x = fmin(1, 0.16f * fmax(0, ringRadius - abs(i + tileSize + p[0])));
                        float alpha_o = fmin(alpha_o_z, alpha_o_x);
                        float alpha = alpha_o;

                        bool outside = false;
                        /*
                        {
                            float dx = -(i - x);
                            float dz = -(j - z);
                            float dist2 = dx*dx + dz*dz;

                            // Check Cone-Frustum
                            float dist = sqrt(dist2)+0.000001f;
                            vec3 relative = { dx + bk[0], 0.0f, dz + bk[2] };
                            float cone = vector_dot(fwd, relative);
                            outside = (cone < 0.46 * dist);
                        }
                         */

                        if (outside || alpha_o <= 0.0f || alpha_i <= 0.0f) {
                            skip = true;
                            continue;
                        } else if (skip) {
                            GL::glEnd();
                            skip = false;
                            GL::glBegin(GL_TRIANGLE_STRIP);
                        }

                        getCachedHeight(i + tileSize, j, color);
                        float h0 = color[Landscape::BUMP] + 0.005f * k;

                        color[3] = alpha;

                        GL::glNormal3fv(&color[4]);
                        GL::glColor4f(fmin(1.0f, color[0]), fmin(1.0f, color[1]), fmin(1.0f, color[2]), fmin(1.0f, color[3]));
#ifdef TWODIMTEX
                        GL::glTexCoord2f((i + tileSize) * ts, j * ts);
#else
                        GL::glTexCoord3f((i + tileSize) * ts, h0*ts, j * ts);
#endif
                        GL::glVertex3f(i + tileSize, h0, j);

                        //

                        getCachedHeight(i + 0, j, color);
                        float h1 = color[Landscape::BUMP] + 0.005f * k;

                        alpha_o_z = fmin(1, 0.16f * fmax(0, ringRadius - abs(j + p[2])));
                        alpha_o_x = fmin(1, 0.16f * fmax(0, ringRadius - abs(i + p[0])));
                        alpha_o = fmin(alpha_o_z, alpha_o_x);
                        alpha = alpha_o;

                        color[3] = alpha;

                        GL::glNormal3fv(&color[4]);
                        GL::glColor4f(fmin(1.0f, color[0]), fmin(1.0f, color[1]), fmin(1.0f, color[2]), fmin(1.0f, color[3]));
#ifdef TWODIMTEX
                        GL::glTexCoord2f((i + 0) * ts, j * ts);
#else
                        GL::glTexCoord3f((i + 0) * ts, h1*ts, j * ts);
#endif
                        GL::glVertex3f(i + 0, h1, j);
                    }
                    GL::glEnd();
                }

                ringRadius *= ringShrink;
                tileSize *= tileShrink;

            } // for k rings

            GL::glUseProgramObjectARB(0);
        }
        GL::glPopAttrib();
    }
    GL::glPopMatrix();

    // Reset Touches for LRU
    for (maptype<unsigned long, sPatch*>::iterator i = patches.begin(); i != patches.end(); ++i) {
        //cout << "A2" << "\n";
        sPatch* patch_ = i->second;
        if (patch_ == NULL) continue;
        patch_->touches = 0;
    }
}


void rPlanetmap::drawBillboardPlant(float x__, float h, float z__, float scale, float* unrotateMatrix) {
    GL::glTranslatef(x__, h - 0.2, z__);
    GL::glMultMatrixf(unrotateMatrix);
    GL::glScalef(scale, scale, scale);
    static unsigned int dlist = 0;
    if (dlist == 0) {
        dlist = GL::glGenLists(1);
        GL::glNewList(dlist, GL_COMPILE_AND_EXECUTE);
        Primitive::glXCenteredTextureSquare();
        //glAxis(0.9);
        GL::glEndList();
    } else {
        GL::glCallList(dlist);
    }
}


void rPlanetmap::drawStarPlant(float x__, float h, float z__, float scale) {
    GL::glTranslatef(x__, h - 0.2-0.3, z__);
    GL::glScalef(scale, scale, scale);
    static unsigned int dlist = 0;
    if (dlist == 0) {
        dlist = GL::glGenLists(1);
        GL::glNewList(dlist, GL_COMPILE_AND_EXECUTE);
        Primitive::glBrush();
        GL::glEndList();
    } else {
        GL::glCallList(dlist);
    }
}


void rPlanetmap::drawTrianglePlant(float x__, float h, float z__, float scale) {
    GL::glTranslatef(x__, h - 0.2, z__);
    GL::glScalef(scale, scale, scale);
    static unsigned int dlist = 0;
    if (dlist == 0) {
        dlist = GL::glGenLists(1);
        GL::glNewList(dlist, GL_COMPILE_AND_EXECUTE);
        Primitive::glSharp();
        GL::glEndList();
    } else {
        GL::glCallList(dlist);
    }
}


void rPlanetmap::drawCrossPlant(float x__, float h, float z__, float scale) {
    GL::glTranslatef(x__, h - 0.2, z__);
    GL::glScalef(scale, scale, scale);
    static unsigned int dlist = 0;
    if (dlist == 0) {
        dlist = GL::glGenLists(1);
        GL::glNewList(dlist, GL_COMPILE_AND_EXECUTE);
        Primitive::glStar();
        GL::glEndList();
    } else {
        GL::glCallList(dlist);
    }
}


void rPlanetmap::drawLeafPlant(float x__, float h, float z__, float scale) {
    GL::glTranslatef(x__, h - 0.2, z__);
    GL::glScalef(scale, scale, scale);
    static unsigned int dlist = 0;
    if (dlist == 0) {
        dlist = GL::glGenLists(1);
        GL::glNewList(dlist, GL_COMPILE_AND_EXECUTE);
        //ploth(x=0,1, sin(x)/(1+1.7*x)+0.1*x)
        
        for (int j = 0; j < 7; j++) {
            
            GL::glRotatef(j * 135.0 + 0.10*(rand()%100), 0,1,0);

            float w = (1.7 - j * 0.05) / 4.0;
            float h = (1.0 + j * 0.01) / 4.0;
            float l = (1.0 - j * 0.05) / 4.0;
            GL::glBegin(GL_TRIANGLE_STRIP);
            
            for (int i = 0; i <= 6; i++) {
                
                double f = double(i) / 6.0;
                double x = 4.0 * f;
                double y = 4.0 * (sin(x) / (1.0 + 1.7 * x) + (0.1 + 0.03 * j) * x);
                GL::glTexCoord2f(0.0f, 1.0-f);
                GL::glVertex3d(l * x, h * y, -w);
                GL::glTexCoord2f(1.0f, 1.0-f);
                GL::glVertex3d(l * x, h * y, +w);
            }
            GL::glEnd();
        }
        
        
        GL::glEndList();
    } else {
        GL::glCallList(dlist);
    }
}


void rPlanetmap::drawStone(float x, float h, float z, float scaleX, float scaleH, float scaleZ) {
    
    GL::glTranslatef(x, h - 0.1*scaleH, z);
    GL::glScalef(scaleX, scaleH, scaleZ);
    
    static unsigned int dlist = 0;
    
    if (dlist == 0) {
        dlist = GL::glGenLists(1);
        GL::glNewList(dlist, GL_COMPILE_AND_EXECUTE);
        Primitive::glBalloid();
        GL::glEndList();
    } else {
        GL::glCallList(dlist);
    }
}


void rPlanetmap::drawEffect() {
    const float totaldensity = vegetation;

    // Pseudorandom Permutation.
    static unsigned char perms[3 * 256];
    static bool init = true;
    if (init) {
        init = false;
        unsigned char b = 131;

        loopi(3 * 256) {
            if ((i & 255) == 255) {
                perms[i] = 255;
            } else {
                perms[i] = b = Noise::LFSR8(b);
            }
        }
    }

    // Get current Camera-Matrix.
    float m[16];
    GL::glGetFloatv(GL_MODELVIEW_MATRIX, m);
    // Position from 4th Col.
    float p[3];
    loop3i(p[i] = m[12 + i]);
    // Reset Position on 4th Col.
    loop3i(m[12 + i] = 0);
    // Apply transpose-inverse to position.
    matrix_transpose(m);
    matrix_apply2(m, p);

    // Setup FOV for culling to be a forward cone set back a bit.
    vec3 fwd = {m[4 * 2 + 0], m[4 * 2 + 1], m[4 * 2 + 2]};
    float back = 16;
    vec3 bk = {back * fwd[0], back * fwd[1], back * fwd[2]};

    // Construct Billboarding Matrix.
    float n[16];
    GLS::glGetTransposeInverseRotationMatrix(n);
    vector_set(&n[4], 0, 1, 0);
    vector_cross(&n[0], &n[4], &n[8]);
    vector_norm(&n[0], &n[0]);
    vector_cross(&n[8], &n[0], &n[4]);

    float* look = &n[8];
    int xlook = -((look[0] < 0) ? +1 : -1);
    int zlook = -((look[2] < 0) ? +1 : -1);

    // nan?
    if (p[0] != p[0] || p[2] != p[2]) return;

    GL::glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        GL::glDisable(GL_CULL_FACE);
        GL::glEnable(GL_LIGHTING);
        GL::glEnable(GL_TEXTURE_2D);
        GL::glEnable(GL_FOG);

        GL::glEnable(GL_ALPHA_TEST);
        GL::glAlphaFunc(GL_GREATER, 0.45f);

        GL::glColor4f(0.8f, 0.8f, 0.8f, 1);
        GL::glNormal3f(0, 1, 0);
        
        const float step = 16;
        //const float near = (step*2)*(step*2);
        const float s = 6 * step;
        const float x = -((int) (p[0] / step)) * step;
        const float z = -((int) (p[2] / step)) * step;
        //cout << x << " " << z << "\n";
        
        const int maxplants = sGrowth.size();

        // Loop in positive or negative x direction (painters algorithm).
        float i = x - s*xlook;
        while (true) {
            if (fabs(x + s * xlook - i) < 0.0001f) break;
            float x_ = i;

            // Loop in positive or negative z direction (painters algorithm).
            float j = z - s*zlook;
            while (true) {
                if (fabs(z + s * zlook - j) < 0.0001f) break;
                float z_ = j;

                float dx = (-p[0] - x_ - 0.5f * step);
                float dz = (-p[2] - z_ - 0.5f * step);
                float dist2 = dx * dx + dz*dz;

                // Check Cone-Frustum
                float dist = sqrt(dist2) + 0.000001f;
                vec3 relative = {dx + bk[0], 0.0f, dz + bk[2]};
                float cone = vector_dot(fwd, relative);
                if (cone < 0.46 * dist) {
                    j += step*zlook;
                    continue;
                }

                // opacity: near = 1, >far = 0
                float opacity = fmax(0.0f, fmin(1.0f, 0.0f + 0.015f * (s + 4.0f * step - dist)));

                // Field index key
                unsigned char a = (char) x_;
                unsigned char b = (char) z_;
                unsigned char key = Noise::permutation2d(perms, a, b);
                
                key = Noise::simplex3(a, b, 0.1f, 123) * 255.0f;

                float plantscale = 1.0f;
                float plantdensity = (totaldensity * 10.00f * key) * oneOver256;
                int visibleplants = plantdensity * opacity;

                key = Noise::simplex3(a, b, 0.1f, 234) * 255.0f;
                float treedensity = (totaldensity * 1.85f * key) * oneOver256;
                float visibletrees = treedensity;

                key = Noise::simplex3(a, b, 0.1f, 243) * 255.0f;
                //float grassdensity = (totaldensity * 2.00f * key);
                //float visiblegrass = grassdensity;
                
                key = Noise::simplex3(a, b, 0.1f, 217) * 255.0f;
                float stonedensity = (totaldensity * 4.00f * key) * oneOver256;
                float visiblestones = stonedensity;

                //cout << "opacity " << opacity << "  density " << density << "\n";

                // Load LFSR using position, key and some scrambling.
                unsigned short lfsr16 = (13 * key) ^ (17 * a) ^ (23 * b);
                unsigned short lfsr16tmp = lfsr16;
                lfsr16 = (lfsr16 == 0) ? 1 : lfsr16;
                lfsr16 ^= 0x7493;

                // Stones
                drawStones(x_, z_, step, visiblestones, opacity, lfsr16);
                
                // Grass                
                if (opacity >= 0.995 && !true) {
                    drawGrass(x_, z_, step, opacity, lfsr16);
                }
                
                // Plants
                drawPlants(x_, z_, step, visibleplants, opacity, maxplants, plantscale, plantdensity, n, lfsr16);

                lfsr16 = lfsr16tmp;

                // trees
                drawTrees(x_, z_, step, visibletrees, opacity, lfsr16);

                j += step*zlook;
            } // while j z

            i += step*xlook;
        } // while i x
    }
    GL::glPopAttrib();
}


void rPlanetmap::drawStones(float x_, float z_, float step, int visiblestones, float opacity, unsigned int lfsr16) {
    
    GL::glDisable(GL_TEXTURE_2D);
    loopi(visiblestones) {
        GL::glPushMatrix();
        {
            lfsr16 = Noise::LFSR16(lfsr16);
            unsigned char a = lfsr16;
            unsigned char b = lfsr16 >> 8;

            float x__ = x_ + step * oneOver256 * a;
            float z__ = z_ + step * oneOver256 * b;
            
            float color[16];
            getCachedHeight(x__, z__, color);
            float h = color[Landscape::BUMP];

            lfsr16 = Noise::LFSR16(lfsr16);
            a = lfsr16;
            b = lfsr16 >> 8;

            float s = 0.1 + 0.5 * (a^b^0x27) * oneOver256;
            GL::glColor4f(0.2f, 0.2f, 0.2f,opacity);
            drawStone(x__, h, z__, s*(1.0f+0.4*a*oneOver256), s, s*(1.0f+0.4*b*oneOver256));
        }
        GL::glPopMatrix();
    }
    GL::glEnable(GL_TEXTURE_2D);
}


void rPlanetmap::drawGrass(float x_, float z_, float step, float opacity, unsigned int lfsr16) {
    
    lfsr16 = Noise::LFSR16(lfsr16);
    unsigned char a = lfsr16;
    unsigned char b = lfsr16 >> 8;

    float x__ = x_ + step * oneOver256 * a;
    float z__ = z_ + step * oneOver256 * b;
    
    float color[16];
    getCachedHeight(x__, z__, color);
    float h = color[Landscape::BUMP];

    GL::glColor4f(0.2,0.4,0.2,opacity);
    GL::glBindTexture(GL_TEXTURE_2D, sGrasses[0]);

    GL::glBegin(GL_TRIANGLE_STRIP);
    {

        loopj(17) {
            float f = j * 0.0625f;
            x__ = x_ + step * f;
            z__ = z_ + step * f;

            unsigned char a = Noise::LFSR16((int) x__ * 123.017454);
            unsigned char b = Noise::LFSR16((int) z__ * 231.017454);
            float u = oneOver256 * a * 4 + 6 * sin(2*f * M_PI);
            float v = oneOver256 * b * 4 + 6 * cos(2*f * M_PI);

            getCachedHeight(x_ + f*step+u, z_ + f*step+v, color);
            h = color[Landscape::BUMP];

            GL::glTexCoord2f(6*f,0);
            GL::glVertex3f(x_ + f*step+u, h-2.5, z_ + f*step+v);
            GL::glTexCoord2f(6*f,1);
            GL::glVertex3f(x_ + f*step+u, h+13, z_ + f*step+v);
        }
        
        loopj(17) {
            float f = j * 0.0625f;
            x__ = x_ + step * f;
            z__ = z_ + step * f;

            unsigned char a = Noise::LFSR16((int) x__ * 71.017454);
            unsigned char b = Noise::LFSR16((int) z__ * 31.017454);
            float u = oneOver256 * a * 4 + 6 * sin(4*f * M_PI);
            float v = oneOver256 * b * 4 + 6 * cos(4*f * M_PI);

            getCachedHeight(x_ + f*step+u, z_ + step-f*step+v, color);
            h = color[Landscape::BUMP];

            GL::glTexCoord2f(6*f,0);
            GL::glVertex3f(x_ + f*step+u, h-2.5, z_ + step-f*step+v);
            GL::glTexCoord2f(6*f,1);
            GL::glVertex3f(x_ + f*step+u, h+13, z_ + step-f*step+v);
        }
    }
    GL::glEnd();
}


void rPlanetmap::drawPlants(float x_, float z_, float step, int visibleplants, float opacity, int maxplants, float plantscale, float plantdensity, float* billboardMatrix, unsigned int lfsr16) {
    
    loopi(visibleplants) {
        GL::glPushMatrix();
        {
            lfsr16 = Noise::LFSR16(lfsr16);
            unsigned char a = lfsr16;
            unsigned char b = lfsr16 >> 8;

            float x__ = x_ + step * oneOver256 * a;
            float z__ = z_ + step * oneOver256 * b;
            
            float color[16];
            getCachedHeight(x__, z__, color);
            float h = color[Landscape::BUMP];

            lfsr16 = Noise::LFSR16(lfsr16);
            //unsigned char kind = lfsr16;
            unsigned char rot = lfsr16 >> 8;

            //lfsr16 = LFSR16(lfsr16);
            //unsigned char size = lfsr16;

            unsigned char tex = (rot >> 2) % maxplants;
            unsigned char size = rot;

            float sizef = size * oneOver256;
            float scale = plantscale * sGrowth[tex]->size * (0.35f + 0.65f * sizef + 0.008f * plantdensity);

            float shift = -0.1 + 0.2f * oneOver256 * (((lfsr16 >> 3) ^ (lfsr16>>7) ^ a ^ b) & 0xFF);
            GL::glColor4f(0.9 + shift, 1.0-fabs(shift), 0.9f-shift, opacity);
            //GL::glColor4f(1, 1, 1, opacity);
            GL::glBindTexture(GL_TEXTURE_2D, sGrowth[tex]->texture);

            switch (sGrowth[tex]->rendertype) {
                case Growth::BILLBOARD:
                    drawBillboardPlant(x__, h, z__, scale, billboardMatrix);
                    break;
                case Growth::STAR:
                    drawStarPlant(x__, h, z__, scale);
                    break;
                case Growth::TRIANGLE:
                    drawTrianglePlant(x__, h, z__, scale);
                    break;
                case Growth::CROSS:
                    drawCrossPlant(x__, h, z__, scale);
                    break;
                case Growth::LEAFS:
                    drawLeafPlant(x__, h, z__, scale);
                    break;
                default:
                    drawBillboardPlant(x__, h, z__, scale, billboardMatrix);
                    break;
            }
        }
        GL::glPopMatrix();
    } // loopi visibleplants
}


void rPlanetmap::drawTrees(float x_, float z_, float step, int visibletrees, float opacity, unsigned int lfsr16) {
    
    loopi(visibletrees) {
        
        GL::glPushMatrix();
        {
            lfsr16 = Noise::LFSR16(lfsr16);
            unsigned char a = lfsr16;
            unsigned char b = lfsr16 >> 8;

            float x__ = x_ + step * oneOver256 * a;
            float z__ = z_ + step * oneOver256 * b;
            
            float color[16];
            getCachedHeight(x__, z__, color);
            float h = color[Landscape::BUMP];

            int age = fmax(0, fmin(6, i * 0.5f + ((a + b)&1)));
            int type = (b * b + (a >> 1))&7;

            tree->tree = rTree::getCompiledTree(1230 + (b & 6), type, 2 + age);
            
            vector_set(tree->pos0, x__, h - 0.2, z__);
            //tree->ori0[1] = a + b;
            float axis[] = {0, 1, 0};
            quat_rotaxis(tree->ori0, (a+b) * PI_OVER_180, axis);
            
            GL::glColor4f(0.15f, 0.09f, 0.03f, opacity);
            tree->drawSolid();
            GL::glColor4f(1, 1, 1, opacity);
            tree->drawEffect();
        }
        GL::glPopMatrix();
        
    } // loopi visibletrees
}

