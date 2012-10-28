/* 
 * File:    rParty.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on October 13, 2012, 11:49 PM
 */


#ifndef RPARTY_H
#define	RPARTY_H

class rParty;

#include "de/hackcraft/world/Component.h"

#include <map>
#include <vector>

class rAvatar;
class rPartyAvatar;
class rPartyQuest;
class rQuest;

class rParty : public Component {
public:
    rParty();
    virtual ~rParty();
private:
    std::vector<rQuest*> quests;
    std::map<rQuest*,rPartyQuest*> partyQuests;
    std::map<rAvatar*,rPartyAvatar*> avatarParties;
};

#endif	/* RPARTY_H */

