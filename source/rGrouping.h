/* 
 * File:     rGrouping.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on April 3, 2011, 8:40 PM
 */

#ifndef RGROUPING_H
#define	RGROUPING_H

#include "rComponent.h"
#include "OID.h"

/**
 * Object Group - as for now intended for messaging maillist-groups.
 */
struct rGrouping : public rComponent {
    /// Group name.
    std::string name;
    /// Lists registered members of this group.
    std::set<OID> members;

    /// Constructor
    rGrouping(cObject* obj = NULL);
    /// Copy Constructor
    rGrouping(rGrouping * original);
    /// Clone this.
    virtual rComponent* clone();
};

#endif	/* RGROUPING_H */

