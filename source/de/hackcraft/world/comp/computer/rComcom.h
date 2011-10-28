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

#include "rComputer.h"

#include "de/hackcraft/world/OID.h"

#include <list>

/** Communication Computer
 *  encapsulates sending and receiving of messages
 *  and stores these messages.
 *  Orders to comrades may enhance strategic advance.
 */
struct rComcom : public rComputer {
protected:
    void* mConsole;
    OID mLastMessage;
public:
    rComcom(Entity* obj = NULL);
    virtual void animate(float spf);
    virtual void drawHUD();
};

#endif	/* _CCOMCOM_H */

