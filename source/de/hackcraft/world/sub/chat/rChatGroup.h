/* 
 * File:    rChatGroup.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on May 12, 2012, 2:12 PM
 */


#ifndef RCHATGROUP_H
#define	RCHATGROUP_H

class rChatGroup;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Component.h"

#include <list>

/**
 * Groups objects together for searching, messaging, tagging, ...
 */
class rChatGroup : public Component {
public:
    rChatGroup();
    ~rChatGroup();
public:
    /** Group id used for reference. */
    OID id;
    /** Group id of parent group if not zero. */
    //OID parent;
    /** Name of the group (use qualified names like: "world/enemies/squads/alpha" or "state/dead"). */
    std::string name;
    /** Objects in this group. */
    std::list<OID> members;
};

#endif	/* RCHATGROUP_H */

