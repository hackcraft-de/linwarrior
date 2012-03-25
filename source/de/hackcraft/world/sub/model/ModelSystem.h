/* 
 * File:    ModelSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on March 22, 2012, 10:52 PM
 */


#ifndef MODELSYSTEM_H
#define	MODELSYSTEM_H

#include "rBillboard.h"
#include "rRigged.h"
#include "rScatter.h"
#include "rTree.h"

#include "de/hackcraft/world/Subsystem.h"

#include <vector>

class ModelSystem : public Subsystem {
public:
    ModelSystem();
    ModelSystem(const ModelSystem& orig);
    virtual ~ModelSystem();
private:
    std::vector<rBillboard> billboardModels;
    std::vector<rRigged> riggedModels;
    std::vector<rScatter> scatterModels;
    std::vector<rTree> treeModels;
};

#endif	/* MODELSYSTEM_H */

