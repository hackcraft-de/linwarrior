/* 
 * File:    IModel.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on April 21, 2012, 5:26 PM
 */


#ifndef IMODEL_H
#define	IMODEL_H

/**
 * Interface for models that have a position for
 * spatial-indexing and that can be rendered.
 */
class IModel {
public:
    virtual int getPosX() = 0;
    virtual int getPosY() = 0;
    virtual int getPosZ() = 0;
    virtual void animate(float spf) = 0;
    virtual void transform() = 0;
    virtual void drawSolid() = 0;
    virtual void drawEffect() = 0;
};

#endif	/* IMODEL_H */

