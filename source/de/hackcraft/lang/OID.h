/* 
 * File:     OID.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 23. Dezember 2010, 15:46
 */

#ifndef OID_H
#define	OID_H

/**
 * Serial-Code or OID for cObject (used for values, too).
 * Use whereever possible to store object references.
 * Object A having a pointer to cObject B is like saying A and B are bound together.
 * Pointers are ok between a Mech and its *own* weapon but not between
 * a targeting weapon (or Mech) and a hostile target Mech.
 * Only bind together what belongs together and what would be destroyed together.
 * Pointers express responsibility whereas OIDs loosely couple.
 * Use World::getInstance()->getObject(oid) to map serial to pointer.
 */
typedef unsigned long long OID;


#endif	/* OID_H */

