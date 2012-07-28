/* 
 * File:    PhysicsSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:46 PM
 */

#ifndef PHYSICSSYSTEM_H
#define	PHYSICSSYSTEM_H

class PhysicsSystem;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/Subsystem.h"

#include <map>
#include <vector>

class Logger;
class rCollider;
class rTraceable;

class PhysicsSystem : public Subsystem {
public:
    PhysicsSystem();
    virtual ~PhysicsSystem();
    
    void add(rCollider* collider);
    void add(rTraceable* traceable);

    float* getGravity();
    
    float getGndfriction();
    
    float getAirdensity();
    
    
     /** Advance simulation time for one frame. */
    virtual void advanceTime(int deltamsec) {};
    /** Re-build spatial clustering of objects. */
    virtual void clusterObjects() {};
    /** Deliver overdue messages to objects. */
    virtual void dispatchMessages() {};
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects();
    /** Let all objects calculate transformation matrices etc. */
    virtual void transformObjects() {};
    /** Setup structures for rendering */
    virtual void setupView(float* pos, float* ori) {};
   /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid() {};
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect() {};
    
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius);
    
    static PhysicsSystem* getInstance();
private:
    
    /** List of allocated components of this type. */
    std::vector<rCollider*> colliders;
    /** Index on id of leased components of this type. */
    std::map<OID,rCollider*> collidersIndex;
    
    /** List of allocated components of this type. */
    std::vector<rTraceable*> traceables;
    /** Index on id of leased components of this type. */
    std::map<OID,rTraceable*> traceablesIndex;
    
    /** Allows searching the world in a structured manner. */
    //std::unordered_map<OID, std::list<rCollider*> > geoToColliders;

    /** Gravity acceleration vector [m/s²]. space = (0,0,0), earth = (0,-9.8,0). */
    vec3 gravity;

    /** Air density in kg per cubic meter (default 1.204 kg/m3 Air at 20°C). */
    float airdensity;

    /** Velocity "damping" when on ground (0.14 ok). */
    float gndfriction;
    
    static PhysicsSystem* instance;
    
    static Logger* logger;
};

#endif	/* PHYSICSSYSTEM_H */

