/* 
 * File:    rQuest.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 11:48 PM
 */


#ifndef RQUEST_H
#define	RQUEST_H

#include "de/hackcraft/world/Component.h"

class rInventory;


class rQuest : public Component {
public:
    rQuest();
    virtual ~rQuest();
private:
    rInventory* requirements;
    rInventory* rewards;
};

#endif	/* RQUEST_H */

