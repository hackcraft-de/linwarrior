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

#ifndef _CBACKGROUNDSYSTEM_H
#define _CBACKGROUNDSYSTEM_H

class BackgroundSystem;

#include "de/hackcraft/world/Subsystem.h"

#include "de/hackcraft/psi3d/math3d.h"

#include <map>
#include <list>

class Logger;
class Propmap;
struct Particle;

/**
 * A background in form of a SkySphere just like
 * those virtual horizons found in aviation instruments.
 * Sets lightsource 0 as skylight according to time.
 */
class BackgroundSystem : public Subsystem {
public:
    BackgroundSystem* getInstance();
    
public:
    BackgroundSystem();
    BackgroundSystem(Propmap* properties);
    
    void init(Propmap* properties);
    void initTextures();

public:
    /**
     * Limit position to accessible areas and returns impact depth.
     * 
     * @param worldpos The particle position to be constrained given in world coordinates.
     * @param radius The size of the particle in terms of a radius.
     * @return Zero if there was no collision, else the maximum impact depth.
     */
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius) { return 0.0f; };
    
public:
    /** Advance simulation time for one frame. */
    virtual void advanceTime(int deltamsec);
    
    /** Re-build spatial clustering of objects. */
    virtual void clusterObjects() {};
    
    /** Deliver overdue messages to objects. */
    virtual void dispatchMessages() {};
    
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects() {};
    
    /** Let all objects calculate transformation matrices etc. */
    virtual void transformObjects() {};
    
    /** Setup structures for rendering */
    virtual void setupView(float* pos, float* ori) {};
    
    /** Draws background (skybox). 
     * @return False if the background was not painted or is not covered completely - true otherwise.
     */
    virtual bool drawBack();
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid() {};
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect() {};
    
private:
    void drawGalaxy();
    void drawUpperDome();
    void drawLowerDome();
    void drawGround();
    void drawClouds();
    //void drawMountains();
    void drawSun();
    void drawOrbit();
    void drawRain();
    void drawPollution();
    
private:
    static BackgroundSystem* instance;
    static Logger* logger;
    static std::map<int, unsigned int> textures;
    
private:
    /** Time in 24 hours format plus fractions of an hour - influences everything. */
    float hour;
    /** Currently simulated rain particles. */
    std::list<Particle*> rain;
    /** Currently simulated pollution particles. */
    std::list<Particle*> dust;
    /** Current randomizer seed backup (make local var?). */
    int seed;
    /** Total amount of rain drops at a time. */
    int raininess;
    /** Factor for the max amount of dust particles generated per seconds. */
    float dustiness;
    /** Zero to negative shift (upward) makes horizontal curvature visible. */
    float heightshift;
    /** Relative amount of clouds [0,1]. */
    float cloudiness;
    /** Windspeed for cloud movement per (scaled) time. */
    float windspeed;
    /** Light direction according to celestial bodies and time (sun/moon). */
    vec4 light;
};

#endif

