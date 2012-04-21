/* 
 * File:     rScatter.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on June 8, 2011, 10:33 PM
 * (dating back before, originated in building)
 */

#ifndef RSCATTER_H
#define	RSCATTER_H

#include "IModel.h"

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/psi3d/math3d.h"

#include <map>
#include <list>

struct Particle;

/**
 * Just the beginning of a particle generator system.
 * Originally thought to display static scattered billboards only:
 * Set/cluster/field of random distributed small
 * environmental decal objects such as stones, flowers,
 * grass, dirt and debris.
 */
class rScatter : public Component, public IModel {
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
    /// Instance counter for resource allocation.
    static int sInstances;
    /// Shared material bindings.
    static std::map<int, long> sTextures;
public:
    enum BillboardType {
        BT_NONE, BT_AROUND, BT_AXIS
    };
protected:
    std::list<Particle*> decalParticles;
public: // INPUT
    /// Base orientation (hook i).
    vec3 pos0;
    /// Base position (hook i).
    quat ori0;
public: // OUTPUT
protected: // INTERNALS
public:
    rScatter(Entity* obj, float radius = 1.0f, float density = 1.0f);
    rScatter(const rScatter& orig);
    virtual ~rScatter();
    
    virtual int getPosX() { return (int) pos0[0]; }
    virtual int getPosZ() { return (int) pos0[2]; }
    virtual void animate(float spf) {};
    virtual void transform() {};
    virtual void drawSolid() {};
    virtual void drawEffect();
private:

};

#endif	/* RSCATTER_H */

