#include "Background.h"

#include "World.h"

#include "de/hackcraft/proc/Noise.h"
#include "de/hackcraft/proc/Distortion.h"
#include "de/hackcraft/proc/Landscape.h"
#include "de/hackcraft/proc/Solid.h"

#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"

#include "de/hackcraft/io/Texfile.h"

#include "de/hackcraft/proc/Ambient.h"

#include <cstdio>
#include <memory>
#include <cstdlib>
#include <iostream>
using namespace std;

#define BKGDETAIL +1

#define DOME_XRES 2*6
#define DOME_YRES 7

enum Texids {
    T_ICE, T_MOUNTAIN, T_EARTH,
    T_LAVOS, T_MOON, T_SUN, T_CLOUDS,
    T_GALAXY_NEGX, T_GALAXY_POSX,
    T_GALAXY_NEGY, T_GALAXY_POSY,
    T_GALAXY_NEGZ, T_GALAXY_POSZ,
    T_SKY_NEGX, T_SKY_POSX,
    T_SKY_NEGY, T_SKY_POSY,
    T_SKY_NEGZ, T_SKY_POSZ,
    T_CLOUDS_NEGX, T_CLOUDS_POSX,
    T_CLOUDS_NEGY, T_CLOUDS_POSY,
    T_CLOUDS_NEGZ, T_CLOUDS_POSZ,
};

unsigned int gPermutationTexture256 = 0;


Background::Background() {
    Propmap properties;
    properties["backscape.windspeed"] = "0.1";
    properties["backscape.raininess"] = "0.0";
    properties["backscape.heightshift"] = "-10.0";
    properties["backscape.cloudiness"] = "0.1";
    init(&properties);
}


Background::Background(Propmap* properties) {
    init(properties);
}


void Background::init(Propmap* properties) {
    /*
    struct Texture {
        const char* filename;
        bool soft, repeat;
        int id;
    } texturedefs[] = {
        { "data/base/decals/mountain.tga", true, false, T_MOUNTAIN},
    };

    loopi(1) {
        //if (i == 1) continue;
        int w, h, bpp;
        unsigned char* texels = Texfile::loadTGA(texturedefs[i].filename, &w, &h, &bpp);
        bool soft = texturedefs[i].soft;
        bool repeat = texturedefs[i].repeat;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
        int id = texturedefs[i].id;
        textures[id] = texname;
        delete texels;
    }
     */

    if (1) {
        cout << "Generating Permutation 256..." << endl;
        int w = 256;
        int h = 16;
        int bpp = 3;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
        unsigned char* p = texels;
        unsigned char b = 131;

        loopij(h, w) {
            if ((j & 255) == 255) {
                *p++ = 255;
                *p++ = 255;
                *p++ = 255;
            } else {
                b = Noise::LFSR8(b);
                *p++ = b;
                *p++ = b;
                *p++ = b;
            }
        }
        bool soft = false;
        bool repeat = true;
        unsigned int texname;
        texname = GLS::glBindTexture2D(0, false, soft, repeat, repeat, w, h, bpp, texels);
        gPermutationTexture256 = texname;
        //saveTGA("permutation256.tga", w, h, bpp, texels);
        delete texels;
    }

    // Ground Texture
    if (1) {
        cout << "Generating Ground..." << endl;
        int w = 1 << (8 + BKGDETAIL);
        int h = w;
        int bpp = 3;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
        unsigned char* p = texels;

        loopij(h, w) {

            float dx = ((float) j / (float) w);
            float dz = ((float) i / (float) h);

            float c = Distortion::sig(Noise::voronoi3_16(dx * 16, 0, dz * 16) * 8 + 4);
            rgba c4f = {c, c, c, 1};

            *p++ = 255 * c4f[2];
            *p++ = 255 * c4f[1];
            *p++ = 255 * c4f[0];
        }
        bool soft = false;
        bool repeat = true;
        unsigned int texname;
        texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
        textures[T_ICE] = texname;
        delete texels;
    }

    // Sun Texture
    if (1) {
        cout << "Generating Sun..." << endl;
        int w = 1 << (6 + BKGDETAIL);
        int h = w;
        int bpp = 4;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
        unsigned char* p = texels;

        loopij(h, w) {

            float dx = (((float) j / (float) w) - 0.5f) * 2.0f;
            float dy = (((float) i / (float) h) - 0.5f) * 2.0f;
            float r = sqrtf(dx * dx + dy * dy);
            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 0.6);
            float rad = atan2(dx, dy);

            float c = 0.5f + 0.5f * Noise::simplex3(rad / (2 * M_PI)*256, 0, 0, 21);
            float d = 0.5f + 0.5f * Noise::simplex3(r * 31, 0, 0, 21);

            rgba c4f = {
                Distortion::exposure(3 * a + 3 * c * a + 3 * d * d * d * a),
                Distortion::exposure(3 * a),
                Distortion::exposure(1 * a),
                a * a
            };

            *p++ = 255 * c4f[2];
            *p++ = 255 * c4f[1];
            *p++ = 255 * c4f[0];
            *p++ = 255 * c4f[3];
        }
        bool soft = true;
        bool repeat = false;
        unsigned int texname;
        texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
        textures[T_SUN] = texname;
        delete texels;
    }

    // Earth Texture
    if (1) {
        cout << "Generating Earth..." << endl;
        int w = 1 << (6 + BKGDETAIL);
        int h = w;
        int bpp = 4;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
        unsigned char* p = texels;

        loopij(h, w) {

            float dx = (((float) j / (float) w) - 0.5f) * 2.0f;
            float dy = (((float) i / (float) h) - 0.5f) * 2.0f;
            float r = sqrtf(dx * dx + dy * dy);

            float dz = sqrtf(1 - dx * dx - dy * dy);

            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 16);

            const float f = 2.0f;
            float v3f[] = {f * dx, f * dy, f * dz};

            float color0[16];
            Solid::planet_cloud(v3f[0], v3f[1], v3f[2], color0);

            float color1[16];
            //getContinental(v3f, color1);
            Landscape::experimental_continents(v3f[0], v3f[1], v3f[2], color1);

            *p++ = 255 * fmin(1.0f, +0.2 * color0[2] + 0.99 * color1[2]) * a;
            *p++ = 255 * fmin(1.0f, +0.2 * color0[1] + 0.99 * color1[1]) * a;
            *p++ = 255 * fmin(1.0f, +0.2 * color0[0] + 0.99 * color1[0]) * a;
            *p++ = 255 * a;
        }
        bool soft = true;
        bool repeat = false;
        unsigned int texname;
        texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
        textures[T_EARTH] = texname;
        //saveTGA("earth.tga", w, h, bpp, texels);
        delete texels;
    }

    // Lavos Texture
    if (1) {
        cout << "Generating Lavos..." << endl;
        int w = 1 << (6 + BKGDETAIL);
        int h = w;
        int bpp = 4;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
        unsigned char* p = texels;

        loopij(h, w) {

            float dx = (((float) j / (float) w) - 0.5f) * 2.0f;
            float dy = (((float) i / (float) h) - 0.5f) * 2.0f;
            float r = sqrtf(dx * dx + dy * dy);

            float dz = sqrtf(1 - dx * dx - dy * dy);

            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 16);

            const float f = 2.0f;
            float v3f[] = {f * dx, f * dy, f * dz};

            //v3f[0] *= 0.1f;
            float color0[16];
            Solid::planet_cloud(v3f[0], v3f[1], v3f[2], color0);
            //v3f[0] *= 10.0;

            float color1[16];
            Solid::stone_lava(v3f[0], v3f[1], v3f[2], color1);

            *p++ = 255 * fmax(0.0f, -0.2f * color0[2] + 0.99f * color1[2]) * a;
            *p++ = 255 * fmax(0.0f, -0.2f * color0[1] + 0.99f * color1[1]) * a;
            *p++ = 255 * fmax(0.0f, -0.2f * color0[0] + 0.99f * color1[0]) * a;
            *p++ = 255 * a;
        }
        bool soft = true;
        bool repeat = false;
        unsigned int texname;
        texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
        textures[T_LAVOS] = texname;
        //saveTGA("lavos.tga", w, h, bpp, texels);
        delete texels;
    }

    // Moon Texture
    if (1) {
        cout << "Generating Moon..." << endl;
        int w = 1 << (6 + BKGDETAIL);
        int h = w;
        int bpp = 4;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
        unsigned char* p = texels;

        loopij(h, w) {

            float dx = (((float) j / (float) w) - 0.5f) * 2.0f;
            float dy = (((float) i / (float) h) - 0.5f) * 2.0f;
            float r = sqrtf(dx * dx + dy * dy);

            float dz = sqrtf(1 - dx * dx - dy * dy);

            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 16);

            const float f = 2.0f;
            float v3f[] = {f * dx, f * dy, f * dz};

            v3f[0] *= 0.1f;
            float color0[16];
            Solid::planet_cloud(v3f[0], v3f[1], v3f[2], color0);
            v3f[0] *= 10.0;

            float color1[16];
            Solid::planet_ground(v3f[0], v3f[1], v3f[2], color1);

            *p++ = 255 * fmax(0.8f * color0[2], 0.8f * color1[2]);
            *p++ = 255 * fmax(0.8f * color0[1], 0.8f * color1[1]);
            *p++ = 255 * fmax(0.8f * color0[0], 0.8f * color1[0]);
            *p++ = 255 * a;
        }
        bool soft = true;
        bool repeat = false;
        unsigned int texname;
        texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
        textures[T_MOON] = texname;
        delete texels;
    }

    // Cloud Texture
    if (1) {
        cout << "Generating Clouds..." << endl;
        int w = 1 << (8 + BKGDETAIL);
        int h = w;
        int bpp = 4;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];
        unsigned char* p = texels;

        loopij(h, w) {

            float dx = (((float) j / (float) w) - 0.5f) * 2.0f;
            float dy = (((float) i / (float) h) - 0.5f) * 2.0f;
            float r = sqrtf(dx * dx + dy * dy);
            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 1.2);

            const float f = 2.0f;
            float v3f[] = {f * (float) j / (float) w, 0, f * (float) i / (float) h};
            float color[16];
            Solid::planet_cloud(v3f[0], v3f[1], v3f[2], color);
            *p++ = 255 * color[2];
            *p++ = 255 * color[1];
            *p++ = 255 * color[0];
            *p++ = 255 * color[3] * a;
        }
        bool soft = true;
        bool repeat = true;
        unsigned int texname;
        texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
        textures[T_CLOUDS] = texname;
        delete texels;
    }

    // Galaxy Box Textures
    if (1) {
        cout << "Generating Galaxy..." << endl;
        int w = 1 << (8 + BKGDETAIL);
        int h = w;
        int bpp = 3;
        unsigned char* texels = new unsigned char[((unsigned long) w) * h * bpp];

        loopk(6) {
            // k even => -1 neg, k odd => +1 pos
            float sign = -1 + 2 * (k & 1);
            // rotation = k / 2
            int rotation = k >> 1;

            char names[6][3] = {"-X", "+X", "-Y", "+Y", "-Z", "+Z"};
            cout << "Generating Galaxy for " << names[k] << "..." << endl;
            //cout << "Generating Galaxy for " << ((sign<0)?"-":"+") << " Axis " << rotation << "..." << endl;

            unsigned char* p = texels;

            loopij(h, w) {
                const float scale = 0.2;
                // Point on cube
                float v[3] = {
                    2.0f * ((float) j / (float) (w)) - 1.0f,
                    2.0f * ((float) i / (float) (h)) - 1.0f,
                    sign
                };

                loop(l, rotation) {
                    float tmp = v[2];
                    v[2] = v[1];
                    v[1] = v[0];
                    v[0] = tmp;
                }
                float color[16];
                Ambient::galaxy(v[0], v[1], v[2], color, scale);
                *p++ = 255 * color[2];
                *p++ = 255 * color[1];
                *p++ = 255 * color[0];
            }
            const bool soft = true;
            const bool repeat = false;
            unsigned int texname;
            texname = GLS::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
            textures[T_GALAXY_NEGX + k] = texname;

            if (0) {
                try {
                    std::string fname = std::string("galaxy") + std::string(names[k]) + std::string(".tga");
                    if (Texfile::saveTGA(fname.c_str(), w, h, bpp, texels)) {
                        cout << "Could not save image" << endl;
                    }
                } catch (char const* s) {
                    cout << "ERROR: " << s << endl;
                }
            }
        }
        delete texels;
    }

    vector_set(light, 0.7, 0.9, -0.3);
    
    heightshift = fmax(-20, fmin(properties->getProperty("backscape.heightshift", 0), 0));
    windspeed = fmax(0, fmin(properties->getProperty("backscape.windspeed", 0), 10000));
    raininess = fmax(0, fmin(properties->getProperty("backscape.raininess", 0), 1));
    cloudiness = fmax(0, fmin(properties->getProperty("backscape.cloudiness", 0), 1));
}

void Background::drawBackground(float h) {
    // Reference: dawn sunrise daylight sunset dusk darkness

    // Push random seed because the current seed is going to be
    // replaced by a (deterministic) constant value for clouds.
    unsigned int seed = rand();

    float speed = 1;
    // every hour
    speed *= 24;
    // every half hour
    speed *= 2;
    // every quarter hour
    speed *= 2;
    // every 5 minutes
    //speed *= 3;
    // every minute
    //speed *= 5;
    hour = fmod(speed*h, 24.00f);
    //hour = rand()%24;
    //hour = 0;
    //hour = 12;

    // Sample and set unlit base ambient haze color.
    rgba haze = {0, 0, 0, 0};
    glFogfv(GL_FOG_COLOR, haze);
    if (1) {
        unsigned char s = 131;
        int samples = 23;

        loopi(samples) {
            s = Noise::LFSR8(s);
            float alpha = s / 256.0f * 2.0f * M_PI;
            s = Noise::LFSR8(s);
            float beta = s / 256.0f * 1.0f * M_PI;
            float color[16];
            vec3 n = { float(sin(alpha) * sin(beta)), float(0.99 * cos(beta)), float(sin(beta) * cos(alpha)) };
            //vector_print(n);
            Ambient::sky(n[0], n[1], n[2], color, hour);
            //quat_print(color);

            loopj(4) {
                haze[j] += color[j];
            }
        }

        loopj(4) {
            haze[j] /= float(samples);
        }
        //printf("Unlit Haze: "); quat_print(haze);
    }

    // Directional sun and moonlight.
    if (1) {
        //float p[] = {0.7, 0.9, -0.3, 0};
        float* p = light;
        //float p[] = {70, 90, -30, 0};
        float t = ((hour / 24.0f * 2 * M_PI) - 0.5f * M_PI); // = [-0.5pi,+1.5pi]
        float s = sin(t) * 0.5f + 0.5f;
        float a[] = {0.25f * s * haze[0], 0.25f * s * haze[1], 0.25f * s * s * haze[2], 1.00f};
        float d[] = {0.90f * s + (1 - s)*0.00f, 0.90f * s + (1 - s)*0.00f, 0.40f * s + (1.00f - s)*0.20f, 1.00f};
        glLightfv(GL_LIGHT0, GL_POSITION, p);
        glLightfv(GL_LIGHT0, GL_AMBIENT, a);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, d);
        //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
        //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0001);
        //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.000001);
        glEnable(GL_LIGHT0);
    }

    // Horizon fog.
    if (1) {
        glFogfv(GL_FOG_COLOR, haze);
        if (1) {
            float density = 0.008*1.1;
            glFogi(GL_FOG_MODE, GL_EXP2);
            glFogf(GL_FOG_DENSITY, density);
        } else {
            glFogi(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, 95.0f);
            glFogf(GL_FOG_START, 0.0f);
            glFogf(GL_FOG_END, 100.0f);
        }
    }

    GLint mode;
    glGetIntegerv(GL_POLYGON_MODE, &mode);
    if (mode == GL_FILL) {
        drawGalaxy();
        drawOrbit();
        drawUpperDome();
        drawSun();
        drawClouds();
        //drawMountains();
        drawLowerDome();
        //drawGround();
        drawRain();
    } else {
        //drawOrbit();
        //drawUpperDome();
        //drawMountains();
        //drawLowerDome();
    }

    // Pop a seed.
    srand(seed);
}

void Background::drawGalaxy() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
    {
        GLS::glUseProgram_bkplaintexture();

        glColor4f(1, 1, 1, 1);

        glPushMatrix();
        {
            // Load current Camera Matrix and
            // set Position (4th Column) = 0.
            float m[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, m);
            loop3i(m[12 + i] = 0);
            glLoadIdentity();
            glMultMatrixf(m);
            glScalef(100, 100, 100);

            float angle = 180 - ((360 / 24.00f) * hour + 90);
            // Sun travels: dawn 0600:East, 1200:South, dusk 1800:West (2400:North -> other side of the globe).
            // 0000/2400: 0 degree, 0600: 90 degree, 1200: 180 degree, 1800: 270 degree, ...
            // Moon is traveling about 180 degree behind/ahead.
            // Well, that is, if you're down-under then east, north, west (not east, south, west).
            glRotatef(-90, 0, 1, 0);
            //	glRotatef(45+7.5, 0, 0, 1); // Longitude? Aquator Angle Offset -> Germany. and for example Poland, GB and Canada.
            glRotatef(55, 0, 0, 1);
            glRotatef(0, 0, 0, 1); // Aquator.

            glRotatef(180 + angle, 1, 0, 0);

            // NEG X
            if (1) {
                glBindTexture(GL_TEXTURE_2D, textures[T_GALAXY_NEGX]);
                glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glVertex3f(-1, -1, -1);
                glTexCoord2i(1, 0);
                glVertex3f(-1, +1, -1);
                glTexCoord2i(1, 1);
                glVertex3f(-1, +1, +1);
                glTexCoord2i(0, 1);
                glVertex3f(-1, -1, +1);
                glEnd();
            }
            // POS X
            glBindTexture(GL_TEXTURE_2D, textures[T_GALAXY_POSX]);
            glBegin(GL_QUADS);
            glTexCoord2i(1, 0);
            glVertex3f(+1, +1, -1);
            glTexCoord2i(0, 0);
            glVertex3f(+1, -1, -1);
            glTexCoord2i(0, 1);
            glVertex3f(+1, -1, +1);
            glTexCoord2i(1, 1);
            glVertex3f(+1, +1, +1);
            glEnd();
            // NEG Y
            if (1) {
                glBindTexture(GL_TEXTURE_2D, textures[T_GALAXY_NEGY]);
                glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glVertex3f(-1, -1, -1);
                glTexCoord2i(1, 0);
                glVertex3f(-1, -1, +1);
                glTexCoord2i(1, 1);
                glVertex3f(+1, -1, +1);
                glTexCoord2i(0, 1);
                glVertex3f(+1, -1, -1);
                glEnd();
            }
            // POS Y
            glBindTexture(GL_TEXTURE_2D, textures[T_GALAXY_POSY]);
            glBegin(GL_QUADS);
            glTexCoord2i(1, 0);
            glVertex3f(-1, +1, +1);
            glTexCoord2i(0, 0);
            glVertex3f(-1, +1, -1);
            glTexCoord2i(0, 1);
            glVertex3f(+1, +1, -1);
            glTexCoord2i(1, 1);
            glVertex3f(+1, +1, +1);
            glEnd();
            // NEG Z
            if (1) {
                glBindTexture(GL_TEXTURE_2D, textures[T_GALAXY_NEGZ]);
                glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glVertex3f(-1, -1, -1);
                glTexCoord2i(1, 0);
                glVertex3f(+1, -1, -1);
                glTexCoord2i(1, 1);
                glVertex3f(+1, +1, -1);
                glTexCoord2i(0, 1);
                glVertex3f(-1, +1, -1);
                glEnd();
            }
            // POS Z
            glBindTexture(GL_TEXTURE_2D, textures[T_GALAXY_POSZ]);
            glBegin(GL_QUADS);
            glTexCoord2i(1, 0);
            glVertex3f(+1, -1, +1);
            glTexCoord2i(0, 0);
            glVertex3f(-1, -1, +1);
            glTexCoord2i(0, 1);
            glVertex3f(-1, +1, +1);
            glTexCoord2i(1, 1);
            glVertex3f(+1, +1, +1);
            glEnd();
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void Background::drawUpperDome() {
    glPushAttrib(GL_ALL_ATTRIB_BITS | GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        GLS::glUseProgram_bkplaincolor();

        // Load current Camera Matrix and
        // set Position (4th Column) = 0.
        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        loop3i(m[12 + i] = 0);
        m[12 + 1] = heightshift;

        glPushMatrix();
        {
            glLoadIdentity();
            glMultMatrixf(m);
            //glScalef(100, 100 * 0.3, 100); //*0.25 original
            glScalef(100, 100 * 0.25, 100); // original

            int vsteps = DOME_YRES;
            double vf = 0.5 * M_PI / (double) vsteps;

            int hsteps = DOME_XRES;
            double hf = 2.0 * M_PI / (double) hsteps;
            double h = 0;

            float s1 = sin(0);
            float c1 = cos(0);
            for (int i = 1; i <= vsteps; i++) {
                float s2 = sin(i * vf);
                float c2 = cos(i * vf);
                glBegin(GL_TRIANGLE_STRIP);
                for (int j = 0; j <= hsteps; j++) {
                    float s = sin(h);
                    float c = cos(h);

                    rgba colour1;
                    Ambient::sky(s1*c, c1 + 0.0001f, s1 * s, colour1, hour);
                    glColor4fv(colour1);
                    //glNormal3f(s1*c, c1, s1 * s);
                    glVertex3f(s1*c, c1, s1 * s);

                    rgba colour2;
                    Ambient::sky(s2*c, c2 + 0.0001f, s2 * s, colour2, hour);
                    glColor4fv(colour2);
                    //glNormal3f(s2*c, c2, s2 * s);
                    glVertex3f(s2*c, c2, s2 * s);
                    h += hf;
                } // for hsteps
                glEnd();
                s1 = s2;
                c1 = c2;
            } // for vsteps
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void Background::drawLowerDome() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        GLS::glUseProgram_bkplaincolor();

        // Load current Camera Matrix and
        // set Position (4th Column) = 0.
        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        loop3i(m[12 + i] = 0);
        m[12 + 1] = heightshift;

        glPushMatrix();
        {
            glLoadIdentity();
            glMultMatrixf(m);
            glScalef(100, 100 * 0.1, 100);

            int vsteps = DOME_YRES;
            double vf = 0.5f * M_PI / (double) vsteps;

            int hsteps = DOME_XRES;
            double hf = 2.0 * M_PI / (double) hsteps;
            double h = 0;

            float s1 = -sin(0);
            float c1 = -cos(0);
            for (int i = 1; i <= vsteps; i++) {
                float s2 = -sin(i * vf);
                float c2 = -cos(i * vf);
                glBegin(GL_TRIANGLE_STRIP);
                for (int j = 0; j <= hsteps; j++) {
                    float s = sin(h);
                    float c = cos(h);

                    rgba colour2;
                    Ambient::sky(s2*c, c2 - 0.0001f, s2 * s, colour2, hour);
                    glColor4fv(colour2);
                    //glNormal3f(s2*c, c2, s2 * s);
                    glVertex3f(s2*c, c2, s2 * s);

                    rgba colour1;
                    Ambient::sky(s1*c, c1 - 0.0001f, s1 * s, colour1, hour);
                    glColor4fv(colour1);
                    //glNormal3f(s1*c, c1, s1 * s);
                    glVertex3f(s1*c, c1, s1 * s);
                    h += hf;
                };
                glEnd();
                s1 = s2;
                c1 = c2;
            } // for vsteps
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void Background::drawGround() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
    {
        GLS::glUseProgram_bkplaintexture();

        glBindTexture(GL_TEXTURE_2D, textures[T_ICE]);

        // Get current Camera-Matrix.
        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
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
        //cout << "height over ground: " << p[1] << "\n";
        //cout << "Position: " << p[0] << " " << p[2] << "\n";

        glPushMatrix();
        {
            glLoadIdentity();
            glMultMatrixf(n); // Transpose to undo previous transpose.
            glTranslatef(0, p[1], 0);

            rgba color2;
            Ambient::sky(0, -1, 0, color2, hour);

            //const float tdiv = 5; // original
            const float tdiv = 16;
            const float radius = 400;

            float dx = -p[0] / tdiv;
            float dz = -p[2] / tdiv;

            glBegin(GL_TRIANGLE_FAN);
            glColor4f(color2[0], color2[1], color2[2], 1.0f);
            //glColor4f(0.1,0.1,0.3,0.93); // water
            glColor4f(1, 1, 1, 1);
            glNormal3f(0, 1, 0);
            glTexCoord2f(dx, dz);
            glVertex3f(0.0, 0.0, 0.0);
            glColor4f(color2[0], color2[1], color2[2], 0.0f);
            //glColor4f(0.1,0.1,0.4,0); // water
            glColor4f(1, 1, 1, 1);
            float e = 20.0 * M_PI / 180.0f;

            loopi(19) {
                glNormal3f(0, 1, 0);
                glTexCoord3f(cos(-i * e) * radius / tdiv + dx, sin(-i * e) * radius / tdiv + dz, 0);
                glVertex3f(cos(-i * e) * radius, 0, sin(-i * e) * radius);
            } // loopi 19
            glEnd();
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void Background::drawClouds() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        GLS::glUseProgram_bkplaintexture();
        //SGL::glUseProgram_2();

        glBindTexture(GL_TEXTURE_2D, textures[T_CLOUDS]);

        // Load current Camera Matrix and
        // set Position (4th Column) = 0.
        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        loop3i(m[12 + i] = 0);
        //m[12 + 0] = m[12 + 2] = 0;
        m[12 + 1] = heightshift;

        glPushMatrix();
        {
            glLoadIdentity();
            glMultMatrixf(m);
            glScalef(100, 100 * 0.24, 100);
            //glScalef(100, 100 * 0.125, 100); // original

            float light = 0.3 + 0.7 * cos((hour - 12.0f) / 12.00f * 0.5f * M_PI);
            
            //cout << hour << endl;
            srand(12421);
            int n = 50 * cloudiness;
            float density = 1.0 / (1.0 + 0.01 * n);
            //density = 0.999f;
            
            for (int j = 1; j <= 2; j++) {
                
                float trad = j * 0.5 * windspeed * 24.0f * (hour / 24.00f) * 2 * M_PI;
                glRotatef(trad / 0.017453f, 1, 0.1, 1);

                for (int i = 0; i < n; i++) {
                    //float xrad = -(rand() % 157) * 0.01f;
                    float xrad = -(157 - (rand() % 314)) * 0.01f;
                    float yrad = (rand() % 628) * 0.01f;

                    /*
                    float xaxis[] = { 1,0,0 };
                    quat qx;
                    quat_rotaxis(qx, xrad, xaxis);

                    float yaxis[] = { 0,1,0 };
                    quat qy;
                    quat_rotaxis(qy, yrad, yaxis);

                    quat qxy;
                    quat_mul(qxy, qx, qy);

                    vec3 x = {1,0,0};
                    vec3 y = {0,1,0};
                    vec3 z = {0,0,1};
                    quat_apply(x, qxy, x);
                    quat_apply(y, qxy, y);
                    quat_apply(z, qxy, z);

                    glColor4f(1,1,1,1);
                    glVertex3f(1*x[0]+z[0], 1+z[1], 0+z[2]);
                    glVertex3f(0, 1, 0);
                    glVertex3f(0, 0, 0);
                    glVertex3f(1, 0, 0);
                    glEnd();
                    */

                    glPushMatrix();
                    {
                        glRotatef(yrad / PI_OVER_180, 0, 1, 0);
                        glRotatef(xrad / PI_OVER_180, 1, 0, 0);
                        glTranslatef(0, 0, 1.7);
                        glColor4f(light, light, light, density);
                        Primitive::glXYCenteredTextureSquare(1);
                    }
                    glPopMatrix();
                }
                density *= density;
            }

        }
        glPopMatrix();
    }
    glPopAttrib();
}

/*
void cDomeBackground::drawMountains() {
    srand(762381);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        SGL::glUseProgram_bkplaintexture();
        glDisable(GL_CULL_FACE);

        glBindTexture(GL_TEXTURE_2D, textures[T_MOUNTAIN]);
        glColor3fv(bottomColor);

        // Load current Camera Matrix and
        // set Position (4th Column) = 0.
        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        loop3i(m[12 + i] = 0);
        //m[12 + 0] = m[12 + 2] = 0;
        m[12 + 1] = heightshift;

        glPushMatrix();
        {
            int n = 30;
            float f = 1.0;
            for (int i = 0; i < n; i++) {
                glLoadIdentity();
                glMultMatrixf(m);
                glScalef(90, 100 * 0.2, 90);

                float w = (0.3 + (rand() % 100) * 0.005f) * 0.5 * f;
                float rad = 0.01f * (rand() % 628);
                glRotatef(rad / 0.017453, 0, 1, 0);
                glTranslatef(0, 0, 1);

                float u = 1.2 * f;
                glBegin(GL_QUADS);
                glTexCoord2f(0, 1);
                glVertex3f(-w, u, 1);
                glTexCoord2f(0, 0);
                glVertex3f(-w, -0.01, 1);
                glTexCoord2f(1, 0);
                glVertex3f(+w, -0.01, 1);
                glTexCoord2f(1, 1);
                glVertex3f(+w, u, 1);
                glEnd();

                f = f * 0.95f;
            }
        }
        glPopMatrix();
    }
    glPopAttrib();
}
 */


void Background::drawSun() {
    if (hour < 5.00 || hour > 22.00) return;

    // Setup rotation only matrix.
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    // 4th Row = 0.
    m[0 * 4 + 3] = m[1 * 4 + 3] = m[2 * 4 + 3] = 0.0f;
    // 4th Col = 0.
    m[3 * 4 + 0] = m[3 * 4 + 1] = m[3 * 4 + 2] = 0.0f;
    m[3 * 4 + 3] = 1.0f;
    //m[12 + 1] += heightshift;

    glPushAttrib(GL_ALL_ATTRIB_BITS | GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        GLS::glUseProgram_bkaddtexture();

        glPushMatrix();
        {
            glLoadIdentity();
            glMultMatrixf(m);
            //glScalef(1,0.3f,1);

            const float scale = 0.1 * 2400.0f;
            float sun = 15 * 80 * 0.15;

            static float angle = 0;
            angle -= 1;
            angle = 180 - ((360 / 24.00f) * hour + 90);

            // Sun travels: dawn 0600:East, 1200:South, dusk 1800:West (2400:North -> other side of the globe).
            // 0000/2400: 0 degree, 0600: 90 degree, 1200: 180 degree, 1800: 270 degree, ...
            // Moon is traveling about 180 degree behind/ahead.
            // Well, that is, if you're down-under then east, north, west (not east, south, west).

            glRotatef(-90, 0, 1, 0);
            // Longitude? Aquator Angle Offset -> Germany. and for example Poland, GB and Canada.
            //glRotatef(45+7.5, 0, 0, 1);
            glRotatef(55, 0, 0, 1);
            glRotatef(0, 0, 0, 1); // Aquator.

            glRotatef(angle, 1, 0, 0);
            glTranslatef(0, 0, scale);
            glColor4f(1, 0.95, 0.8, 1);
            glBindTexture(GL_TEXTURE_2D, textures[T_SUN]);
            Primitive::glXYCenteredTextureSquare(sun);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glLoadIdentity();
            const float scale = 0.1 * 2400.0f;
            static float angle = 0;
            angle -= 1;
            angle = 180 - ((360 / 24.00f) * hour + 90);
            glRotatef(-90, 0, 1, 0);
            glRotatef(55, 0, 0, 1);
            glRotatef(0, 0, 0, 1); // Aquator.
            glRotatef(angle, 1, 0, 0);
            glTranslatef(0, 0, scale);
            // Calculate sun/moon direction vector and set as light when above horizon.
            {
                mat4 m;
                vec3 v = { 0, 0, 0 };
                glGetFloatv(GL_MODELVIEW_MATRIX, m);
                matrix_apply2(m, v);
                vector_norm(v, v);
                if (v[1] < 0.0f) vector_scale(v, v, -1.0f);
                //vector_print(v);
                vector_cpy(light, v);
            }
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void Background::drawOrbit() {
    if (hour > 10.00 && hour < 16.00) return;

    // Setup rotation only matrix.
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    // 4th Row = 0.
    m[0 * 4 + 3] = m[1 * 4 + 3] = m[2 * 4 + 3] = 0.0f;
    // 4th Col = 0.
    m[3 * 4 + 0] = m[3 * 4 + 1] = m[3 * 4 + 2] = 0.0f;
    m[3 * 4 + 3] = 1.0f;
    //m[12 + 1] += heightshift;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        GLS::glUseProgram_bkplaintexture();

        glPushMatrix();
        {
            glLoadIdentity();
            glMultMatrixf(m);
            //glScalef(1,0.3f,1);

            const float scale = 0.1 * 2400.0f;
            float sun = 15 * 80 * 0.1;
            //const float corona = 1.2f;
            float moon = sun * 0.25;

            static float angle = 0;
            angle -= 1;
            angle = 180 - ((360 / 24.00f) * hour + 90);

            // Sun travels: dawn 0600:East, 1200:South, dusk 1800:West (2400:North -> other side of the globe).
            // 0000/2400: 0 degree, 0600: 90 degree, 1200: 180 degree, 1800: 270 degree, ...
            // Moon is traveling about 180 degree behind/ahead.
            // Well, that is, if you're down-under then east, north, west (not east, south, west).

            glRotatef(-90, 0, 1, 0);
            //	glRotatef(45+7.5, 0, 0, 1); // Longitude? Aquator Angle Offset -> Germany. and for example Poland, GB and Canada.
            glRotatef(55, 0, 0, 1);
            glRotatef(0, 0, 0, 1); // Aquator.

            glRotatef(angle + 180, 1, 0, 0);
            glTranslatef(0, 0, scale);
            glColor4f(0.8, 0.8, 0.8, 0.99);
            glBindTexture(GL_TEXTURE_2D, textures[T_EARTH]);
            Primitive::glXYCenteredTextureSquare(moon);
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void Background::drawRain() {
    srand(seed);
    seed = rand();

    // Get current Camera-Matrix.
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    // Position from 4th Col.
    float pnt[3];
    loop3i(pnt[i] = m[12 + i]);
    // Reset Position on 4th Col.
    loop3i(m[12 + i] = 0);
    // Apply transpose-inverse to position.
    float n[16];
    memcpy(n, m, sizeof (float) *16);
    matrix_transpose(m);
    matrix_apply2(m, pnt);
    //cout << "height over ground: " << pnt[1] << "\n";
    //cout << "Position: " << pnt[0] << " " << pnt[2] << "\n";
    
    if (raininess > 0) {

        int n = 200 * raininess;
        
        loopi(rand() % n) {
            if (rain.size() >= 1000) break;
            float dx = 0.25f;
            float dz = 0.10f;
            Particle* p = new Particle();
            float alpha = (rand() % 628) * 0.01;
            float dist = 0.2 + 0.8 * (rand() % 75);
            float h = 15;
            vector_set(p->pos, -pnt[0]+sin(alpha) * dist - 0.3 * dx*h, -pnt[1]+h, -pnt[2]+cos(alpha) * dist - 0.3 * dz * h);
            vector_cpy(p->old, p->pos);
            vector_set(p->vel, dx, -8 - 0.03f * (rand() % 100), dz);
            vector_set(p->fce, 0, 0, 0);
            rain.push_back(p);
        }
    }
    
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        GLS::glUseProgram_fgplaincolor();
        glLineWidth(1.25);

        glPushMatrix();
        {
            //glLoadIdentity();
            //glMultMatrixf(n);

            glBegin(GL_LINES);

            foreachNoInc(i, rain) {
                Particle* p = *i++;
                float end[3];
                vector_scale(end, p->vel, -0.4f);
                vector_add(end, end, p->pos);
                glColor4f(0.8, 0.8, 0.8, 0.3);
                glVertex3fv(end);
                glColor4f(0.3, 0.3, 0.5, 0.9);
                glVertex3fv(p->pos);
                p->stepEuler(0.1, 0);
                //std::cout << p->pos[0] << " " << p->pos[1] << " " << p->pos[2] << "\n";
                if (p->pos[1] < -pnt[1]-10) rain.remove(p);
            }
            glEnd();
        }
        glPopMatrix();

    }
    glPopAttrib();
}
