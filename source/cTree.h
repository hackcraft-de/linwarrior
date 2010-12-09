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
class cTree : public cObject {
    friend class cPlanetmap;
    struct rTree {
        unsigned int seed;
        unsigned char type;
        unsigned char age; // > 0
        unsigned int list;
    };
    rTree tree;
public:
    cTree(float* pos, float* rot, int seed, int type, int age);
    
    virtual void drawSolid();

    virtual float constrainParticle(float* worldpos, float radius = 0.0f, float* localpos = NULL, cObject* enactor = NULL);
private:
    //static std::list<rTree*> cache;
    static std::map<OID,rTree*> cache;
    static GLuint compileTreeDisplaylist(int seed, int type, int age);
    static int drawTreePart(int depth, int maxdepth, float length, int seed, GLuint trunk_displaylist, GLuint leaf_displaylist);
    static void drawRubberTreeLeaf();
    static void drawCaribeanTreeLeaf();
    static void drawHalmTreeLeaf();
    static void drawButterflyTreeLeaf();
};


#endif	/* _CTREE_H */

