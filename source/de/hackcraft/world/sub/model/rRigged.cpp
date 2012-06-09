#include "rRigged.h"

#include "de/hackcraft/psi3d/GLS.h"
#include "de/hackcraft/psi3d/Primitive.h"
#include "de/hackcraft/psi3d/GLF.h"

#include "de/hackcraft/proc/Solid.h"

#include "de/hackcraft/io/Filesystem.h"

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <GL/glew.h>

static GLenum loadMaterial() {
    static bool fail = false;
    static GLenum prog = 0;

    if (prog == 0 && !fail) {
        char* vtx = Filesystem::loadTextFile("data/base/material/base.v");
        if (vtx) cout << "--- Vertex-Program Begin ---\n" << vtx << "\n--- Vertex-Program End ---\n";
        char* fgm = Filesystem::loadTextFile("data/base/material/base3d.f");
        if (fgm) cout << "--- Fragment-Program Begin ---\n" << fgm << "\n--- Fragment-Program End ---\n";
        fail = (vtx[0] == 0 && fgm[0] == 0);
        if (!fail) {
            prog = GLS::glCompileProgram(vtx, fgm, cout);
        }
        delete[] vtx;
        delete[] fgm;
    }

    if (fail) return 0;
    return prog;
}


#define DRAWJOINTS !true

#define MECHDETAIL 0

std::string rRigged::cname = "RIGGED";
unsigned int rRigged::cid = 4900;

std::map<std::string,unsigned long> rRigged::materials;


void rRigged::initMaterials() {
    if (materials.empty()) {
        if (1) {

            cout << "Generating Camoflage..." << endl;

            const int SIZE = 1 << (7 + MECHDETAIL);
            unsigned char* texels = new unsigned char[SIZE * SIZE * SIZE * 3];

            enum {
                WOOD,
                RUSTY,
                URBAN,
                DESERT,
                SNOW,
                CAMO,
                GLASS,
                RUBBER,
                STEEL,
                WARN,
                MAX_TEX
            };

            const char* names[] = {
                "wood",
                "rusty",
                "urban",
                "desert",
                "snow",
                "camo",
                "glass",
                "rubber",
                "steel",
                "warn"
            };

            for (int l = 0; l < MAX_TEX; l++) {
                long t = 0;

                loopijk(SIZE, SIZE, SIZE) {
                    float color[16];
                    const float f = 0.25f * 0.25f * 64.0f / SIZE;
                    float x = f*i, y = f*j, z = f*k;
                    switch (l) {
                        case WOOD: Solid::camo_wood(x, y, z, color); 
                            break;
                        case RUSTY: Solid::camo_rust(x, y, z, color);
                            break;
                        case URBAN: Solid::camo_urban(x, y, z, color);
                            break;
                        case DESERT: Solid::camo_desert(x, y, z, color);
                            break;
                        case SNOW: Solid::camo_snow(x, y, z, color);
                            break;
                        case CAMO: Solid::camo_desert(x, y, z, color); // camo
                            break;
                        case GLASS: Solid::metal_damast(x, y, z, color); // glass
                            break;
                        case RUBBER: //Solid::stone_lava(x, y, z, color); // rubber
                            color[0] = color[1] = color[2] = 0.2f;
                            color[3] = 1.0f;
                            break;
                        case STEEL: Solid::metal_sheets(x, y, z, color); // steel
                            break;
                        case WARN: Solid::pattern_warning(x, y, z, color); // warn
                            break;
                        default:
                            Solid::camo_rust(x, y, z, color);
                    }
                    texels[t++] = 255.0f * color[0];
                    texels[t++] = 255.0f * color[1];
                    texels[t++] = 255.0f * color[2];
                }
                unsigned int texname = GLS::glBindTexture3D(0, true, true, true, true, true, SIZE, SIZE, SIZE, texels);
                materials[string(names[l])] = texname;
            }
            delete[] texels;
        }
    }
}

    
void rRigged::drawBones() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    {
        GLS::glUseProgram_fgplaincolor();

        loopi(model->numJoints) {
            glPushMatrix();
            {
                glTranslatef(joints[i].v[0], joints[i].v[1], joints[i].v[2]);
                GLS::glRotateq(joints[i].q);
                Primitive::glAxis(0.7);
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
        GLS::glUseProgram_fglittexture3d();
        glUseProgramObjectARB(loadMaterial());
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

        /*
        std::map<char,float> colors;
        colors['r'] = 0.10;
        colors['c'] = 0.50;
        colors['s'] = 0.80;
        colors['g'] = 1.00;
        */

        loopi(model->numMeshes) {
            //cout << "Shader:" << msh->shader << "\n";
            //float co = colors[msh->shader[0]];
            //glColor3f(co,co,co);
            
            string shader = string(msh->shader);
            if (shader.compare("camo") == 0) {
                switch(basetexture3d) {
                    case 0: shader = "wood"; break;
                    case 1: shader = "rusty"; break;
                    case 2: shader = "urban"; break;
                    case 3: shader = "snow"; break;
                    default: break;
                }
            }
            glBindTexture(GL_TEXTURE_3D, materials[shader]);

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
                glTexCoord3f(vox[3 * k + 0] * s, vox[3 * k + 1] * s, vox[3 * k + 2] * s);
                glVertex3fv(&vtx[3 * k]);
                k = tris[j].b;
                //glTexCoord2fv(verts[k].tmap);
                //glTexCoord3fv(&vox[3 * k]);
                glNormal3fv(&nrm[3 * k]);
                glTexCoord3f(vox[3 * k + 0] * s, vox[3 * k + 1] * s, vox[3 * k + 2] * s);
                glVertex3fv(&vtx[3 * k]);
                k = tris[j].c;
                //glTexCoord2fv(verts[k].tmap);
                //glTexCoord3fv(&vox[3 * k]);
                glNormal3fv(&nrm[3 * k]);
                glTexCoord3f(vox[3 * k + 0] * s, vox[3 * k + 1] * s, vox[3 * k + 2] * s);
                glVertex3fv(&vtx[3 * k]);
            }
            glEnd();
            // Calculate boundaries of current pose.
            {
                int nvtx = msh->numverts;

                loopj(nvtx) {
                    float* v = &vtx[3 * j];
                    /*
                    loopk(3) {
                        mins[k] = (v[k] < mins[k]) ? v[k] : mins[k];
                        maxs[k] = (v[k] > maxs[k]) ? v[k] : maxs[k];
                    }
                     */
                    float r = v[0] * v[0] + v[2] * v[2];
                    radius = (r > radius) ? r : radius;
                    height = v[1] > height ? v[1] : height;
                }
            }
            //
            delete[] vtx;
            delete[] nrm;
            // Process next mesh of model in next iteration.
            msh = MD5Format::getNextMesh(msh);
        }
        radius = sqrt(radius);
        //cout << " Model-Dimensions: r = " << radius << " h = " << height << "\n";
        //cout << " Model-Dimensions: (" << mins[0] << " " << mins[1] << " " << mins[2] << ") (" << maxs[0] << " " << maxs[1] << " " << maxs[2] << ")\n";
        glUseProgramObjectARB(0);
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
    if (vel != NULL && ori0 != NULL) {
        quat ori_inv;
        quat_cpy(ori_inv, ori0);
        quat_conj(ori_inv);
        quat_apply(v, ori_inv, vel);
    } else if (vel != NULL) {
        vector_cpy(v, vel);
    } else {
        vector_zero(v);
    }

    float fwdvel = copysign(sqrtf(v[0] * v[0] + v[2] * v[2]), v[2]) * -0.16f;
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
    if (bf * l1 > 0) l1 *= l1scale;
    if (bf * r1 > 0) r1 *= l1scale;

    float l2scale = 0.45;
    if (bf * l2 < 0) l2 *= l2scale;
    if (bf * r2 < 0) r2 *= l2scale;

    float l3scale = 0.25;
    if (bf * l3 > 0) l3 *= l3scale;
    if (bf * r3 > 0) r3 *= l3scale;

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
    delete[] manipulators;
}

std::string rRigged::resolveFilename(std::string modelname) {
    std::map<string,string> m2f;
    
    m2f["frogger"] = "data/base/device/wanzer/frogger/frogger.md5mesh";
    m2f["gorilla_ii"] = "data/base/device/wanzer/gorilla/gorilla_ii.md5mesh";
    m2f["lemur"] = "data/base/device/wanzer/lemur/lemur.md5mesh";
    m2f["kibitz"] = "data/base/device/wanzer/kibitz/kibitz.md5mesh";

    m2f["pod"] = "data/base/device/turret/pod/pod.md5mesh";

    m2f["bug"] = "data/base/device/tank/bug/bug.md5mesh";
    m2f["ant"] = "data/base/device/tank/ant/ant.md5mesh";
    m2f["warbuggy"] = "data/base/device/tank/warbuggy/warbuggy.md5mesh";

    m2f["flopsy"] = "data/com/blendswap/flopsy/flopsy.md5mesh";

    m2f["scorpion"] = "data/org/opengameart/scorpion/scorpion.md5mesh";
    m2f["thunderbird"] = "data/org/opengameart/thunderbird/thunderbird.md5mesh";

    m2f["gausscan"] = "data/org/opengameart/gausscan/gausscan.md5mesh";
    m2f["twinblaster"] = "data/org/opengameart/twinblaster/twinblaster.md5mesh";
    m2f["reactor"] = "data/org/opengameart/reactor/reactor.md5mesh";

    m2f["soldier"] = "/media/44EA-7693/workspaces/mm3d/soldier/soldier.md5mesh";

    return m2f[modelname];
}

void rRigged::loadModel(std::string filename) {
    model = MD5Format::mapMD5Mesh(filename.c_str());

    if (!true) cout << MD5Format::getModelStats(model) << endl;

    // Make joints local for later animation and transformation to global.
    MD5Format::joint* joints_ = MD5Format::getJoints(model);
    MD5Format::toLocalJoints(model->numJoints, joints_, joints_);

    // Rotate around X to swap Y/Z.
    quat convaxes;
    vec3 xaxis = {1, 0, 0};
    quat_rotaxis(convaxes, (-0.5f * M_PI), xaxis);
    quat_mul(joints_[0].q, joints_[0].q, convaxes);

    // FIXME: Rotate yaw 180 to make frontfacing.
    if (true) {
        quat convfacing;
        vec3 yaxis = {0, 1, 0};
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
                    float* a = &cur_baseverts[3 * tris[j].a];
                    float* b = &cur_baseverts[3 * tris[j].b];
                    float* c = &cur_baseverts[3 * tris[j].c];
                    float ab[3];
                    float bc[3];
                    float n[3];
                    vector_sub(ab, b, a);
                    vector_sub(bc, c, b);
                    vector_cross(n, bc, ab);
                    vector_norm(n, n);
                    vector_cpy(&cur_basenorms[3 * tris[j].a], n);
                    vector_cpy(&cur_basenorms[3 * tris[j].b], n);
                    vector_cpy(&cur_basenorms[3 * tris[j].c], n);
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
        glTranslatef(pos0[0], pos0[1], pos0[2]);
        GLS::glRotateq(ori0);
        //cPrimitives::glAxis(3.0f);
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
            glTranslatef(pos0[0], pos0[1], pos0[2]);
            GLS::glRotateq(ori0);
            drawBones();
        }
        glPopMatrix();
    }
    // Draw jumpjet exaust if jet is somewhat on.
    if (jetting > 0.3f) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        {
            GLS::glUseProgram_fgaddcolor();

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

                        glTranslatef(pos0[0], pos0[1], pos0[2]);
                        GLS::glRotateq(ori0);
                        glTranslatef(v[0], v[1], v[2]);

                        float n[16];
                        GLS::glGetTransposeInverseRotationMatrix(n);
                        glMultMatrixf(n);

                        glColor4f(1, 1, 0.3, 0.6);
                        Primitive::glDisk(7, f + 0.0003 * (rand() % 100));
                        glColor4f(1, 0.5, 0.3, 0.7);
                        Primitive::glDisk(7, f * 0.6 + 0.001 * (rand() % 100));
                        glColor4f(1, 1, 1, 0.8);
                        Primitive::glDisk(7, f * 0.3 + 0.001 * (rand() % 100));
                    }
                    glPopMatrix();
                } // if
            } // loopi
        }
        glPopAttrib();
    }
}

