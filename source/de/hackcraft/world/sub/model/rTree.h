/* 
 * File:   rTree.h
 * Author: benben
 *
 * Created on September 25, 2011, 10:27 AM
 */

#ifndef RTREE_H
#define	RTREE_H

struct rTree;

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/IModel.h"

#include "de/hackcraft/world/OID.h"

#include "de/hackcraft/psi3d/math3d.h"
#include "de/hackcraft/psi3d/GLS.h"

#include <list>
#include <map>

class rPlanetmap;

/**
 * Generates and caches proceduraly generated 3d tree models.
 */
struct rTree : public Component, public IModel {
public: // SYSTEM
    /// Identifier for this component (all uppercase letters without leading "r").
    static std::string cname;
    /// A unique random number (0-9999) to identify this component.
    static unsigned int cid;
public: // INPUT
    /// Base position vector.
    vec3 pos0;
    /// Base orientation quaternion.
    quat ori0;
    /// Seconds since spawn for leaf animation.
    float seconds;
public: // OUTPUT
protected: // INTERNALS
    friend class rPlanetmap;
    struct TreeType {
        // Random seed that produced this tree.
        unsigned int seed;
        // Type of tree, equals leaves.
        unsigned char type;
        // Age of tree > 0, equals depth of branching.
        unsigned char age;
        // Compiled display list.
        unsigned int list;
        // Experimental: Tree leaf billboard locations (3 coords each).
        std::vector<float> leaves;
        // Experimental: Tree height.
        float height;
    };
    TreeType* tree;
public:
    rTree(Entity* obj, float* pos = NULL, float* rot = NULL, int seed = 0, int type = 0, int age = 0);
    
    virtual int getPosX() { return (int) pos0[0]; }
    virtual int getPosY() { return (int) pos0[1]; }
    virtual int getPosZ() { return (int) pos0[2]; }
    virtual void animate(float spf);
    virtual void transform() {};
    virtual void drawSolid();
    virtual void drawEffect();

    virtual float constrain(float* worldpos, float radius = 0.0f, float* localpos = NULL, Entity* enactor = NULL);
private:
    /// Instance counter for init only.
    static int sInstances;
    /// Instance shared Textures.
    static std::vector<long> sLeaftexs;
    static std::vector<long> sBarktexs;
    /// Instance shared Tree cache.
    static std::map<OID,TreeType*> sTrees;

    static TreeType* getCompiledTree(int seed, int type, int age);
    static int drawTreePart(int depth, int maxdepth, float length, int seed, GLuint trunk_displaylist, GLuint leaf_displaylist, std::vector<float>* leaves, float* totalheight);
    static void drawRubberTreeLeaf();
    static void drawCaribeanTreeLeaf();
    static void drawHalmTreeLeaf();
    static void drawButterflyTreeLeaf();
    static void drawTrunk(float segments = 9, float vrepeat = 1);
};


#endif	/* RTREE_H */

