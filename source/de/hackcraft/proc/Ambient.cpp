#include "Ambient.h"

#include "Solid.h"

#include <math.h>

void Ambient::galaxy(float x, float y, float z, float* color, float scale) {
    // Normalise direction vector and scale.
    float oolen = (0.5 * scale) / sqrtf(x * x + y * y + z * z);
    float v[3] = {
        0.0f + x * oolen,
        0.0f + y * oolen,
        0.0f + z * oolen
    };
    //cSolid::star_nebula(v[0], v[1], v[2], color);
    Solid::star_fastnebula(v[0], v[1], v[2], color);
}

void Ambient::clouds(float x, float y, float z, float* color) {
    // Normalise direction vector.
    float oolen = 1.0f / sqrtf(x * x + y * y + z * z);
    float v[3] = {x * oolen, y * oolen, z * oolen};
    Solid::planet_cloud(v[0], v[1], v[2], color);
}

void Ambient::sky(float x, float y, float z, float* color, float hour) {
    // Remember 2pi ~ 360°
    double t = ((hour / 24.0 * 2 * M_PI) - 0.5 * M_PI); // = [-0.5pi,+1.5pi]

    double light = 0.1 + 0.8 * cos((hour - 12.0) / 12.00 * 0.5 * M_PI);

    // Daylight gradient.
    // hour = [0,24]
    // => [-90,   0, 90, 180, 270]
    // => [ -1,   0,  1,   0,  -1] = sin
    // => [  0, 0.5,  1, 0.5,   0] = s daylight sine-wave.
    double s = sin(t) * 0.5 + 0.5; // = [0,1]
    double topColor[] = {s * 0.2 + 0.1, s * 0.2 + 0.1, s * 0.8 + 0.2, light};
    double middleColor[] = {s * 0.95 + 0.05, s * 0.95 + 0.05, s * 0.95 + 0.05, 1};
    double bottomColor[] = {s * 0.55 + 0.05, s * 0.55 + 0.05, s * 0.55 + 0.05, 1};

    // Dusk and dawn gradient.
    // => [ 0,    1,  0,   1,   0] = c
    double c = pow(fabs(cos(t)) * 0.5 + 0.5, 4); // = [0,1]
    double top_[] = {c * 0.25, c * 0.25, c * 0.0, light};
    double mid_[] = {c * 0.55, c * 0.25, c * 0.0, 1};
    double bot_[] = {c * 0.25, c * 0.11, c * 0.0, 1};

    // Normalise direction vector.
    double oolen = 1.0 / sqrtf(x * x + y * y + z * z);
    double yd = y * oolen;

    // top = 1 when yd > 0, top = 0 otherwise.
    double top = 0.5 + copysign(0.5, yd);
    double alpha = fabs(yd);

    // middleColor could be factored out but it may change.

    for (int i = 0; i < 4; i++) {
        
        topColor[i] += top_[i];
        middleColor[i] += mid_[i];
        bottomColor[i] += bot_[i];
        
        color[i] = float(
                top * (alpha * topColor[i] + (1.0f - alpha) * middleColor[i]) +
                (1.0f - top) * (alpha * bottomColor[i] + (1.0f - alpha) * middleColor[i])
                );
    }
}
