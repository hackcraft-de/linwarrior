/* 
 * File:     rChatMember.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on August 4, 2009, 8:44 PM
 */

#ifndef _RCHATMEMBER_H
#define	_RCHATMEMBER_H

struct rChatMember;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Component.h"

#include <list>

class rChatMessage;
class Console;

/** 
 * Chat member (Communication Computer)
 * encapsulates sending and receiving of messages
 * and stores these messages.
 * Orders to comrades may enhance strategic advantage.
 */
struct rChatMember : public Component {
public:
    /** Identifier for this component (all uppercase letters without leading "r"). */
    static std::string cname;
    /** A unique random number (0-9999) to identify this component. */
    static unsigned int cid;
public:
    /** Id of this instance. */
    OID id;
protected:
    /** Console instance for chat message output. */
    Console* mConsole;
    /** Timestamp of last received/displayed chat message. */
    OID mLastMessage;
public:
    rChatMember(Entity* obj = NULL);

    virtual void animate(float spf);
    virtual void drawHUD();
    
    void recvMessage(rChatMessage* msg);
};

#endif	/* _RCHATMEMBER_H */

