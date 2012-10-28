/* 
 * File:    rAvatar.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 11:50 PM
 */


#ifndef RAVATAR_H
#define	RAVATAR_H

#include "de/hackcraft/world/Component.h"

class rInventory;
class rParty;


class rAvatar : public Component {
public:
    rAvatar();
    virtual ~rAvatar();
private:
    std::vector<rInventory*> inventories;
    std::vector<rParty*> parties;
};

#endif	/* RAVATAR_H */

