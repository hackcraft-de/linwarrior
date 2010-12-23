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

#include <map>
#include <list>
#include "cParticle.h"

class cBackground;

/**
 * A background in form of a SkySphere just like
 * those virtual horizons found in aviation instruments.
 */
class cBackground {
public:
    float hour;
    std::map<int, unsigned int> textures;
    float topColor[3];
    float middleColor[3];
    float bottomColor[3];
    std::list<cParticle*> rain;
    int seed;
    float heightshift;
public:
    cBackground();
    void drawBackground(float hour = 12.00f); // Calls other draw*()
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

