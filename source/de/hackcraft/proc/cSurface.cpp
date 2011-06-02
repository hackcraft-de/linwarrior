#include "cSurface.h"

#include "noise.h"
#include "cDistortion.h"
#include "cSolid.h"

void cSurface::stone_plates(float x, float y, float z, float* color, unsigned char seed) {
    float grain = 0.2f + 0.1f * (0.5f + 0.5f * cNoise::simplex3(x * 128, y * 128, z * 128, seed));
    //grain *= 0.6f + 0.05f * cos(4.0f * x * 2.0f * M_PI);

    float cracks0 = cDistortion::sig(cNoise::voronoi3(x * 16, y * 16, z * 16, 0.99f, 0.5f, 2.0f, seed) * 16.0f - 16.0f);
    float cracks1 = 0.0f; //cDistortion::sig(cNoise::voronoi3(x*32, y*32, z*32, 0.99f, 0.5f, 2.0f, seed+1) * 16.0f - 16.0f);
    float cracks2 = fmax(cracks0 * 0.99, 0.4 * cracks1);
    float cracks3 = cracks2 * (0.2f + 0.8f * cNoise::simplex3(x * 8, y * 8, z * 8, seed + 1));

    float stones = 0.1 + 1.8f * fmin(0.5f, 3 * cDistortion::rampPyramid(0, 0, 1, 1, fmod(x * 8, 1), fmod(y * 8, 1)));
    stones = stones - stones * 0.9f * cracks3;

    color[0] = color[1] = color[2] = (grain - grain * 0.9f * cracks3) * 0.8f + 0.4f;
    color[3] = 1.0f;

    color[0] *= stones;
    color[1] *= stones;
    color[2] *= stones;
}
