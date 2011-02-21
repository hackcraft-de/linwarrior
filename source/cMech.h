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
#include "cWeapon.h"
#include "cComcom.h"

#include <vector>
#include <map>

class cWeapon;

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
    /// Model scale.
    float scale;
    /// Position hook.
    vec3 pos;
    /// Orientation hook.
    quat ori;
    /// Velocity hook.
    vec3 vel;
    /// Grounded hook.
    float grounded;
    /// Jetting hook.
    float jetting;
    /// The "static" model just as it is loaded.
    MD5Format::model* model;
    /// Actual local-(model-)space joints for this instance.
    MD5Format::joint* joints;
    /// Maps jointpoint identifier to actual joint index of the model (-1 ~ NULL).
    std::map<int, int> jointpoints;
    /// Joint angles for animation.
    std::map<int, std::map<int, float> > rotators;
    // Mech Joint and Mountpoint Matrices.
    float HDMount[16];
    float CTMount[16];
    float BKMount[16];
    float RSMount[16];
    float RAMount[16];
    float LSMount[16];
    float LAMount[16];
    // Untransformed vertices for mesh i.
    std::map<int, float*> baseverts;
    // Untransformed normals for mesh i.
    std::map<int, float*> basenorms;
    /// Enumeration for indexing joints in animation.
    enum Jointpoints {
        EYE, HEADPITCH, HEADYAW,
        CTMOUNT, LAMOUNT, RAMOUNT, LSMOUNT, RSMOUNT, BKMOUNT,
        JET0, JET1, JET2, JET3, JET4,
        YAW, PITCH, LEFTLEG, RIGHTLEG, LEFTCALF, RIGHTCALF, LEFTFOOT, RIGHTFOOT, MAX_JOINTPOINTS
    };
    /// Animation time counter in seconds.
    float seconds;

    /// Constructor

    rRigged(cObject* obj = NULL) : rRole("RIGGED"), scale(1.0f), grounded(0.0f), jetting(0.0f), model(NULL), joints(NULL), seconds(0.0f) {
        object = obj;
        vector_zero(pos);
        vector_zero(vel);
        quat_zero(ori);
    }

    rRigged(rRigged* original) : rRole("RIGGED") {
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

    float* getMountMatrix(char* point) {
        if (strcmp(point, "LTorsor") == 0) return LSMount;
        else if (strcmp(point, "LUpArm") == 0) return LSMount;
        else if (strcmp(point, "LLoArm") == 0) return LAMount;
        else if (strcmp(point, "RTorsor") == 0) return RSMount;
        else if (strcmp(point, "RUpArm") == 0) return RSMount;
        else if (strcmp(point, "RLoArm") == 0) return RAMount;
        else if (point[0] == 'L') return LSMount;
        else if (point[0] == 'R') return RSMount;
        else if (point[0] == 'C') return CTMount;
        else return BKMount;
    }

    void drawBones();
    void drawMeshes();

    void poseJumping(float spf);
    void poseRunning(float spf);

    void transformJoints();
    void transformMounts();

    void loadModel(std::string filename);

    virtual void animate(float spf);
    virtual void transform();
    virtual void drawSolid();
    virtual void drawEffect();
    //virtual void drawHUD() { }
};


struct rComputerised : public rRole {
    cComcom* comcom;
    cTarcom* tarcom;
    cSyscom* syscom;
    cWepcom* wepcom;
    cForcom* forcom;
    cNavcom* navcom;
    /// Constructor
    rComputerised(cObject * obj);
    /// Destructor
    ~rComputerised();
    virtual rRole* clone() {
        return new rComputerised(object);
    }
    /// Time step computers.
    virtual void animate(float dt);
    virtual void drawHUD();
};

struct rMobile : public rRole {
    /// List of mounted weapons.
    std::vector<cWeapon*> weapons;

    /// Index of selected weapon.
    int currentWeapon;

    /// Weapon target.
    OID target;

    // Exploding end.
    cWeaponExplosion explosion;

    /// Base angles in radians.
    vec3 bse;

    /// Tower angles in radians.
    vec3 twr;

    /// Current jumpjet set-point.
    float jetthrottle;

    /// Current throttle set-point.
    float throttle;

    /// Current Camera mode, negative number is indicating transition.
    int camerastate;

    /// Average friction for groundedness and pose.
    float grounded;

    /// Behave like a immobile gunpod.
    bool immobile;

    /// Constructor
    rMobile(cObject * obj) : rRole("MOBILE"), currentWeapon(0), target(0), jetthrottle(0), throttle(0), camerastate(1), grounded(0.5f), immobile(false) {
        object = obj;
        twr[0] = twr[1] = twr[2] = 0.0f;
        bse[0] = bse[1] = bse[2] = 0.0f;
    }
    /// Destructor
    ~rMobile() { }

    // Steering
    void ChassisLR(float radians);
    void ChassisUD(float radians);
    float TowerLR(float radians);
    void TowerUD(float radians);

    // Weaponry
    void fireCycleWeapons();
    void fireAllWeapons();
    void fireWeapon(unsigned n);

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
    friend class cWepcom;
    friend class cForcom;
protected:


    /// # of camera modes: ISO C++ forbids const int with initialization >:(

    enum {
        MAX_CAMERAMODES = 6
    };


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

protected:

    rMobile* mobile;

    rComputerised* computerised;

    rRigged* rigged;


public:
    cMech(float* pos = NULL, float* rot = NULL);
    ~cMech();

    // Events
    virtual void onMessage(cMessage* message);
    virtual void onSpawn();

    // Camera and Headphone
    void multEyeMatrix();
    void setAsAudioListener();

    // Weapons
    void mountWeapon(char* point, cWeapon *weapon, bool add = true);

    // World Step
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

