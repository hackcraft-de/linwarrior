/* 
 * File:     rMobile.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:25 PM
 */

#ifndef RMOBILE_H
#define	RMOBILE_H

#include "OID.h"
#include "rComponent.h"
#include "psi3d/math3d.h"

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
    /// Aim target. (hook i)
    OID target;
    /// Movement target, set Not-a-Number for non-validity. (hook i)
    vec3 destination;

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

#endif	/* RMOBILE_H */

