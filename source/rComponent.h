/* 
 * File:     rComponent.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 6:13 PM
 */

#ifndef RCOMPONENT_H
#define	RCOMPONENT_H

#include "OID.h"
#include "psi3d/math3d.h"
#include "cParticle.h"
#include "psi3d/md5frmt.h"

#include <cassert>
#include <string>
#include <set>
#include <map>

class cObject;
class cPad;
class cController;
class cMessage;

/**
 * Base-Class of all components.
 * A components can be understood as a set of attributes reflecting
 * a certain part, component or aspect of the whole object.
 * Input to and output from components should rely soley on attributes
 * or signal variables for i/o impules.
 */
struct rComponent {
    cObject* object;
    std::string role;
    bool active;

    rComponent(rComponent* original = NULL) {
        if (original != NULL) {
            object = original->object;
            active = original->active;
            role = original->role;
        } else {
            object = NULL;
            active = true;
            role = "";
        }
    }

    virtual rComponent* clone() {
        return new rComponent(this);
    }

    virtual float constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) { return 0.0f; };
    virtual void message(cMessage* message) {};
    virtual void animate(float spf) {};
    virtual void transform() {};
    virtual void drawSolid() {};
    virtual void drawEffect() {};
    virtual void drawHUD() {};
};

/**
 *  Encapsulates onscreen descriptive texts about an object.
 */
struct rNameable : public rComponent {
    /// Base position for rendering. (hook i)
    vec3 pos0;
    /// Base orientation for rendering. (hook i)
    quat ori0;
    /// Extended position for rendering. (hook i)
    vec3 pos1;
    /// Extended orientation for rendering. (hook i)
    quat ori1;
    /// Color for rendering. (hook i)
    vec4 color;
    /// Enable effect rendering. (hook i)
    bool effect;
    /// Name or title of object or the document.
    std::string name;
    /// String describing the object or the document.
    std::string description;
    /// { 0, .., 25 } for { a, ..., z }.
    unsigned int designation;

    /// Constructor.
    rNameable(cObject* obj = NULL);
    /// Copy Constructor.
    rNameable(rNameable * original);
    /// Clone this.
    virtual rComponent* clone();

    virtual void drawEffect();
};

/**
 * Encapsulates physical state as far as movement and position is concerned.
 */
struct rTraceable : public rComponent, public cParticle {
    /// Averaged amount of groundedness [0,1] result from collision checks.
    float grounded;
    /// Jetdrive throttle setting hook usually [0,1].
    float jetthrottle;
    /// Grounddrive throttle setting hook usually [0,1].
    float throttle;

    /// Constructor.
    rTraceable(cObject* obj = NULL);
    /// Copy Constructor.
    rTraceable(rTraceable * original);
    /// Clone this.
    virtual rComponent* clone();
    /// Accumulate steering and environmental forces.
    void accumulate(float spf);
    /// Integrate position and derive velocity given forces and old state.
    void integrate(float spf);
    /// Adjust position to nullify collisions.
    void collide(float spf);
    /// Accumulate, integrate and collide.
    virtual void animate(float spf);
};


/**
 * Encapsulates attributes related to body damage and armor state.
 */
struct rDamageable : public rComponent {
    /// Marks object as still alife.
    bool alife;
    /// Enumeration of entity body part units: Body, Legs, Left, Right.
    enum Parts {
        BODY = 0, // Some Objects only have this
        LEGS,
        LEFT,
        RIGHT,
        MAX_PARTS
    };
    /// Health-Points for Body-Parts.
    float hp[MAX_PARTS];
    // Diverse armor for Body-Parts.
    // float beam[MAX_PARTS];
    // float pierce[MAX_PARTS];
    // float splash[MAX_PARTS];
    // float heat[MAX_PARTS];
    // float sinks[MAX_PARTS];
    /// Some object that dealt some damage to the object. (hook o) [(oid, damage)]?
    OID disturber;

    /// Constructor.
    rDamageable(cObject* obj = NULL);
    /// Copy Constructor.
    rDamageable(rDamageable * original);
    /// Clone this.
    virtual rComponent* clone();
    /// Apply damage to a hitzone, and return alife.
    virtual bool damage(int hitzone, float damage, cObject* enactor);
    /// Display damaging.
    virtual void drawHUD();
};


/**
 * Encapsulates thoughtful steering behavior attributes and code.
 */
struct rControlled : public rComponent {
    /// Aim target.
    OID target;
    /// Movement target, set Not-a-Number for non-validity.
    vec3 destination;
    /// Human, AI/Remote Pad-Control over the Object.
    cPad* pad;
    /// Command Stack controlls the pad when enabled.
    cController* controller;

    /// Constructor.
    rControlled(cObject* obj = NULL);
    /// Copy Constructor.
    rControlled(rControlled * original);
    /// Clone this.
    virtual rComponent* clone();
    /// Process autopilot-controller.
    virtual void animate(float spf);
};


/**
 * Object Group - as for now intended for messaging maillist-groups.
 */
struct rGrouping : public rComponent {
    /// Group name.
    std::string name;
    /// Lists registered members of this group.
    std::set<OID> members;

    /// Constructor
    rGrouping(cObject* obj = NULL);
    /// Copy Constructor
    rGrouping(rGrouping * original);
    /// Clone this.
    virtual rComponent* clone();
};



struct rRigged : public rComponent {
    /// Model scale. (1.0f, unused/removed now)
    float scale;
    /// Animation time counter in seconds.
    float seconds;
    /// Position. (hook i)
    vec3 pos;
    /// Orientation. (hook i)
    quat ori;
    /// Velocity. (hook i)
    vec3 vel;
    /// Grounded. (hook i)
    float grounded;
    /// Jetting. (hook i)
    float jetting;
    /// The "static" model just as it is loaded.
    MD5Format::model* model;
    /// Actual local-(model-)space joints for this instance.
    MD5Format::joint* joints;
    /// Maps jointpoint identifier to actual joint index of the model (-1 ~ NULL).
    std::map<int, int> jointpoints;
    /// Joint angles for animation.
    std::map<int, std::map<int, float> > rotators;
    // Untransformed vertices for mesh i.
    std::map<int, float*> baseverts;
    /// Untransformed normals for mesh i.
    std::map<int, float*> basenorms;
    /// Basic 3d texture bind when greater 0.
    int basetexture3d;
    /// Current model height - only messured above ground ie. > 0. (hook o)
    float height;
    /// Current model radius as seen from above. (hook o)
    float radius;

    /// Enumeration for indexing joints in animation.
    enum Jointpoints {
        EYE, HEADPITCH, HEADYAW,
        CTMOUNT, LAMOUNT, RAMOUNT, LSMOUNT, RSMOUNT, BKMOUNT,
        JET0, JET1, JET2, JET3, JET4,
        YAW, PITCH, LEFTLEG, RIGHTLEG, LEFTCALF, RIGHTCALF, LEFTFOOT, RIGHTFOOT, MAX_JOINTPOINTS
    };

    /// Constructor

    rRigged(cObject* obj = NULL) : scale(1.0f), seconds(0.0f), grounded(0.0f), jetting(0.0f), model(NULL), joints(NULL), height(0.1f), radius(0.1f) {
        role = "RIGGED";
        object = obj;
        vector_zero(pos);
        vector_zero(vel);
        quat_zero(ori);
    }

    rRigged(rRigged* original) {
        assert(0);
    }

    /// Destructor

    ~rRigged() {
        delete model;
        delete joints;
    }

    virtual rComponent* clone() {
        return new rRigged(this);
    }

    std::string getJointname(unsigned int num) {
        const char* names[] = {
            "EYE", "HEADPITCH", "HEADYAW",
            "CTMOUNT", "LAMOUNT", "RAMOUNT", "LSMOUNT", "RSMOUNT", "BKMOUNT",
            "JET0", "JET1", "JET2", "JET3", "JET4",
            "YAW", "PITCH", "LEFTLEG", "RIGHTLEG", "LEFTCALF", "RIGHTCALF", "LEFTFOOT", "RIGHTFOOT"
        };
        if (num >= MAX_JOINTPOINTS) return string("");
        return string(names[num]);
    }

    int getMountpoint(char* point) {
        int jp = 0;
        if (strcmp(point, "LTorsor") == 0) jp = LSMOUNT;
        else if (strcmp(point, "LUpArm") == 0) jp = LSMOUNT;
        else if (strcmp(point, "LLoArm") == 0) jp = LAMOUNT;
        else if (strcmp(point, "RTorsor") == 0) jp = RSMOUNT;
        else if (strcmp(point, "RUpArm") == 0) jp = RSMOUNT;
        else if (strcmp(point, "RLoArm") == 0) jp = RAMOUNT;
        else if (point[0] == 'L') jp = LSMOUNT;
        else if (point[0] == 'R') jp = RSMOUNT;
        else if (point[0] == 'C') jp = CTMOUNT;
        else jp = BKMOUNT;
        return jointpoints[jp];
    }

    void drawBones();
    void drawMeshes();

    void poseJumping(float spf);
    void poseRunning(float spf);

    void transformJoints();

    void loadModel(std::string filename);

    virtual void animate(float spf);
    virtual void transform();
    virtual void drawSolid();
    virtual void drawEffect();
    //virtual void drawHUD() { }
};


struct rCamera : public rComponent {
    /// # of camera modes: ISO C++ forbids const int with initialization >:(
    enum { MAX_CAMERAMODES = 6 };
    /// Current Camera mode, negative number is indicating transition.
    int camerastate;
    /// Switch camera state one step when greater zero. (hook i)
    float cameraswitch;
    /// Strenght of camera shaking [0,1], scaled internally. (hook i)
    float camerashake;
    /// True when camerastate is in first perspective mode. (hook o)
    bool firstperson;
    /// Base position (hook i).
    quat ori0;
    /// Base orientation (hook i).
    vec3 pos0;
    /// Position relative to base position and orientation (hook o).
    quat ori1;
    /// Orientation relative to base position and orientation (hook o).
    vec3 pos1;

    /// Constructor
    rCamera(cObject * obj);
    /// Mult in camera matrix.
    virtual void camera();

    virtual void animate(float spf);
};

/**
 * Flat-Motoric-Behavior of a mobile unit with base-heading and turret-heading.
 * TODO: Add pos1 for upper body/turret.
 */
struct rMobile : public rComponent {
    /// Base position. (hook i)
    vec3 pos;
    /// Base angles in radians. (hook o)
    vec3 bse;
    /// Base orientation. (hook o)
    quat ori;
    /// Tower angles relative to base in radians. (hook o)
    vec3 twr;
    /// Tower orientation. (hook o)
    quat ori1;
    /// Tower orientation.
    //quat twr_ori;
    /// Current jumpjet set-point [-1,1]. (hook i)
    float jeten;
    /// Current internal smooth jumpjet set-point. (hook o)
    float jetthrottle;
    /// Current drive throttle set-point [-1,1]. (hook i)
    float driveen;
    /// Current internal smooth drive throttle set-point. (hook o)
    float drivethrottle;
    /// Behave like a immobile gunpod with fixed base. (hook i)
    bool immobile;
    /// Steering angle for base, left/right radians-per-second wish. (hook i)
    float chassis_lr;
    /// Smoothed steering angle for base, left/right. (hook o)
    float chassis_lr_;
    /// Steering angle for base, left/right radians-per-second wish. (hook i)
    float chassis_ud;
    /// Smoothed steering angle for base, left/right. (hook o)
    float chassis_ud_;
    /// Steering angle for turret, left/right radians-per-second wish. (hook i)
    float tower_lr;
    /// Steering angle for turret, left/right radians-per-second wish. (hook i)
    float tower_ud;

    /// Constructor
    rMobile(cObject * obj);
    /// Destructor
    ~rMobile() { }

    // Steering
    void ChassisLR(float radians);
    void ChassisUD(float radians);
    float TowerLR(float radians);
    void TowerUD(float radians);

    virtual void animate(float spf);
    virtual void transform();
    virtual void drawSolid();
    virtual void drawEffect();
};

/**
 * Component for collision and collision response.
 * Currently only for cylinder collisions.
 */
struct rCollider : public rComponent {
    quat ori;
    vec3 pos;
    /// Radius for cylinder or sphere.
    float radius;
    /// Ratio of radius to height (height = ratio * radius) if not zero.
    float ratio;
    /// Height of cylinder if ratio not zero.
    float height;

    /// Constructor
    rCollider(cObject * obj);
    /// Destructor
    ~rCollider() { }

    virtual float constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor);
    virtual void animate(float spf);
    virtual void drawEffect();
};


#endif	/* RCOMPONENT_H */

