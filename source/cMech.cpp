// cMech.cpp

#include "cMech.h"

#include "cWorld.h"

#include "cController.h"
#include "cPad.h"
#include "psi3d/macros.h"
#include "psi3d/snippetsgl.h"

#include "cSolid.h"

#include <cassert>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#define MECHDETAIL -1

//#include "psi3d/instfont.h"
//DEFINE_glprintf

#define EXPERIMENT true

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


// -------------------------------------------------------------------


int cMech::sInstances = 0;
std::map<int, long> cMech::sTextures;


cMech::cMech(float* pos, float* rot) {
    sInstances++;
    if (sInstances == 1) {
        if (1) {
            //registerRole(new rMobile((cObject*)NULL), FIELDOFS(mobile), ROLEPTR(cMech::mobile));
            //registerRole(new rRigged((cObject*)NULL), FIELDOFS(rigged), ROLEPTR(cMech::rigged));
            //registerRole(new rComputerised((cObject*)NULL), FIELDOFS(computerised), ROLEPTR(cMech::computerised));

            cout << "Generating Camoflage..." << endl;

            const int SIZE = 1<<(7+MECHDETAIL);
            unsigned char* texels = new unsigned char[SIZE * SIZE * SIZE * 3];

            for (int l = 0; l < 4; l++) {
                long t = 0;
                loopijk(SIZE, SIZE, SIZE) {
                    float color[16];
                    const float f = 0.25f*0.25f * 64.0f / SIZE;
                    float x = f*i, y = f*j, z = f*k;
                    switch(l) {
                        //case TEXTURE_WOOD: cSolid::camo_wood(x, y, z, color); break;
                        case TEXTURE_WOOD: cSolid::camo_rust(x, y, z, color); break;
                        case TEXTURE_URBAN: cSolid::camo_urban(x, y, z, color); break;
                        case TEXTURE_DESERT: cSolid::camo_desert(x, y, z, color); break;
                        case TEXTURE_SNOW: cSolid::camo_snow(x, y, z, color); break;
                        default:
                            cSolid::camo_rust(x, y, z, color);
                    }
                    texels[t++] = 255.0f * color[0];
                    texels[t++] = 255.0f * color[1];
                    texels[t++] = 255.0f * color[2];
                }
                unsigned int texname = SGL::glBindTexture3D(0, true, true, true, true, true, SIZE, SIZE, SIZE, texels);
                sTextures[l] = texname;
            }
            delete texels;
        }
    }

    collider = new rCollider(this);
    rigged = new rRigged(this);
    damageable = new rDamageable(this);
    controlled = new rControlled(this);
    nameable = new rNameable(this);
    traceable = new rTraceable(this);
    camera = new rCamera(this);
    mobile = new rMobile(this);

    comcom = new rComcom(this);
    tarcom = new rTarcom(this);
    wepcom = new rWepcom(this);
    forcom = new rForcom(this);
    navcom = new rNavcom(this);

    if (rot != NULL) vector_scale(mobile->bse, rot, PI_OVER_180);

    if (pos != NULL) vector_cpy(traceable->pos, pos);
    vector_cpy(traceable->old, traceable->pos);
    float axis[] = {0, 1, 0};
    quat_rotaxis(traceable->ori, mobile->bse[1], axis);

    // Mech Speaker
    if (0) {
        try {
            ALuint buffer;
            //buffer = alutCreateBufferHelloWorld();
            buffer = alutCreateBufferFromFile("data/base/device/pow.wav");
            //if (buffer == AL_NONE) throw "could not load pow.wav";
            ALuint* soundsource = &traceable->sound;
            alGenSources(1, soundsource);
            if (alGetError() == AL_NO_ERROR && alIsSource(*soundsource)) {
                alSourcei(*soundsource, AL_BUFFER, buffer);
                alSourcef(*soundsource, AL_PITCH, 1.0f);
                alSourcef(*soundsource, AL_GAIN, 1.0f);
                alSourcefv(*soundsource, AL_POSITION, traceable->pos);
                alSourcefv(*soundsource, AL_VELOCITY, traceable->vel);
                alSourcei(*soundsource, AL_LOOPING, AL_FALSE);
            }
        } catch (...) {
            cout << "Sorry, no mech sound possible.";
        }
    }

    do_moveFor(NULL);

    try {
        int mod = (9+rand())%8; // fr
        //int mod = (13+rand())%8; // le
        const char* fns[] = {
            //"/media/44EA-7693/workspaces/mm3d/soldier/soldier.md5mesh",
            "data/base/wanzers/frogger/frogger.md5mesh",
            "data/blendswap.com/flopsy/flopsy.md5mesh",
            "data/base/wanzers/gorilla/gorilla_ii.md5mesh",
            "data/opengameart.org/scorpion/scorpion.md5mesh",
            "data/base/wanzers/lemur/lemur.md5mesh",
            "data/base/tanks/bug/bug.md5mesh",
            "data/base/tanks/ant/ant.md5mesh",
            "data/base/wanzers/kibitz/kibitz.md5mesh",
            //"data/base/tanks/pod/pod.md5mesh"
        };
        rigged->loadModel(string(fns[mod]));
    } catch (const char* s) {
        cout << "CATCHING: " << s << endl;
        rigged->loadModel(string("data/base/wanzers/frogger/frogger.md5mesh"));
    }

    traceable->radius = 1.75f;
    traceable->cwm2 = 0.4f /*very bad cw*/ * traceable->radius * traceable->radius * M_PI /*m2*/;
    traceable->mass = 11000.0f; // TODO mass is qubic to size.
    traceable->mass_inv = 1.0f / traceable->mass;

    vector_cpy(this->pos, traceable->pos);
    quat_cpy(this->ori, traceable->ori);
    this->radius = traceable->radius;

    vector_cpy(collider->pos, this->pos);
    quat_cpy(collider->ori, this->ori);
    collider->radius = this->radius;

    explosion = new rWeaponExplosion(this);
    mountWeapon((char*) "CTorsor", explosion, false);
}

cMech::~cMech() {
    // FIXME: delete all components.
    if (sInstances == 1) {
        // Last one cleans up

    }
    sInstances--;
}

void cMech::message(cMessage* message) {
    cout << "I (" << this->oid << ":" << this->name << ") just received: \"" << message->getText() << "\" from sender " << message->getSender() << endl;
    forcom->message(message);
}

void cMech::onSpawn() {
    //cout << "cMech::onSpawn()\n";
    ALuint soundsource = traceable->sound;
    if (hasTag(HUMANPLAYER) && alIsSource(soundsource)) alSourcePlay(soundsource);
    //cout << "Mech spawned " << oid << "\n";
}

void cMech::multEyeMatrix() {
    camera->camera();
}

void cMech::setAsAudioListener() {
    float s = -0.1;
    float step = s * cWorld::instance->getTiming()->getSPF();
    float vel[] = {
        traceable->vel[0] * step,
        traceable->vel[1] * step,
        traceable->vel[2] * step
    };
    float pos[] = {
        traceable->pos[0], traceable->pos[1], traceable->pos[2]
    };
    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_VELOCITY, vel);
    vec3 fwd = { 0, 0, +1 };
    quat_apply(fwd, rigged->ori, fwd);
    vec3 uwd = { 0, -1, 0 };
    quat_apply(uwd, rigged->ori, uwd);
    float at_and_up[] = {
        fwd[0], fwd[1], fwd[2],
        uwd[0], uwd[1], uwd[2]
    };
    alListenerfv(AL_ORIENTATION, at_and_up);
}

void cMech::mountWeapon(char* point, rWeapon *weapon, bool add) {
    if (weapon == NULL) throw "Null weapon for mounting given.";
    weapon->weaponMount = rigged->getMountpoint(point);
    //weapon->weaponBasefv = rigged->getMountMatrix(point);
    weapon->object = this;
    if (add) {
        weapons.push_back(weapon);
#if 0
        int n = weapons.size();
        loopi(n) {
            weapons[i]->triggeren = false;
            weapons[i]->triggered = false;
            weapons[i]->triggereded = false;
        }
        weapon->triggeren = true;
        weapon->triggered = true;
        weapon->triggereded = true;
#endif
    }
}

#if 0
void cMech::fireCycleWeapons() {
    cout << "fireCycleWeapons()\n";
    int n = weapons.size();
    loopi(n) {
        cout << "W " << i << ", triggeren = " << weapons[i]->triggeren << ", triggered = " << weapons[i]->triggered << ", triggereded = " << weapons[i]->triggereded << endl;
        bool selfdone = weapons[i]->triggered && weapons[i]->triggereded;
        bool otherdone = weapons[(i+1)%n]->triggered;
        weapons[i]->triggeren ^= selfdone;
        weapons[i]->triggeren |= otherdone;
        weapons[i]->triggered ^= selfdone;
        weapons[i]->triggereded ^= selfdone;
        weapons[i]->target = target;
        weapons[i]->trigger = true;
    }
    loopi(n) {
        cout << "W'" << i << ", triggeren = " << weapons[i]->triggeren << ", triggered = " << weapons[i]->triggered << ", triggereded = " << weapons[i]->triggereded << endl;
    }
}
#else
void cMech::fireCycleWeapons() {
    if (weapons.size() == 0) return;
    currentWeapon %= weapons.size();
    fireWeapon(currentWeapon);
    currentWeapon++;
    currentWeapon %= weapons.size();
}
#endif

void cMech::fireAllWeapons() {

    loopi(weapons.size()) {
        weapons[i]->trigger = true;
    }
}

void cMech::fireWeapon(unsigned n) {
    if (n >= weapons.size()) return;
    weapons[n]->trigger = true;
}


void cMech::animate(float spf) {
    // Read in.
    vector_cpy(this->pos, traceable->pos);
    quat_cpy(this->ori, traceable->ori);
    this->radius = traceable->radius;

    /* Index of Component order
     * ------------------------
     * COLLIDER
     * DAMAGEABLE
     * COMPUTERs
     * CONTROLLED
     * MOBILE
     * TRACEABLE
     * RIGGED
     * NAMEABLE
     * CAMERA
     * WEAPON
     * EXPLOSION
     */

    // COLLIDER
    {
        // from SELF:
        {
            vector_cpy(collider->pos, this->pos);
            quat_cpy(collider->ori, this->ori);
            collider->radius = this->radius;
        }
        // from RIGGED:
        {
            collider->radius = rigged->radius;
            collider->ratio = 0.0f;
            collider->height = rigged->height;
        }

        collider->animate(spf);
    }

    // DAMAGEABLE
    {
        // from DAMAGEABLE
        {
            damageable->active = damageable->alife;
        }
        damageable->animate(spf);
    }

    // begin COMPUTERs -->

    // COMCOM
    {
        // from DAMAGEABLE
        {
            comcom->active = damageable->alife;
        }
        comcom->animate(spf);
    }

    // TARCOM
    {
        // from CONTROLLED
        {
            tarcom->switchnext = controlled->pad->getButton(cPad::MECH_NEXT_BUTTON);
            tarcom->switchprev = controlled->pad->getButton(cPad::MECH_PREV_BUTTON);
        }
        // from DAMAGEABLE
        {
            tarcom->active = damageable->alife;
        }
        // from TRACEABLE
        {
            vector_cpy(tarcom->pos, traceable->pos);
            quat_cpy(tarcom->ori, traceable->ori);
        }
        tarcom->animate(spf);
    }

    // WEPCOM
    {
        // from DAMAGEABLE
        {
            wepcom->active = damageable->alife;
        }
        wepcom->animate(spf);
    }

    // FORCOM
    {
        // from DAMAGEABLE
        {
            forcom->active = damageable->alife;
        }
        // from traceable
        {
            quat_cpy(forcom->ori, traceable->ori);
        }
        // from MOBILE
        {
            vector_cpy(forcom->twr, mobile->twr);
        }
        // from CAMERA
        {
            forcom->reticle = camera->firstperson;
        }
        forcom->animate(spf);
    }

    // NAVCOM
    {
        // from DAMAGEABLE
        {
            navcom->active = damageable->alife;
        }
        // from TRACEABLE
        {
            vector_cpy(navcom->pos, traceable->pos);
            quat_cpy(navcom->ori, traceable->ori);
        }
        navcom->animate(spf);
    }

    // <-- end COMPUTERs

    // CONTROLLED
    {
        // from Damageable
        {
            controlled->active = damageable->alife;
            controlled->controller->disturbedBy = damageable->disturber;
            //controlled->controller->disturbedBy = this->disturbedBy();
        }

        // FIXME: from anywhere
        {
            //controlled->controller->enemyNearby = this->enemyNearby();
            controlled->controller->enemyNearby = this->tarcom->nearbyEnemy;
        }

        controlled->animate(spf);
    }

    // MOBILE
    {
        // from CONTROLLED
        {
            mobile->tower_lr = controlled->pad->getAxis(cPad::MECH_TURRET_LR_AXIS);
            mobile->tower_ud = controlled->pad->getAxis(cPad::MECH_TURRET_UD_AXIS);
            mobile->chassis_lr = controlled->pad->getAxis(cPad::MECH_CHASSIS_LR_AXIS);
            mobile->driveen = controlled->pad->getAxis(cPad::MECH_THROTTLE_AXIS);
            mobile->jeten = (controlled->pad->getButton(cPad::MECH_JET_BUTTON1) + controlled->pad->getButton(cPad::MECH_JET_BUTTON2));
        }
        // from DAMAGEABLE
        {
            mobile->active = damageable->alife;
        }
        // from traceable
        {
            vector_cpy(mobile->pos, traceable->pos);
        }
        mobile->animate(spf);
    }

    // TRACEABLE: Rigid Body, Collisions etc.
    {
        // from MOBILE:
        {
            quat_cpy(traceable->ori, mobile->ori);
            traceable->jetthrottle = mobile->jetthrottle;
            traceable->throttle = mobile->drivethrottle;
        }

        traceable->animate(spf);

        // FIXME: move to rSoundsource to be able to add loading/streaming
        // and to mount it somewhere.
        if (traceable->sound != 0) {
            alSourcefv(traceable->sound, AL_POSITION, traceable->pos);
            //alSourcefv(traceable->sound, AL_VELOCITY, traceable->vel);
        }
    }

    // RIGGED
    {
        // from MOBILE: Steering state.
        {
            rigged->rotators[rigged->YAW][1] = -mobile->twr[1];
            rigged->rotators[rigged->PITCH][0] = mobile->twr[0];
            rigged->rotators[rigged->HEADPITCH][0] = mobile->twr[0];
            rigged->jetting = mobile->jetthrottle;
        }

        // from TRACEABLE: Physical movement state.
        {
            vector_cpy(rigged->pos, traceable->pos);
            quat_cpy(rigged->ori, traceable->ori);
            vector_cpy(rigged->vel, traceable->vel);
            rigged->grounded = traceable->grounded;
        }
        // FIXME: from object tags
        {
            // Group-to-texture.
            int texture = 0;
            texture = hasTag(RED) ? 0 : texture;
            texture = hasTag(BLUE) ? 1 : texture;
            texture = hasTag(GREEN) ? 2 : texture;
            texture = hasTag(YELLOW) ? 3 : texture;
            rigged->basetexture3d = sTextures[texture];
        }

        rigged->animate(spf);
        rigged->transform();
    }

    // NAMEABLE
    {
        // from Object
        {
            nameable->color[0] = hasTag(RED) ? 1.0f : 0.0f;
            nameable->color[1] = hasTag(GREEN) ? 1.0f : 0.0f;
            nameable->color[2] = hasTag(BLUE) ? 1.0f : 0.0f;
            nameable->effect = !hasTag(HUMANPLAYER) && !hasTag(DEAD);
        }
        // from RIGGED
        {
            vector_cpy(nameable->pos0, rigged->pos);
            quat_cpy(nameable->ori0, rigged->ori);
            int eye = rigged->jointpoints[rRigged::EYE];
            vector_cpy(nameable->pos1, rigged->joints[eye].v);
            nameable->pos1[1] += 2.0f;
            quat_cpy(nameable->ori1, rigged->joints[eye].q);
        }

        nameable->animate(spf);
    }

    // CAMERA
    {
        // from RIGGED
        {
            int eye = rigged->jointpoints[rRigged::EYE];
            vector_cpy(camera->pos0, rigged->pos);
            quat_cpy(camera->ori0, rigged->ori);
            vector_cpy(camera->pos1, rigged->joints[eye].v);
            quat_cpy(camera->ori1, rigged->joints[eye].q);
        }
        
        // from MOBILE
        {
            camera->camerashake = mobile->jetthrottle;
        }
        
        // from CONTROLLED
        {
            camera->cameraswitch = controlled->pad->getButton(cPad::MECH_CAMERA_BUTTON);
        }
        
        camera->animate(spf);
    }

    // FIXME: This block needs to be factored out to fit the used component pattern.
    // FIXME: Weapons need to be chained in an independent style.
    if (damageable->alife) {
        if (controlled->pad->getButton(cPad::MECH_FIRE_BUTTON1) || controlled->pad->getButton(cPad::MECH_FIRE_BUTTON2)) {
            if (true) {
                fireCycleWeapons();
            } else {
                fireAllWeapons();
            }
        }
    }

    // WEAPON
    loopi(weapons.size()) {
        // from CONTROLLED:
        {
            weapons[i]->target = controlled->target;
        }
        // from RIGGED:
        {
            rWeapon* weapon = weapons[i];
            MD5Format::joint* joint = &rigged->joints[weapon->weaponMount];
            quat_cpy(weapon->weaponOri0, traceable->ori);
            vector_cpy(weapon->weaponPos0, traceable->pos);
            quat_cpy(weapon->weaponOri1, joint->q);
            vector_cpy(weapon->weaponPos1, joint->v);
        }
        
        weapons[i]->animate(spf);
        weapons[i]->transform();
    }

    // EXPLOSION (WEAPON)
    {
        // from RIGGED:
        {
            rWeapon* weapon = explosion;
            MD5Format::joint* joint = &rigged->joints[weapon->weaponMount];
            quat_cpy(weapon->weaponOri0, traceable->ori);
            vector_cpy(weapon->weaponPos0, traceable->pos);
            quat_cpy(weapon->weaponOri1, joint->q);
            vector_cpy(weapon->weaponPos1, joint->v);
        }
        explosion->animate(spf);
        explosion->transform();
    }

    // Write back.
    vector_cpy(this->pos, traceable->pos);
    quat_cpy(this->ori, traceable->ori);
    this->radius = traceable->radius;
}

void cMech::transform() {
    /*
    {
        rigged->transform();
    }
    {
        mobile->transform();
    }
    loopi(weapons.size()) {
        weapons[i]->transform();
    }
    {
        explosion->transform();
    }
    */
}

void cMech::drawSolid() {
    // Setup jumpjet light source - for player only so far. move to rLightsource?
    if (hasTag(HUMANPLAYER)) {
        int light = GL_LIGHT1;
        if (mobile->jetthrottle > 0.001f) {
            float p[] = {traceable->pos[0], traceable->pos[1]+1.2, traceable->pos[2], 1};
            //float zero[] = {0, 0, 0, 1};
            float s = mobile->jetthrottle;
            float a[] = {0.0,0.0,0.0,1};
            float d[] = {0.9 * s, 0.9 * s, 0.4 * s, 1};
            //glPushMatrix();
            {
                //glLoadIdentity();
                //glTranslatef(traceable->pos[0], traceable->pos[1]+1, traceable->pos[2]);
                //glLightfv(light, GL_POSITION, zero);
                glLightfv(light, GL_POSITION, p);
                glLightfv(light, GL_AMBIENT, a);
                glLightfv(light, GL_DIFFUSE, d);
                glLightf(light, GL_CONSTANT_ATTENUATION, 1.0);
                glLightf(light, GL_LINEAR_ATTENUATION, 0.001);
                glLightf(light, GL_QUADRATIC_ATTENUATION, 0.005);
                glEnable(light);
            }
            //glPopMatrix();
        } else {
            glDisable(light);
        }
    }

    {
        rigged->drawSolid();
    }
    {
        mobile->drawSolid();
    }
    loopi(weapons.size()) {
        weapons[i]->drawSolid();
    }
    {
        explosion->drawSolid();
    }
}

void cMech::drawEffect() {
    {
        collider->drawEffect();
    }
    {
        rigged->drawEffect();
    }
    {
        mobile->drawEffect();
    }
    loopi(weapons.size()) {
        weapons[i]->drawEffect();
    }
    {
        explosion->drawEffect();
    }
    {
        nameable->drawEffect();
    }
}

void cMech::drawHUD() {
    //computerised->drawHUD();
    glPushAttrib(GL_ALL_ATTRIB_BITS /* more secure */);
    {
        SGL::glUseProgram_bkplaincolor();
        glDisable(GL_CULL_FACE);
        glLineWidth(2);

        SGL::glPushOrthoProjection();
        {
            glPushMatrix();
            {
                glLoadIdentity();

                float bk[] = {0.0, 0.2, 0.3, 0.2};
                //float bk[] = {0, 0, 0, 0.0};
                float w = 5;
                float h = 4;
                float sx = 1.0f / w;
                float sy = 1.0f / h;

                rRole* displays[4][5] = {
                    { navcom, NULL, NULL, NULL, comcom},
                    { NULL, NULL, NULL, NULL, NULL},
                    { NULL, NULL, NULL, NULL, NULL},
                    { tarcom, NULL, wepcom, NULL, damageable}
                };

                loopj(4) {

                    loopi(5) {
                        if (displays[j][i] == NULL) continue;
                        glPushMatrix();
                        {
                            glScalef(sx, sy, 1);
                            glTranslatef(i, 3 - j, 0);
                            glColor4fv(bk);
                            displays[j][i]->drawHUD();
                        }
                        glPopMatrix();
                    }
                }

                if (true) {
                    glPushMatrix();
                    {
                        glTranslatef(0.25, 0.25, 0);
                        glScalef(0.5, 0.5, 1);
                        glTranslatef(0, 0, 0);
                        glColor4fv(bk);
                        forcom->drawHUD();
                    }
                    glPopMatrix();
                }

            }
            glPopMatrix();
        }
        SGL::glPopProjection();
    }
    glPopAttrib();
}

void cMech::damageByParticle(float* localpos, float damage, cObject* enactor) {
    if (!damageable->alife || damage == 0.0f) return;

    int hitzone = rDamageable::BODY;
    if (localpos[1] < 1.1*rigged->height && damageable->hp[rDamageable::LEGS] > 0) hitzone = rDamageable::LEGS;
    else if (localpos[0] < -0.5 && damageable->hp[rDamageable::LEFT] > 0) hitzone = rDamageable::LEFT;
    else if (localpos[0] > +0.5 && damageable->hp[rDamageable::RIGHT] > 0) hitzone = rDamageable::RIGHT;

    if (!damageable->damage(hitzone, damage, enactor)) {
        cout << "cMech::damageByParticle(): DEAD\n";
        //explosion->fire();
        explosion->triggeren = true;
        explosion->trigger = true;
    }
    int body = rDamageable::BODY;
    if (damageable->hp[body] <= 75) addTag(WOUNDED);
    if (damageable->hp[body] <= 50) addTag(SERIOUS);
    if (damageable->hp[body] <= 25) addTag(CRITICAL);
    if (damageable->hp[body] <= 0) addTag(DEAD);
}

float cMech::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float maxdepth = 0.0f;
    {
        float depth = collider->constrainParticle(worldpos, radius, localpos, enactor);
        if (depth > maxdepth) {
            maxdepth = depth;
        }
    }
    return maxdepth;
}

float cMech::inDestinationRange() {
    float a = 0;
    float b = 6;
    float d = vector_distance(mobile->pos, controlled->destination);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inMeeleRange() {
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 16;
    float b = 24;
    float d = vector_distance(mobile->pos, target->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inWeaponRange() {
    // TODO inWeaponRange should depend on ready to fire weapons properties.
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 36 + 10;
    float b = 44 + 10;
    float d = vector_distance(mobile->pos, target->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inTargetRange() {
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 56;
    float b = 124;
    float d = vector_distance(mobile->pos, target->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

void cMech::do_moveTowards() {
    //cout << "do_moveTowards():\n";
    if (controlled->pad == NULL) return;

    // Determine Target (ie. Move- or Aim-Target if former is not available).
    float* target_pos = NULL;
    if (finitef(controlled->destination[0])) {
        target_pos = controlled->destination;
    } else if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) {
            target_pos = target->pos;
        } else {
            controlled->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori);
    float lr = rd[0];
    // Determine throttling according to angle.
    float thr = 1.0f * (1.0f - 0.6 * fabs(rd[0]));
    float throttle = -thr;
    //cout << "Throttle: " << throttle << "\n";

    controlled->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, lr);
    controlled->pad->setAxis(cPad::MECH_THROTTLE_AXIS, throttle);
}

void cMech::do_moveNear() {
    //cout << "do_moveNear():\n";
    if (controlled->pad == NULL) return;

    // Determine Target (ie. Move- or Aim-Target if former is not available).
    float* target_pos = NULL;
    if (finitef(controlled->destination[0])) {
        target_pos = controlled->destination;
    } else if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) {
            target_pos = target->pos;
        } else {
            controlled->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction.
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori);
    float lr = 2 * rd[0];
    // Determine distance.
    float d = vector_distance(mobile->pos, target_pos);
    // Throttle according to angle and distance.
    float thr = 1.0f * (1.0f - 0.7 * fabs(rd[0]));
    float f = (d - 23);
    f = fmin(+1.0f, fmax(-1.0f, f));
    f *= thr;
    float throttle = -f;
    //cout << "Throttle: " << throttle << "\n";

    controlled->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, lr);
    controlled->pad->setAxis(cPad::MECH_THROTTLE_AXIS, throttle);
}

void cMech::do_aimAt() {
    //cout "do_aimAt():\n";
    if (controlled->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) target_pos = target->pos;
        else return;
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori, mobile->ori1);
    float lr = 2 * rd[0];
    float ud = rd[1];

    controlled->pad->setAxis(cPad::MECH_TURRET_LR_AXIS, lr);
    controlled->pad->setAxis(cPad::MECH_TURRET_UD_AXIS, ud);
}

void cMech::do_fireAt() {
    //cout << "do_fireAt():\n";
    if (controlled->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) target_pos = target->pos;
        else return;
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, mobile->pos, target_pos, mobile->ori, mobile->ori1);
    //  Fire at random and only if angle small enough.
    bool fire = (rand() % 100 <= 40 && fabs(rd[0]) < 0.5);
    //cout "Fire: " << fire << " \n";

    controlled->pad->setButton(cPad::MECH_FIRE_BUTTON1, fire);
}

void cMech::do_idle() {
    if (controlled->pad == NULL) return;
    mobile->drivethrottle = 0;
    controlled->pad->reset();
}

void cMech::do_aimFor(OID target) {
    controlled->target = target;
}

void cMech::do_moveFor(float* dest) {
    if (dest != NULL) {
        vector_cpy(controlled->destination, dest);
    } else {
        // Set XYZ to Not-A-Number (NaN) for no location.
        // Note that NaN-ity can only be tested either by
        // isnanf(x), !finite(x) or by x!=x as NaN always != NaN.
        vector_set(controlled->destination, float_NAN, float_NAN, float_NAN);
        assert(controlled->destination[0] != controlled->destination[0]);
        //cout << "Destination is " << ((finitef(mDestination[0])) ? "finite" : "infinite" ) << "\n";
    }
}

