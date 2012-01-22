/**
 * File:     cBackground.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 *
 * Virtual Horizon with Sky and Ground
 * (SkySphere / SkyBox).
 */

#ifndef _CBACKGROUND_H
#define _CBACKGROUND_H

class Background;

#include <map>
#include <list>

#include "de/hackcraft/psi3d/Particle.h"

#include "de/hackcraft/world/Propmap.h"

/**
 * A background in form of a SkySphere just like
 * those virtual horizons found in aviation instruments.
 * Sets lightsource 0 as skylight according to time.
 */
class Background {
public:
    float hour;
    std::map<int, unsigned int> textures;
    std::list<Particle*> rain;
    int seed;
    /** Total amount of rain drops at a time. */
    int raininess;
    /** Zero to negative shift (upward) makes horizontal curvature visible. */
    float heightshift;
    /** Relative amount of clouds [0,1]. */
    float cloudiness;
    /** Windspeed for cloud movement per (scaled) time. */
    float windspeed;
    /** Light direction according to celestial bodies and time (sun/moon). */
    vec3 light;
public:
    Background();
    Background(Propmap* properties);
    void init(Propmap* properties);
    void drawBackground(float h = 12.00f); // Calls other draw*()
    void drawGalaxy();
    void drawUpperDome();
    void drawLowerDome();
    void drawGround();
    void drawClouds();
    //void drawMountains();
    void drawSun();
    void drawOrbit();
    void drawRain();
};

#endif

