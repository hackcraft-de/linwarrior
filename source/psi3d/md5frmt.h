/* 
 * File:   md5frmt.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on July 5, 2009, 6:01 PM
 */


#ifndef _MD5FRMT_H_
#define	_MD5FRMT_H_

#include "math3d.h"

#include <stdlib.h>

// memcpy, memset
#include <string.h>

#include <cassert>

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

// Yes, md5format commandlines can get very very long.
#define MD5COMMLINE 4096
#define MD5SHADERNAME 256
#define MD5JOINTNAME 64
#define MD5MESHNAME 64
#define MD5LINESIZE 4096

struct MD5Format {

public: // STRUCTURES

    struct animation {
        int version;
        char commandline[MD5COMMLINE];
        int numJoints;
        int numFrames;
        int frameRate;
        int numAnimatedComponents;
        // joint joints[numJoints]
        // bound bounds[numFrames]
        // float frames[numFrames][numAnimatedComponents]
    };

    struct bound {
        float min[3];
        float max[3];
    };

    struct model {
        int version;
        char commandline[MD5COMMLINE];
        int numJoints;
        int numMeshes;
        // joint joints[numJoints]
        // mesh meshes[numMeshes]
    };

    struct joint {
        char name[MD5JOINTNAME];
        int parent;
        float v[4], q[4];
    };

    struct mesh {
        char name[MD5MESHNAME];
        char shader[MD5SHADERNAME];
        int material;
        int numverts;
        int numtris;
        int numweights;
        // vert verts[numverts]
        // tri tris[numtris]
        // weight weights[numweights]
    };

    struct vert {
        int idx;
        float tmap[2];
        int start, length;
    };

    struct tri {
        int idx;
        int a, b, c;
    };

    struct weight {
        int idx, joint;
        float bias;
        float v[3];
    };

public: // QUERIES

    static inline size_t sizeofMesh(mesh * meshptr) {
        return sizeof (mesh)
                + sizeof (vert) * meshptr->numverts
                + sizeof (tri) * meshptr->numtris
                + sizeof (weight) * meshptr->numweights;
    }

    static inline size_t sizeofModel(model * modelptr) {
        size_t size = sizeof (model) + sizeof (joint) * modelptr->numJoints;
        int i = modelptr->numMeshes;
        mesh* m = getFirstMesh(modelptr);
        while (i-- > 0) {
            size += sizeofMesh(m);
            m = getNextMesh(m);
        }
        return size;
    }

    static inline mesh * getMesh(model* modelptr, int idx) {
        char* curr = (char*) getFirstMesh(modelptr);
        while (idx-- > 0) curr = (char*) getNextMesh((mesh*) curr);
        return ((mesh*) curr);
    }

    static inline mesh * getFirstMesh(model * modelptr) {
        joint* joints = (joint*) (modelptr + 1);
        mesh* first = (mesh*) (joints + modelptr->numJoints);
        return first;
    }

    static inline mesh * getNextMesh(mesh * curr) {
        char* next = ((char*) curr) + sizeofMesh(curr);
        return ((mesh*) next);
    }

    static inline joint * getJoints(model * modelptr) {
        return ( (joint*) (modelptr + 1));
    }

    static inline vert * getVerts(mesh * meshptr) {
        return ( (vert*) (meshptr + 1));
    }

    static inline tri * getTris(mesh * meshptr) {
        vert* verts = getVerts(meshptr);
        tri* tris = (tri*) (verts + meshptr->numverts);
        return tris;
    }

    static inline weight * getWeights(mesh * meshptr) {
        tri* tris = getTris(meshptr);
        weight* weights = (weight*) (tris + meshptr->numtris);
        return weights;
    }

    static inline int findJoint(model* modelptr, const char* name) {
        joint* joints = getJoints(modelptr);
        for (int i = 0; i < modelptr->numJoints; i++) {
            if (strcasecmp(joints[i].name, name) == 0) return i;
        }
        return -1;
    }
    
    static string getModelStats(model * mod) {
        stringstream s;
        s << "version " << mod->version << endl;
        s << "numJoints " << mod->numJoints << endl;
        s << "numMeshes " << mod->numMeshes << endl;
        s << endl;
        joint* j = (joint*) (mod + 1);
        for (int i = 0; i < mod->numJoints; i++) {
            s << j->name << endl;
            j++;
        }
        mesh* m = (mesh*) j;
        for (int i = 0; i < mod->numMeshes; i++) {
            s << endl;
            s << m->name << endl;
            s << m->shader << endl;
            s << "numverts " << m->numverts << endl;
            s << "numtris " << m->numtris << endl;
            s << "numweights " << m->numweights << endl;
            m = (mesh*) (((char*) m) + sizeofMesh(m));
        }
        return s.str();
    }

protected: // PARSING UTILITIES

    static vert * readVerts(FILE* f, int numverts) {
        vert* verts = NULL;
        try {

            verts = new vert[numverts];
            if (verts == NULL) throw "no memory for mesh verts";
            char line[1024];
            int vertcount = 0;
            while (vertcount != numverts) {
                char* result = fgets(line, sizeof (line), f);
                if (feof(f) != 0 || result == NULL) throw "unexpected end of file when reading mesh verts";
                vert v;
                int r = sscanf(line, " vert %i ( %f %f ) %i %i", &v.idx, (&v.tmap[0]), (&v.tmap[1]), &v.start, &v.length);
                if (r == 5) {
                    memcpy(&verts[vertcount], &v, sizeof (vert));
                    vertcount++;
                }
            }
            return verts;

        } catch (...) {
            delete verts;
            throw;
        }
    }

    static tri * readTris(FILE* f, int numtris) {
        tri* tris = NULL;
        try {

            tris = new tri[numtris];
            if (tris == NULL) throw "no memory for mesh tris";
            char line[1024];
            int tricount = 0;
            while (tricount != numtris) {
                char* result = fgets(line, sizeof (line), f);
                if (feof(f) != 0 || result == NULL) throw "unexpected end of file when reading mesh tris";
                tri t;
                int r = sscanf(line, " tri %i %i %i %i", &t.idx, &t.a, &t.b, &t.c);
                if (r == 4) {
                    memcpy(&tris[tricount], &t, sizeof (tri));
                    tricount++;
                }
            }
            return tris;

        } catch (...) {
            delete tris;
            throw;
        }
    }

    static weight * readWeights(FILE* f, int numweights) {
        weight* weights = NULL;
        try {

            weights = new weight[numweights];
            if (weights == NULL) throw "no memory for mesh weights";
            char line[1024];
            int weightcount = 0;
            while (weightcount != numweights) {
                char* result = fgets(line, sizeof (line), f);
                if (feof(f) != 0 || result == NULL) throw "unexpected end of file when reading mesh weights";
                weight w;
                int r = sscanf(line, " weight %i %i %f ( %f %f %f )", &w.idx, &w.joint, &w.bias, (&w.v[0]), (&w.v[1]), (&w.v[2]));
                if (r == 6) {
                    memcpy(&weights[weightcount], &w, sizeof (weight));
                    weightcount++;
                }
            }
            return weights;

        } catch (...) {
            delete weights;
            throw;
        }
    }

    static mesh * readMesh(FILE * f) {
        char* chunk = NULL;
        vert* verts = NULL;
        tri* tris = NULL;
        weight* weights = NULL;

        try {

            mesh m;
            m.name[0] = '\0';
            m.shader[0] = '\0';
            m.material = -1;
            m.numverts = 0;
            m.numtris = 0;
            m.numweights = 0;

            while (1) {
                char line[1024];
                char* result = fgets(line, sizeof (line), f);
                if (line[0] == '}') break;
                if (feof(f) != 0 || result == NULL) throw "unexpected end of file when reading mesh";
                //cout << line;

                // Name of mesh - this is only an optional comment
                int r = sscanf(line, " // meshes: %[^\n]", m.name);
                if (r == 1) {
                    //cout << "meshes " << m.name << endl;
                    continue;
                }

                // Name of mesh shader/material
                r = sscanf(line, " shader \"%[^\"]\"", m.shader);
                if (r == 1) {
                    //cout << "shader " << m.shader << endl;
                    continue;
                }

                // Eat verts
                r = sscanf(line, " numverts %i", &m.numverts);
                if (r == 1) {
                    if (verts != NULL) throw "duplicate vertlist";
                    //cout << "numverts " << m.numverts << endl;
                    verts = (vert*) readVerts(f, m.numverts);
                    continue;
                } // if numverts

                // Eat tris
                r = sscanf(line, " numtris %i", &m.numtris);
                if (r == 1) {
                    if (tris != NULL) throw "duplicate trilist";
                    //cout << "numtris " << m.numtris << endl;
                    tris = (tri*) readTris(f, m.numtris);
                    continue;
                } // if numtris

                // Eat weights
                r = sscanf(line, " numweights %i", &m.numweights);
                if (r == 1) {
                    if (weights != NULL) throw "duplicate weightlist";
                    //cout << "numweights " << m.numweights << endl;
                    weights = (weight*) readWeights(f, m.numweights);
                    continue;
                } // if numweights

            } // while (1) if EOF break

            if (verts == NULL) throw "missing verts";
            if (tris == NULL) throw "missing tris";
            if (weights == NULL) throw "missing weights";

            chunk = new char[sizeofMesh(&m)];
            if (chunk == NULL) throw "no memory for mesh chunk";

            char* chunk_ptr = chunk;
            memcpy(chunk_ptr, &m, sizeof (mesh));

            chunk_ptr += sizeof (mesh);
            memcpy(chunk_ptr, verts, sizeof (vert) * m.numverts);

            chunk_ptr += sizeof (vert) * m.numverts;
            memcpy(chunk_ptr, tris, sizeof (tri) * m.numtris);

            chunk_ptr += sizeof (tri) * m.numtris;
            memcpy(chunk_ptr, weights, sizeof (weight) * m.numweights);

            delete verts;
            delete tris;
            delete weights;

            return (mesh*) chunk;

        } catch (...) {
            delete verts;
            delete tris;
            delete weights;
            delete chunk;
            throw;
        }
    }

    static joint * readJoints(FILE* f, int numjoints) {
        joint* joints = NULL;

        try {
            joints = new joint[numjoints];
            if (joints == NULL) throw "no memory for mesh joints";

            int jointcount = -1;
            while (1) {
                char line[1024];
                char* result = fgets(line, sizeof (line), f);
                if (line[0] == '}' || result == NULL) break;
                if (feof(f) != 0) throw "unexpected end of file when reading joints";
                //cout << line;

                if (jointcount == -1) {
                    char s[1024];
                    int r = sscanf(line, " %s {", s);
                    if (r == 1 && strcmp("joints", s) == 0) {
                        //cout << "starting joints" << endl;
                        jointcount = 0;
                        continue;
                    }
                } else {
                    joint j;
                    int r = sscanf(line, " \"%[^\"]\" %i ( %f %f %f ) ( %f %f %f )", j.name, &j.parent, (&j.v[0]), (&j.v[1]), (&j.v[2]), (&j.q[0]), (&j.q[1]), (&j.q[2]));
                    if (r == 8) {
                        j.v[3] = 0.0f;
                        float w2 = 1.0f - (j.q[0] * j.q[0] + j.q[1] * j.q[1] + j.q[2] * j.q[2]);
                        j.q[3] = (w2 <= 0) ? 0.0f : -sqrt(w2);
                        if (jointcount >= numjoints) throw "too many joints";
                        memcpy(&joints[jointcount], &j, sizeof (joint));
                        //cout << j.name << " " << j.parent << " (" << j.v[0] << " " << j.v[1] << " " << j.v[2] << " ) ( " << j.q[0] << " " << j.q[1] << " " << j.q[2] << " )" << endl;
                        jointcount++;
                    }
                }
            }
            if (jointcount < numjoints) throw "too few joints";
            return joints;

        } catch (...) {
            delete joints;
            throw;
        }
    }

public: // LOADING

    static model * mapMD5Mesh(const char* filename) {
        joint* joints = NULL;
        mesh** meshes = NULL;
        char* modelchunk = NULL;
        model m;
        m.version = 0;
        m.commandline[0] = '\0';
        m.numJoints = 0;
        m.numMeshes = 0;
        try {

            FILE* f = fopen(filename, "rt");
            if (f == NULL) throw "could not open file for reading";
            int r;

            // MD5Version

            int header = 0;
            int flags = ( 1 + 0 + 4 + 8 );
            while ((header & flags) != flags) {
                char line[MD5LINESIZE];
                char* result = fgets(line, sizeof (line), f);
                if (feof(f) != 0 || result == NULL) throw "unexpected end of file when reading header";

                r = sscanf(line, " MD5Version %i", &m.version);
                if (r == 1) header |= 1;

                r = sscanf(line, " commandline \"%[^\"]\"", m.commandline);
                if (r == 1) header |= 2; // text optional, empty string otherwise.

                r = sscanf(line, " numJoints %i", &m.numJoints);
                if (r == 1) header |= 4;

                r = sscanf(line, " numMeshes %i", &m.numMeshes);
                if (r == 1) header |= 8;
            }

            //cout << "MD5Version " << m.version << endl;
            //cout << "commandline " << m.commandline << endl;
            //cout << "numJoints " << m.numJoints << endl;
            //cout << "numMeshes " << m.numMeshes << endl;

            joints = (joint*) readJoints(f, m.numJoints);

            unsigned long chunksize = sizeof (model) + sizeof (joint) * m.numJoints;

            // Meshes
            typedef mesh* mesh_star;
            meshes = new mesh_star[m.numMeshes];
            if (meshes == NULL) throw "no memory for model meshes";
            memset(meshes, 0, sizeof (mesh_star) * m.numMeshes);
            for (int i = 0; i < m.numMeshes; i++) {
                meshes[i] = (mesh*) readMesh(f);
                chunksize += sizeofMesh(meshes[i]);
            } // for numMeshes

            fclose(f);

            modelchunk = new char[chunksize];
            if (modelchunk == NULL) throw "no memory for model chunk";

            char* chunk_ptr = modelchunk;
            memcpy(chunk_ptr, &m, sizeof (model));

            chunk_ptr += sizeof (model);
            memcpy(chunk_ptr, joints, sizeof (joint) * m.numJoints);

            chunk_ptr += sizeof (joint) * m.numJoints;
            for (int i = 0; i < m.numMeshes; i++) {
                unsigned long meshsize = sizeofMesh(meshes[i]);
                memcpy(chunk_ptr, meshes[i], meshsize);
                chunk_ptr += meshsize;
            }

            //cout << chunksize << " vs " << (chunk_ptr - modelchunk) << endl;

            return (model*) modelchunk;

        } catch (...) {
            delete joints;
            delete modelchunk;
            if (meshes != NULL) {
                for (int i = 0; i < m.numMeshes; i++) delete meshes[i];
                delete meshes;
            }
            throw;
        }
    }

public: // TESTING

    static string formatJoint(float* p, float* q) {
        char buffer[1024];
        sprintf(buffer, "( %.4f %.4f %.4f ) ( %.4f %.4f %.4f %.4f )", p[0],p[1],p[2], q[0],q[1],q[2],q[3]);
        return string(buffer);
    }

    static void testMD5MeshLoad() {
        cout << "MD5 Reader" << endl;

        MD5Format::model* m = NULL;
        try {
            m = MD5Format::mapMD5Mesh("/home/benben/Desktop/workspaces/milk_workspace/diestel.md5mesh");
            //m = mapMD5Mesh("/home/benben/workspaces/mm3d-1.3.8/plugins/MD5Data/t.md5mesh");
        } catch (char* s) {
            cout << s << endl;
        }

        cout << "=======================================================" << endl;
        cout << MD5Format::getModelStats(m) << endl;

        cout << "=======================================================" << endl;
        MD5Format::joint* joints = (MD5Format::joint*) (m + 1);
        //joint* joints_ = new joint[m->numJoints];
        MD5Format::toLocalJoints(m->numJoints, joints, joints);
        MD5Format::toGlobalJoints(m->numJoints, joints, joints);
        delete m;
    }

public: // TRANSFORMING


    static void toLocalJoints(int numJoints, joint* joints, joint * joints_) {
        //cout << "toLocalJoints() -------------------------\n";
        joint* locals = joints_;
        if (joints == joints_) {
            locals = new joint[numJoints];
            assert(locals != NULL);
        }
        memcpy(locals, joints, sizeof (joint) * numJoints);
        joint* j = joints;
        joint* j_ = locals;
        for (int i = 0; i < numJoints; i++) {
            //cout << j->name << " " << i << " ";
            //cout << "parent " << j->parent << " ";
            float v_local[4];
            float q_local[4];
            if (j->parent < 0) {
                // origin, no change
                vector_cpy(v_local, j->v);
                quat_cpy(q_local, j->q);
            } else {
                // other joint, make local to parent.
                joint* p = &joints[j_->parent];

                float p_q[4];
                quat_cpy(p_q, p->q);
                quat_conj(p_q);

                float j_q[4];
                quat_cpy(j_q, j->q);
                quat_conj(j_q);

                // -----------
                
                vector_sub(v_local, j->v, p->v);
                quat_apply(v_local, p_q, v_local);

                quat_mul(q_local, j->q, p_q);
            }

            quat_rectify_neg(q_local);
            string s = formatJoint(v_local, q_local);
            //cout << s << endl;

            vector_cpy(j_->v, v_local);
            quat_cpy(j_->q, q_local);
            
            j++;
            j_++;
        }
        if (locals != joints_) {
            memcpy(joints_, locals, sizeof (joint) * numJoints);
            delete locals;
        }
    }

    static void toGlobalJoints(int numJoints, joint* joints, joint * joints_, joint* manipulators = NULL) {
        //cout << "toGlobalJoints() -------------------------\n";
        joint* globals = joints_;
        if (joints == joints_) {
            globals = new joint[numJoints];
            assert(globals != NULL);
        }
        memcpy(globals, joints, sizeof (joint) * numJoints);
        joint* j = joints;
        joint* j_ = globals;
        for (int i = 0; i < numJoints; i++) {
            //cout << j->name << " " << i << " ";
            //cout << "parent " << j->parent << " ";
            float v_global[4];
            float q_global[4];
            if (j->parent < 0) {
                // origin, no change
                vector_cpy(v_global, j->v);
                quat_cpy(q_global, j->q);
            } else {
                joint* p = &globals[j->parent];

                // -----------

                quat_mul(q_global, j->q, p->q);

                quat_apply(v_global, p->q, j->v);
                vector_add(v_global, v_global, p->v);    
            }
            
            if (manipulators) {
                quat_mul(q_global, q_global, manipulators[i].q);
                vector_add(v_global, v_global, manipulators[i].v);
            }

            quat_rectify_neg(q_global);
            string s = formatJoint(v_global, q_global);
            //cout << s << endl;

            vector_cpy(j_->v, v_global);
            quat_cpy(j_->q, q_global);

            j++;
            j_++;
        }
        if (globals != joints_) {
            memcpy(joints_, globals, sizeof (joint) * numJoints);
            delete globals;
        }
    }
    
    static void animatedMeshVertices(mesh* msh, joint* joints, float* vertices) {
        vert* verts = getVerts(msh);
        weight* weights = getWeights(msh);
        float* v_ = vertices;
        int numverts = msh->numverts;
        vert* v = verts;
        while (numverts-- > 0) {
            //cout << v->start << " " << v->length << endl;
            vector_set(v_, 0, 0, 0);
            weight* w = &weights[v->start];
            //cout << w->idx << " " << w->v[0] << " " << w->v[1] << " " << w->v[2] << endl;
            int numweights = v->length;
            while (numweights-- > 0) {
                joint* j = &joints[w->joint];
                float q[4];
                quat_cpy(q, j->q);
                float temp[3];
                quat_apply(temp, q, w->v);
                vector_add(temp, temp, j->v);
                vector_scale(temp, temp, w->bias);
                vector_add(v_, v_, temp);
                w++;
            }
            v++;
            v_ += 3;
        }
    }

    static void animatedMeshNormals(mesh* msh, joint* joints, float* normals) {
        // TODO: implement animatedMeshNormals
    }

}; // MD5Format


#endif	/* _MD5FRMT_H */

