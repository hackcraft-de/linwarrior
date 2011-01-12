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
    /// The "static" model just as it is loaded.
    MD5Format::model* model;
    /// Actual local-(model-)space joints for this instance.
    MD5Format::joint* joints;
    /// Maps jointpoint identifier to actual joint index of the model.
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
    // Untransformed vertices.
    std::map<int, float*> baseverts;
    // Untransformed normals.
    std::map<int, float*> basenorms;
    /// Enumeration for indexing joints in animation.

    enum Jointpoints {
        EYE, HEAD, NECK,
        CTMOUNT, LAMOUNT, RAMOUNT, LSMOUNT, RSMOUNT, BKMOUNT,
        JET0, JET1, JET2, JET3, JET4,
        SPINE, TORSOR, LEFTLEG, RIGHTLEG, LEFTCALF, RIGHTCALF, MAX_JOINTPOINTS
    };

    /// Constructor

    rRigged() : rRole("RIGGED"), scale(1.0f), model(NULL), joints(NULL) {
    }

    rRigged(rRigged* original) : rRole("RIGGED"), scale(1.0f), model(NULL), joints(NULL) {
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
            "EYE", "HEAD", "NECK",
            "CTMOUNT", "LAMOUNT", "RAMOUNT", "LSMOUNT", "RSMOUNT", "BKMOUNT",
            "JET0", "JET1", "JET2", "JET3", "JET4",
            "SPINE", "TORSOR", "LEFTLEG", "RIGHTLEG", "LEFTCALF", "RIGHTCALF"
        };
        if (num >= MAX_JOINTPOINTS) return string("");
        return string(names[num]);
    }
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

    struct rMisc {
        /// List of mounted weapons.
        std::vector<cWeapon*> weapons;

        /// Index of selected weapon.
        int currentWeapon;

        // Exploding end.
        cWeaponExplosion explosion;

        /// Base angles in radians.
        float bse[3];

        /// Tower angles in radians.
        float twr[3];

        /// Current jumpjet set-point.
        float jetpower;

        /// Current throttle set-point.
        float throttle;

        /// Current Camera mode, negative number is indicating transition.
        int camerastate;

        /// Current average(d) speed.
        float avgspeed;

        /// Behave like a immobile gunpod.
        bool immobile;
    };

    rMisc* misc;


    struct rComputerised {
        cComcom* comcom;
        cTarcom* tarcom;
        cSyscom* syscom;
        cWepcom* wepcom;
        cForcom* forcom;
        cNavcom* navcom;
        /// Constructor
        rComputerised(cObject * o);
        /// Destructor
        ~rComputerised();
        /// Time step computers.
        void process(float dt);
    };

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

    // Physics and posing accordingly
    void animatePhysics(float spf);
    void poseRunning(float spf);
    void poseJumping(float spf);

    // Steering
    void ChassisLR(float radians);
    void ChassisUD(float radians);
    float TowerLR(float radians);
    void TowerUD(float radians);

    // Weapons
    void fireAllWeapons();
    void fireWeapon(unsigned n);
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
    float constrainParticleToWorld(float* worldpos, float radius = 0.0f);
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

