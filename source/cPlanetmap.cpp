#include "cPlanetmap.h"

#include "psi3d/macros.h"
#include "psi3d/noise.h"
#include "psi3d/snippetsgl.h"

#include "cTree.h"

#include <cassert>

#include <ostream>

using namespace std;


int cPlanetmap::sInstances = 0;
std::vector<long> cPlanetmap::sTextures;


cPlanetmap::cPlanetmap() {
    sInstances++;
    if (sInstances == 1) {
        // Solid Ground
        {
            const float diffusion = 0.2;
            const float shift = 0.0;
            const int size = 64;
            unsigned char texels[size * size * size * 3];
            int samples = 3;
            float amt = 1.0f / (float)(samples);
            for (int l = 0; l < samples; l++) {
                unsigned char* texel = texels;
                loopijk(size, size, size) {
                    float f = 256.0f / (float)size;
                    float i_ = (rand()%100) * 0.01;
                    float j_ = (rand()%100) * 0.01;
                    float k_ = (rand()%100) * 0.01;
                    unsigned char r = 0, g = 0, b = 0;
                    float a = ((cNoise::voronoi3((i+i_)*f,(j+j_)*f,(k+k_)*f, diffusion, shift) + 1.0f) * 0.5);
                    a = 0.3 + a * 0.8;
                    a = a > 1.0 ? 1.0 : a;
                    a = a < 0.0 ? 0.0 : a;
                    r = a * 255;

                    //float a = ((simplex3(i*f,j*f,k*f, 43) + 1.0f) * 0.5);
                    //r = (0.5 + 0.5 * (a - int(a))) * 255;
                    g = b = r ;
                    if (l == 0) {
                        *texel++ = r * amt;
                        *texel++ = g * amt;
                        *texel++ = b * amt;
                    } else {
                        *texel++ += r * amt;
                        *texel++ += g * amt;
                        *texel++ += b * amt;
                    }
                }
            }
            unsigned int texname = SGL::glBindTexture3D(0, true, true, true, true, true, size, size, size, texels);
            sTextures.push_back(texname);
        }
        // Scattering 1
        {
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = loadTGA("data/base/decals/desertblossom.tga", &w, &h, &bpp);
            texname = SGL::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete texels;
            sTextures.push_back(texname);
        }
        // Scattering 2
        {
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = loadTGA("data/base/decals/hybridplant.tga", &w, &h, &bpp);
            texname = SGL::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete texels;
            sTextures.push_back(texname);
        }
        // Scattering 3
        {
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = loadTGA("data/base/decals/desertplant.tga", &w, &h, &bpp);
            texname = SGL::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete texels;
            sTextures.push_back(texname);
        }
        // Scattering 4
        {
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = loadTGA("data/base/decals/hybridus.tga", &w, &h, &bpp);
            texname = SGL::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete texels;
            sTextures.push_back(texname);
        }
        sTextures.push_back(sTextures.back());
    }
    this->traceable->radius = 10;
    this->traceable->pos[0] = float_NAN;
    this->traceable->pos[1] = float_NAN;
    this->traceable->pos[2] = float_NAN;
    collideable = new rCollideable;
    addRole(COLLIDEABLE);
    damageable = new rDamageable;
    addRole(DAMAGEABLE);
    nameable->name = "PLANETMAP";
}



void cPlanetmap::getHeight(float x, float z, float* const color) {
    float h = 0;
    float y = 0;

    if (1) {
        float grass[16] = {0,0,0, };
        float snow[16] = {0,0,0, };
        float stone[16] = {0,0,0, };

        // Blending between types.
        const int seed1 = 121;
        const float p1 = 1.0f / 301.0f;
        float o1 = cNoise::simplex3(7 + x*p1, 15 + y*p1, 19 + z*p1, seed1);
        o1 = cDistortion::sig(o1*28.0f);

        float h1 = 0;
        float h0 = 0;
#if 0
        // Snowset
        if (o1 > 0.005f) {
            cLandscape::land_snow(x, y, z, snow);
            h1 = 1 * snow[cLandscape::BUMP];
        }
        if (o1 < 0.995f) {
            cLandscape::land_snow(x, y, z, grass);
            h0 = 10 * grass[cLandscape::BUMP];
        }
#elif 0
        // Lavaset
        if (o1 > 0.005f) {
            cLandscape::mashup_lava(x, y, z, snow);
            h1 = 1 * snow[cLandscape::BUMP];
        }
        if (o1 < 0.995f) {
            cLandscape::land_lava(x, y, z, grass);
            h0 = 10 * grass[cLandscape::BUMP];
        }
#elif 1
        // Duneset
        if (o1 > 0.005f) {
            cLandscape::land_dunes(x, y, z, snow);
            h1 = 1 * snow[cLandscape::BUMP];
        }
        if (o1 < 0.995f) {
            cLandscape::land_dunes(x*0.5f, y*0.5f, z*0.5f, grass);
            h0 = 15 * grass[cLandscape::BUMP];
        }
#else
        // Grassset
        if (o1 > 0.005f) {
            cLandscape::land_grass(x, y, z, snow);
            h1 = 1 * snow[cLandscape::BUMP];
        }
        if (o1 < 0.995f) {
            cLandscape::land_grass(x, y, z, grass);
            h0 = 40 * grass[cLandscape::BUMP];
        }
#endif
        
        // Original 1:1 mix
        //h = (1.0f - o1) * h0 + o1 * h1 - 9.0f;

        // (1+0.5):1 mix
        h = (1.0f - o1) * (h0+0.5*h1) + o1 * h1 - 9.0f;

        float r = (1.0f-o1)*grass[0] + o1*snow[0];
        float g = (1.0f-o1)*grass[1] + o1*snow[1];
        float b = (1.0f-o1)*grass[2] + o1*snow[2];

        cLandscape::decoration_stones(x, y, z, stone);
        float hs = 7.0f * stone[cLandscape::BUMP];

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
        float o2 = cNoise::simplex3(73 + x*p2, 21 + y*p2, 39 + z*p2, seed2);
        h += cDistortion::sig(o2*4-1.4f) * 30;
    }

    if (0) {
        float f = 0.5;
        float cont[16];
        cLandscape::experimental_continents(x*f, y*f, z*f, cont);
        float hx = 15.0f / f * (cont[cLandscape::BUMP] - 0.5f);
        //if (hx > 0.0) h += 2.0f*hx;
        //else h = hx;
        //h = hx+h;
        h = hx;
        color[0] = cont[0];
        color[1] = cont[1];
        color[2] = cont[2];
    }

    // Modification (Big City)
    {
        const float range = 70+65;
        const float height = 0.009f;
        const float inv_range = 1.0f / range;
        float x_ = x - 50;
        float z_ = z - 50;
        if (-range < x_ && x_ < +range && -range < z_ && z_ < +range) {
            float dx = fabs(x_);
            float dz = fabs(z_);
            float dy = 1.0f - fmax(dx,dz) * inv_range;
            dy = (dy < 0.25f) ? (dy * 4.0f) : 1.0f;
            h *= (1.0f - dy);
            h += height * dy;
        }
    }
    // Modification (Far City)
    {
        const float range = 60;
        const float height = 0.011f;
        const float inv_range = 1.0f / range;
        float x_ = x - 360;
        float z_ = z - 125;
        if (-range < x_ && x_ < +range && -range < z_ && z_ < +range) {
            float dx = fabs(x_);
            float dz = fabs(z_);
            float dy = 1.0f - fmax(dx,dz) * inv_range;
            dy = (dy < 0.5f) ? (dy * 2.0f) : 1.0f;
            h *= (1.0f - dy);
            h += height * dy;
        }
    }
    // Modification Space-Port (-260 -180)
    {
        const float range = 90;
        const float height = 0.011f;
        const float inv_range = 1.0f / range;
        float x_ = x + 260;
        float z_ = z + 180;
        if (-range < x_ && x_ < +range && -range < z_ && z_ < +range) {
            float dx = fabs(x_);
            float dz = fabs(z_);
            float dy = 1.0f - fmax(dx,dz) * inv_range;
            dy = (dy < 0.25f) ? (dy * 4.0f) : 1.0f;
            h *= (1.0f - dy);
            h += height * dy;
        }
    }

    // Modification Pyramid (-210 285)
    {
        const float range = 100;
        const float height = 0.011f;
        const float inv_range = 1.0f / range;
        //float x_ = x + 210;
        //float z_ = z - 285;
        float x_ = x + 140;
        float z_ = z - 210;
        if (-range < x_ && x_ < +range && -range < z_ && z_ < +range) {
            float dx = fabs(x_);
            float dz = fabs(z_);
            float dy = 1.0 - fmax(dx,dz) * inv_range;
            dy = (dy < 0.25f) ? (dy * 4.0f) : 1.0f;
            h *= (1.0f - dy);
            h += height * dy;
        }
    }

    color[cLandscape::BUMP] = h;
} // getHeight



void cPlanetmap::getCachedHeight(float x, float z, float* const color) {
    // nan?
    if (x != x || z != z) {
        color[0] = 1;
        color[1] = 0;
        color[2] = 0;
        color[3] = 0;
    }
#if 0
    getHeight(x, z, color);
    return color[cLandscape::BUMP];
#else
    const unsigned CACHESIZE = 64; // 24
    const long PATCHSIZE = 7; // 2^patchsize (8)
    const long TILESIZE = 9; // 2^tilesize (10)
    const long tilesize_ = (1L << TILESIZE)-1;
    const long detailfactor = (1L << (TILESIZE - PATCHSIZE));
    long tx = ((long) x) >> PATCHSIZE;
    long tz = ((long) z) >> PATCHSIZE;
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
            std::map<unsigned long,sPatch*>::iterator mini;
            std::map<unsigned long,sPatch*>::iterator i;
            for (i = patches.begin(); i != patches.end(); i++) {
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
            assert(patch != NULL);
        }
        patch->touches = 0;
        patch->key = key;
        patches[patch->key] = patch;
        float* ptr = patch->heightcolor;

        const unsigned long totaltilesize = (1UL << (TILESIZE + TILESIZE)) << 2;
        for (unsigned long i = 0; i < totaltilesize; i++) {
            *ptr++ = float_NAN;
        }

        /*
        tx = ((long) x) >> patchsize;
        tz = ((long) z) >> patchsize;
        loopj(tilesize_+1) {
            loopi(tilesize_+1) {
                loopk(4) {
                *ptr++ = float_NAN;
                //*ptr++ = getHeight((tx << patchsize)+i*0.5f, (tz << patchsize)+j*0.5f, 4);
                }
            }
        }
        */
        ptr = patch->heightcolor;
        cout << "Patch=" << (recycled?"recycled":"new") << ", total=" << patches.size() << ", key=" << key << ", ptr=" << ptr << "\n";
    }
    patch->touches++;

    unsigned long fx = ((unsigned long)(fabs(x) * detailfactor));
    unsigned long fz = ((unsigned long)(fabs(z) * detailfactor));
    fx  &= tilesize_;
    fz  &= tilesize_;

    float* h = &(patch->heightcolor[(((fx << TILESIZE) | fz))<<2]);

    if (*h != *h) {
        getHeight(x, z, color);
        *h++ = color[0];
        *h++ = color[1];
        *h++ = color[2];
        *h = color[3];
    } else {
        color[0] = *h++;
        color[1] = *h++;
        color[2] = *h++;
        color[3] = *h;
    }
#endif
} // getCachedHeight


float cPlanetmap::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    //if (enactor == NULL) return 0.0;
    //if (!enactor->hasRole(HUMANPLAYER)) return 0.0;
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
        float near[] = {0, 0, 0};

        loopi(samples) {
            float x_ = localpos_[0];
            float y_ = localpos_[1];
            float z_ = localpos_[2];
            float alpha = 0;
            float beta = 0;
            if (i != 0) {
                //alpha = i * a;
                //beta = i * b;
                alpha = (rand()%62831)*0.0001f; // 2PI 0-360
                beta =  (rand()%15707)*0.0001f; // 0.5PI 0-90
            }
            x_ += sin(alpha) * sin(beta) * radius;
            y_ += -cos(beta) * radius;
            z_ += cos(alpha) * sin(beta) * radius;

            // TODO: When already deep in ground there needs to be special handling.

            getCachedHeight(x_, z_, color);
            float h00 = color[cLandscape::BUMP];
            float delta = y_ - h00;
            if (delta > 0) continue;
            if (h00 > localpos_[1]) {
                h00 = localpos_[1];
            }
            float v[] = {localpos_[0] - x_, localpos_[1] - h00, localpos_[2] - z_};
            float dist2 = vector_dot(v, v);
            if (dist2 < nearest2) {
                nearest2 = dist2;
                vector_cpy(near, v);
            }
        }
        float nearest = sqrt(nearest2);
        float delta = radius - nearest;
        if (delta > 0.01) {
            //vector_print(near);
            //cout << radius << " " << nearest << " " << delta << endl;

            vector_scale(near, near, 1.0f / (nearest+0.000001f));
            vector_scale(near, near, delta * relax);
            vector_add(localpos_, localpos_, near);
        } else if (j > 2) {
            break;
        }
        maxdelta = fmax(maxdelta, delta);
    }
    //cout << "it " << runs << endl;

    if (maxdelta > 0) {
        vector_cpy(worldpos, localpos_);
        if (localpos != NULL) vector_cpy(localpos, localpos_);
    }
    return maxdelta;
}


int T = 0;
int detail = 0;


#if 0

void cPlanetmap::drawSolid() {

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

    // nan position?
    if (p[0] != p[0] || p[2] != p[2]) return;

    //cout << "here!"<< endl;
    if (T % 100 == 0) detail = (detail + 1) % 4;
    detail = 5;

    float color[16];
    getCachedHeight(-p[0],-p[2], color);
    float relheight = p[1] + color[cLandscape::BUMP];

    int hscale = -relheight * 0.05f;
    hscale = (hscale < 2) ? hscale : 1;
    if (0) detail -= hscale;

    //detail = 8;
    T++;
    glPushMatrix();
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_3D);
            glEnable(GL_LIGHTING);
            glEnable(GL_FOG);
            glEnable(GL_COLOR_MATERIAL);
            //glEnable(GL_NORMALIZE);

            glBindTexture(GL_TEXTURE_3D, sTextures[0]);

            float step = 64;
            int s = step * 20; // (80)

            loopi(detail) {
                step *= 0.5;
                s >>= 1;
            }
            detail = 2;
            float x = -((int) (p[0] / step)) * step;
            float z = -((int) (p[2] / step)) * step;
            assert(!(x!=x));
            assert(!(z!=z));
            //float c[3];
            float ts = 0.2;
            //cout << x << " " << z << endl;


            float scale = 1.2;
            int maxlevels = 3;
            step = scale * 0.8f * (1 << maxlevels);
            s = scale * 2*64;
            float areashrink = 0.5f;
            float stepshrink = 0.5f;

            // From outer to inner ring
            for (int k = 1; k <= maxlevels; k++) {

                //glDepthMask(false);
                glColor4f(0,0,1,1);

                float s_ = (areashrink * s) - 3*step;
                if (k == maxlevels) s_ = 0;

                x = -((int) (p[0] / step)) * step;
                z = -((int) (p[2] / step)) * step;

                for (float i = x - s; i < x + s; i += step) {
                    glBegin(GL_TRIANGLE_STRIP);
                    bool skip = false;
                    for (float j = z - s; j <= z + s; j += step) {

                        float alpha_i_z = fmin(1, 0.1f * fmax(0, fabs(j - -p[2]) - s_));
                        float alpha_i_x = fmin(1, 0.1f * fmax(0, fabs(i - -p[0]) - s_));
                        float alpha_o_z = fmin(1, 0.16f * fmax(0, s - fabs(j - -p[2])));
                        float alpha_o_x = fmin(1, 0.16f * fmax(0, s - fabs(i - -p[0])));
                        float alpha_o = fmin(alpha_o_z, alpha_o_x);
                        float alpha_i = fmax(alpha_i_z, alpha_i_x);

                        float alpha = alpha_o;//fmin(alpha_i, alpha_o);

                        if (alpha_o <= 0.0f || alpha_i <= 0.0f) {
                        //if ((fabs(z - j) < (s_)) && ((x - i) > -s_ && (x - i) < s_ )) {
                            skip = true;
                            continue;
                        }
                        if (skip) {
                            glEnd();
                            skip = false;
                            glBegin(GL_TRIANGLE_STRIP);
                        }

                        getHeight(i + step, j, color);
                        float h0 = color[cLandscape::BUMP] + 0.005f * k;

                        //const float dext = s*s*0.9;//318*318;
                        //float d2 = ((j + p[2])*(j + p[2]) + (i + step + p[0])*(i + step + p[0]));
                        //float alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                        color[3] = alpha;

                        glNormal3f(0,1,0);
                        glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                        glTexCoord3f((i + step)*ts, h0*ts, j*ts);
                        glVertex3f(i + step, h0, j);

                        getHeight(i + 0, j, color);
                        float h1 = color[cLandscape::BUMP] + 0.005f * k;

                        //d2 = ((j + p[2])*(j + p[2]) + (i + p[0])*(i + p[0]));
                        //alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                        color[3] = alpha;

                        glNormal3f(0,1,0);
                        glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                        glTexCoord3f((i + 0)*ts, h1*ts, j*ts);
                        glVertex3f(i + 0, h1, j);
                    }
                    glEnd();
                }

                s *= areashrink;
                step *= stepshrink;

            } // for k rings

        }
        glPopAttrib();
    }
    glPopMatrix();

    // Reset Touches for LRU
    for (std::map<unsigned long,sPatch*>::iterator i = patches.begin(); i != patches.end(); i++) {
        //cout << "A2" << endl;
        sPatch* patch_ = i->second;
        if (patch_ == NULL) continue;
        patch_->touches = 0;
    }
}


#else
void cPlanetmap::drawSolid() {

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

    // nan position?
    if (p[0] != p[0] || p[2] != p[2]) return;

    //cout << "here!"<< endl;
    if (T % 100 == 0) detail = (detail + 1) % 4;
    detail = 5;

    float color[16];
    getCachedHeight(-p[0],-p[2], color);
    float relheight = p[1] + color[cLandscape::BUMP];

    int hscale = -relheight * 0.05f;
    hscale = (hscale < 2) ? hscale : 1;
    if (0) detail -= hscale;

    //detail = 8;
    T++;
    glPushMatrix();
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_3D);
            glEnable(GL_LIGHTING);
            glEnable(GL_FOG);
            glEnable(GL_COLOR_MATERIAL);
            //glEnable(GL_NORMALIZE);
            
            glBindTexture(GL_TEXTURE_3D, sTextures[0]);

            float step = 64;
            int s = step * 20; // (80)

            loopi(detail) {
                step *= 0.5;
                s >>= 1;
            }
            detail = 2;
            float x = -((int) (p[0] / step)) * step;
            float z = -((int) (p[2] / step)) * step;
            assert(!(x!=x));
            assert(!(z!=z));
            //float c[3];
            float ts = 0.2;
            //cout << x << " " << z << endl;



            // Outer
            //glDepthMask(false);
            glColor4f(0,0,1,1);
            step *= 4;
            float s_ = 3*s* 0.999 - 1.6*step;
            s *= 6;
            x = -((int) (p[0] / step)) * step;
            z = -((int) (p[2] / step)) * step;
            for (float i = x - s; i < x + s; i += step) {
                glBegin(GL_TRIANGLE_STRIP);
                bool skip = false;
                for (float j = z - s; j < z + s; j += step) {

                    if ((fabs(z - j) < (s_)) && (fabs(x - i) < (s_))) {
                        skip = true;
                        continue;
                    }
                    if (skip) {
                        glEnd();
                        skip = false;
                        glBegin(GL_TRIANGLE_STRIP);
                    }

                    getHeight(i + step, j, color);
                    float h0 = color[cLandscape::BUMP] - 0.1f;

                    const float dext = 218*218;
                    float d2 = ((j + p[2])*(j + p[2]) + (i + step + p[0])*(i + step + p[0]));
                    float alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                    color[3] = alpha;

                    glNormal3f(0,1,0);
                    glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                    glTexCoord3f((i + step)*ts, h0*ts, j*ts);
                    glVertex3f(i + step, h0, j);

                    getHeight(i + 0, j, color);
                    float h1 = color[cLandscape::BUMP] - 0.1f;

                    d2 = ((j + p[2])*(j + p[2]) + (i + p[0])*(i + p[0]));
                    alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                    color[3] = alpha;

                    glNormal3f(0,1,0);
                    glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                    glTexCoord3f((i + 0)*ts, h1*ts, j*ts);
                    glVertex3f(i + 0, h1, j);
                }
                glEnd();
            }
            step /= 4;
            s /= 6;



            // Middle
            //glDepthMask(false);
            glColor4f(0,1,0,1);
            step *= 2;
            s_ = s * 0.999 - 1.6*step;
            s *= 3;
            x = -((int) (p[0] / step)) * step;
            z = -((int) (p[2] / step)) * step;
            for (float i = x - s; i < x + s; i += step) {
                glBegin(GL_TRIANGLE_STRIP);
                bool skip = false;
                for (float j = z - s; j < z + s; j += step) {
                    
                    if ((fabs(z - j) < (s_)) && (fabs(x - i) < (s_))) {
                        skip = true;
                        continue;
                    }
                    if (skip) {
                        glEnd();
                        skip = false;
                        glBegin(GL_TRIANGLE_STRIP);
                    }

                    getCachedHeight(i + step, j, color);
                    float h0 = color[cLandscape::BUMP] - 0.1f;

                    const float dext = 288*288;
                    float d2 = ((j + p[2])*(j + p[2]) + (i + step + p[0])*(i + step + p[0]));
                    float alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                    color[3] = alpha;

                    glNormal3f(0,1,0);
                    glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                    glTexCoord3f((i + step)*ts, h0*ts, j*ts);
                    glVertex3f(i + step, h0, j);

                    getCachedHeight(i + 0, j, color);
                    float h1 = color[cLandscape::BUMP] - 0.1f;

                    d2 = ((j + p[2])*(j + p[2]) + (i + p[0])*(i + p[0]));
                    alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                    color[3] = alpha;

                    glNormal3f(0,1,0);
                    glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                    glTexCoord3f((i + 0)*ts, h1*ts, j*ts);
                    glVertex3f(i + 0, h1, j);
                }
                glEnd();
            }
            step /= 2;
            s /= 3;



            // Inner
            x = -((int) (p[0] / step)) * step;
            z = -((int) (p[2] / step)) * step;
            glDepthMask(true);
            glColor4f(1,1,1,1);
            for (float i = x - s; i < x + s; i += step) {
                glBegin(GL_TRIANGLE_STRIP);
                for (float j = z - s; j < z + s; j += step) {

                    getCachedHeight(i + step, j, color);
                    float h0 = color[cLandscape::BUMP];

                    //const float dext = 200*200;//110*110;//158*158;
                    //float d2 = ((j + p[2])*(j + p[2]) + (i + step + p[0])*(i + step + p[0]));
                    //float alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                    color[3] = 1.0f;//alpha;

                    glNormal3f(0,1,0);
                    glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                    glTexCoord3f((i + step)*ts, h0*ts, j*ts);
                    glVertex3f(i + step, h0, j);

                    getCachedHeight(i + 0, j, color);
                    float h1 = color[cLandscape::BUMP];
                    
                    //d2 = ((j + p[2])*(j + p[2]) + (i + p[0])*(i + p[0]));
                    //alpha = (d2 < dext) ? 1 : 1.0f/(1.0f + 0.002f*(d2-dext));
                    color[3] = 1.0f;//alpha;

                    glNormal3f(0,1,0);
                    glColor4f(fmin(1.0f,color[0]),fmin(1.0f,color[1]),fmin(1.0f,color[2]),fmin(1.0f,color[3]));
                    glTexCoord3f((i + 0)*ts, h1*ts, j*ts);
                    glVertex3f(i + 0, h1, j);
                }
                glEnd();
            }

        }
        glPopAttrib();
    }
    glPopMatrix();

    // Reset Touches for LRU
    for (std::map<unsigned long,sPatch*>::iterator i = patches.begin(); i != patches.end(); i++) {
        //cout << "A2" << endl;
        sPatch* patch_ = i->second;
        if (patch_ == NULL) continue;
        patch_->touches = 0;
    }
}
#endif


void cPlanetmap::drawEffect() {
    // Pseudorandom Permutation.
    static unsigned char perms[3*256];
    static bool init = true;
    if (init) {
        init = false;
        unsigned char b = 131;
        loopi(3*256) {
            if ((i & 255) == 255) {
                perms[i] = 255;
            } else {
                perms[i] = b = cNoise::LFSR8(b);
            }
        }
    }

    // Get current Camera-Matrix.
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    // Position from 4th Col.
    float p[3];
    loop3i(p[i] = m[12 + i]);
    // Reset Position on 4th Col.
    loop3i(m[12 + i] = 0);
    // Apply transpose-inverse to position.
    matrix_transpose(m);
    matrix_apply2(m, p);

    // Construct Billboarding Matrix.
    float n[16];
    SGL::glGetTransposeInverseRotationMatrix(n);
    vector_set(&n[4], 0,1,0);
    vector_cross(&n[0], &n[4], &n[8]);
    vector_cross(&n[8], &n[0], &n[4]);

    float* look = &n[8];
    int xlook = -((look[0]<0)?+1:-1);
    int zlook = -((look[2]<0)?+1:-1);

    // nan?
    if (p[0] != p[0] || p[2] != p[2]) return;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_FOG);
        glColor4f(0.8f, 0.8f, 0.8f, 1);
        glNormal3f(0,1,0);

        const float step = 16;
        //const float near = (step*2)*(step*2);
        const float s = 4 * step;
        const float x = -((int) (p[0] / step)) * step;
        const float z = -((int) (p[2] / step)) * step;
        //cout << x << " " << z << endl;
        float  color[16];

        glBindTexture(GL_TEXTURE_2D, sTextures[1]);

        // Loop in positive or negative x direction (painters algorithm).
        float i = x-s*xlook;
        while (true) {
            if (fabs(x+s*xlook - i) < 0.0001f) break;
            float x_ = i;

            // Loop in positive or negative z direction (painters algorithm).
            float j = z-s*zlook;
            while (true) {
                if (fabs(z+s*zlook - j) < 0.0001f) break;
                float z_ = j;

                float dx = (-p[0]-x_-0.5f*step);
                float dz = (-p[2]-z_-0.5f*step);
                float dist2 = dx*dx + dz*dz;

                const float maxz = 80;
                // opacity: near = 1, >far = 0
                float opacity = fmax(0.0f, (2.0f / (1.0f + (1.0f/maxz*maxz) * dist2)) - 1.0f);
                opacity = (2.0f / (1.0f + (1.0f/(maxz*maxz)) * dist2) - 1.0f);

                glColor4f(1, 1, 1, opacity);

                // Field index key
                unsigned char a = (char) x_;
                unsigned char b = (char) z_;
                unsigned char key = cNoise::permutation2d(perms, a,b);

                float plantdensity = (key*3) / 256.0f;
                int visibleplants = plantdensity * opacity;

                key = cNoise::LFSR16(key);
                float treedensity = (key*1.05) / 256.0f;
                float visibletrees = treedensity;
                
                //cout << "opacity " << opacity << "  density " << density << endl;

                // Load LFSR using position, key and some scrambling.
                unsigned short lfsr16 = (((unsigned short)(a^key)) << 8) | (b^key);
                unsigned short lfsr16tmp = lfsr16;
                lfsr16 = (lfsr16 == 0) ? 1 : lfsr16;
                lfsr16 ^= 0x7493;

                loopi(visibleplants) {
                    glPushMatrix();
                    {
                        lfsr16 = cNoise::LFSR16(lfsr16);
                        a = lfsr16;
                        b = lfsr16 >> 8;

                        float x__ = x_ + step * 0.003906f * a;
                        float z__ = z_ + step * 0.003906f * b;
                        getCachedHeight(x__, z__, color);
                        float h = color[cLandscape::BUMP];

                        lfsr16 = cNoise::LFSR16(lfsr16);
                        //unsigned char kind = lfsr16;
                        unsigned char rot = lfsr16 >> 8;

                        //lfsr16 = LFSR16(lfsr16);
                        //unsigned char size = lfsr16;
                        unsigned char size = rot;

                        unsigned char tex = (rot >> 4) & 3;
                        glBindTexture(GL_TEXTURE_2D, sTextures[tex + 1]);

                        glTranslatef(x__, h-0.2, z__);
                        glMultMatrixf(n);
                        //glRotatef(rot*0.351563f, 0, 1, 0);
                        float s = 0.6 + 1.1*(size * 0.003906f) + 0.2 * plantdensity;
                        glScalef(1*s,0.65*s,1*s);
                        //glAxis(0.9);
                        cPrimitives::glXCenteredTextureSquare();
                        //glRotatef(90, 0, 1, 0);
                        //glXCenteredTextureSquare(sTextures[tex + 1]);
                    }
                    glPopMatrix();
                } // loopi visibleplants

                lfsr16 = lfsr16tmp;
                loopi(visibletrees) {
                    glPushMatrix();
                    {
                        lfsr16 = cNoise::LFSR16(lfsr16);
                        a = lfsr16;
                        b = lfsr16 >> 8;

                        float x__ = x_ + step * 0.003906f * a;
                        float z__ = z_ + step * 0.003906f * b;
                        getCachedHeight(x__, z__, color);
                        float h = color[cLandscape::BUMP];

                        lfsr16 = cNoise::LFSR16(lfsr16);
                        //unsigned char kind = lfsr16;
                        //unsigned char rot = lfsr16 >> 8;

                        //lfsr16 = LFSR16(lfsr16);
                        //unsigned char size = lfsr16;
                        //unsigned char size = rot;

                        int age = fmax(0, fmin(3, i));

                        glTranslatef(x__, h-0.2, z__);
                        glDisable(GL_TEXTURE_2D);
                        //GLuint dlist = cTree::compileTreeDisplaylist(1230+(b&2), c, 2+(a&1));
                        //GLuint dlist = cTree::compileTreeDisplaylist(1230+(b&2), 2, 2+(a&1));
                        GLuint dlist = cTree::compileTreeDisplaylist(1230+(b&2), (b*b)&3, 2+age);
                        glCallList(dlist);
                        glEnable(GL_TEXTURE_2D);
                        glColor4f(1, 1, 1, 1);
                        glNormal3f(0,1,0);
                    }
                    glPopMatrix();
                } // loopi visibletrees

                j += step*zlook;
            } // while j z

            i += step*xlook;
        } // while i x
    }
    glPopAttrib();
}

