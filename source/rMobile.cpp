#include "rMobile.h"

#include "cWorld.h"

#include "psi3d/macros.h"
#include "GL/glew.h"

#include <iostream>
using std::cout;
using std::endl;


rMobile::rMobile(cObject * obj) :
        jeten(0),
        driveen(0),
        immobile(false),
        chassis_lr(0),
        chassis_ud(0),
        tower_lr(0),
        tower_ud(0),
        aimtarget(0),
        firetarget(false),
        walktargetdist(0),
        jetthrottle(0),
        drivethrottle(0),
        drive_tgt(0),
        chassis_lr_(0),
        chassis_lr_tgt(0),
        chassis_ud_(0),
        chassis_ud_tgt(0),
        tower_lr_tgt(0),
        tower_ud_tgt(0),
        firetarget_tgt(false),
        aimrange(0),
        walkrange(0)
{
    role = "MOBILE";
    object = obj;
    vector_zero(pos0);
    vector_zero(twr);
    vector_zero(bse);
    quat_zero(ori0);
    quat_zero(ori1);
    vector_set(walktarget, float_NAN, float_NAN, float_NAN);
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
        float excess = copysign((fabs(twr[1]) > limit ? fabs(twr[1]) - limit : 0), twr[1]);
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

    // Determine Aim-Range
    {
        cObject* tgt = cWorld::instance->getObject(aimtarget);
        if (tgt == NULL) {
            aimrange = 10000.0f;
        } else {
            float d = vector_distance(pos0, tgt->pos);
            aimrange = d;//log(1.0f+d) / log(4);
        }
        aimrange = fmin(aimrange, 10000.0f);
    }

    // Determine Walk-Range
    {
        float d = vector_distance(pos0, walktarget);
        if (d != d) {
            walkrange = 10000.0f;
        } else {
            walkrange = d;//log(1.0f+d) / log(4);
        }
        walkrange = fmin(walkrange, 10000.0f);
    }

    if ((walkrange < 200.0f) || (aimrange < 200.0f)) {
        //cout << "aimrange " << aimrange << " \t walkrange " << walkrange << endl;
    }

    // Apply turret steering values and get exceeding over limit values.
    float excess_lr = TowerLR((-0.25f * M_PI) * tower_lr * spf);
    float excess_ud = 0;
    TowerUD((+0.25f * M_PI) * tower_ud * spf);
    if (!immobile) {
        // Smoothen steering and throttle values, adds heaviness.
        chassis_lr_ += 0.75f * (excess_lr / spf + (-0.25f * M_PI) * chassis_lr) - chassis_lr_;
        chassis_ud_ += 0.75f * (excess_ud / spf + (+0.25f * M_PI) * chassis_ud) - chassis_ud_;
        drivethrottle += 0.25f * (+fmax(-0.25f, fmin(-driveen, 1.0f)) - drivethrottle);
        jetthrottle += 0.05f * (+fmax(-0.00f, fmin(jeten, 1.0f)) - jetthrottle);
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
    quat_mul(ori0, yaw_ori, pitch_ori);

    // yaw
    quat tower_ori;
    quat_set(tower_ori, 0, sin(0.5 * twr[1]), 0, cos(0.5 * twr[1]));
    // pitch
    quat tower_ori_v;
    quat_set(tower_ori_v, sin(0.5 * twr[0]), 0, 0, cos(0.5 * twr[0]));
    // combined pitch and yaw
    quat_mul(ori1, tower_ori, tower_ori_v);

    // Determine nearest rotation direction for aim-target and tower
    {
        bool fire = false;
        cObject* tgt = NULL;
        float* target_pos = NULL;
        if (aimtarget != 0) {
            tgt = cWorld::instance->getObject(aimtarget);
            if (tgt != NULL) target_pos = tgt->pos;
        }
        if (target_pos == NULL) {
            // TODO: determind rotation to level head forward.
            tower_lr_tgt = 0;
            tower_ud_tgt = 0;
        } else {
            vec2 rd;
            cParticle::rotationTo(rd, pos0, target_pos, ori0, ori1);
            float lr = rd[0];// * 2;
            float ud = rd[1];
            tower_lr_tgt = lr;
            tower_ud_tgt = ud;
            //fire = (rand() % 100 <= 40 && fabs(rd[0]) < 0.5);
            fire = (fabs(rd[0]) < 0.5);
        }
        firetarget_tgt = fire && firetarget;
    }

    // Determine nearest rotation direction and throttle for walk-target and base
    {
        float* target_pos = NULL;
        if (finitef(walktarget[0])) {
            target_pos = walktarget;
        }
        if (target_pos == NULL) {
            chassis_lr_tgt = 0;
            chassis_ud_tgt = 0;
            drive_tgt = 0;
        } else {
            vec2 rd;
            cParticle::rotationTo(rd, pos0, target_pos, ori0);
            float lr = rd[0];// * 2;
            float ud = rd[1];
            chassis_lr_tgt = lr;
            chassis_ud_tgt = ud;
            {
                // Determine distance.
                float d = vector_distance(pos0, target_pos);
                // Throttle according to angle and distance.
                float towards = 1.0f * (1.0f - 0.7 * fabs(rd[0]));
                // FIXME: Need to parameterise target distance.
                //const float target_distance = 23;
                float target_distance = walktargetdist;
                float relative = (d - target_distance);
                float relative_clipped = fmin(+1.0f, fmax(-1.0f, relative));
                float throttle = -(relative_clipped * towards);
                drive_tgt = throttle;
            }
        }
    }
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

