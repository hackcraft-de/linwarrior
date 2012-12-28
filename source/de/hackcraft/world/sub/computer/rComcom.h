/* 
 * File:     rComcom.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on August 4, 2009, 8:44 PM
 */

#ifndef _CCOMCOM_H
#define	_CCOMCOM_H

struct rComcom;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Component.h"

#include "de/hackcraft/world/sub/computer/IComputer.h"

#include <list>

class Console;

/** Communication Computer
 *  encapsulates sending and receiving of messages
 *  and stores these messages.
 *  Orders to comrades may enhance strategic advance.
 */
struct rComcom : public Component, public IComputer {
public:
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
protected:
    Console* mConsole;
    OID mLastMessage;
public:
    rComcom(Entity* obj = NULL);
    virtual void animate(float spf);
    virtual void drawHUD();
};

#endif	/* _CCOMCOM_H */

