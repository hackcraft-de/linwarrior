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
    //float f = -8.1 + 5.95;
    float f = -8.1 + 1.0;

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



vec2 warpDualShift(vec2 pix, float shift)
{
    vec2 p = pix;
    p.x = mod(p.x * 2.0, 1.0);
    
    bool left = (pix.x < 0.5);
    
    if (left) {
        p.x = p.x - shift;
    } else {
        p.x = p.x + shift;
    }
    
    return warpMono(p);
}

