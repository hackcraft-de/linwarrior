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

#include <vector>

class rItem;


class rQuest : public Component {
public:
    rQuest();
    virtual ~rQuest();
private:
    std::vector<rItem*> requirements;
    std::vector<rItem*> rewards;
};

#endif	/* RQUEST_H */

