/**
 * File:     cTree.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 25, 2008, 10:48 PM
 */

#ifndef _CTREE_H
#define	_CTREE_H

class cTree;

#include "cObject.h"
#include <list>

class cPlanetmap;

/**
 * Generates and caches proceduraly generated 3d tree models.
 */
struct cTree : public cObject {
    friend class cPlanetmap;
    struct rTree {
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
    rTree* tree;
public:
    cTree(float* pos = NULL, float* rot = NULL, int seed = 0, int type = 0, int age = 0);
    
    virtual void animate(float spf);
    virtual void drawSolid();
    virtual void drawEffect();

    virtual float constrain(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL);
private:
    /// Instance counter for init only.
    static int sInstances;
    /// Instance shared Textures.
    static std::vector<long> sTextures;
    /// Instance shared Tree cache.
    static std::map<OID,rTree*> sTrees;

    static rTree* getCompiledTree(int seed, int type, int age);
    static int drawTreePart(int depth, int maxdepth, float length, int seed, GLuint trunk_displaylist, GLuint leaf_displaylist, std::vector<float>* leaves, float* totalheight);
    static void drawRubberTreeLeaf();
    static void drawCaribeanTreeLeaf();
    static void drawHalmTreeLeaf();
    static void drawButterflyTreeLeaf();
};


#endif	/* _CTREE_H */

