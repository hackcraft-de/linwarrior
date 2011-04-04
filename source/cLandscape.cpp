#include "cLandscape.h"

#include "psi3d/noise.h"

void cLandscape::land_rockies(float x, float y, float z, float* color, unsigned char seed) {
    float utahcolors[][3] = {
        {0.40f, 0.30f, 0.20f},
        {0.70f, 0.55f, 0.25f},
        {0.80f, 0.70f, 0.40f},
        {0.40f, 0.30f, 0.20f},
        {0.70f, 0.55f, 0.25f},
        {0.80f, 0.70f, 0.40f},
        {0.70f, 0.55f, 0.25f},
        {0.80f, 0.70f, 0.40f},
    };

    float x_ = x;
    float z_ = z;
    float y_ = y;
    // Prime periods => large smallest common multiple.
    const float p2 = 1.0f / 107.0f;
    const float p3 = 1.0f / 47.0f;
    float o2 = cNoise::simplex3(50 + x_*p2, 50 + y_*p2, 50 + z_*p2, seed + 0);
    float o3 = cNoise::simplex3(10 + x_*p3, 10 + y_*p3, 90 + z_*p3, seed + 1);

    float h = exp((o2 + 0.5f * o3) - 0.6f) * 0.249f;

    h = (h > 0.6f) ? 0.6f : h;
    h = (h < 0.2f) ? 0.0f : h;

    int i = ((int) (h * 16.0f)) & 7;

    color[RED] = utahcolors[i][0];
    color[GRN] = utahcolors[i][1];
    color[BLU] = utahcolors[i][2];
    color[BUMP] = h;
}

void cLandscape::land_dunes(float x, float y, float z, float* color, unsigned char seed) {
    // Prime periods => large smallest common multiple.
    const float p1 = 1.0f / 101.0f;

    float x_ = 90 + x * p1; // + 5*sin(z * PI_OVER_180*30);
    float y_ = 10 + 2.0f * y * p1;
    float z_ = 10 + 2.0f * z * p1;

    float o1 = cNoise::simplex3(x_, y_, z_, seed + 0);

    const float a = p1 * 0.5f;
    const float b = a * 0.5f;
    const float c = b * 0.5f;

    float o1_ = cNoise::simplex3(a + x_, c + y_ + c, b + z_, seed + 1);

    // Limit to about [0,1] range.
    float h = (fmin(o1, -o1_)*0.98f + 0.96f);

    const float f = 1.0f / 255.0f;
    float c1[] = {f * 0xfc, f * 0xd4, f * 0x96};
    float c0[] = {f * 0x6b, f * 0x4f, f * 0x28};
    //    float c1[] = {1.00f, 0.96f, 0.79f};
    //    float c0[] = {0.86f, 0.78f, 0.59f};
    float alpha = h;
    float alpha_ = 1.0f - h;

    color[RED] = c0[0] * alpha_ + c1[0] * alpha;
    color[GRN] = c0[1] * alpha_ + c1[1] * alpha;
    color[BLU] = c0[2] * alpha_ + c1[2] * alpha;
    color[BUMP] = h;
}

void cLandscape::land_dunes_red(float x, float y, float z, float* color, unsigned char seed) {
    // Prime periods => large smallest common multiple.
    const float p1 = 1.0f / 101.0f;

    float x_ = 90 + x * p1; // + 5*sin(z * PI_OVER_180*30);
    float y_ = 10 + 2.0f * y * p1;
    float z_ = 10 + 2.0f * z * p1;

    float o1 = cNoise::simplex3(x_, y_, z_, seed + 0);

    const float a = p1 * 0.5f;
    const float b = a * 0.5f;
    const float c = b * 0.5f;

    float o1_ = cNoise::simplex3(a + x_, c + y_ + c, b + z_, seed + 1);

    // Limit to about [0,1] range.
    float h = (fmin(o1, -o1_)*0.98f + 0.96f);
    const float f = 1.0f / 255.0f;
    float c1[] = {f * 0xe3, f * 0x7f, f * 0x2a};
    float c0[] = {f * 0xd0, f * 0x59, f * 0x1f};
    float alpha = h;
    float alpha_ = 1.0f - h;

    color[RED] = c0[0] * alpha_ + c1[0] * alpha;
    color[GRN] = c0[1] * alpha_ + c1[1] * alpha;
    color[BLU] = c0[2] * alpha_ + c1[2] * alpha;
    color[BUMP] = h;
}

void cLandscape::land_lava(float x, float y, float z, float* color, unsigned char seed) {
    float power = 5;
    float wlen = 1.0f / (2 * power);
    float dx = power * cNoise::simplex3(37 + x*wlen, 65 + y*wlen, 97 + 2.0f * z*wlen, seed + 0);
    float dz = power * cNoise::simplex3(67 + x*wlen, 31 + y*wlen, 7 + 2.0f * z*wlen, seed + 1);
    x += dx;
    z += dz;

    // Prime periods => large smallest common multiple.
    const float p1 = 1.0f / 101.0f;

    float x_ = 90 + x * p1;
    float y_ = 10 + y * p1;
    float z_ = 10 + z * p1;

    float o1 = cNoise::simplex3(x_, y_, z_, seed + 0);

    const float a = 0.5f * p1;
    const float b = a * 0.5f;

    float o1_ = cNoise::simplex3(x_ + a, y_ + b, z_ + b, seed + 1);

    // Limit to about [0,1] range.
    float h = 1.0f - (fmin(o1, -o1_)*0.98f + 0.96f);

    //float diffusion = 0.9;
    //float shift = 0.5;
    //h = 1.0f-voronoi3(x_, y_, z_,diffusion,shift);

    float ash[] = {0.3f, 0.3f, 0.35f};
    float lava[] = {1.0f, 0.0f, 0.0f};
    float glow[] = {1.1f, 1.1f, 1.1f};
    float ash_alpha = h*h;
    float lava_alpha = 0.25f * (2.0f - 2.0f * h)*(2.0f - 2.0f * h);
    float glow_alpha = (h < 0.2f) ? 1.2f - 5.0f * h : 0.0f;

    color[RED] = (lava[0] * lava_alpha + ash[0] * ash_alpha + glow[0] * glow_alpha);
    color[GRN] = (lava[1] * lava_alpha + ash[1] * ash_alpha + glow[1] * glow_alpha);
    color[BLU] = (lava[2] * lava_alpha + ash[2] * ash_alpha + glow[2] * glow_alpha);
    color[RED] = (color[RED] > 1.0f ? 1.0f : color[RED])*0.8f + 0.1;
    color[GRN] = (color[GRN] > 1.0f ? 1.0f : color[GRN])*0.8f + 0.1;
    color[BLU] = (color[BLU] > 1.0f ? 1.0f : color[BLU])*0.8f + 0.1;
    color[BUMP] = h;
}

void cLandscape::land_grass(float x, float y, float z, float* color, unsigned char seed) {
    const float p1 = 1.0f / 101.0f;
    float o1 = cNoise::simplex3(37 + x*p1, 67 + y*p1, 97 + z*p1, seed + 0);

    const float p2 = 1.0f / 47.0f;
    float o2 = cNoise::simplex3(71 + x*p2, 57 + y*p2, 19 + z*p2, seed + 1);

    float h = (0.9f * o1 + 0.5f * o2) * 0.5 + 0.5;

    const float p3 = 1.0f / 17.0f;
    float o3 = cNoise::simplex3(21 + x*p3, 41 + y*p3, 79 + z*p3, seed + 2);

    //float top[] = {0.59f, 0.80f, 0.58f};
    //float bottom[] = {0.85f, 0.93f, 0.58f};
    //float splat[] = {0.93f, 0.95f, 0.61f};
    const float f = 1.0f / 256.0f;
    float top[] = {f * 0x94, f * 0x9b, f * 0x57};
    float bottom[] = {f * 0x20, f * 0x39, f * 0x1b};
    //float splat[] = {f*0x75, f*0x8b, f*0x67};
    float splat[] = {f * 0x8d, f * 0x98, f * 0x49};
    float top_alpha = h;
    float bottom_alpha = 1.0f - h;
    float splat_alpha = (exp(o2 * 0.5 + 0.5))*0.1 + (exp(o3 * 0.5 + 0.5))*0.05;

    color[RED] = top[0] * top_alpha + bottom[0] * bottom_alpha + splat[0] * splat_alpha;
    color[GRN] = top[1] * top_alpha + bottom[1] * bottom_alpha + splat[1] * splat_alpha;
    color[BLU] = top[2] * top_alpha + bottom[2] * bottom_alpha + splat[2] * splat_alpha;
    color[BUMP] = h;
}

void cLandscape::land_snow(float x, float y, float z, float* color, unsigned char seed) {
    const float p1 = 1.0f / 101.0f;
    float o1 = cNoise::simplex3(37 + x*p1, 67 + y*p1, 97 + z*p1, seed + 0);

    const float p2 = 1.0f / 47.0f;
    float o2 = cNoise::simplex3(71 + x*p2, 57 + y*p2, 19 + z*p2, seed + 1);

    float h = (0.9f * o1 + 0.5f * o2) * 0.5 + 0.5;

    const float p3 = 1.0f / 17.0f;
    float o3 = cNoise::simplex3(21 + (x + sin(z)) * p3, 41 + (y + sin(z)) * p3, 79 + (z + sin(x)) * p3, seed + 2);

    float top[] = {0.86f, 0.89f, 0.89f};
    float bottom[] = {0.98f, 0.98f, 0.98f};
    float splat[] = {0.58f, 0.85f, 0.95f};
    float top_alpha = h;
    float bottom_alpha = 1.0f - h;
    float splat_alpha = (exp(o2 * 0.5f + 0.5f))*0.1f + (exp(o3 * 0.5f + 0.5f))*0.3;

    color[RED] = top[0] * top_alpha + bottom[0] * bottom_alpha + splat[0] * splat_alpha;
    color[GRN] = top[1] * top_alpha + bottom[1] * bottom_alpha + splat[1] * splat_alpha;
    color[BLU] = top[2] * top_alpha + bottom[2] * bottom_alpha + splat[2] * splat_alpha;
    color[BUMP] = h;
}

void cLandscape::land_dirt(float x, float y, float z, float* color, unsigned char seed) {
    const float p1 = 1.0f / 101.0f;
    float o1 = cNoise::simplex3(37 + x*p1, 67 + y*p1, 97 + z*p1, seed + 0);

    const float p2 = 1.0f / 47.0f;
    float o2 = cNoise::simplex3(71 + x*p2, 57 + y*p2, 19 + z*p2, seed + 1);

    float h = (0.9f * o1 + 0.5f * o2) * 0.5 + 0.5;

    const float p3 = 1.0f / 17.0f;
    float o3 = cNoise::simplex3(21 + x*p3, 41 + y*p3, 79 + z*p3, seed + 2);

    float top[] = {0.59f, 0.50f, 0.18f};
    float bottom[] = {0.75f, 0.73f, 0.28f};
    float splat[] = {0.43f, 0.35f, 0.21f};
    float top_alpha = h;
    float bottom_alpha = 1.0f - h;
    float splat_alpha = (exp(o2 * 0.5f + 0.5f))*0.1f + (exp(o3 * 0.5f + 0.5f))*0.05f;

    color[RED] = top[0] * top_alpha + bottom[0] * bottom_alpha + splat[0] * splat_alpha;
    color[GRN] = top[1] * top_alpha + bottom[1] * bottom_alpha + splat[1] * splat_alpha;
    color[BLU] = top[2] * top_alpha + bottom[2] * bottom_alpha + splat[2] * splat_alpha;
    color[BUMP] = h;
}

void cLandscape::decoration_spikes(float x, float y, float z, float* color, unsigned char seed) {
    const int n = 4;
    float o = 0;
    const float p[] = {1.0f / 47.0f, 1.0f / 23.0f, 1.0f / 11.0f, 1.0f / 7.0f};
    const float sx[] = {231, 123, 45, 31, 13, 7, 3};
    float factor = 0.9f;
    x *= 1.9f;
    z *= 1.9f;

    loopi(n) {
        o += factor * cNoise::simplex3(sx[i] + x * p[i], i * (i + 1) + y * p[i], i * (i + 1) + z * p[i], seed);
        factor *= 0.8f;
        seed = (seed + 1) << 1;
    }

    float h = exp(o);

    h -= 0.6f + 4;
    h = (h < 0.2f) ? 0.0f : h;
    h *= 1.0f;

    color[RED] = h;
    color[GRN] = h;
    color[BLU] = h;
    color[BUMP] = h;
}

void cLandscape::decoration_stones(float x, float y, float z, float* color, unsigned char seed) {
    const int n = 4;
    float o = 0;
    const float p[] = {1.0f / 47.0f, 1.0f / 23.0f, 1.0f / 11.0f, 1.0f / 7.0f};
    const float sx[] = {231, 123, 45, 31, 13, 7, 3};
    float factor = 0.9f;
    x *= 0.5f;
    z *= 0.5f;

    loopi(n) {
        o += factor * cNoise::simplex3(sx[i] + x * p[i], i * (i + 1) + y * p[i], i * (i + 1) + z * p[i], seed);
        factor *= 0.8f;
        seed = (seed + 1) << 1;
    }

    float h = o;
    float t = 1.25f;
    h = t * o * sin(t * o) - 0.9f;

    h -= 0.2f;
    h = (h < 0.2f) ? 0.0f : h;
    h *= 1.0f;
    h = (h > 1.0f) ? 1.0f : h;

    color[RED] = h;
    color[GRN] = h;
    color[BLU] = h;
    color[BUMP] = h;
}

void cLandscape::mashup_lava(float x, float y, float z, float* color, unsigned char seed) {
    const float p1 = 1.0f / 301.0f;
    float o1 = 0.5f + 0.5f * cNoise::simplex3(37 + x*p1, 57 + y*p1, 97 + z*p1, seed);

    float dunecolor[16];
    float utahcolor[16];

    //getDune(x, z, dunecolor);
    //float dune = 12 * o1 * dunecolor[BUMP];

    //getUtah(x,z,utahcolor);
    //float utah = 90 * utahcolor[BUMP];

    land_lava(x, y, z, dunecolor);
    float dune = 12 * o1 * dunecolor[BUMP];

    decoration_spikes(x, y, z, utahcolor);
    float utah = 5 * utahcolor[BUMP];

    bool isdune = dune > utah;

    float h = (isdune ? dune : utah);

    float stonecolor[16];
    decoration_stones(x, y, z, stonecolor);
    float stone = 5 * stonecolor[BUMP];

    bool isstone = (stone > 0.001) && isdune;
    if (isstone) {
        h += stone;
    }

    if (isstone) {
        color[RED] = stonecolor[0];
        color[GRN] = stonecolor[1];
        color[BLU] = stonecolor[2];
    } else if (isdune) {
        color[RED] = dunecolor[0];
        color[GRN] = dunecolor[1];
        color[BLU] = dunecolor[2];
    } else {
        color[RED] = utahcolor[0];
        color[GRN] = utahcolor[1];
        color[BLU] = utahcolor[2];
    }
    color[BUMP] = h;
}

void cLandscape::experimental_continents(float x, float y, float z, float* color, unsigned char seed) {
    const int n = 8;
    float o = 0;
    const float p[] = {1.0f / 5003.0f, 1.0f / 1001.0f, 1.0f / 301.0f, 1.0f / 47.0f, 1.0f / 23.0f, 1.0f / 11.0f, 1.0f / 7.0f, 1.0f / 3.0f};
    const float sx[] = {231, 123, 45, 31, 13, 7, 3, 1};
    float factor = 0.9;
    x *= 0.1f;
    y *= 0.1f;
    z *= 0.1f;

    float scale = 128;

    loopi(n) {
        o += factor * cNoise::simplex3(sx[i] + x * p[i] * scale, sx[i] + y * p[i] * scale, sx[i] + z * p[i] * scale, seed);
        factor *= 0.8;
        seed = (seed + 1) << 1;
    }

    float h = cDistortion::continentalRidge(o * 0.5f + 0.5f);

    float sea[] = {0.1, 0.1, 0.3};
    float beach[] = {0.7, 0.7, 0.5};
    //float grass[] = { 0.3, 0.7, 0.3 };
    //float peak[] = { 1,1,1 };
    float peak[] = {0.4f, 0.3f, 0};

    float sea_alpha = (1.0f - h)*(1.0f - h);
    float beach_alpha = (1.0f - fabs(h - 0.5f))*(1.0f - fabs(h - 0.5f));
    float peak_alpha = h*h;

    loopi(3) {
        //color[i] = sea[i] * sea_alpha + beach[i] * beach_alpha + peak[i] * peak_alpha;
        color[i] = cDistortion::exposure(5 * (sea[i] * sea_alpha + beach[i] * beach_alpha + peak[i] * peak_alpha));
    }
    color[BUMP] = h;
}

