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

#include "cObject.h"

struct rComcom;
struct rTarcom;
struct rWepcom;
struct rForcom;
struct rNavcom;

struct rNameable;
struct rDamageable;
struct rTraceable;
struct rController;
struct rCamera;
struct rMobile;
struct rRigged;
struct rCollider;

struct rWeapon;
struct rWeaponExplosion;

#include <vector>
#include <map>

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
    static std::map<int, long> sTextures;

public:

    // COMPUTERs
    rComcom* comcom;
    rTarcom* tarcom;
    rWepcom* wepcom;
    rForcom* forcom;
    rNavcom* navcom;

    rNameable* nameable;
    rDamageable* damageable;
    rTraceable* traceable;
    rController* controller;
    rCamera* camera;
    rMobile* mobile;
    rRigged* rigged;
    rCollider* collider;

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

