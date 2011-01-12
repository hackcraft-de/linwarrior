// cBackground.cpp

#include "cBackground.h"

#include <cstdio>
#include <memory>
#include <cstdlib>

#include <iostream>
using std::cout;
using std::endl;

#include "cWorld.h"
#include "cLandscape.h"
#include "cSolid.h"

#include "psi3d/macros.h"
#include "psi3d/noise.h"
#include "psi3d/snippetsgl.h"

#define BKGDETAIL 0

#define DOME_XRES 2*6
#define DOME_YRES 7

extern unsigned char* loadTGA(const char *fname, int *w, int* h, int* bpp);

enum Texids {
    T_ICE, T_MOUNTAIN, T_EARTH,
    T_LAVOS, T_MOON, T_SUN, T_CLOUDS,
    T_GALAXY_NEGX, T_GALAXY_POSX,
    T_GALAXY_NEGY, T_GALAXY_POSY,
    T_GALAXY_NEGZ, T_GALAXY_POSZ,
};

unsigned int gPermutationTexture256 = 0;


void ambient_sky(float x, float y, float z, float* color, float hour = 24.00f) {
    // Remember 2pi ~ 360°
    float t = ((hour / 24.0f * 2 * M_PI) - 0.5f * M_PI); // = [-0.5pi,+1.5pi]

    float light = 0.1 + 0.8 * cos((hour - 12.0f) / 12.00f * 0.5f * M_PI);

    // Daylight gradient.
    // hour = [0,24]
    // => [-90,   0, 90, 180, 270]
    // => [ -1,   0,  1,   0,  -1] = sin
    // => [  0, 0.5,  1, 0.5,   0] = s daylight sine-wave.
    float s = sin(t) * 0.5f + 0.5f; // = [0,1]
    float topColor[] = { s * 0.2 + 0.1, s * 0.2 + 0.1, s * 0.8 + 0.2, light };
    float middleColor[] = { s * 0.95 + 0.05, s * 0.95 + 0.05, s * 0.95 + 0.05, 1 };
    float bottomColor[] = { s * 0.55 + 0.05, s * 0.55 + 0.05, s * 0.55 + 0.05, 1 };

    // Dusk and dawn gradient.
    // => [ 0,    1,  0,   1,   0] = c
    float c = pow(fabs(cos(t)) * 0.5f + 0.5f, 4); // = [0,1]
    float top_[] = { c * 0.25, c * 0.25, c * 0.0, light };
    float mid_[] = { c * 0.55, c * 0.25, c * 0.0, 1 };
    float bot_[] = { c * 0.25, c * 0.11, c * 0.0, 1 };
    vector_add(topColor, topColor, top_);
    vector_add(middleColor, middleColor, mid_);
    vector_add(bottomColor, bottomColor, bot_);

    // Normalise direction vector.
    float oolen = 1.0f / sqrt(x*x + y*y + z*z);
    float yd = y * oolen;

    // top = 1 when yd > 0, top = 0 otherwise.
    float top = 0.5f + copysign(0.5f, yd);
    float alpha = fabs(yd);

    // middleColor could be factored out but it may change.
    loopi(4) {
        color[i] =
                top * (alpha * topColor[i] + (1.0f-alpha) * middleColor[i]) +
                (1.0f-top) * (alpha * bottomColor[i] + (1.0f-alpha) * middleColor[i]);
    }
}


char vertexshader[] = " \
    varying vec3 normal, position, lightdir; \
    varying float direction; \
    void main() \
    { \
        gl_FrontColor = gl_Color; \
        gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \
        gl_Position = ftransform(); \
        position = vec3(gl_ModelViewMatrix * gl_Vertex); \
        normal = normalize(gl_NormalMatrix * gl_Normal); \
        vec3 lightdir = normalize(vec3(gl_LightSource[0].position.xyz - position)); \
        direction = max(dot(lightdir,normalize(normal)),0.0); \
    } \
";

char fragmentshader3D[] = " \
    varying vec3 normal, position, lightdir; \
    varying float direction; \
    uniform sampler3D tex; \
    void main() \
    { \
        vec4 ambient = gl_LightSource[0].ambient; \
        vec4 diffuse = gl_LightSource[0].diffuse * direction; \
        vec4 basecolor = texture3D(tex,gl_TexCoord[0].xyz) * gl_Color; \
        gl_FragColor = vec4(basecolor.rgb * diffuse + basecolor.rgb * ambient, basecolor.a); \
    } \
";

char fragmentshader2D[] = " \
    varying vec3 normal, position, lightdir; \
    varying float direction; \
    uniform sampler2D tex; \
    void main() \
    { \
        vec4 ambient = gl_LightSource[0].ambient; \
        vec4 diffuse = gl_LightSource[0].diffuse * direction; \
        vec4 basecolor = texture2D(tex,gl_TexCoord[0].xy) * gl_Color; \
        gl_FragColor = vec4(basecolor.rgb * diffuse + basecolor.rgb * ambient, basecolor.a); \
    } \
";

//        float shade = direction*0.8 + 0.0 * (0.1 * float(int(direction*9.99))) * 0.8 + 0.2; \


char vertexshader_[] = " \
    varying vec3 normal, position; \
    void main() \
    { \
        gl_FrontColor = gl_Color; \
        gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \
        gl_Position = ftransform(); \
        position = vec3(gl_ModelViewMatrix * gl_Vertex); \
        normal = normalize(gl_NormalMatrix * gl_Normal); \
    } \
";

char fragmentshader3D_[] = " \
    varying vec3 normal, position; \
    uniform sampler3D tex; \
    void main() \
    { \
        vec3 lightDir = normalize(gl_LightSource[0].position.xyz - 1.0*position); \
        float direction = max(dot(lightDir,normalize(normal)),0.0); \
        float shade = direction + 0.0 * 0.1 * float(int(direction*10.0)); \
        vec3 lightcolor = 1.0*shade * gl_FrontMaterial.diffuse.rgb + 0.7*gl_FrontMaterial.ambient.rgb; \
        vec4 basecolor = texture3D(tex,gl_TexCoord[0].xyz) * gl_Color * 1.0 + 0.0 * gl_Color; \
        gl_FragColor = vec4(0.99 * shade * basecolor.rgb, basecolor.a); \
    } \
";

char fragmentshader2D_[] = " \
    varying vec3 normal, position; \
    uniform sampler2D tex; \
    void main() \
    { \
        vec3 lightDir = normalize(gl_LightSource[0].position.xyz - 1.0*position); \
        float direction = max(dot(lightDir,normalize(normal)),0.0); \
        float shade = direction + 0.0 * 0.1 * float(int(direction*10.0)); \
        vec3 lightcolor = 1.0*shade * gl_FrontMaterial.diffuse.rgb + 0.7*gl_FrontMaterial.ambient.rgb; \
        vec4 basecolor = texture2D(tex,gl_TexCoord[0].xy) * gl_Color * 1.0 + 0.0 * gl_Color; \
        gl_FragColor = vec4(0.99 * shade * basecolor.rgb, basecolor.a); \
    } \
";


cBackground::cBackground() {

    if (0) {
        GLenum p = glCreateProgramObjectARB();

        GLenum vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        const GLcharARB* vs_source = vertexshader;
        glShaderSourceARB(vs, 1, &vs_source, NULL);
        glCompileShaderARB(vs);
        glAttachObjectARB(p, vs);

        {
            int length;
            glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &length);
            char* log = new char[length];
            glGetShaderInfoLog(vs, length, &length, log);
            cout << "Vertex Shader Log:\n" << log << "\n";
            delete log;
        }

        GLenum fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        const GLcharARB* fs_source = fragmentshader3D;
        glShaderSourceARB(fs, 1, &fs_source, NULL);
        glCompileShaderARB(fs);
        glAttachObjectARB(p, fs);

        {
            int length;
            glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);
            char* log = new char[length];
            glGetShaderInfoLog(fs, length, &length, log);
            cout << "Fragment Shader Log:\n" << log << "\n";
            delete log;
        }

        glLinkProgramARB(p);
        glUseProgramObjectARB(p);

        // Back to normal
        //glUseProgramObjectARB(0);
        //glUseProgram(0);
    }

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
        unsigned char* texels = loadTGA(texturedefs[i].filename, &w, &h, &bpp);
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
                b = cNoise::LFSR8(b);
                *p++ = b;
                *p++ = b;
                *p++ = b;
            }
        }
        bool soft = false;
        bool repeat = true;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, false, soft, repeat, repeat, w, h, bpp, texels);
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
            //float v3f[] = { dx, 0, dz };

            float c = cDistortion::sig(cNoise::voronoi3_16(dx * 16, 0, dz * 16) * 8 + 4);
            float c4f[4] = {c, c, c};

            *p++ = 255 * c4f[2];
            *p++ = 255 * c4f[1];
            *p++ = 255 * c4f[0];
            //*p++ = 255*c4f[3];
        }
        bool soft = false;
        bool repeat = true;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
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
            //float dz = sqrt(1.0f - dx * dx + dy * dy);
            float r = sqrt(dx * dx + dy * dy);
            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 0.6);
            float rad = atan2(dx, dy);

            float c = 0.5f + 0.5f * cNoise::simplex3(rad/(2*M_PI)*256, 0, 0, 21);
            float d = 0.5f + 0.5f * cNoise::simplex3(r*31, 0, 0, 21);

            float c4f[4] = {
                cDistortion::exposure(3 * a + 3 * c * a + 3 * d*d*d * a),
                cDistortion::exposure(3 * a),
                cDistortion::exposure(1 * a),
                a * a
            };

            *p++ = 255 * c4f[2];
            *p++ = 255 * c4f[1];
            *p++ = 255 * c4f[0];
            *p++ = 255 * c4f[3];
        }
        bool soft = true;
        bool repeat = true;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
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
            float r = sqrt(dx * dx + dy * dy);

            float dz = sqrt(1 - dx * dx - dy * dy);

            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 16);

            const float f = 2.0f;
            float v3f[] = {f * dx, f * dy, f * dz};

            float color0[16];
            cSolid::planet_cloud(v3f[0], v3f[1], v3f[2], color0);

            float color1[16];
            //getContinental(v3f, color1);
            cLandscape::experimental_continents(v3f[0], v3f[1], v3f[2], color1);

            *p++ = 255 * fmin(1.0f, +0.2 * color0[2] + 0.99 * color1[2]) * a;
            *p++ = 255 * fmin(1.0f, +0.2 * color0[1] + 0.99 * color1[1]) * a;
            *p++ = 255 * fmin(1.0f, +0.2 * color0[0] + 0.99 * color1[0]) * a;
            *p++ = 255 * a;
        }
        bool soft = true;
        bool repeat = true;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
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
            float r = sqrt(dx * dx + dy * dy);

            float dz = sqrt(1 - dx * dx - dy * dy);

            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 16);

            const float f = 2.0f;
            float v3f[] = {f * dx, f * dy, f * dz};

            //v3f[0] *= 0.1f;
            float color0[16];
            cSolid::planet_cloud(v3f[0], v3f[1], v3f[2], color0);
            //v3f[0] *= 10.0;

            float color1[16];
            cSolid::stone_lava(v3f[0], v3f[1], v3f[2], color1);

            *p++ = 255 * fmax(0.0f, -0.2f * color0[2] + 0.99f * color1[2]) * a;
            *p++ = 255 * fmax(0.0f, -0.2f * color0[1] + 0.99f * color1[1]) * a;
            *p++ = 255 * fmax(0.0f, -0.2f * color0[0] + 0.99f * color1[0]) * a;
            *p++ = 255 * a;
        }
        bool soft = true;
        bool repeat = true;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
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
            float r = sqrt(dx * dx + dy * dy);

            float dz = sqrt(1 - dx * dx - dy * dy);

            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 16);

            const float f = 2.0f;
            float v3f[] = {f * dx, f * dy, f * dz};

            v3f[0] *= 0.1f;
            float color0[4];
            cSolid::planet_cloud(v3f[0], v3f[1], v3f[2], color0);
            v3f[0] *= 10.0;

            float color1[16];
            cSolid::planet_ground(v3f[0], v3f[1], v3f[2], color1);

            *p++ = 255 * fmax(0.8f * color0[2], 0.8f * color1[2]);
            *p++ = 255 * fmax(0.8f * color0[1], 0.8f * color1[1]);
            *p++ = 255 * fmax(0.8f * color0[0], 0.8f * color1[0]);
            *p++ = 255 * a;
        }
        bool soft = true;
        bool repeat = true;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
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
            float r = sqrt(dx * dx + dy * dy);
            float r_ = fmin(1.0f, r);
            float a = 1.0f - pow(r_, 1.2);

            const float f = 2.0f;
            float v3f[] = {f * (float) j / (float) w, 0, f * (float) i / (float) h};
            float color[16];
            cSolid::planet_cloud(v3f[0], v3f[1], v3f[2], color);
            *p++ = 255 * color[2];
            *p++ = 255 * color[1];
            *p++ = 255 * color[0];
            *p++ = 255 * color[3] * a;
        }
        bool soft = true;
        bool repeat = true;
        unsigned int texname;
        texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
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
                const float f = 0.1;
                const float t = f;

                // Project onto Sphere
                float u = 2.0f * ((float) j / (float) (w)) - 1.0f;
                float v = 2.0f * ((float) i / (float) (h)) - 1.0f;
                float a = sign;
                float oolen = 1.0f / sqrt(u * u + v * v + 1.0f);
                u *= oolen;
                v *= oolen;
                a *= oolen;

                float v3f[] = {a, u, v};

                loop(l, rotation) {
                    float tmp = v3f[2];
                    v3f[2] = v3f[1];
                    v3f[1] = v3f[0];
                    v3f[0] = tmp;
                }

                v3f[0] = v3f[0] * f + t;
                v3f[1] = v3f[1] * f + t;
                v3f[2] = v3f[2] * f + t;

                float color[16];
                cSolid::star_nebula(v3f[0], v3f[1], v3f[2], color);
                *p++ = 255 * color[2];
                *p++ = 255 * color[1];
                *p++ = 255 * color[0];
                //*p++ = 255 * color[3];
            }
            const bool soft = true;
            const bool repeat = false;
            unsigned int texname;
            texname = SGL::glBindTexture2D(0, true, soft, repeat, repeat, w, h, bpp, texels);
            textures[T_GALAXY_NEGX + k] = texname;

            if (0) {
                try {
                    std::string fname = std::string("galaxy") + std::string(names[k]) + std::string(".tga");
                    if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                        cout << "Could not save image" << endl;
                    }
                } catch (char const* s) {
                    cout << "ERROR: " << s << endl;
                }
            }
        }
        delete texels;
    }

    heightshift = -5;
    rainstrength = 0;
}

void cBackground::drawBackground(float hour) {
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
    //speed *= 2;
    // every 5 minutes
    //speed *= 3;
    // every minute
    //speed *= 5;
    this->hour = fmod(speed*hour, 24.00f);
    //hour = rand()%24;

    // Sample and set ambient haze color.
    float haze[4] = { 0,0,0,0 };
    if (1) {
        unsigned char s = 131;
        int samples = 23;
        loopi(samples) {
            s = cNoise::LFSR8(s);
            float alpha = s / 256.0f * 2 * M_PI;
            s = cNoise::LFSR8(s);
            float beta = s / 256.0f * 1 * M_PI;
            float color[16];
            float n[4] = { sin(alpha)*sin(beta),cos(beta),sin(beta)*cos(alpha), 1 };
            //vector_print(n);
            ambient_sky(n[0],n[1],n[2], color, hour);
            //quat_print(color);
            loopj(4) {
                haze[j] += color[j];
            }
        }
        loopj(4) {
            haze[j] /= float(samples);
        }
    }
    glFogfv(GL_FOG_COLOR, haze);

    //printf("Haze: "); quat_print(haze);

    // Directional sun and moonlight.
    if (1) {
        float p[] = {0.7, 0.9, -0.3, 0};
        //float p[] = {70, 90, -30, 0};
        float a[] = {0.2 * haze[0], 0.2 * haze[1], 0.2 * haze[2], 1};
        float t = ((hour / 24.0f * 2 * M_PI) - 0.5f * M_PI); // = [-0.5pi,+1.5pi]
        float s = sin(t) * 0.5f + 0.5f;
        float d[] = {0.9 * s + (1-s)*0.15, 0.9 * s + (1-s)*0.15, 0.4 * s + (1-s)*0.2, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, p);
        glLightfv(GL_LIGHT0, GL_AMBIENT, a);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, d);
        //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
        //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0001);
        //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.000001);
        //glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHT0);
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

void cBackground::drawGalaxy() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glEnable(GL_CULL_FACE);

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

void cBackground::drawUpperDome() {
    glPushAttrib(GL_ALL_ATTRIB_BITS | GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glDisable(GL_FOG);
        //glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA );
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // original
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);

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

                    float colour1[4];
                    ambient_sky(s1*c, c1+0.0001f, s1 * s, colour1, hour);
                    glColor4fv(colour1);
                    //glNormal3f(s1*c, c1, s1 * s);
                    glVertex3f(s1*c, c1, s1 * s);

                    float colour2[4];
                    ambient_sky(s2*c, c2+0.0001f, s2 * s, colour2, hour);
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

void cBackground::drawLowerDome() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glDisable(GL_FOG);

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

                    float colour2[4];
                    ambient_sky(s2*c, c2-0.0001f, s2 * s, colour2, hour);
                    glColor4fv(colour2);
                    //glNormal3f(s2*c, c2, s2 * s);
                    glVertex3f(s2*c, c2, s2 * s);

                    float colour1[4];
                    ambient_sky(s1*c, c1-0.0001f, s1 * s, colour1, hour);
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

void cBackground::drawGround() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
    {
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glDisable(GL_CULL_FACE);
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
            glTranslatef(0, p[Y], 0);

            float color2[4] = {0.4, 0.2, 0.1, 1};
            ambient_sky(0, -1, 0, color2, hour);

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

void cBackground::drawClouds() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glDisable(GL_FOG);
        glEnable(GL_TEXTURE_2D);

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

            float windspeed = 2;
            float trad = windspeed * 24.0f * (hour / 24.00f) * 2 * M_PI;
            float light = 0.3 + 0.7 * cos((hour - 12.0f) / 12.00f * 0.5f * M_PI);
            glRotatef(trad / 0.017453f, 1, 0.1, 1);

            //cout << hour << endl;
            srand(12421);
            int n = 5;
            float density = 3.0f / float(1.0f + n);
            density = 0.999f;
            for (int i = 0; i < n; i++) {
                //float xrad = -(rand() % 157) * 0.01f;
                float xrad = -(157 - (rand() % 314)) * 0.01f;
                float yrad = (rand() % 628) * 0.01f;

                /*
                float xaxis[] = { 1,0,0 };
                float qx[4];
                quat_rotaxis(qx, xrad, xaxis);

                float yaxis[] = { 0,1,0 };
                float qy[4];
                quat_rotaxis(qy, yrad, yaxis);

                float qxy[4];
                quat_mul(qxy, qx, qy);

                float x[] = {1,0,0};
                float y[] = {0,1,0};
                float z[] = {0,0,1};
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
                    glRotatef(yrad / 0.017454, 0, 1, 0);
                    glRotatef(xrad / 0.017454, 1, 0, 0);
                    glTranslatef(0, 0, 1.7);
                    glColor4f(light, light, light, density);
                    cPrimitives::glXYCenteredTextureSquare(1);
                }
                glPopMatrix();
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
        glEnable(GL_FOG);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);

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


#if 0

inline float InvSqrt(float x) {
    float xhalf = 0.5f * x;
    int i = *(int*) &x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*) &i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}
#endif

void cBackground::drawSun() {
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
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

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
            cPrimitives::glXYCenteredTextureSquare(sun);
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void cBackground::drawOrbit() {
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
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

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
            cPrimitives::glXYCenteredTextureSquare(moon);

        }
        glPopMatrix();
    }
    glPopAttrib();
}

void cBackground::drawRain() {
    srand(seed);
    seed = rand();

    if (rainstrength > 0) {
        loopi(rand() % rainstrength) {
            if (rain.size() >= 1000) break;
            float dx = 0.3;
            float dz = 0.1;
            cParticle* p = new cParticle();
            float alpha = (rand() % 628) * 0.01;
            float dist = 0.2 + 0.1 * (rand() % 75);
            float h = 15;
            vector_set(p->pos, sin(alpha) * dist - 0.3 * dx*h, h, cos(alpha) * dist - 0.3 * dz * h);
            vector_cpy(p->old, p->pos);
            vector_set(p->vel, dx, -2 - rand() % 3, dz);
            vector_set(p->fce, 0, 0, 0);
            rain.push_back(p);
        }
    }

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    {
        //glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        //glDisable(GL_FOG);
        glLineWidth(1.25);

        glPushMatrix();
        {
            float m[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, m);
            loop3i(m[12 + i] = 0);
            glLoadIdentity();
            glMultMatrixf(m);

            glBegin(GL_LINES);

            foreachNoInc(i, rain) {
                cParticle* p = *i++;
                float end[3];
                vector_scale(end, p->vel, -0.4f);
                vector_add(end, end, p->pos);
                glColor4f(0.8, 0.8, 0.8, 0.3);
                glVertex3fv(end);
                glColor4f(0.3, 0.3, 0.5, 0.9);
                glVertex3fv(p->pos.data());
                p->stepEuler(0.1, 0);
                //std::cout << p->pos[0] << " " << p->pos[1] << " " << p->pos[2] << "\n";
                if (p->pos[1] < -10) rain.remove(p);
            }
            glEnd();
        }
        glPopMatrix();

    }
    glPopAttrib();
}
