#include "Road.h"

#include "de/hackcraft/proc/Solid.h"
#include "de/hackcraft/proc/Surface.h"

#include <cmath>

void Road::getBasicRoad(float x, float y, float z, float* color, unsigned char seed) {
    Solid::concrete_cracked(x, y, z, color, seed);

    float marks = 0.6f + 0.05f * cos(2.0f * 4.0f * x * 2.0f * M_PI);
    color[0] *= marks;
    color[1] *= marks;
    color[2] *= marks;

    float walk4fv[4];
    Surface::stone_plates(x, y, z, walk4fv, seed);

    unsigned short pattern = 0x6666;
    int bitshift = (int) (y * 7.999999f * 2.0f);
    bool bit = (pattern >> bitshift) & 1;

    if (x > 0.495f && x < 0.505f && bit) {
        color[0] = color[0] + 0.35;
        color[1] = color[1] + 0.35;
        color[2] = color[2] + 0.35;
    }

    if ((x > 0.25 && x < 0.26) || (x > (1.0f - 0.26f) && x < (1.0f - 0.25f))) {
        color[0] = color[0] + 0.35;
        color[1] = color[1] + 0.35;
        color[2] = color[2] + 0.1;
    }

    // sidewalk
    if (x < 0.23 || x > (1.0f - 0.23f)) {
        float f = 1;
        if (x < 0.22 || x > (1.0f - 0.22f)) {
        } else {
            f = 0.7;
        }
        color[0] = f * walk4fv[0];
        color[1] = f * walk4fv[1];
        color[2] = f * walk4fv[2];
    }
}

void Road::getStoplineMask(float x, float y, float z, float* color, unsigned char seed) {
    float mark = 0.0f;
    float a = 0.00f;
    float b = a + 0.04f;
    float u = 0.28f;
    float v = 0.49f;
    if (y < b && y >= a) {
        if (x > (1.00f - v) && x < (1.00f - u)) {
            mark = 1.0f;
        }
    } else if (y > (1.00f - b) && y <= (1.00f - a)) {
        if (x < v && x > u) {
            mark = 1.0f;
        }
    }
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 0.0f; //mark;
}

void Road::getZebraMask(float x, float y, float z, float* color, unsigned char seed) {
    float mark = 0.0f;
    float a = 0.10f;
    float b = a + 0.1f;
    float u = 0.27f;
    float v = 1.0f - u;
    if ((y < b && y > a) || (y > (1.00f - b) && y < (1.00f - a))) {
        if (x < v && x > u) {
            mark = 1.0f;
        }
    }
    {
        unsigned short pattern = 0x6;
        int bitshift = (int) fmod(x * 52.0f, 3.0f);
        bool bit = (pattern >> bitshift) & 1;
        mark = bit ? mark : 0.0f;
    }
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = mark;
}

void Road::getBasicCrossRoad(float x, float y, float z, float* color, unsigned char seed) {
    float ns[4];
    float ew[4];
    getBasicRoad(x, y, z, ns, seed + 0);
    getBasicRoad(y, x, z, ew, seed + 1);

    {
        float ns_stop[4];
        float ew_stop[4];
        getStoplineMask(x, y, z, ns_stop, seed + 0);
        getStoplineMask(y, 1.0f - x, z, ew_stop, seed + 1);

        float a = ns_stop[3] * 0.6f;
        float b = 1.0f - a;
        ns[0] = ns_stop[0] * a + b * ns[0];
        ns[1] = ns_stop[1] * a + b * ns[1];
        ns[2] = ns_stop[2] * a + b * ns[2];
        //ns[3] = ns_stop[3] * a + b * ns[3];

        a = ew_stop[3] * 0.6f;
        b = 1.0f - a;
        ew[0] = ew_stop[0] * a + b * ew[0];
        ew[1] = ew_stop[1] * a + b * ew[1];
        ew[2] = ew_stop[2] * a + b * ew[2];
    }

    {
        float ns_zeb[4];
        float ew_zeb[4];
        getZebraMask(x, y, z, ns_zeb, seed + 0);
        getZebraMask(y, 1.0f - x, z, ew_zeb, seed + 1);

        float a = ns_zeb[3] * 0.6f;
        float b = 1.0f - a;
        ns[0] = ns_zeb[0] * a + b * ns[0];
        ns[1] = ns_zeb[1] * a + b * ns[1];
        ns[2] = ns_zeb[2] * a + b * ns[2];

        a = ew_zeb[3] * 0.6f;
        b = 1.0f - a;
        ew[0] = ew_zeb[0] * a + b * ew[0];
        ew[1] = ew_zeb[1] * a + b * ew[1];
        ew[2] = ew_zeb[2] * a + b * ew[2];
    }

    float alpha = (fabs(0.5f - fmod(x, 1)) < fabs(0.5f - fmod(y, 1))) ? 1.0f : 0.0f;
    //alpha = 0.8f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicTSRoad(float x, float y, float z, float* color, unsigned char seed) {
    float news[4];
    float ew[4];
    getBasicCrossRoad(y, x, z, news, seed + 0);
    getBasicRoad(y, x, z, ew, seed + 1);
    float alpha = ((fmod(y, 1) < fmod(x, 1)) && (fmod(y, 1) < 1.0f - fmod(x, 1))) ? 1.0f : 0.0f;
    color[0] = alpha * news[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * news[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * news[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * news[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicTNRoad(float x, float y, float z, float* color, unsigned char seed) {
    float news[4];
    float ew[4];
    getBasicCrossRoad(y, x, z, news, seed + 0);
    getBasicRoad(y, x, z, ew, seed + 1);
    float alpha = ((fmod(y, 1) > fmod(x, 1)) && (fmod(y, 1) > 1.0f - fmod(x, 1))) ? 1.0f : 0.0f;
    color[0] = alpha * news[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * news[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * news[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * news[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicTERoad(float x, float y, float z, float* color, unsigned char seed) {
    float news[4];
    float ew[4];
    getBasicCrossRoad(y, x, z, news, seed + 0);
    getBasicRoad(x, 1.0f - y, z, ew, seed + 1);
    float alpha = ((fmod(x, 1) > fmod(y, 1)) && (fmod(x, 1) > 1.0f - fmod(y, 1))) ? 1.0f : 0.0f;
    color[0] = alpha * news[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * news[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * news[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * news[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicTWRoad(float x, float y, float z, float* color, unsigned char seed) {
    float news[4];
    float ew[4];
    getBasicCrossRoad(y, x, z, news, seed + 0);
    getBasicRoad(x, 1.0f - y, z, ew, seed + 1);
    float alpha = ((fmod(x, 1) < fmod(y, 1)) && (fmod(x, 1) < 1.0f - fmod(y, 1))) ? 1.0f : 0.0f;
    color[0] = alpha * news[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * news[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * news[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * news[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicNERoad(float x, float y, float z, float* color, unsigned char seed) {
    float ns[4];
    float ew[4];
    getBasicRoad(x, y, z, ns, seed + 0);
    getBasicRoad(y, x, z, ew, seed + 1);
    float alpha = (fmod(y, 1) > fmod(x, 1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicNWRoad(float x, float y, float z, float* color, unsigned char seed) {
    float ns[4];
    float ew[4];
    getBasicRoad(x, y, z, ns, seed + 0);
    getBasicRoad(y, x, z, ew, seed + 1);
    float alpha = (fmod(y, 1) > 1.0f - fmod(x, 1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicSERoad(float x, float y, float z, float* color, unsigned char seed) {
    float ns[4];
    float ew[4];
    getBasicRoad(x, y, z, ns, seed + 0);
    getBasicRoad(y, x, z, ew, seed + 1);
    float alpha = (1.0f - fmod(y, 1) > fmod(x, 1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

void Road::getBasicSWRoad(float x, float y, float z, float* color, unsigned char seed) {
    float ns[4];
    float ew[4];
    getBasicRoad(x, y, z, ns, seed + 0);
    getBasicRoad(y, x, z, ew, seed + 1);
    float alpha = (1.0f - fmod(y, 1) > 1.0f - fmod(x, 1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

