/* 
 * File:    Border.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 14, 2012, 8:54 PM
 */


#ifndef BORDER_H
#define	BORDER_H

class Border;

class Border {
public:
    Border();
    virtual ~Border();
    double getLeft();
    double getRight();
    double getTop();
    double getBottom();
private:

};

#endif	/* BORDER_H */

