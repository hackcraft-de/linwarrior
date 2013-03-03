// Author:   Benjamin Pickhardt
// Project:  LinWarrior 3d
// Home:     hackcraft.de

// --------------------------------
// Screen space warping conversions


vec2 warpNone(vec2 pix)
{
    return pix;
}


vec2 warpMono(vec2 pix)
{
    float f = -8.1;

    vec2 v = pix * 2.0 - 1.0;

    vec2 warped = vec2(
        f * v.x / (v.y * v.y + f),
        f * v.y / (v.x * v.x + f)
    ) * 0.5 + 0.5;

    return warped;
}


vec2 warpDual(vec2 pix)
{
    pix.x = mod(pix.x * 2.0, 1.0);
    return warpMono(pix);
}


