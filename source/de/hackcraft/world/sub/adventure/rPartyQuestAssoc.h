/* 
 * File:    rPartyQuestAssoc.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 11:49 PM
 */


#ifndef RPARTYQUESTASSOC_H
#define	RPARTYQUESTASSOC_H

#include "de/hackcraft/world/Component.h"

class rParty;
class rQuest;


class rPartyQuestAssoc : public Component {
public:
    rPartyQuestAssoc();
    virtual ~rPartyQuestAssoc();
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

#endif	/* RPARTYQUESTASSOC_H */

