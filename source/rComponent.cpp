#include "rComponent.h"

#include "psi3d/macros.h"
#include "cWorld.h"

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#include "psi3d/snippetsgl.h"
#include "psi3d/instfont.h"
DEFINE_glprintf



// NAMEABLE

#include "psi3d/snippetsgl.h"

rNameable::rNameable(cObject* obj) : name("Unnamed"), description("Undescribed"), designation(0) {
    role = "NAMEABLE";
    object = obj;
    vector_zero(pos0);
    vector_zero(pos1);
    quat_zero(ori0);
    quat_zero(ori1);
    color[0] = color[1] = color[2] = color[3] = 1.0f;
    effect = true;
}

rNameable::rNameable(rNameable * original) : name("Unnamed"), description("Undescribed"), designation(0) {
    role = "NAMEABLE";
    if (original != NULL) {
        object = original->object;
        name = original->name;
        description = original->description;
        designation = original->designation;
    }
}

rComponent* rNameable::clone() {
    return new rNameable(this);
}

void rNameable::drawEffect() {
    if (!effect) return;

    glPushMatrix();
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            SGL::glUseProgram_fgplaintexture();
            glColor4fv(color);

            float s = 0.75;
            //glTranslatef(pos[0], pos[1] + 0.5f*s, pos[2]);
            glTranslatef(pos0[0], pos0[1], pos0[2]);
            SGL::glRotateq(ori0);
            glTranslatef(pos1[0], pos1[1], pos1[2]);
            bool billboard = true;
            if (!billboard) SGL::glRotateq(ori1);
            glScalef(s, s, s);
            if (billboard) {
                float n[16];
                SGL::glGetTransposeInverseRotationMatrix(n);
                glMultMatrixf(n);
            }
            int l = name.length();
            glTranslatef(-l * 0.5f, 0, 0);
            glprintf(name.c_str());
        }
        glPopAttrib();
    }
    glPopMatrix();
}


// DAMAGEABLE


rDamageable::rDamageable(cObject* obj) {
    role = "DAMAGEABLE";
    object = obj;
    alife = true;
    loopi(MAX_PARTS) hp[i] = 100.0f;
    disturber = 0;
}

rDamageable::rDamageable(rDamageable * original) {
    role = "DAMAGEABLE";
    if (original == NULL) {
        rDamageable();
    } else {
        object = original->object;
        alife = original->alife;
        loopi(MAX_PARTS) hp[i] = original->hp[i];
        disturber = original->disturber;
    }
}

rComponent* rDamageable::clone() {
    return new rDamageable(this);
}

bool rDamageable::damage(int hitzone, float damage, cObject* enactor) {
    hp[hitzone] -= damage;
    if (hp[hitzone] < 0.0f) hp[hitzone] = 0.0f;
    if (hp[BODY] <= 0.0f) {
        alife = false;
    }
    if (enactor != NULL && damage > 0.0001f) {
        disturber = enactor->oid;
    }
    return alife;
}

void rDamageable::drawHUD() {
    if (!active) return;

    //glColor4f(0,0.1,0,0.2);
    glBegin(GL_QUADS);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    glColor4f(0.3, 0.3, 0.3, 0.3);
    for (int i = 1; i <= 5; i += 4) {
        if (i == 1) glColor4f(1, 1, 1, 0.9);
        glLineWidth(i);
        glPushMatrix();
        {
            //glLoadIdentity();
            glTranslatef(0.0f, 0.4f, 0.0f);
            glScalef(0.333f, 0.3f, 1.0f);
            //glTranslatef(0.8, 0.1, 0);
            //glScalef(0.06, 0.08, 1.0);
            // Left Arm
            int left = rDamageable::LEFT;
            if (i != 1) glColor4f(1 - object->damageable->hp[left]*0.01, object->damageable->hp[left]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Torsor&Head
            glTranslatef(1, 0, 0);
            glScalef(1, 1.5, 1);
            int body = rDamageable::BODY;
            if (i != 1) glColor4f(1 - object->damageable->hp[body]*0.01, object->damageable->hp[body]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Right Arm&Shoulder
            glTranslatef(1, 0, 0);
            glScalef(1, 1.0f / 1.5f, 1);
            int right = rDamageable::RIGHT;
            if (i != 1) glColor4f(1 - object->damageable->hp[right]*0.01, object->damageable->hp[right]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
            // Legs
            glTranslatef(-1.6, -1, 0);
            glScalef(2.2, 1, 1);
            int legs = rDamageable::LEGS;
            if (i != 1) glColor4f(1 - object->damageable->hp[legs]*0.01, object->damageable->hp[legs]*0.01, 0.4, 0.2);
            cPrimitives::glLineSquare(0.1f);
        }
        glPopMatrix();
    }

    glPushMatrix();
    {
        glColor4f(0.09, 0.99, 0.09, 1);
        glScalef(1.0f / 20.0f, 1.0f / 10.0f, 1.0f);
        glTranslatef(0, 1, 0);
        glprintf("hackcraft.de");
        glTranslatef(0, 9, 0);
        int left = rDamageable::LEFT;
        int body = rDamageable::BODY;
        int right = rDamageable::RIGHT;
        int legs = rDamageable::LEGS;
        glprintf("L %3.0f  T %3.0f  R %3.0f\n       B %3.0f", object->damageable->hp[left], object->damageable->hp[body], object->damageable->hp[right], object->damageable->hp[legs]);
    }
    glPopMatrix();
}


// CONTROLLED

#include "cPad.h"
#include "cController.h"

rControlled::rControlled(cObject* obj) : target(0), pad(new cPad()), controller(new cController(obj, true)) {
    role = "CONTROLLED";
    object = obj;
    vector_set(destination, float_NAN, float_NAN, float_NAN);
}

rControlled::rControlled(rControlled * original) {
    role = "CONTROLLED";
    if (original == NULL) {
        rControlled();
    } else {
        object = original->object;
        target = original->target;
        vector_cpy(destination, original->destination);
        pad = original->pad;
        controller = original->controller;
    }
}

rComponent* rControlled::clone() {
    return new rControlled(this);
}

void rControlled::animate(float spf) {
    if (!active) return;
    //cout << mPad->toString();

    if (controller->controllerEnabled) {
        pad->reset();
        controller->controlledDevice = object;
        controller->process();
    }
}


// TRACEABLE

#include "cWorld.h"

rTraceable::rTraceable(cObject* obj) : cParticle() {
    role = "TRACEABLE";
    object = obj;
    grounded = true;
    jetthrottle = 0.0f;
    throttle = 0.0f;
}

rTraceable::rTraceable(rTraceable * original) : cParticle(original) {
    role = "TRACEABLE";
    object = original->object;
    grounded = original->grounded;
    jetthrottle = original->jetthrottle;
    throttle = original->throttle;
}

rComponent* rTraceable::clone() {
    return new rTraceable(this);
}

void rTraceable::accumulate(float spf) {
    // Accumulate steering forces
    {
        float jetuwd =  130000.0f * jetthrottle;
        float jetfwd =  -80000.0f * throttle * jetthrottle;
        float whlfwd = -600000.0f * throttle;
        if (friction <= 0.01f) whlfwd = 0.0f;
        float force[] = {0, jetuwd, jetfwd + whlfwd};
        quat_apply(force, ori, force);
        vector_add(fce, fce, force);
    }

    // Accumulate environmental forces
    applyGravityForce(cWorld::instance->getGravity());
    applyFrictionForce(spf);
    applyAirdragForce(cWorld::instance->getAirdensity());
}

void rTraceable::integrate(float spf) {
    // Integrate position and estimate current velocity...
    stepVerlet(1.0f / spf, spf*spf, 0.001f, 0.99f);
}

void rTraceable::collide(float spf) {
    // Constraint position...
    float depth = 0;
    {
        float center[3];
        vector_cpy(center, pos);
        center[1] += radius;
        depth = cWorld::instance->constrainParticle(object, center, radius);
        if (depth > 0.0f) {
            // There was a collision.
            // Set current position to corrected position.
            vector_cpy(pos, center);
            pos[1] -= radius;

            float delta[3];
            vector_set3i(delta, pos[i] - old[i]);
            float dist = vector_mag(delta);
            bool infinite = !finitef(pos[0]) || !finitef(pos[1]) || !finitef(pos[2]);
            if (infinite) {
                // The new position is not feasible.
                // (Re-)Set position to a position higher than the old position.
                old[1] += 0.3f;
                vector_cpy(pos, old);
                vector_set(vel, 0,0,0);
            } else if (dist > 1.3f) {
                // Way too fast.
                // Reset position.
                vector_cpy(old, pos);
            } else if (dist > 1.0f) {
                // Just too fast.
                // Rescale (normalize) movement.
                vector_scale(delta, delta, 1.0f / dist);
                vector_add(pos, old, delta);
                vector_cpy(vel, delta);
            }
        }
    }
    // Assume ground contact if there was a collision.
    float onground = (depth > 0.0f) ? 1.0f : 0.0f;
    // Average groundedness to decide on friction.
    grounded += 0.1 * (onground - grounded);
    // Set friction for next frame.
    friction = ((grounded > 0.1)?1.0f:0.0f) * cWorld::instance->getGndfriction();
}

void rTraceable::animate(float spf) {
    accumulate(spf);
    integrate(spf);
    collide(spf);
}



// GROUPING

rGrouping::rGrouping(cObject* obj) : name("Unnamed") {
    role = "GROUPING";
    object = obj;
}

rGrouping::rGrouping(rGrouping * original) : name("Unnamed") {
    role = "GROUPING";
    if (original != NULL) {
        object = original->object;
        name = original->name;
        members.clear();
        //mMembers.insert(mMembers.begin(), original->mMembers.begin(), original->mMembers.end());
    }
}

rComponent* rGrouping::clone() {
    return new rGrouping(this);
}


// -------------------------------------------------------------------


#define DRAWJOINTS !true

void rRigged::drawBones() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        SGL::glUseProgram_fgplaincolor();

        loopi(model->numJoints) {
            glPushMatrix();
            {
                glTranslatef(joints[i].v[0], joints[i].v[1], joints[i].v[2]);
                SGL::glRotateq(joints[i].q);
                cPrimitives::glAxis(0.7);
            }
            glPopMatrix();
            glColor3f(0.5, 0.5, 0.5);
            glBegin(GL_LINES);
            {
                int j = joints[i].parent;
                if (j >= 0) {
                    glVertex3fv(joints[i].v);
                    glVertex3fv(joints[j].v);
                }
            }
            glEnd();
        }
    }
    glPopAttrib();
}

void rRigged::drawMeshes() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        SGL::glUseProgram_fglittexture3d();
        glColor4f(1, 1, 1, 1);
        glFrontFace(GL_CW);

        // Get first mesh of model to iterate through.
        MD5Format::mesh* msh = MD5Format::getFirstMesh(model);

        // Bounding box min and max corner.
        // Bounding box dimensions are model orientation dependent...
        /*
        const float e = 100000.0f;
        vec3 mins = { +e, +e, +e };
        vec3 maxs = { -e, -e, -e };
        */
        // Reset cylindrical dimensions.
        radius = 0;
        height = 0;

        loopi(model->numMeshes) {
            //cout << curr->numverts << " " << curr->numtris << " " << curr->numweights << endl;
            float* vtx = new float[msh->numverts * 3];
            float* nrm = new float[msh->numverts * 3];
            MD5Format::animatedMeshVertices(msh, joints, vtx, nrm);
            MD5Format::tri* tris = MD5Format::getTris(msh);
            //MD5Format::vert* verts = MD5Format::getVerts(msh);
            // For 3d texturing.
            float* vox = baseverts[i];
            //
            glBegin(GL_TRIANGLES);
            const float s = 0.25f;
            for (int j = 0; j < msh->numtris; j++) {
                int k = tris[j].a;
                //glTexCoord2fv(verts[k].tmap);
                //glTexCoord3fv(&vox[3 * k]);
                glNormal3fv(&nrm[3 * k]);
                glTexCoord3f(vox[3 * k+0]*s, vox[3 * k+1]*s, vox[3 * k+2]*s);
                glVertex3fv(&vtx[3 * k]);
                k = tris[j].b;
                //glTexCoord2fv(verts[k].tmap);
                //glTexCoord3fv(&vox[3 * k]);
                glNormal3fv(&nrm[3 * k]);
                glTexCoord3f(vox[3 * k+0]*s, vox[3 * k+1]*s, vox[3 * k+2]*s);
                glVertex3fv(&vtx[3 * k]);
                k = tris[j].c;
                //glTexCoord2fv(verts[k].tmap);
                //glTexCoord3fv(&vox[3 * k]);
                glNormal3fv(&nrm[3 * k]);
                glTexCoord3f(vox[3 * k+0]*s, vox[3 * k+1]*s, vox[3 * k+2]*s);
                glVertex3fv(&vtx[3 * k]);
            }
            glEnd();
            // Calculate boundaries of current pose.
            {
                int nvtx = msh->numverts;
                loopj(nvtx) {
                    float* v = &vtx[3*j];
                    /*
                    loopk(3) {
                        mins[k] = (v[k] < mins[k]) ? v[k] : mins[k];
                        maxs[k] = (v[k] > maxs[k]) ? v[k] : maxs[k];
                    }
                    */
                    float r = v[0]*v[0] + v[2]*v[2];
                    radius = (r > radius) ? r : radius;
                    height = v[1] > height ? v[1] : height;
                }
            }
            //
            delete vtx;
            delete nrm;
            // Process next mesh of model in next iteration.
            msh = MD5Format::getNextMesh(msh);
        }
        radius = sqrt(radius);
        //cout << " Model-Dimensions: r = " << radius << " h = " << height << "\n";
        //cout << " Model-Dimensions: (" << mins[0] << " " << mins[1] << " " << mins[2] << ") (" << maxs[0] << " " << maxs[1] << " " << maxs[2] << ")\n";
    }
    glPopAttrib();
}

void rRigged::poseJumping(float spf) {
    // Hanging Legs
    float one = PI_OVER_180;
    const float a = -30.0f * one;
    const float b = -60.0f * one;
    const float s = +30.0f * one;
    const float f = b / a;

    if (rotators[LEFTLEG][0] > -a) rotators[LEFTLEG][0] -= s * spf;
    if (rotators[LEFTLEG][0] < -a + one) rotators[LEFTLEG][0] += s * spf;

    if (rotators[LEFTCALF][0] > +b) rotators[LEFTCALF][0] -= f * s * spf;
    if (rotators[LEFTCALF][0] < +b + one) rotators[LEFTCALF][0] += f * s * spf;

    if (rotators[RIGHTLEG][0] > -a) rotators[RIGHTLEG][0] -= s * spf;
    if (rotators[RIGHTLEG][0] < -a + one) rotators[RIGHTLEG][0] += s * spf;

    if (rotators[RIGHTCALF][0] > +b) rotators[RIGHTCALF][0] -= f * s * spf;
    if (rotators[RIGHTCALF][0] < +b + one) rotators[RIGHTCALF][0] += f * s * spf;
}

void rRigged::poseRunning(float spf) {
    // Animate legs according to real forward velocity.
    vec3 v;
    if (vel != NULL && ori != NULL) {
        quat ori_inv;
        quat_cpy(ori_inv, ori);
        quat_conj(ori_inv);
        quat_apply(v, ori_inv, vel);
    } else if (vel != NULL){
        vector_cpy(v, vel);
    } else {
        vector_zero(v);
    }

    float fwdvel = copysign( sqrtf(v[0] * v[0] + v[2] * v[2]), v[2] ) * -0.16f;
    if (fwdvel > 2.8) fwdvel = 2.8;
    if (fwdvel < -1.0) fwdvel = -1.0;

    // This is full of hand crafted magic numbers and magic code don't ask...
    const float e = 0.017453;
    float o = e * -60;
    seconds += e * spf * 180.0f * (1.0f + 0.2 * fabs(fwdvel));
    float t = seconds;

    float l1 = 25 * +sin(t) * fwdvel;
    float l2 = 15 * (+sin(copysign(t, fwdvel) + o) + 1) * fabs(fwdvel);
    float l3 = 10 * +cos(t) * fwdvel;

    float r1 = 20 * -sin(t) * fwdvel;
    float r2 = 20 * (-sin(copysign(t, fwdvel) + o) + 1) * fabs(fwdvel);
    float r3 = 15 * -cos(t) * fwdvel;

    bool bird = false;
    if (model != NULL) {
        if (MD5Format::findJoint(model, "BIRD") >= 0) {
            bird = true;
        }
    }
    float bf = bird ? -1.0f : +1.0f;

    float l1scale = 0.65;
    if (bf*l1 > 0) l1 *= l1scale;
    if (bf*r1 > 0) r1 *= l1scale;

    float l2scale = 0.45;
    if (bf*l2 < 0) l2 *= l2scale;
    if (bf*r2 < 0) r2 *= l2scale;

    float l3scale = 0.25;
    if (bf*l3 > 0) l3 *= l3scale;
    if (bf*r3 > 0) r3 *= l3scale;

    float s = PI_OVER_180;
    rotators[LEFTLEG][0] = -l1 * s;
    rotators[LEFTCALF][0] = -l2 * s;
    rotators[LEFTFOOT][0] = -l3 * s;
    rotators[RIGHTLEG][0] = -r1 * s;
    rotators[RIGHTCALF][0] = -r2 * s;
    rotators[RIGHTFOOT][0] = -r3 * s;
}

void rRigged::transformJoints() {
    MD5Format::joint* manipulators = new MD5Format::joint[model->numJoints];
    assert(manipulators != NULL);

    loopi(model->numJoints) {
        quat_set(manipulators[i].q, 0, 0, 0, -1);
        vector_set(manipulators[i].v, 0, 0, 0);
    }

    { // set joint manipulators
        float xaxis[] = {-1, 0, 0};
        float yaxis[] = {0, -1, 0};
        //float zaxis[] = {0, 0, -1};

        int yaw_idx = jointpoints[YAW];
        int pitch_idx = jointpoints[PITCH];
        if (yaw_idx < 0) yaw_idx = pitch_idx;

        if (0) {
            quat qy;
            quat_rotaxis(qy, +1 * M_PI, yaxis);
            quat_cpy(manipulators[0].q, qy);
        }

        if (yaw_idx == pitch_idx && pitch_idx >= 0) {
            quat qy;
            quat_rotaxis(qy, rotators[YAW][1], yaxis);
            quat qx;
            quat_rotaxis(qx, rotators[PITCH][0], xaxis);
            quat q;
            quat_mul(q, qy, qx);
            quat_cpy(manipulators[pitch_idx].q, q);
        } else {
            if (yaw_idx >= 0) {
                quat qy;
                quat_rotaxis(qy, rotators[YAW][1], yaxis);
                quat_cpy(manipulators[yaw_idx].q, qy);
            }
            if (pitch_idx >= 0) {
                quat qx;
                quat_rotaxis(qx, rotators[PITCH][0], xaxis);
                quat_cpy(manipulators[pitch_idx].q, qx);
            }
        }

        if (true) {
            quat qx;
            quat_rotaxis(qx, rotators[HEADPITCH][0], xaxis);
            int jidx = jointpoints[HEADPITCH];
            if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
        }

        bool left = true;
        if (left) {
            quat qx;
            quat_rotaxis(qx, rotators[LEFTLEG][0], xaxis);
            int jidx = jointpoints[LEFTLEG];
            if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
        }
        if (left) {
            quat qx;
            quat_rotaxis(qx, rotators[LEFTCALF][0], xaxis);
            int jidx = jointpoints[LEFTCALF];
            if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
        }
        if (left) {
            quat qx;
            quat_rotaxis(qx, rotators[LEFTFOOT][0], xaxis);
            int jidx = jointpoints[LEFTFOOT];
            if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
        }
        bool right = true;
        if (right) {
            quat qx;
            quat_rotaxis(qx, rotators[RIGHTLEG][0], xaxis);
            int jidx = jointpoints[RIGHTLEG];
            if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
        }
        if (right) {
            quat qx;
            quat_rotaxis(qx, rotators[RIGHTCALF][0], xaxis);
            int jidx = jointpoints[RIGHTCALF];
            if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
        }
        if (right) {
            quat qx;
            quat_rotaxis(qx, rotators[RIGHTFOOT][0], xaxis);
            int jidx = jointpoints[RIGHTFOOT];
            if (jidx >= 0) quat_cpy(manipulators[jidx].q, qx);
        }
    } // set joint manipulators

    // Transform local skeleton using manipulators to global skeleton.
    MD5Format::joint* joints_orig = MD5Format::getJoints(model);
    MD5Format::toGlobalJoints(model->numJoints, joints_orig, joints, manipulators);
    delete manipulators;
}

void rRigged::loadModel(std::string filename) {
    model = MD5Format::mapMD5Mesh(filename.c_str());

    if (!true) cout << MD5Format::getModelStats(model) << endl;

    // Make joints local for later animation and transformation to global.
    MD5Format::joint* joints_ = MD5Format::getJoints(model);
    MD5Format::toLocalJoints(model->numJoints, joints_, joints_);

    // Rotate around X to swap Y/Z.
    quat convaxes;
    vec3 xaxis = {1,0,0};
    quat_rotaxis(convaxes, (-0.5f * M_PI), xaxis);
    quat_mul(joints_[0].q, joints_[0].q, convaxes);

    // FIXME: Rotate yaw 180 to make frontfacing.
    if (true) {
        quat convfacing;
        vec3 yaxis = {0,1,0};
        quat_rotaxis(convfacing, M_PI, yaxis);
        quat_mul(joints_[0].q, joints_[0].q, convfacing);
    }

    // Allocate space for animated global joints.
    joints = new MD5Format::joint[model->numJoints];
    memcpy(joints, joints_, sizeof (MD5Format::joint) * model->numJoints);

    loopi(MAX_JOINTPOINTS) {
        jointpoints[i] = MD5Format::findJoint(model, getJointname(i).c_str());
        //if (jointpoints[i] < 0) jointpoints[i] = MD5Format::findJoint(model, getJointname2(i).c_str());
    }

    // Generate base vertices and normals for 3d-tex-coords:

    if (baseverts.empty()) {
        MD5Format::mesh* msh = MD5Format::getFirstMesh(model);
        MD5Format::joint* staticjoints = MD5Format::getJoints(model);

        loopi(model->numMeshes) {
            float* cur_baseverts = new float[msh->numverts * 3];
            float* cur_basenorms = new float[msh->numverts * 3];
            MD5Format::animatedMeshVertices(msh, staticjoints, cur_baseverts, cur_basenorms);
            baseverts[i] = cur_baseverts;
            basenorms[i] = cur_basenorms;
            // Recalculate normals, invert and set as normal-weights.
            {
                MD5Format::tri* tris = MD5Format::getTris(msh);
                // TODO: Normals need to be averaged/smoothed.
                for (int j = 0; j < msh->numtris; j++) {
                    float* a = &cur_baseverts[3*tris[j].a];
                    float* b = &cur_baseverts[3*tris[j].b];
                    float* c = &cur_baseverts[3*tris[j].c];
                    float ab[3];
                    float bc[3];
                    float n[3];
                    vector_sub(ab, b, a);
                    vector_sub(bc, c, b);
                    vector_cross(n, bc, ab);
                    vector_norm(n, n);
                    vector_cpy(&cur_basenorms[3*tris[j].a], n);
                    vector_cpy(&cur_basenorms[3*tris[j].b], n);
                    vector_cpy(&cur_basenorms[3*tris[j].c], n);
                }
                MD5Format::unanimatedMeshNormals(msh, staticjoints, cur_basenorms);
            }
            msh = MD5Format::getNextMesh(msh);
        }
    }
}

void rRigged::animate(float spf) {
    if (!active) return;
    if (model == NULL) return;
    if (grounded > 0.15f) {
        poseRunning(spf);
    } else {
        poseJumping(spf);
    }
}

void rRigged::transform() {
    if (!active) return;
    if (model == NULL) return;
    transformJoints();
    //transformMounts();
}

void rRigged::drawSolid() {
    if (!active) return;
    if (model == NULL) return;
    glPushMatrix();
    {
        glTranslatef(pos[0], pos[1], pos[2]);
        SGL::glRotateq(ori);
        //cPrimitives::glAxis(3.0f);
        if (basetexture3d > 0) {
            glBindTexture(GL_TEXTURE_3D, basetexture3d);
        }
        drawMeshes();
    }
    glPopMatrix();
}

void rRigged::drawEffect() {
    if (!active) return;
    if (model == NULL) return;
    if (DRAWJOINTS) {
        glPushMatrix();
        {
            glTranslatef(pos[0], pos[1], pos[2]);
            SGL::glRotateq(ori);
            drawBones();
        }
        glPopMatrix();
    }
    // Draw jumpjet exaust if jet is somewhat on.
    if (jetting > 0.3f) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            SGL::glUseProgram_fgaddcolor();

            //std::map<int, int>& jointpoints = rigged->jointpoints;
            int jet[5];
            jet[0] = jointpoints[JET0];
            jet[1] = jointpoints[JET1];
            jet[2] = jointpoints[JET2];
            jet[3] = jointpoints[JET3];
            jet[4] = jointpoints[JET4];

            loopi(5) {
                if (jet[i] >= 0) {
                    float* v = joints[jet[i]].v;
                    glPushMatrix();
                    {
                        float f = jetting * 0.5f;

                        glTranslatef(pos[0], pos[1], pos[2]);
                        SGL::glRotateq(ori);
                        glTranslatef(v[0], v[1], v[2]);

                        float n[16];
                        SGL::glGetTransposeInverseRotationMatrix(n);
                        glMultMatrixf(n);

                        glColor4f(1, 1, 0.3, 0.6);
                        cPrimitives::glDisk(7, f + 0.0003 * (rand() % 100));
                        glColor4f(1, 0.5, 0.3, 0.7);
                        cPrimitives::glDisk(7, f * 0.6 + 0.001 * (rand() % 100));
                        glColor4f(1, 1, 1, 0.8);
                        cPrimitives::glDisk(7, f * 0.3 + 0.001 * (rand() % 100));
                    }
                    glPopMatrix();
                } // if
            } // loopi
        }
        glPopAttrib();
    }
}


// -------------------------------------------------------------------

#define grand() ((rand()%100 + rand()%100 + rand()%100 + rand()%100 + rand()%100) * 0.01f * 0.2f - 0.5f)

rCamera::rCamera(cObject * obj) : camerastate(1), cameraswitch(0), camerashake(0), firstperson(true) {
    object = obj;
    role = "CAMERA";

    quat_zero(ori1);
    vector_zero(pos1);
    quat_zero(ori0);
    vector_zero(pos0);
}

void rCamera::camera() {
    float cam[16];
    float rot_inv[16];
    float pos_inv[16];

    float intensity = 0.0 + pow(camerashake, 1.5f);
    float shake = 0.004f * intensity;
    float jerk = 0.95f * intensity;

    glPushMatrix();
    {
        // Get inverse components of head pos and ori.
        glLoadIdentity();
        glTranslatef(pos0[0], pos0[1], pos0[2]);
        SGL::glRotateq(ori0);
        glTranslatef(pos1[0], pos1[1], pos1[2]);
        SGL::glRotateq(ori1);
        // FIXME: Camera forward is inverted, therefore rotate.
        glRotatef(180, 0,1,0);

        SGL::glGetTransposeInverseRotationMatrix(rot_inv);
        SGL::glGetInverseTranslationMatrix(pos_inv);
        // Compose Camera Matrix from inverse components
        glLoadIdentity();
        glMultMatrixf(rot_inv);
        glRotatef(grand()*jerk, 1,0,0);
        glRotatef(grand()*jerk, 0,1,0);
        glRotatef(grand()*jerk, 0,0,1);
        glMultMatrixf(pos_inv);
        glTranslatef(grand()*shake, grand()*shake, grand()*shake);
        glGetFloatv(GL_MODELVIEW_MATRIX, cam);
        //matrix_print(cam);
    }
    glPopMatrix();

    int cs = abs(camerastate);
    if (cs == 1) { // 1st
        glMultMatrixf(cam);
    } else if (cs == 2) { // 3rd
        glTranslatef(0, 0, -5);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(cam);
    } else if (cs == 3) { // 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(cam);
    } else if (cs == 4) { // Reverse 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glMultMatrixf(cam);
    } else if (cs == 5) { // Map Near
        glTranslatef(0, 0, -50);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } else if (cs == 6) { // Map Far
        glTranslatef(0, 0, -100);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } // if camerastate
}

void rCamera::animate(float spf) {
    if (cameraswitch) {
        // Only if Camera State is not transitional
        // then switch to next perspective on button press.
        if (camerastate > 0) {
            camerastate = (1 + (camerastate % MAX_CAMERAMODES));
            camerastate *= -1; // Set transitional.
        }
    } else {
        // Set Camera State as fixed.
        camerastate = abs(camerastate);
    }
    firstperson = (abs(camerastate) == 1);
}


// -------------------------------------------------------------------

rMobile::rMobile(cObject * obj) : jeten(0), jetthrottle(0), driveen(0), drivethrottle(0), immobile(false), chassis_lr(0), chassis_lr_(0), chassis_ud(0), chassis_ud_(0), tower_lr(0), tower_ud(0) {
    role = "MOBILE";
    object = obj;
    vector_zero(pos);
    vector_zero(twr);
    vector_zero(bse);
    quat_zero(ori);
    quat_zero(ori1);
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


// -------------------------------------------------------------------


rCollider::rCollider(cObject * obj) {
    object = obj;
    role = "COLLIDER";

    quat_zero(ori);
    vector_zero(pos);
    radius = 0.1f;
    ratio = 0.0f;
    height = 0.1f;
}

float rCollider::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float localpos_[3];
    { // Transform to local.
        quat ori_inv;
        quat_cpy(ori_inv, this->ori);
        quat_conj(ori_inv);

        vector_cpy(localpos_, worldpos);
        vector_sub(localpos_, localpos_, this->pos);
        quat_apply(localpos_, ori_inv, localpos_);
    }

    float base[3] = {0, -0.0 - radius, 0};
    float radius_ = this->radius + radius;
    float height = this->height + 2 * radius;
    float localprj[3] = { 0, 0, 0 };

    float depth = 0;
    depth = cParticle::constraintParticleByCylinder(localpos_, base, radius_, height, localprj);
    //depth = cParticle::constraintParticleByCone(localpos_, base, radius_, height, localprj);

    if (depth <= 0) return 0;

    if (localpos != NULL) vector_cpy(localpos, localprj);

    { // Transform to global.
        quat_apply(worldpos, this->ori, localprj);
        vector_add(worldpos, worldpos, this->pos);
    }

    return depth;
}

void rCollider::animate(float spf) {
    height = (ratio > 0) ? radius * ratio : height;
}

void rCollider::drawEffect() {
    return; // !!

    glPushMatrix();
    {
        glTranslatef(pos[0], pos[1], pos[2]);
        SGL::glRotateq(ori);

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            SGL::glUseProgram_fgplaincolor();

            cPrimitives::glAxis(1.0f);

            float c1[] = { 1.0f, 0.0f, 0.0f, 1.0f };
            float c2[] = { 0.5f, 0.0f, 0.0f, 1.0f };
            vec3 range = { radius, height*0.5f, radius };
            vec3 center = { 0, height*0.5f, 0 };
            // Draw cylinder.
            {
                float a = 0;
                const int n = 12;
                const float a_inc = M_PI * 2.0 / (double)n;
                // XZ-Bottom-Plane
                glColor4fv(c1);
                a = 0;
                glBegin(GL_LINE_STRIP);
                loopi(n+1) {
                    float rx = sin(a)*range[0];
                    float rz = cos(a)*range[2];
                    a += a_inc;
                    glVertex3f(center[0]+rx,center[1]-range[1],center[2]+rz);
                }
                glEnd();
                // XZ-Top-Plane
                glColor4fv(c1);
                a = 0;
                glBegin(GL_LINE_STRIP);
                loopi(n+1) {
                    float rx = sin(a)*range[0];
                    float rz = cos(a)*range[2];
                    a += a_inc;
                    glVertex3f(center[0]+rx,center[1]+range[1],center[2]+rz);
                }
                glEnd();
                // Sides
                glColor4fv(c2);
                a = 0;
                glBegin(GL_LINES);
                loopi(n) {
                    float rx = sin(a)*range[0];
                    float rz = cos(a)*range[2];
                    a += a_inc;
                    glVertex3f(center[0]+rx,center[1]-range[1],center[2]+rz);
                    glVertex3f(center[0]+rx,center[1]+range[1],center[2]+rz);
                }
                glEnd();
                //break;
            }
        }
        glPopAttrib();
    }
    glPopMatrix();
}

