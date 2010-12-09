/* 
 * File:     cSolid.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 28. November 2010, 13:48
 */

#ifndef CSOLID_H
#define	CSOLID_H

#include "psi3d/noise.h"
#include <cmath>

struct cSolid {
    enum {
        RED, GREEN, BLUE, ALPHA, BUMPMAP, SHININESS
    };

    /**
     * ~[0,1] normalized lavatic landscape.
     */
    static void stone_lava(float x, float y, float z, float* color, unsigned char seed = 131) {
        const float scale = 128;

        x = scale*x;
        y = scale*y;
        z = scale*z;

        float power = 5;
        float wlen = 1.0f / (2 * power);
        float dx = power * cNoise::simplex3(37 + x*wlen, 56 + y*wlen, 97 + z*wlen, seed + 0);
        float dz = power * cNoise::simplex3(67 + x*wlen, 31 + y*wlen, 17 + z*wlen, seed + 1);
        x += dx;
        z += dz;

        // Prime periods => large smallest common multiple.
        const float p1 = 1.0f / 101.0f;

        float x_ = 90 + x * p1;
        float y_ = 39 + y * p1;
        float z_ = 10 + z * p1;

        float o1 = cNoise::simplex3(x_, y_, z_, seed + 2);

        const float a = 0.5f * p1;
        const float b = a * 0.5f;
        const float c = b * 0.5f;

        float o1_ = cNoise::simplex3(x_ + a, y_ + c, z_ + b, seed + 3);

        // Limit to about [0,1] range.
        float h = 1.0f - (fmin(o1, -o1_)*0.98f + 0.96f);
        //h = (h < 0.999999f) ? h : 0.999999f;

        //float diffusion = 0.9;
        //float shift = 0.5;
        //h = 1.0f-voronoi3(x_, 0, z_,diffusion,shift);

        float ash[] = {0.3f, 0.3f, 0.35f};
        float lava[] = {1.0f, 0.0f, 0.0f};
        float glow[] = {1.1f, 1.1f, 1.1f};
        float ash_alpha = h*h;
        float lava_alpha = 0.25f * (2.0f - 2.0f * h)*(2.0f - 2.0f * h);
        float glow_alpha = (h < 0.2f) ? 1.2f - 5.0f * h : 0.0f;
        color[0] = cDistortion::sig(6 * (lava[0] * lava_alpha + ash[0] * ash_alpha + glow[0] * glow_alpha) - 3);
        color[1] = cDistortion::sig(6 * (lava[1] * lava_alpha + ash[1] * ash_alpha + glow[1] * glow_alpha) - 3);
        color[2] = cDistortion::sig(6 * (lava[2] * lava_alpha + ash[2] * ash_alpha + glow[2] * glow_alpha) - 3);
        color[3] = 1.0f;
        //color[BUMP] = h;
    }

    static void concrete_cracked(float x, float y, float z, float* color, unsigned char seed = 131) {
        float grain = 0.2f + 0.1f * (0.5f + 0.5f * cNoise::simplex3(x*128, y*128, z*128, seed + 0) );

        float cracks0 = cDistortion::sig(cNoise::voronoi3(x*16, y*16, z*16, 0.99f, 0.5f, 2.0f, seed + 0) * 16.0f - 16.0f);
        float cracks1 = 0.0f;//cDistortion::sig(cNoise::voronoi3(x*32, y*32, z*32, 0.99f, 0.5f, 2.0f, seed+1) * 16.0f - 16.0f);
        float cracks2 = fmax(cracks0*0.99, 0.4*cracks1);
        float cracks3 = cracks2 * (0.2f + 0.8f * cNoise::simplex3(x*8, y*8, z*8, seed+1) );

        color[0] = color[1] = color[2] = grain - grain * 0.9f * cracks3;
        color[3] = 1.0f;
    }

    static void camo_urban(float x, float y, float z, float* color, unsigned char seed = 131) {
        float gradient = cNoise::simplex3(x, y, z, seed + 0) * 0.5f + 0.5f;
        float band = 3;
        float steps = ((int)(band * gradient + 0.01f)) / band;
        float r = steps * 0.85f;
        float g = r;
        float b = r;

        float f = 8.0f;
        float pattern = cNoise::simplex3(f*x, f*y, f*z, seed + 1) * 0.5f + 0.5f;
        r += pattern * 0.117f;
        g += pattern * 0.117f;
        b += pattern * 0.117f;

        r *= 0.7f;
        g *= 0.7f;
        b *= 0.8f;

        color[0] = r;
        color[1] = g;
        color[2] = b;
    }


    static void camo_wood(float x, float y, float z, float* color, unsigned char seed = 131) {
        float gradient = cNoise::simplex3(x, y, z, seed + 0) * 0.5f + 0.5f;
        float band = 3;
        float steps = floor(band * gradient + 0.01f) / band;
        float r = steps * 0.85f;
        float g = r;
        float b = r;

        float f = 8.0f;
        float grain = cNoise::simplex3(f*x, f*y, f*z, seed + 1) * 0.5f + 0.5f;
        r += grain * 0.117f;
        g += grain * 0.117f;
        b += grain * 0.117f;

        r = 0.1 + r * 0.5f;
        g = 0.1 + r * 0.6f;
        b = 0.0 + r * 0.1f;

        color[0] = r;
        color[1] = g;
        color[2] = b;
        color[3] = 1.0f;
    }


    static void camo_desert(float x, float y, float z, float* color, unsigned char seed = 131) {
        float gradient = cNoise::simplex3(x, y, z, seed + 0) * 0.5f + 0.5f;
        float band = 3;
        float steps = ((int)(band * gradient + 0.01f)) / band;
        float a1 = steps * 0.85f;

        float f = 2.0f;
        float grain = cNoise::simplex3(f*x, f*y, f*z, seed + 1);
        float a2 = cDistortion::sig(grain*16+9);

        color[0] = fmin(0.50f + a1 * 0.11f, a2);
        color[1] = fmin(0.45f + a1 * 0.10f, a2);
        color[2] = fmin(0.30f + a1 * 0.00f, a2);
        color[3] = 1.0f;
    }


    static void camo_snow(float x, float y, float z, float* color, unsigned char seed = 131) {
        float gradient = cNoise::simplex3(x, y, z, seed + 0) * 0.5f + 0.5f;
        float band = 3;
        float steps = ((int)(band * gradient + 0.01f)) / band;
        float a1 = steps * 0.85f;

        float f = 2.0f;
        float grain = cNoise::simplex3(f*x, f*y, f*z, seed + 1);
        float a2 = cDistortion::sig(grain*16.0f + 9.0f);

        color[0] = fmin(0.6f + a1 * 0.12f, a2);
        color[1] = fmin(0.6f + a1 * 0.12f, a2);
        color[2] = fmin(0.6f + a1 * 0.12f, a2);
        color[3] = 1.0f;
    }


    static void skin_gecko(float x, float y, float z, float* color, unsigned char seed = 131) {
        float alpha = cDistortion::sig(9.0f * cNoise::simplex3(x, y, z, seed));
        color[0] = alpha;
        color[1] = alpha;
        color[2] = 0.00f;
        color[3] = 1.0f;
    }


    static void flesh_organix(float x, float y, float z, float* color, unsigned char seed = 131) {
        float gradient = cNoise::simplex3(x, y, z, seed) * 0.5f + 0.5f;
        gradient = sin(20.0f * gradient);
        gradient = cDistortion::sig(2.0f * gradient);

        const float c0[] = { 0.3f, 0.0f, 0.0f };
        const float c1[] = { 0.6f, 0.5f, 0.3f };

        color[0] = c0[0] * gradient + (1.0f - gradient) * c1[0];
        color[1] = c0[1] * gradient + (1.0f - gradient) * c1[1];
        color[2] = c0[2] * gradient + (1.0f - gradient) * c1[2];
        color[3] = 1.0f;
    }


    static void metal_damast(float x, float y, float z, float* color, unsigned char seed = 131) {
        float gradient = cNoise::simplex3(x, y, z, seed) * 0.5f + 0.5f;
        gradient = sin(gradient*30);
        gradient = cDistortion::sig(2.0f*gradient-0.2);

        const float c0[] = { 0.40f, 0.45f, 0.45f };
        const float c1[] = { 0.70f, 0.70f, 0.70f };

        color[0] = c0[0] * gradient + (1.0f - gradient) * c1[0];
        color[1] = c0[1] * gradient + (1.0f - gradient) * c1[1];
        color[2] = c0[2] * gradient + (1.0f - gradient) * c1[2];
        color[3] = 1.0f;
    }

    static void metal_rust(float x, float y, float z, float* color, unsigned char seed = 131) {
        float f = 2*2.0f;
        float z0 = cNoise::simplex3(f*x, f*y, f*z, seed + 0) * 0.5f + 0.5f;

        f *= 4.0f;
        float z1 = cNoise::simplex3(f*x, f*y, f*z, seed + 1) * 0.5f + 0.5f;

        z0 = cDistortion::sig(4.0f*(z0 + 0.5f*z1));

        const float c0[] = { 0.30f, 0.05f, 0.00f };
        const float c1[] = { 0.80f, 0.80f, 0.00f };

        color[0] = c0[0] * z0 + (1.0f - z0) * c1[0];
        color[1] = c0[1] * z0 + (1.0f - z0) * c1[1];
        color[2] = c0[2] * z0 + (1.0f - z0) * c1[2];
        color[3] = 1.0f;
    }

    static inline void camo_rust(float x, float y, float z, float* color, unsigned char seed = 131) {
        float f = 1;
        float shiny = cNoise::simplex3(f*x, f*y, f*z, seed);
        shiny = cDistortion::sig(9.0f * shiny + 3);

        float camo[3] = { 0.60f, 0.65f, 0.70f };
        float rust[3];
        camo_urban(x, y, z, camo);
        metal_rust(x, y, z, rust);
        color[0] = shiny * camo[0] + (1.0f - shiny) * rust[0];
        color[1] = shiny * camo[1] + (1.0f - shiny) * rust[1];
        color[2] = shiny * camo[2] + (1.0f - shiny) * rust[2];
        color[3] = 1.0f;
    }

    static inline void metal_sheets(float x, float y, float z, float* color, unsigned char seed = 131) {
        float f = 4*4;
        float s = cNoise::samplex3(f*x, f*y, f*z, seed);
        s = cDistortion::sig(4.0f * s + 2.3f);
        color[0] = color[1] = color[2] = 0.3f * s + 0.1f;
        color[3] = 1.0f;
    }

    static inline void star_nebula(float x, float y, float z, float* color, unsigned char seed = 131) {
        float c[4];

        loopj(3) {
            unsigned char seed2 = seed + j;//31 + j * 17;
            float sum = 0;
            float fsum = 0;
            for (int i = 1; i < 7; i++) {
                float p = (1 << i)*0.5;
                float f = (8 - i)*4;
                sum += f * cNoise::simplex3(x*p, y*p, z*p, seed2);
                fsum += f;
                seed2 += 7;
            }
            sum /= fsum;
            c[j] = cDistortion::sig(12 * sum - 1);
        }

        //float z = 100;
        //c[3] = pow(z, 1-(nextrand(nextrand(nextrand(c[3]*987654321))) % 1723) * 0.00058f) / z;

        float p2 = 10001;
        float diffusion2 = 0.5f, shift2 = 0.5f, e2 = 2.0f;
        //float spots1 = sig( -voronoi3(x*p1, y*p1, z*p1, diffusion1, shift1, e1) * 18 - 15 );
        float spots2 = pow(-(0.5f - 0.5f * cNoise::voronoi3(x*p2, y*p2, z*p2, diffusion2, shift2, e2)), 14);

        float p1 = 2001;
        float diffusion1 = 0.5f, shift1 = 0.5f, e1 = 2.0f;
        //float spots1 = sig( -voronoi3(x*p1, y*p1, z*p1, diffusion1, shift1, e1) * 18 - 15 );
        float spots1 = pow(-(0.5f - 0.5f * cNoise::voronoi3(x*p1, y*p1, z*p1, diffusion1, shift1, e1)), 14);

        float p0 = 201;
        float diffusion = 0.5f, shift = 0.5f, e = 2.0f;
        //float spots = sig( -voronoi3(x*p0, y*p0, z*p0, diffusion, shift, e) * 18 - 15 );
        float spots = pow(-(0.5f - 0.5f * cNoise::voronoi3(x*p0, y*p0, z*p0, diffusion, shift, e)), 14);

        float stars = 1.29f;
        float nebula = 0.1f;
        //c[3] = stars*exposure( spots1*0.6 + 0.8*spots );
        c[3] = stars * (spots2 * 0.8f + spots1 * 0.9f + 0.99f * spots);

        color[0] = fmin(1, 0.00f + 0.91f * nebula * c[0] + c[3]);
        color[1] = fmin(1, 0.00f + 0.91f * nebula * c[1] + c[3]);
        color[2] = fmin(1, 0.05f + 0.99f * nebula * c[2] + c[3]);
        color[3] = 1.0f;
    }

    static inline void planet_cloud(float x, float y, float z, float* color, unsigned char seed = 131) {
        float sum = 0;
        float fsum = 0;
        for (int i = 0; i < 8; i++) {
            float p = 1 << i;
            float f = (8 - i)*2;
            sum += f * cNoise::simplex3(x*p, y*p, z*p, seed);
            fsum += f;
            seed += 7;
        }
        sum /= fsum;

        float f00 = cDistortion::sig(9 * sum);

        float covering = 0.55f;
        f00 = fmax(0.0f, f00 - (1.0f - covering)) / covering;

        float sharpness = 0.98f;
        f00 = pow(f00, 2.0f - sharpness);

        float f01 = sin(2.14f * f00);

        color[0] = f01;
        color[1] = f01;
        color[2] = f01;
        color[3] = f00;
    }

    static inline void planet_ground(float x, float y, float z, float* color) {
        const float scale = 4 * 512;
        const int seed1 = 11;
        const float p1 = scale / 101.0f;
        float o1 = cNoise::simplex3(37 + x*p1, 12 + y*p1, 97 + z*p1, seed1);

        const int seed2 = seed1 + 43;
        const float p2 = scale / 47.0f;
        float o2 = cNoise::simplex3(71 + x*p2, 45 + y*p2, 19 + z*p2, seed2);

        float h = (0.9f * o1 + 0.5f * o2) * 0.5 + 0.5;

        const int seed3 = seed1 + 43;
        const float p3 = scale / 17.0f;
        float o3 = cNoise::simplex3(21 + x*p3, 41 + y*p3, 79 + z*p3, seed3);

        float top[] = {0.59f, 0.50f, 0.18f};
        float bottom[] = {0.75f, 0.73f, 0.28f};
        float splat[] = {0.43f, 0.35f, 0.21f};
        float top_alpha = h;
        float bottom_alpha = 1.0f - h;
        float splat_alpha = (exp(o2 * 0.5 + 0.5))*0.1 + (exp(o3 * 0.5 + 0.5))*0.05;

        color[0] = top[0] * top_alpha + bottom[0] * bottom_alpha + splat[0] * splat_alpha;
        color[1] = top[1] * top_alpha + bottom[1] * bottom_alpha + splat[1] * splat_alpha;
        color[2] = top[2] * top_alpha + bottom[2] * bottom_alpha + splat[2] * splat_alpha;
        color[3] = 1;
        //color[BUMP] = h;
    }

};

#endif	/* CSOLID_H */

