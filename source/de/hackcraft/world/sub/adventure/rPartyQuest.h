/* 
 * File:    rPartyQuest.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 11:49 PM
 */


#ifndef RPARTYQUEST_H
#define	RPARTYQUEST_H

#include "de/hackcraft/world/Component.h"

class rParty;
class rQuest;


class rPartyQuest : public Component {
public:
    rPartyQuest();
    virtual ~rPartyQuest();
private:
    enum State {
        INACTIVE,
        ACTIVE,
        SUCCESS,
        FAILED
    };
    rParty* party;
    rQuest* quest;
    State state;
};

#endif	/* RPARTYQUEST_H */

