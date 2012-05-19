/* 
 * File:     rTrigger.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 22, 2012, 9:33 PM
 */

#ifndef RTRIGGER_H
#define	RTRIGGER_H

#include "de/hackcraft/world/Component.h"
#include "de/hackcraft/world/OID.h"

#include "de/hackcraft/psi3d/math3d.h"

#include <set>
#include <algorithm>

/**
 * At this moment only a spot with an id and tags
 * for triggering alarms of different kinds.
 */
class rTrigger : public Component {
public:
    OID id;
    vec3 pos0;
    /** Tags (IDs, Social-Roles, Parties, States...) this trigger belongs to. */
    std::set<OID> tags;
public:
    rTrigger(Entity* obj = NULL);
    //rTrigger();
    //rTrigger(const rTrigger& orig);
    //virtual ~rTrigger();
    
    void drawSystemEffect() {};

    /** Check wether this Object has at least one of the given tags. */
    bool anyTags(std::set<OID>* tagset) {
        std::set<OID> result;
        std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
        return (!result.empty());
    }

    /** Check wether this Object has all the given tags. */
    bool allTags(std::set<OID>* tagset) {
        std::set<OID> result;
        std::set_intersection(tags.begin(), tags.end(), tagset->begin(), tagset->end(), std::inserter(result, result.begin()));
        return (result.size() == tagset->size());
    }

    /** Check wether this Object has the given tag. */
    bool hasTag(OID tag) {
        return (tags.find(tag) != tags.end());
    }

    /** Add a tag to this object. */
    void addTag(OID tag) {
        tags.insert(tag);
    }

    /** Remove a tag from this object. */
    void remTag(OID tag) {
        tags.erase(tag);
    }
private:

};

#endif	/* RTRIGGER_H */

