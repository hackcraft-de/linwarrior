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

// Include or ChatMessage is necessary for priority queue template.
#include "de/hackcraft/world/sub/chat/rChatMessage.h"

#include "de/hackcraft/world/Subsystem.h"

#include <queue>
#include <list>
#include <map>
#include <string>

class Logger;
class rChatGroup;
class rChatMember;

class ChatSystem : public Subsystem {
public:
    /** Get current singleton instance.*/
    static ChatSystem* getInstance();

public:
    ChatSystem();
    virtual ~ChatSystem();

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

public:
    /**
     * Limit position to accessible areas and returns impact depth.
     * 
     * @param worldpos The particle position to be constrained given in world coordinates.
     * @param radius The size of the particle in terms of a radius.
     * @return Zero if there was no collision, else the maximum impact depth.
     */
    virtual float constrainParticle(Entity* ex, float* worldpos, float radius) { return 0.0f; };
    
public:
    /** Advance simulation time for one frame. */
    virtual void advanceTime(int deltamsec) {};
    
    /** Re-build spatial clustering of objects. */
    virtual void clusterObjects() {};
    
    /** Deliver overdue messages to objects. */
    virtual void dispatchMessages();
    
    /** Let all objects process input, adjust pose and calculate physics. */
    virtual void animateObjects() {};
    
    /** Let all objects calculate transformation matrices etc. */
    virtual void transformObjects() {};
    
    /** Setup structures for rendering */
    virtual void setupView(float* pos, float* ori) {};
    
    /** Draws background (skybox). 
     * @return False if the background was not painted or is not covered completely - true otherwise.
     */
    virtual bool drawBack() { return false; };
    
    /** Draw all Object's solid surfaces (calls their drawSolid method). */
    virtual void drawSolid() {};
    
    /** Draw all Object's effects (calls their drawEffect method). */
    virtual void drawEffect() {};
    
private:
    static Logger* logger;
    
    /** Current singleton instance */
    static ChatSystem* instance;
    
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
};

#endif	/* CHATSYSTEM_H */

