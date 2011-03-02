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

DEFINE_glprintf

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
    // Generate base vertices and normals for 3d-tex-coords.
    if (baseverts.empty()) {
        MD5Format::mesh* msh = MD5Format::getFirstMesh(model);
        MD5Format::joint* staticjoints = MD5Format::getJoints(model);

        loopi(model->numMeshes) {
            float* cur_baseverts = new float[msh->numverts * 3];
            float* cur_basenorms = new float[msh->numverts * 3];
            MD5Format::animatedMeshVertices(msh, staticjoints, cur_baseverts, cur_basenorms);
            baseverts[i] = cur_baseverts;
            basenorms[i] = cur_basenorms;
            // TODO: recalculate normals, invert and to weights.
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
            }
            MD5Format::unanimatedMeshNormals(msh, staticjoints, cur_basenorms);
            msh = MD5Format::getNextMesh(msh);
        }
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        SGL::glUseProgram_fglittexture3d();
        glColor4f(1, 1, 1, 1);
        glFrontFace(GL_CW);

        MD5Format::mesh* msh = MD5Format::getFirstMesh(model);

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
            delete vtx;
            delete nrm;
            msh = MD5Format::getNextMesh(msh);
        }
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

void rRigged::transformMounts() {
    // Create mountpoint matrices. Later only quaternions and points.
    float* matrices[] = {
        HDMount,
        CTMount, BKMount,
        LSMount, RSMount,
        LAMount, RAMount
    };

    int mounts[] = {
        EYE,
        CTMOUNT, BKMOUNT,
        LSMOUNT, RSMOUNT,
        LAMOUNT, RAMOUNT,
    };

    MD5Format::joint* joints_ = joints;
    loopi(3) {
        int jidx = jointpoints[mounts[i]];
        if (jidx < 0) continue;

        glPushMatrix();
        {
            glLoadIdentity();

            if (pos != NULL) glTranslatef(pos[0], pos[1], pos[2]);
            if (ori != NULL) SGL::glRotateq(ori);
            glScalef(scale, scale, scale);

            glTranslatef(joints_[jidx].v[0], joints_[jidx].v[1], joints_[jidx].v[2]);
            SGL::glRotateq(joints_[jidx].q);
            glScalef(1.0f / scale, 1.0f / scale, 1.0f / scale);
            glRotatef(180, 0, 1, 0); // Hack, need to invert weapons and camera.
            if (matrices[i] != NULL) glGetFloatv(GL_MODELVIEW_MATRIX, matrices[i]);
        }
        glPopMatrix();
    }
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

    // Rotate yaw 180 to make frontfacing.
    quat convfacing;
    vec3 yaxis = {0,1,0};
    quat_rotaxis(convfacing, M_PI, yaxis);
    quat_mul(joints_[0].q, joints_[0].q, convfacing);

    // Allocate space for animated global joints.
    joints = new MD5Format::joint[model->numJoints];
    memcpy(joints, joints_, sizeof (MD5Format::joint) * model->numJoints);

    loopi(MAX_JOINTPOINTS) {
        jointpoints[i] = MD5Format::findJoint(model, getJointname(i).c_str());
        //if (jointpoints[i] < 0) jointpoints[i] = MD5Format::findJoint(model, getJointname2(i).c_str());
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
    transformMounts();
}

void rRigged::drawSolid() {
    if (!active) return;
    if (model == NULL) return;
    glPushMatrix();
    {
        glTranslatef(pos[0], pos[1], pos[2]);
        SGL::glRotateq(ori);
        //cPrimitives::glAxis(3.0f);
        float rscale = scale;
        glScalef(rscale, rscale, rscale);
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
            float rscale = scale;
            glScalef(rscale, rscale, rscale);
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
                        float rscale = scale;
                        glScalef(rscale, rscale, rscale);
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


rComputerised::rComputerised(cObject* obj) {
    role = "COMPUTERISED";
    object = obj;
    
    comcom = new cComcom(object);
    assert(comcom != NULL);

    tarcom = new cTarcom(object);
    assert(tarcom != NULL);

    syscom = new cSyscom(object);
    assert(syscom != NULL);

    wepcom = new cWepcom(object);
    assert(wepcom != NULL);

    forcom = new cForcom((cMech*) object);
    assert(forcom != NULL);

    navcom = new cNavcom(object);
    assert(navcom != NULL);
};

rComputerised::~rComputerised() {
    delete comcom;
    delete tarcom;
    delete syscom;
    delete wepcom;
    delete forcom;
    delete navcom;
}

void rComputerised::animate(float dt) {
    if (!active) return;
    comcom->process(dt);
    tarcom->process(dt);
    syscom->process(dt);
    wepcom->process(dt);
    forcom->process(dt);
    navcom->process(dt);
}

void rComputerised::drawHUD() {
    if (!active) return;
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

                cComputer * displays[4][5] = {
                    { navcom, NULL, NULL, NULL, comcom},
                    { NULL, NULL, NULL, NULL, NULL},
                    { NULL, NULL, NULL, NULL, NULL},
                    { tarcom, NULL, wepcom, NULL, syscom}
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


// -------------------------------------------------------------------


void rMobile::ChassisLR(float radians) {
    const float limit = 0.01 * M_PI;
    float e = radians;
    if (e > +limit) e = +limit;
    if (e < -limit) e = -limit;
    bse[1] += e;
    const float fullcircle = 2 * M_PI;
    bse[1] = fmod(bse[1], fullcircle);

    float axis[] = {0, 1, 0};
    quat_rotaxis(object->traceable->ori, bse[1], axis);
}

void rMobile::ChassisUD(float radians) {
    float e = radians;
    const float limit = 0.005 * M_PI;
    if (e > +limit) e = +limit;
    if (e < -limit) e = -limit;
    bse[0] += e;
    const float fullcircle = 2 * M_PI;
    bse[0] = fmod(bse[0], fullcircle);

    float axis[] = {1, 0, 0};
    quat_rotaxis(object->traceable->ori, bse[0], axis);
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

void rMobile::fireCycleWeapons() {
    if (weapons.size() == 0) return;
    currentWeapon %= weapons.size();
    fireWeapon(currentWeapon);
    currentWeapon++;
    currentWeapon %= weapons.size();
}

void rMobile::fireAllWeapons() {

    loopi(weapons.size()) {
        weapons[i]->fire(target);
    }
}

void rMobile::fireWeapon(unsigned n) {
    if (n >= weapons.size()) return;
    weapons[n]->fire(target);
}

void rMobile::animate(float spf) {
    if (!active) return;
    loopi(weapons.size()) {
        weapons[i]->animate(spf);
    }
    explosion.animate(spf);
}

void rMobile::transform() {
    if (!active) return;
    loopi(weapons.size()) {
        weapons[i]->transform();
    }
    explosion.transform();
}

void rMobile::drawSolid() {
    if (!active) return;
    loopi(weapons.size()) {
        weapons[i]->drawSolid();
    }
    explosion.drawSolid();
}

void rMobile::drawEffect() {
    if (!active) return;
    loopi(weapons.size()) {
        weapons[i]->drawEffect();
    }
    explosion.drawEffect();
}


// -------------------------------------------------------------------


int cMech::sInstances = 0;
std::map<int, long> cMech::sTextures;


cMech::cMech(float* pos, float* rot) {
    sInstances++;
    if (sInstances == 1) {
        if (1) {
            registerRole(new rMobile((cObject*)NULL), FIELDOFS(mobile), ROLEPTR(cMech::mobile));
            registerRole(new rRigged((cObject*)NULL), FIELDOFS(rigged), ROLEPTR(cMech::rigged));
            registerRole(new rComputerised((cObject*)NULL), FIELDOFS(computerised), ROLEPTR(cMech::computerised));

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

    rigged = new rRigged(this);
    damageable = new rDamageable(this);
    computerised = new rComputerised(this);
    controlled = new rControlled(this);
    socialised = new rSocialised(this);
    mobile = new rMobile(this);

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
            if (buffer == AL_NONE) throw "could not load pow.wav";
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

    controlled->controller = new cController(this, true);
    assert(controlled->controller != NULL);

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

    traceable->radius = 1.75f * rigged->scale;
    traceable->cwm2 = 0.4 /*very bad cw*/ * traceable->radius * traceable->radius * M_PI * rigged->scale /*m2*/;
    traceable->mass = 11000 * rigged->scale * 1.00f; // TODO mass is qubic to size.
    traceable->mass_inv = 1.0f / traceable->mass;
}

cMech::~cMech() {
    delete computerised;
    delete rigged;
    if (sInstances == 1) {
        // Last one cleans up

    }
    sInstances--;
}

void cMech::onMessage(cMessage* message) {
    cout << "I (" << this->oid << ":" << this->name << ") just received: \"" << message->getText() << "\" from sender " << message->getSender() << endl;
    this->computerised->forcom->addMessage(message->getText());
}

void cMech::onSpawn() {
    //cout << "cMech::onSpawn()\n";
    this->mountWeapon((char*) "CTorsor", &mobile->explosion, false);
    ALuint soundsource = traceable->sound;
    if (hasTag(HUMANPLAYER) && alIsSource(soundsource)) alSourcePlay(soundsource);
    //cout << "Mech spawned " << oid << "\n";
}

#define grand() ((rand()%100 + rand()%100 + rand()%100 + rand()%100 + rand()%100) * 0.01f * 0.2f - 0.5f)

void cMech::multEyeMatrix() {
    float camera[16];
    float rot_inv[16];
    float pos_inv[16];

    float intensity = 0.0 + pow(this->mobile->jetthrottle, 1.5f);
    float shake = 0.004f * intensity;
    float jerk = 0.95f * intensity;

    glPushMatrix();
    {
        // Get Inverse Components of Head Matrix
        glLoadMatrixf(rigged->HDMount);
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
        glGetFloatv(GL_MODELVIEW_MATRIX, camera);
        //matrix_print(camera);
    }
    glPopMatrix();

    if (abs(mobile->camerastate) == 1) { // 1st
        glMultMatrixf(camera);
    } else if (abs(mobile->camerastate) == 2) { // 3rd
        glTranslatef(0, 0, -5);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(camera);
    } else if (abs(mobile->camerastate) == 3) { // 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glMultMatrixf(camera);
    } else if (abs(mobile->camerastate) == 4) { // Reverse 3rd Far
        glTranslatef(0, 0, -15);
        glRotatef(15, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glMultMatrixf(camera);
    } else if (abs(mobile->camerastate) == 5) { // Map Near
        glTranslatef(0, 0, -50);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } else if (abs(mobile->camerastate) == 6) { // Map Far
        glTranslatef(0, 0, -100);
        glRotatef(90, 1, 0, 0);
        glMultMatrixf(pos_inv);
    } // if camerastate
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
    float* head = rigged->HDMount;
    float at_and_up[] = {
        head[8], head[9], head[10],
        0, -1, 0
    };
    alListenerfv(AL_ORIENTATION, at_and_up);
}

// move to rMobile?
void cMech::mountWeapon(char* point, rWeapon *weapon, bool add) {
    if (weapon == NULL) throw "Null weapon for mounting given.";
    weapon->weaponMount = rigged->getMountpoint(point);
    weapon->weaponBasefv = rigged->getMountMatrix(point);
    weapon->object = this;
    weapon->weaponScale = rigged->scale;
    if (add) mobile->weapons.push_back(weapon);
}

void cMech::animate(float spf) {
    {
        computerised->active = damageable->alife;
        computerised->animate(spf);
    }

    if (!damageable->alife) {
        // Stop movement when dead
        mobile->throttle = 0;
        mobile->jetthrottle = 0;
        /*
        // If already exploded then frag.
        if (mobile->explosion.ready()) {
            mobile->ChassisLR(0);
            mobile->TowerLR(0);
            mobile->TowerUD(0);
            //cWorld::instance->fragObject(this);
        }
        if (controlled->pad) controlled->pad->reset();
        */
    }

    // Process computers and Pad-input when not dead.
    if (damageable->alife && controlled->pad != NULL) {
        //cout << mPad->toString();

        // Let AI-Controller set Pad-input.
        if (controlled->controller && controlled->controller->controllerEnabled) {
            if (controlled->pad) controlled->pad->reset();
            controlled->controller->process();
        }

        float excess = mobile->TowerLR(0.25f * M_PI * spf * -controlled->pad->getAxis(cPad::MECH_TURRET_LR_AXIS));
        mobile->TowerUD(50 * spf * controlled->pad->getAxis(cPad::MECH_TURRET_UD_AXIS) * 0.017453f);

        if (!mobile->immobile) {
            // Steer left right
            mobile->ChassisLR(excess + 0.25f * M_PI * spf * -controlled->pad->getAxis(cPad::MECH_CHASSIS_LR_AXIS));
            // Speed
            float tdelta = -controlled->pad->getAxis(cPad::MECH_THROTTLE_AXIS);
            //cout << "tdelta: " << tdelta << "  speed: " << mSpeed << "\n";
            mobile->throttle = fmax(-0.5f, tdelta * 1.0f);
        }

        if (controlled->pad->getButton(cPad::MECH_NEXT_BUTTON) && controlled->pad->getButtonEvent(cPad::MECH_NEXT_BUTTON)) {
            computerised->tarcom->nextTarget();
        } else if (controlled->pad->getButton(cPad::MECH_PREV_BUTTON) && controlled->pad->getButtonEvent(cPad::MECH_PREV_BUTTON)) {
            computerised->tarcom->prevTarget();
        }

        if (controlled->pad->getButton(cPad::MECH_FIRE_BUTTON1) || controlled->pad->getButton(cPad::MECH_FIRE_BUTTON2)) {
            mobile->target = controlled->target;
            if (true) {
                mobile->fireCycleWeapons();
            } else {
                mobile->fireAllWeapons();
            }
        }

        // Set jumpjet throttle 0-1 smoothly.
        bool jeten = (controlled->pad->getButton(cPad::MECH_JET_BUTTON1) || controlled->pad->getButton(cPad::MECH_JET_BUTTON2));
        mobile->jetthrottle += 0.05f * ((jeten ? 1.0f : 0.0f) - mobile->jetthrottle);

        if (controlled->pad->getButton(cPad::MECH_CAMERA_BUTTON)) {
            // Only if Camera State is not transitional
            // then switch to next perspective on button press.
            if (mobile->camerastate > 0) {
                mobile->camerastate = (1 + (mobile->camerastate % MAX_CAMERAMODES));
                mobile->camerastate *= -1; // Set transitional.
            }
        } else {
            // Set Camera State as fixed.
            mobile->camerastate = abs(mobile->camerastate);
        }
    }

    // MOBILE
    {
        mobile->grounded = traceable->grounded;
        mobile->animate(spf);
    }

    // TRACEABLE
    {
        traceable->jetthrottle = mobile->jetthrottle;
        traceable->throttle = mobile->throttle;
        // Rigid Body, Collisions etc.
        traceable->animate(spf);
        if (traceable->sound != 0) {
            alSourcefv(traceable->sound, AL_POSITION, traceable->pos);
            //alSourcefv(traceable->sound, AL_VELOCITY, traceable->vel);
        }
    }

    // RIGGED
    {
        // Steering state.
        rigged->rotators[rigged->YAW][1] = -mobile->twr[1];
        rigged->rotators[rigged->PITCH][0] = mobile->twr[0];
        rigged->rotators[rigged->HEADPITCH][0] = mobile->twr[0];

        // Drivetrain state.
        rigged->grounded = mobile->grounded;
        rigged->jetting = mobile->jetthrottle;

        // Physical movement state.
        vector_cpy(rigged->pos, traceable->pos);
        quat_cpy(rigged->ori, traceable->ori);
        vector_cpy(rigged->vel, traceable->vel);

        rigged->animate(spf);
    }
}

void cMech::transform() {
    {
        rigged->transform();
    }
    {
        loopi(mobile->weapons.size()) {
            rWeapon* weapon = mobile->weapons[i];
            MD5Format::joint* joint = &rigged->joints[weapon->weaponMount];
            quat_cpy(weapon->weaponOri0, traceable->ori);
            vector_cpy(weapon->weaponPos0, traceable->pos);
            quat_cpy(weapon->weaponOri1, joint->q);
            vector_cpy(weapon->weaponPos1, joint->v);
            weapon->weaponBasefv = NULL;
        }

        mobile->transform();
    }
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
        // Group-to-texture.
        int texture = 0;
        texture = hasTag(RED) ? 0 : texture;
        texture = hasTag(BLUE) ? 1 : texture;
        texture = hasTag(GREEN) ? 2 : texture;
        texture = hasTag(YELLOW) ? 3 : texture;
        glBindTexture(GL_TEXTURE_3D, sTextures[texture]);

        rigged->drawSolid();
    }

    {
        mobile->drawSolid();
    }
}

void cMech::drawEffect() {
    {
        rigged->drawEffect();
    }

    {
        mobile->drawEffect();
    }
    
    // Draw name above head. move to nameable?
    if (!hasTag(HUMANPLAYER)) {
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

            // Print name above head.
            if (nameable) {
                SGL::glUseProgram_fgplaintexture();
                glColor4f(0.95, 0.95, 1, 1);
                glPushMatrix();
                {
                    glMultMatrixf(rigged->HDMount);
                    glTranslatef(0, +0.9, 0);
                    float s = 0.65;
                    glScalef(s, s, s);
                    float n[16];
                    SGL::glGetTransposeInverseRotationMatrix(n);
                    glMultMatrixf(n);
                    int l = nameable->name.length();
                    glTranslatef(-l * 0.5f, 0, 0);
                    glprintf(nameable->name.c_str());
                }
                glPopMatrix();
            }
        }
        glPopAttrib();
    }

}

void cMech::drawHUD() {
    computerised->drawHUD();
}

void cMech::damageByParticle(float* localpos, float damage, cObject* enactor) {
    if (!damageable->alife || damage == 0.0f) return;

    int hitzone = rDamageable::BODY;
    if (localpos[1] < 1.3 && damageable->hp[rDamageable::LEGS] > 0) hitzone = rDamageable::LEGS;
    else if (localpos[0] < -0.5 && damageable->hp[rDamageable::LEFT] > 0) hitzone = rDamageable::LEFT;
    else if (localpos[0] > +0.5 && damageable->hp[rDamageable::RIGHT] > 0) hitzone = rDamageable::RIGHT;

    if (enactor != NULL) controlled->disturber = enactor->oid;
    if (!damageable->damage(hitzone, damage, enactor)) {
        cout << "cMech::damageByParticle(): DEAD\n";
        mobile->explosion.fire(0);
    }
    int body = rDamageable::BODY;
    if (damageable->hp[body] <= 75) addTag(WOUNDED);
    if (damageable->hp[body] <= 50) addTag(SERIOUS);
    if (damageable->hp[body] <= 25) addTag(CRITICAL);
    if (damageable->hp[body] <= 0) addTag(DEAD);
}

// move to traceable? or rigged?
float cMech::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float localpos_[3];
    { // Transform to local.
        quat ori_inv;
        quat_cpy(ori_inv, traceable->ori);
        quat_conj(ori_inv);

        vector_cpy(localpos_, worldpos);
        vector_sub(localpos_, localpos_, traceable->pos);
        quat_apply(localpos_, ori_inv, localpos_);
    }

    float base[3] = {0, -0.0 - radius, 0};
    float radius_ = traceable->radius + radius;
    float height = traceable->radius * 2.5 + 2 * radius;
    float localprj[3] = { 0, 0, 0 };

    float depth = 0;
    depth = cParticle::constraintParticleByCylinder(localpos_, base, radius_, height, localprj);
    //depth = cParticle::constraintParticleByCone(localpos_, base, radius_, height, localprj);

    if (depth <= 0) return 0;

    if (localpos != NULL) vector_cpy(localpos, localprj);

    { // Transform to global.
        quat_apply(worldpos, traceable->ori, localprj);
        vector_add(worldpos, worldpos, traceable->pos);
    }

    return depth;
}

OID cMech::enemyNearby() {
    OID result = 0;
    // Filter objects by distance
    std::list<cObject*>* scan = cWorld::instance->filterByRange(this, traceable->pos, 0.0f, 50.0f, -1, NULL);
    if (!scan->empty()) {
        // Find all objects belonging to any enemy party/role.
        std::list<cObject*>* roles = cWorld::instance->filterByTags(this, &socialised->inc_enemies, false, -1, scan);
        if (!roles->empty()) {
            for (std::list<cObject*>::iterator i=roles->begin(); i != roles->end(); i++) {
                cObject* o = *i;
                if (!o->anyTags(&socialised->exc_enemies)) {
                    result = o->oid;
                }
            }
        }
        delete roles;
    }
    delete scan;
    return result;
}

OID cMech::disturbedBy() {
    OID disturb = controlled->disturber;
    controlled->disturber = 0;
    return disturb;
}

float cMech::inDestinationRange() {
    float a = 0;
    float b = 6;
    float d = vector_distance(traceable->pos, controlled->destination);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inMeeleRange() {
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 16;
    float b = 24;
    float d = vector_distance(traceable->pos, target->traceable->pos);
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
    float d = vector_distance(traceable->pos, target->traceable->pos);
    if (d < a) return 1.0f;
    if (d > b) return 0.0f;
    return (1.0f - (d - a) / (b - a));
}

float cMech::inTargetRange() {
    cObject* target = cWorld::instance->getObject(controlled->target);
    if (target == NULL) return 0.0f;
    float a = 56;
    float b = 124;
    float d = vector_distance(traceable->pos, target->traceable->pos);
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
            target_pos = target->traceable->pos;
        } else {
            controlled->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, traceable->pos, target_pos, traceable->ori);
    controlled->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, rd[0]);

    //float thr = 20.0f * (1.0f - fabs(rd[0]) / 360.0f);
    float thr = 1.0f * (1.0f - 0.6 * fabs(rd[0]));
    //cout << "Throttle: " << thr << "\n";
    //mPad->setAxis(cPad::MECH_THROTTLE_AXIS, (char) - thr);
    controlled->pad->setAxis(cPad::MECH_THROTTLE_AXIS, -thr);
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
            target_pos = target->traceable->pos;
        } else {
            controlled->target = 0;
            return;
        }
    } else return;

    // Determine nearest rotation direction
    vec2 rd;
    cParticle::rotationTo(rd, traceable->pos, target_pos, traceable->ori);
    controlled->pad->setAxis(cPad::MECH_CHASSIS_LR_AXIS, 2 * rd[0]);

    float d = vector_distance(traceable->pos, target_pos);

    float thr = 1.0f * (1.0f - 0.7 * fabs(rd[0]));

    float f = (d - 23);
    if (f < -1) f = -1;
    if (f > +1) f = +1;

    f *= thr;

    controlled->pad->setAxis(cPad::MECH_THROTTLE_AXIS, -f);
    //cout << "Throttle: " << thr << "\n";
    //mPad->setAxis(cPad::MECH_THROTTLE_AXIS, (char) - thr);
}

void cMech::do_aimAt() {
    //cout "do_aimAt():\n";
    if (controlled->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) target_pos = target->traceable->pos;
        else return;
    } else return;

    // Determine nearest rotation direction
    quat tower_ori;
    quat_set(tower_ori, 0, sin(0.5 * this->mobile->twr[1]), 0, cos(0.5 * this->mobile->twr[1]));

    quat tower_ori_v;
    quat_set(tower_ori_v, sin(0.5 * this->mobile->twr[0]), 0, 0, cos(0.5 * this->mobile->twr[0]));

    quat_mul(tower_ori, tower_ori, tower_ori_v);

    vec2 rd;
    cParticle::rotationTo(rd, traceable->pos, target_pos, traceable->ori, tower_ori);
    controlled->pad->setAxis(cPad::MECH_TURRET_LR_AXIS, 2 * rd[0]);
    controlled->pad->setAxis(cPad::MECH_TURRET_UD_AXIS, rd[1]);
}

void cMech::do_fireAt() {
    //cout << "do_fireAt():\n";
    if (controlled->pad == NULL) return;

    // Get aim-target position.
    float* target_pos = NULL;
    if (controlled->target != 0) {
        cObject* target = cWorld::instance->getObject(controlled->target);
        if (target != NULL) target_pos = target->traceable->pos;
        else return;
    } else return;

    // Determine nearest rotation direction
    quat tower_ori;
    quat_set(tower_ori, 0, sin(0.5 * this->mobile->twr[1]), 0, cos(0.5 * this->mobile->twr[1]));
    vec2 rd;
    cParticle::rotationTo(rd, traceable->pos, target_pos, traceable->ori, tower_ori);
    // Fire at random and only if angle small enough.
    if (rand() % 100 <= 40 && fabs(rd[0]) < 0.5) {
        controlled->pad->setButton(cPad::MECH_FIRE_BUTTON1, true);
        //cout "fire\n";
    }
}

void cMech::do_idle() {
    if (controlled->pad == NULL) return;
    mobile->throttle = 0;
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

