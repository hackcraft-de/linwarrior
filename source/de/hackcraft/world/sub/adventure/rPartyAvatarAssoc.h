/* 
 * File:    rPartyAvatarAssoc.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 11:48 PM
 */


#ifndef RPARTYAVATARASSOC_H
#define	RPARTYAVATARASSOC_H

#include "de/hackcraft/world/Component.h"


class rPartyAvatarAssoc : public Component {
public:
    rPartyAvatarAssoc();
    virtual ~rPartyAvatarAssoc();
private:
    enum State {
        INVITED,
        APPLIED,
        BANNED,
        JOINED,
        ACTIVE
    };
    State state;
};

#endif	/* RPARTYAVATARASSOC_H */

