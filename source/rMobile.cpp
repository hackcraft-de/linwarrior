#include "rMobile.h"

#include "psi3d/macros.h"
#include "GL/glew.h"


rMobile::rMobile(cObject * obj) : jeten(0), jetthrottle(0), driveen(0), drivethrottle(0), immobile(false), chassis_lr(0), chassis_lr_(0), chassis_ud(0), chassis_ud_(0), tower_lr(0), tower_ud(0), target(0) {
    role = "MOBILE";
    object = obj;
    vector_zero(pos);
    vector_zero(twr);
    vector_zero(bse);
    quat_zero(ori);
    quat_zero(ori1);
    vector_set(destination, float_NAN, float_NAN, float_NAN);
}

void rMobile::ChassisLR(float radians) {
    const float limit = 0.01 * M_PI;
    float e = radians;
    if (e > +limit) e = +limit;
    if (e < -limit) e = -limit;
    bse[1] += e;
    const float fullcircle = 2 * M_PI;
    bse[1] = fmod(bse[1], fullcircle);
}

void rMobile::ChassisUD(float radians) {
    float e = radians;
    const float limit = 0.005 * M_PI;
    if (e > +limit) e = +limit;
    if (e < -limit) e = -limit;
    bse[0] += e;
    const float fullcircle = 2 * M_PI;
    bse[0] = fmod(bse[0], fullcircle);
}

float rMobile::TowerLR(float radians) {
    float e = radians;
    const float steplimit = 0.01 * M_PI;
    if (e > +steplimit) e = +steplimit;
    if (e < -steplimit) e = -steplimit;
    twr[1] += e;
    if (immobile) {
        const float fullcircle = 2 * M_PI; // 360
        twr[1] = fmod(twr[1], fullcircle);
        return 0.0f;
    } else {
        const float limit = 0.4 * M_PI;
        float excess = copysign( (fabs(twr[1]) > limit ? fabs(twr[1]) - limit : 0), twr[1]);
        if (twr[1] > +limit) twr[1] = +limit;
        if (twr[1] < -limit) twr[1] = -limit;
        return excess;
    }
}

void rMobile::TowerUD(float radians) {
    float e = radians;
    const float steplimit = 0.01 * M_PI;
    if (e > +steplimit) e = +steplimit;
    if (e < -steplimit) e = -steplimit;
    twr[0] += e;
    const float limit = 0.4 * M_PI;
    if (twr[0] > +limit) twr[0] = +limit;
    if (twr[0] < -limit) twr[0] = -limit;
}

void rMobile::animate(float spf) {
    if (!active) {
        drivethrottle = 0;
        jetthrottle = 0;
        return;
    }

    // Apply turret steering values and get exceeding over limit values.
    float excess_lr = TowerLR((-0.25f * M_PI) * tower_lr * spf);
    float excess_ud = 0; TowerUD((+0.25f * M_PI) * tower_ud * spf);
    if (!immobile) {
        // Smoothen steering and throttle values, adds heaviness.
        chassis_lr_ += 0.75f * (excess_lr / spf + (-0.25f * M_PI) * chassis_lr) - chassis_lr_;
        chassis_ud_ += 0.75f * (excess_ud / spf + (+0.25f * M_PI) * chassis_ud) - chassis_ud_;
        drivethrottle += 0.25f * (+fmax(-0.25f, fmin(-driveen, 1.0f)) - drivethrottle);
        jetthrottle   += 0.05f * (+fmax(-0.00f, fmin(   jeten, 1.0f)) - jetthrottle);
        // Apply steering values.
        ChassisLR(chassis_lr_ * spf);
        ChassisUD(chassis_ud_ * spf);
    } else {
        drivethrottle = 0;
        jetthrottle = 0;
    }

    // build yaw ori
    float yaw[] = {0, 1, 0};
    quat yaw_ori;
    quat_rotaxis(yaw_ori, bse[1], yaw);
    // build pitch ori
    float pitch[] = {1, 0, 0};
    quat pitch_ori;
    quat_rotaxis(pitch_ori, bse[0], pitch);
    // combine pitch, yaw
    quat_mul(ori, yaw_ori, pitch_ori);

    // yaw
    quat tower_ori;
    quat_set(tower_ori, 0, sin(0.5 * twr[1]), 0, cos(0.5 * twr[1]));
    // pitch
    quat tower_ori_v;
    quat_set(tower_ori_v, sin(0.5 * twr[0]), 0, 0, cos(0.5 * twr[0]));
    // combined pitch and yaw
    quat_mul(ori1, tower_ori, tower_ori_v);
}

void rMobile::transform() {
    return;
}

void rMobile::drawSolid() {
    return;
}

void rMobile::drawEffect() {
    /*
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        SGL::glUseProgram_fgaddcolor();

        // Draw colored base.
        glPushMatrix();
        {
            glTranslatef(traceable->pos[0], traceable->pos[1], traceable->pos[2]);
            SGL::glRotateq(traceable->ori);
            glTranslatef(0, +0.1, 0);
            glRotatef(90, 1, 0, 0);
            //glColor4f(0.1,0.1,0.1,0.9);
            //cPrimitives::glDisk(4, 1.3f);
            if (hasTag(RED)) glColor4f(1, 0, 0, 0.9);
            else if (hasTag(GREEN)) glColor4f(0, 1, 0, 0.9);
            else if (hasTag(BLUE)) glColor4f(0, 0, 1, 0.9);
            else glColor4f(1, 1, 0, 0.9);
            cPrimitives::glDisk(1, -1.3f);
        }
        glPopMatrix();
    }
    glPopAttrib();
    */
}

