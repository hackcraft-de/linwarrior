/* 
 * File:     rMobile.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:25 PM
 */

#ifndef RMOBILE_H
#define	RMOBILE_H

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/world/OID.h"

#include "de/hackcraft/psi3d/math3d.h"

/**
 * Flat-Motoric-Behavior of a mobile unit with base-heading and turret-heading.
 * It should process own position/headings, take steering input
 * to calculate new headings and drive forces, and shall output steering
 * suggestions to reach or aim for that target. These suggestions may be
 * looped back as steering input to achieve automatic/NPC movement and aiming.
 * A player may ignore steering suggestions and just deliver steering input.
 * 
 * TODO: Add pos1 for upper body/turret.
 */
struct rMobile : public Component {
public: // INPUT
    /// Base position. (hook i)
    vec3 pos0;
    /// Current jumpjet set-point [-1,1]. (hook i)
    float jeten;
    /// Current drive throttle set-point [-1,1]. (hook i)
    float driveen;
    /// Behave like a immobile gunpod with fixed base. (hook i)
    bool immobile;
    /// Steering angle for base, left/right radians-per-second wish. (hook i)
    float chassis_lr;
    /// Steering angle for base, left/right radians-per-second wish. (hook i)
    float chassis_ud;
    /// Steering angle for turret, left/right radians-per-second wish. (hook i)
    float tower_lr;
    /// Steering angle for turret, left/right radians-per-second wish. (hook i)
    float tower_ud;
    /// Aim target. (hook i)
    OID aimtarget;
    /// Fire at aim target if appropriate. (hook i)
    bool firetarget;
    /// Movement target, set Not-a-Number for non-validity. (hook i)
    vec3 walktarget;
    /// Desired distance to walk-target. (hook i)
    float walktargetdist;
public: // OUTPUT
    /// Base angles in radians. (hook o)
    vec3 bse;
    /// Base orientation. (hook o)
    quat ori0;
    /// Tower angles relative to base in radians. (hook o)
    vec3 twr;
    /// Tower orientation. (hook o)
    quat ori1;
    /// Current internal smooth jumpjet set-point. (hook o)
    float jetthrottle;
    /// Current internal smooth drive throttle set-point. (hook o)
    float drivethrottle;
    /// Throttle towards target. (hook o)
    float drive_tgt;
    /// Smoothed steering angle for base, left/right. (hook o)
    float chassis_lr_;
    /// Steering angle for base towards target. (hook o)
    float chassis_lr_tgt;
    /// Smoothed steering angle for base, left/right. (hook o)
    float chassis_ud_;
    /// Steering angle for base towards target. (hook o)
    float chassis_ud_tgt;
    /// Steering angle for turret towards target. (hook o)
    float tower_lr_tgt;
    /// Steering angle for turret towards target. (hook o)
    float tower_ud_tgt;
    /// Firing at aim target is appropriate. (hook o)
    bool firetarget_tgt;
    /// Distance to aim-target. (hook o)
    float aimrange;
    /// Distance to walk-target. (hook o)
    float walkrange;
protected: // INTERNALS
public:

    /// Constructor
    rMobile(Entity * obj);
    /// Destructor

    ~rMobile() {
    }

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

#endif	/* RMOBILE_H */

