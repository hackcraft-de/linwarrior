/* 
 * File:    rPartyAvatar.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 11:48 PM
 */


#ifndef RPARTYAVATAR_H
#define	RPARTYAVATAR_H

#include "de/hackcraft/world/Component.h"


class rPartyAvatar : public Component {
public:
    rPartyAvatar();
    virtual ~rPartyAvatar();
private:
    enum State {
        INVITED,
        APPLIED,
        BANNED,
        JOINED,
        ACTIVE
    };
};

#endif	/* RPARTYAVATAR_H */

