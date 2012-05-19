/* 
 * File:     rRigged.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:36 PM
 */

#ifndef RRIGGED_H
#define	RRIGGED_H

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/IModel.h"

#include "de/hackcraft/io/MD5Format.h"

#include <map>

/**
 * Encapsulates a rigged mesh in md5mesh format.
 */
struct rRigged : public Component, public IModel {
public: // SYSTEM
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
    /** Shared material bindings. */
    static std::map<std::string,unsigned long> materials;
public: // INPUT
    /** Model scale. (1.0f, unused/removed now) */
    float scale;
    /** Animation time counter in seconds. */
    float seconds;
    /** Position. (hook i) */
    vec3 pos0;
    /** Orientation. (hook i) */
    quat ori0;
    /** Velocity. (hook i) */
    vec3 vel;
    /** Grounded. (hook i) */
    float grounded;
    /** Jetting. (hook i) */
    float jetting;
    /** Basic 3d texture bind when greater 0. (still a hook i) */
    int basetexture3d;
public: // OUTPUT
    /** Actual local-(model-)space joints for this instance. */
    MD5Format::joint* joints;
    /** Maps jointpoint identifier to actual joint index of the model (-1 ~ NULL). */
    std::map<int, int> jointpoints;
    /** Joint angles for animation. */
    std::map<int, std::map<int, float> > rotators;
    /** Current model height - only messured above ground ie. > 0. (hook o) */
    float height;
    /** Current model radius as seen from above. (hook o) */
    float radius;
protected: // INTERNALS
    /** The "static" model just as it is loaded. */
    MD5Format::model* model;
    // Untransformed vertices for mesh i.
    std::map<int, float*> baseverts;
    /** Untransformed normals for mesh i. */
    std::map<int, float*> basenorms;
public:
    /** Enumeration for indexing joints in animation. */

    enum Jointpoints {
        EYE, HEADPITCH, HEADYAW,
        CTMOUNT, LAMOUNT, RAMOUNT, LSMOUNT, RSMOUNT, BKMOUNT,
        JET0, JET1, JET2, JET3, JET4,
        YAW, PITCH, LEFTLEG, RIGHTLEG, LEFTCALF, RIGHTCALF, LEFTFOOT, RIGHTFOOT, MAX_JOINTPOINTS
    };

    /** Constructor */

    rRigged(Entity* obj = NULL) : scale(1.0f), seconds(0.0f), grounded(0.0f), jetting(0.0f), basetexture3d(0), joints(NULL), height(0.1f), radius(0.1f), model(NULL) {
        object = obj;
        vector_zero(pos0);
        vector_zero(vel);
        quat_zero(ori0);
        initMaterials();
    }

    rRigged(rRigged * original) {
        assert(0);
        initMaterials();
    }

    /** Destructor */

    ~rRigged() {
        delete model;
        delete joints;
    }

    virtual Component * clone() {
        return new rRigged(this);
    }

    std::string getJointname(unsigned int num) {
        const char* names[] = {
            "EYE", "HEADPITCH", "HEADYAW",
            "CTMOUNT", "LAMOUNT", "RAMOUNT", "LSMOUNT", "RSMOUNT", "BKMOUNT",
            "JET0", "JET1", "JET2", "JET3", "JET4",
            "YAW", "PITCH", "LEFTLEG", "RIGHTLEG", "LEFTCALF", "RIGHTCALF", "LEFTFOOT", "RIGHTFOOT"
        };
        if (num >= MAX_JOINTPOINTS) return std::string("");
        return std::string(names[num]);
    }

    int getMountpoint(const char* point) {
        int jp = 0;
        if (strcmp(point, "LTorsor") == 0) jp = LSMOUNT;
        else if (strcmp(point, "LUpArm") == 0) jp = LSMOUNT;
        else if (strcmp(point, "LLoArm") == 0) jp = LAMOUNT;
        else if (strcmp(point, "RTorsor") == 0) jp = RSMOUNT;
        else if (strcmp(point, "RUpArm") == 0) jp = RSMOUNT;
        else if (strcmp(point, "RLoArm") == 0) jp = RAMOUNT;
        else if (point[0] == 'L') jp = LSMOUNT;
        else if (point[0] == 'R') jp = RSMOUNT;
        else if (point[0] == 'C') jp = CTMOUNT;
        else jp = BKMOUNT;
        return jointpoints[jp];
    }
    
    void initMaterials();

    void drawBones();
    void drawMeshes();

    void poseJumping(float spf);
    void poseRunning(float spf);

    void transformJoints();

    std::string resolveFilename(std::string modelname);
    void loadModel(std::string filename);

    virtual int getPosX() { return (int) pos0[0]; }
    virtual int getPosY() { return (int) pos0[1]; }
    virtual int getPosZ() { return (int) pos0[2]; }
    virtual void animate(float spf);
    virtual void transform();
    virtual void drawSolid();
    virtual void drawEffect();
    //virtual void drawHUD() { }
};


#endif	/* RRIGGED_H */

