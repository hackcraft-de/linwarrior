/* 
 * File:    ChatSystem.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on May 12, 2012, 1:49 PM
 */


#ifndef CHATSYSTEM_H
#define	CHATSYSTEM_H

class ChatSystem;

#include "de/hackcraft/world/sub/chat/rChatGroup.h"
#include "de/hackcraft/world/sub/chat/rChatMember.h"
#include "de/hackcraft/world/sub/chat/rChatMessage.h"

#include "de/hackcraft/world/Subsystem.h"

#include <queue>
#include <list>
#include <map>
#include <string>

class ChatSystem : public Subsystem {
public:
    ChatSystem();
    virtual ~ChatSystem();

    /** Get current singleton instance.*/
    static ChatSystem* getInstance();

    /** Finds and returns a group or creates a new one. */
    OID getGroup(std::string name);

    /** Adds a managed member to an existing group. */
    void addToGroup(OID gid, Entity* member);
    
    /** Adds a chat member to the managed members (without adding to group) */
    void add(rChatMember* member);
    
    /** For resolving an entity's currently registered chat member. */
    rChatMember* findChatMemberByEntity(OID entityID);
    
    /**
     * Sent messages are currently collected with an attached timestamp.
     * @param delay time offset for sending see getOID() for calculation.
     * @param sender a value of 0 currently defines a broadcast.
     * @param recvid is the receiver the message is sent to - 0 null/def receiver.
     * @param text the message subject/text to send, will be copied to string.
     * @param blob binary data, you give away ownership may be deleted by world.
     */
    virtual void sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, std::string type, std::string text, void* blob = NULL);
    
    /** Dispatches messages to their receivers and updates queue states. */
    void dispatchMessages();
    
private:
    /** Chat or communication participants (Communication computers). */
    std::map<OID,rChatMember*> members;
    
    /** Groups of objects */
    std::map<OID,rChatGroup*> groups;

    /** For resolving an entity's currently registered chat member. */
    std::map<OID,rChatMember*> memberByEntity;
    
    /** Not yet dispatched messages (because they are not yet overdue). */
    std::priority_queue<rChatMessage*, std::vector<rChatMessage*>, rChatMessage> mMessages;

    /** Messages already delivered to group participants. */
    std::list<rChatMessage*> mDispatchedMessages;

    /** Current singleton instance */
    static ChatSystem* instance;
};

#endif	/* CHATSYSTEM_H */

