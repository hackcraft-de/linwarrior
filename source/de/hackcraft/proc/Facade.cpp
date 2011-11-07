#include "Facade.h"

#include "Noise.h"
#include "Distortion.h"

#include "de/hackcraft/psi3d/ctrl.h"
#include "de/hackcraft/psi3d/math3d.h"

void Facade::getVStrings(float x, float y, float* color4f, unsigned char seed) {
    float x_ = fmod(x, 1.0f);
    float sinx = Noise::simplex3(((x_ < 0.5f) ? (x_) : (1.0f - x_))*16.0, 0, 0, seed);
    float sx = 0.8 + 0.2 * sinx;
    sx *= sx;
    float grain = 0.9f + 0.1f * (0.5f + 0.5f * Noise::simplex3(x * 256.0f, y * 256.0f, 0, seed));
    color4f[0] = color4f[1] = color4f[2] = grain * sx;
    color4f[3] = 1.0f;
}

void Facade::getHStrings(float x, float y, float* color4f, unsigned char seed) {
    float y_ = fmod(y, 1.0f);
    float siny = Noise::simplex3(0, y_ * 8.0f, 0, seed);
    float sy = 0.8 + 0.2 * siny;
    sy *= sy;
    float grain = 0.9f + 0.1f * (0.5f + 0.5f * Noise::simplex3(x * 256.0f, y * 256.0f, 0, seed));
    color4f[0] = color4f[1] = color4f[2] = grain * sy;
    color4f[3] = 1.0f;
}

void Facade::getInterrior(float x, float y, float* color4fv, unsigned char seed) {
    float x_ = fmod(x, 1.0f);
    float y_ = fmod(y, 1.0f);
    
    bool bk = (x_ > 0.3f) && (x_ < 0.7f) && (y_ > 0.3f) && (y_ < 0.7f);
    bool d1 = x_ > y_;
    bool d2 = (1-x_) > y_;
    
    float t = (d1 && d2 && !bk) ? 1.0f : 0.0f;
    float r = (d1 && !d2 && !bk) ? 1.0f : 0.0f;
    float b = (!d1 && !d2 && !bk) ? 1.0f : 0.0f;
    float l = (!d1 && d2 && !bk) ? 1.0f : 0.0f;
    float f = t * 0.8 + (l + r) * 0.7 + b * 0.5 + bk * 0.9;
    
    float alpha = 0.3f + 0.7f * (0.5f + 0.5f * Noise::samplex3(x * 63, y * 63, 0, seed));
    alpha *= f;
    
    float dark[] = {0.05f, 0.05f, 0.05f, 1.0f};
    float lite[] = {0.99f, 0.99f, 0.70f, 1.0f};
    Distortion::fade4(alpha, dark, lite, color4fv);
}

void getBlinds(float x, float y, float* color4fv, unsigned char seed) {
    float y_ = fmod(y, 1.0f);
    
    unsigned char h = (3*((unsigned char)(x))) + (7*((unsigned char)(y)));
    unsigned char n = Noise::LFSR8(seed + h) ^ h;
    n &= 0x0F;

    float y__ = y_ * 16;
    float f = fmod(y__, 1.0f);
    f = (y__ > n) ? 0 : f;
    float alpha = 1.0f - (1.0f - f) * (1.0f - f);
    float dark[] = {0.2f, 0.2f, 0.0f, 1.0f};
    float lite[] = {0.99f, 0.99f, 0.99f, 1.0f};
    Distortion::fade4(alpha, dark, lite, color4fv);
    color4fv[3] = (f <= 0.0) ? 0 : color4fv[3];
}

void Facade::getExterrior(float x, float y, float* color4fv, unsigned char seed) {
    float alpha = 0.3f + 0.7f * (0.5f + 0.5f * Noise::simplex3(x, y, 0, seed));
    float dark[] = {0.30f, 0.30f, 0.40f, 1.0f};
    float lite[] = {0.50f, 0.50f, 0.70f, 1.0f};
    Distortion::fade4(alpha, dark, lite, color4fv);
}

void Facade::getConcrete(float x, float y, float z, float* color4fv, unsigned char seed) {
    float grain = 0.15f + 0.15f * (0.5f + 0.5f * Noise::simplex3(x, y, z, seed));
    color4fv[0] = color4fv[1] = color4fv[2] = grain;
    color4fv[3] = 1.0f;
}

unsigned char Facade::generateFrame(float* dims4x4, float (*sums5)[4], unsigned char seed) {
    
    enum Levels {
        MARGIN, BORDER, PADDING, CONTENT, MAX_LEVELS
    };
    
    float* dims = dims4x4;
    float (*sum)[4] = sums5;
    
    // Get the random numbers rolling...
    unsigned char r = Noise::LFSR8(seed);
    r = Noise::LFSR8(r);
    r = Noise::LFSR8(r);
    r = Noise::LFSR8(r);
    r = Noise::LFSR8(r);
    
    const float inv256 = 0.003906f;

    float* margin = &dims[0];
    {
        r = Noise::LFSR8(r);
        margin[0] = 0.20f * r * inv256 + 0.006f;
        margin[2] = margin[0];
        r = Noise::LFSR8(r);
        margin[1] = 0.14f * r * inv256 + 0.006f;
        // should be related
        r = Noise::LFSR8(r);
        margin[3] = 0.36f * r * inv256 + 0.006f;
    }

    float* border = &dims[4];
    {
        float w = 1.0f - (margin[2] + margin[0]);
        float h = 1.0f - (margin[3] + margin[1]);
        r = Noise::LFSR8(r);
        border[0] = r * 0.0002f * w + 0.0002f * w;
        border[2] = border[0];
        r = Noise::LFSR8(r);
        border[1] = r * 0.0002f * h + 0.0002f * h;
        border[3] = border[1];
    }

    float* padding = &dims[8];
    {
        float w = 1.0f - (margin[2] + border[2] + margin[0] + border[0]);
        float h = 1.0f - (margin[3] + border[3] + margin[1] + border[1]);
        r = Noise::LFSR8(r);
        padding[0] = r * 0.0002f * w + 0.0002f * w;
        padding[2] = padding[0];
        r = Noise::LFSR8(r);
        padding[1] = r * 0.0002f * h + 0.0002f * h;
        padding[3] = padding[1];
    }

    float* content = &dims[12];
    {
        float w = 1.0f - (margin[2] + border[2] + padding[2] + margin[0] + border[0] + padding[0]);
        float h = 1.0f - (margin[3] + border[3] + padding[3] + margin[1] + border[1] + padding[1]);
        r = Noise::LFSR8(r);
        content[0] = r * 0.0002f * w + 0.0002f * w;
        content[2] = content[0];
        r = Noise::LFSR8(r);
        content[1] = r * 0.0001f * h + 0.0001f * h;
        r = Noise::LFSR8(r);
        content[3] = r * 0.0001f * h + 0.0001f * h;
    }

    sum[0][0] = 0;
    sum[0][1] = 0;
    sum[0][2] = 0;
    sum[0][3] = 0;

    loopi(MAX_LEVELS) {
        sum[i + 1][0] = sum[i][0] + dims[4 * i + 0];
        sum[i + 1][1] = sum[i][1] + dims[4 * i + 1];
        sum[i + 1][2] = sum[i][2] + dims[4 * i + 2];
        sum[i + 1][3] = sum[i][3] + dims[4 * i + 3];
    }
    
    // Row major if result is even.
    return Noise::LFSR8(r);
}

void Facade::getFacade(float x, float y, int gx, int gy, float age, float* c3f, unsigned char seed) {

    // Border Levels from outside to inside.

    enum Levels {
        MARGIN, BORDER, PADDING, CONTENT, MAX_LEVELS
    };
    
    float dims[4 * MAX_LEVELS];
    float sum[MAX_LEVELS + 1][4];
    
    unsigned char r = generateFrame(dims, sum, seed);
    bool rowmajor = (r & 1) == 0;
    
    float* margin = &dims[0];
    //float* border = &dims[4];
    //float* padding = &dims[8];
    //float* content = &dims[12];
    
    const float inv256 = 0.003906f;

    float pyr[MAX_LEVELS + 1];

    loopi(MAX_LEVELS + 1) {
        pyr[i] = Distortion::rampPyramid(sum[i][0], sum[i][1], 1.0f - sum[i][2], 1.0f - sum[i][3], x, y);
    }

    float inside[MAX_LEVELS + 1];

    loopi(MAX_LEVELS + 1) {
        inside[i] = (pyr[i] >= 0) ? 1.0f : 0.0f;
    }

    rgba colors[] = {
        { 1, 0, 0, 1},
        { 0, 1, 0, 1},
        { 0, 0, 1, 1},
        { 0, 1, 1, 1},
        { 1, 1, 1, 1},
    };

    loopi(MAX_LEVELS + 1) {
        if (pyr[i] >= 0) {
            c3f[0] = colors[i][0];
            c3f[1] = colors[i][1];
            c3f[2] = colors[i][2];
        }
    }

    float chma = 2.0f * 0.1f;
    chma *= rowmajor ? 0.1f : 1.0f;
    float cr = r = Noise::LFSR8(r);
    float cg = r = Noise::LFSR8(r);
    float cb = r = Noise::LFSR8(r);
    float cl = r = Noise::LFSR8(r);
    cl = 0.6f + 0.4f * inv256*cl;
    cl *= (1.0f - chma);
    chma *= inv256;
    float c0[] = {cl + chma*cr, cl + chma*cg, cl + chma*cb, 1.0f};

    chma = 2.0f * 0.1f;
    chma *= rowmajor ? 1.0f : 0.1f;
    cr = r = Noise::LFSR8(r);
    cg = r = Noise::LFSR8(r);
    cb = r = Noise::LFSR8(r);
    cl = r = Noise::LFSR8(r);
    cl = 0.6f + 0.4f * inv256*cl;
    cl *= (1.0f - chma);
    chma *= inv256;
    float c1[] = {cl + chma*cr, cl + chma*cg, cl + chma*cb, 1.0f};

    //return;


    // Vertical Column Pattern.
    float col[4];
    getVStrings(x, y, col, seed + 21);

    loopi(4) {
        col[i] *= c0[i];
    }

    // Horizontal Row Pattern.
    float row[4];
    getHStrings(x, y, row, seed + 11);

    loopi(4) {
        row[i] *= c1[i];
    }

    // Glass distortion
    float dx = 0.02f * Noise::simplex3((gx + x)*27, (gy + y)*27, 0, seed + 43);
    float dy = 0.02f * Noise::simplex3((gx + x)*27, (gy + y)*27, 0, seed + 31);

    // Interrior Room Pattern.
    float interrior[4];
    getInterrior(gx + x + dx, gy + y + dy, interrior);
    
    // Blinds
    float blinds[4];
    getBlinds(gx + x + 0.0*dy, gy + y + 0.0*dy, blinds, r);
    Distortion::fade4(blinds[3], interrior, blinds, interrior);

    // Exterrior Scene Reflection Pattern.
    float exterrior[4];
    getExterrior(gx + x + 2 * dx, gy + y + 2 * dy, exterrior);

    // Light smearing
    float smear = 0.8 + 0.2f * Noise::simplex3((gx + x)*8 + (gy + y)*8, (gx + x)*1 - (gy + y)*1, 0, seed + 41);


    //float glass[] = { 0.90f, 0.90f, 0.99f, 1.0f };
    //float window[] = { 0.3f, 0.35f, 0.3f, 1.0f };

    int u = (x < margin[0]) ? 0 : ((x < 1.0f - margin[2]) ? 1 : 2);
    int v = (y < margin[1]) ? 0 : ((y < 1.0f - margin[3]) ? 1 : 2);

    int colpart = ((!rowmajor || v == 1) && u != 1) ? 1 : 0;
    int rowpart = ((rowmajor || u == 1) && v != 1) ? 1 : 0;

    float mirror = 0.7f;
    mirror *= smear;

    float shade[4];
    Distortion::fade4(mirror, interrior, exterrior, shade);
    Distortion::mix3(0.56f * colpart, col, 0.60f * rowpart, row, c3f);

    int winpart = inside[BORDER + 1];
    Distortion::mix3(1.0f - winpart, c3f, winpart, shade, c3f);

    //return;

    /*
    float dirt[] = {0.3, 0.3, 0.25, 1.0f};
    float dirtf = 1.0f + pyr[BORDER + 1];
    dirtf = (dirtf > 1.0f) ? 0.0f : dirtf;
    dirtf *= 0.99f;
    Distortion::mix3(1.0f - dirtf, c3f, dirtf, dirt, c3f);
    */
    
    float windowf = -cos((x - 0.5) * 6.0 * M_PI);
    //float windowf = cos((x - 0.5) * 4.0 * M_PI);
    windowf = fmax(windowf, cos((y - 0.5) * 12.0 * M_PI));
    windowf = ((windowf < 0.98) || !winpart) ? 0.0 : windowf;
    
    float wframe[] = { 0, 0, 0, 1 };
    Distortion::mix3(1.0f - windowf, c3f, windowf, wframe, c3f);
}

