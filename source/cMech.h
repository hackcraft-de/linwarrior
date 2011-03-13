/**
 * File:     cMech.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM (Dez1999)
 */

#ifndef _CMECH_H
#define	_CMECH_H

class cMech;

#include "psi3d/md5frmt.h"

#include "cObject.h"
#include "rWeapon.h"
#include "cComcom.h"

#include <vector>
#include <map>

class rWeapon;

// Gamepad Mapping
#define MECH_CHASSIS_LR_AXIS  AX_LR1
#define MECH_TURRET_LR_AXIS   AX_LR2
#define MECH_TURRET_UD_AXIS   AX_UD2
#define MECH_THROTTLE_AXIS    AX_UD1
#define MECH_FIRE_BUTTON1     BT_PL
#define MECH_FIRE_BUTTON2     BT_R2
#define MECH_JET_BUTTON1      BT_PD
#define MECH_JET_BUTTON2      BT_L2
#define MECH_CAMERA_BUTTON    BT_PU
#define MECH_NEXT_BUTTON      BT_PR
#define MECH_PREV_BUTTON      BT_PL


struct rRigged : public rRole {
    /// Model scale. (1.0f, unused/removed now)
    float scale;
    /// Animation time counter in seconds.
    float seconds;
    /// Position. (hook)
    vec3 pos;
    /// Orientation. (hook)
    quat ori;
    /// Velocity. (hook)
    vec3 vel;
    /// Grounded. (hook)
    float grounded;
    /// Jetting. (hook)
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

    /// Enumeration for indexing joints in animation.
    enum Jointpoints {
        EYE, HEADPITCH, HEADYAW,
        CTMOUNT, LAMOUNT, RAMOUNT, LSMOUNT, RSMOUNT, BKMOUNT,
        JET0, JET1, JET2, JET3, JET4,
        YAW, PITCH, LEFTLEG, RIGHTLEG, LEFTCALF, RIGHTCALF, LEFTFOOT, RIGHTFOOT, MAX_JOINTPOINTS
    };

    /// Constructor

    rRigged(cObject* obj = NULL) : scale(1.0f), seconds(0.0f), grounded(0.0f), jetting(0.0f), model(NULL), joints(NULL) {
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

    virtual rRole* clone() {
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


struct rCamera : public rRole {
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
    /// Base position (hook).
    quat ori0;
    /// Base orientation (hook).
    vec3 pos0;
    /// Position relative to base position and orientation (hook).
    quat ori1;
    /// Orientation relative to base position and orientation (hook).
    vec3 pos1;

    /// Constructor
    rCamera(cObject * obj);
    /// Mult in camera matrix.
    virtual void camera();
    
    virtual void animate(float spf);
};


struct rMobile : public rRole {
    /// Base angles in radians. (hook o)
    vec3 bse;
    /// Base orientation. (hook o)
    quat bse_ori;
    /// Tower angles in radians. (hook o)
    vec3 twr;
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
 * Models Mechlike Objects.
 * 
 */
class cMech : public cObject {
    friend class rWepcom;
    friend class rForcom;
protected:

    /// Instance counter.
    static int sInstances;

    enum {
        TEXTURE_DESERT,
        TEXTURE_WOOD,
        TEXTURE_SNOW,
        TEXTURE_URBAN
    };

    /// Texture Binds shared between instances.
    static std::map<int,long> sTextures;

public:

    // COMPUTERs
    rComcom* comcom;
    rTarcom* tarcom;
    rWepcom* wepcom;
    rForcom* forcom;
    rNavcom* navcom;

    rCamera* camera;
    rMobile* mobile;
    rRigged* rigged;

    // WEAPON: EXPLOSION
    rWeaponExplosion* explosion;

    /// List of WEAPONs.
    std::vector<rWeapon*> weapons;

    /// Index of selected weapon.
    int currentWeapon;

public:
    cMech(float* pos = NULL, float* rot = NULL);
    ~cMech();

    // Events
    virtual void onSpawn();

    // Camera and Headphone
    void multEyeMatrix();
    void setAsAudioListener();

    // Weapons
    void mountWeapon(char* point, rWeapon *weapon, bool add = true);
    void fireCycleWeapons();
    void fireAllWeapons();
    void fireWeapon(unsigned n);

    // World Step
    virtual void message(cMessage* message); // Conditionally/repeatedly called.
    virtual void animate(float spf);
    virtual void transform();
    virtual void drawSolid();
    virtual void drawEffect();
    virtual void drawHUD();

    // Particle constraining
    virtual void damageByParticle(float* localpos, float damage, cObject* enactor = NULL);
    virtual float constrainParticle(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL);
public:
    // Queries
    virtual OID enemyNearby();
    virtual OID disturbedBy();

    // Fuzzy Predicates [0,1]
    virtual float inDestinationRange();
    virtual float inMeeleRange();
    virtual float inWeaponRange();
    virtual float inTargetRange();

    // Output Actions
    virtual void do_moveTowards();
    virtual void do_moveNear();
    virtual void do_aimAt();
    virtual void do_fireAt();
    virtual void do_idle();
    virtual void do_aimFor(OID target);
    virtual void do_moveFor(float* dest);
};


#endif	/* _CMECH_H */

