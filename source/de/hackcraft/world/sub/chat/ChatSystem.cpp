#include "ChatSystem.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/chat/rChatGroup.h"
#include "de/hackcraft/world/sub/chat/rChatMember.h"
#include "de/hackcraft/world/sub/chat/rChatMessage.h"

#include <cassert>


Logger* ChatSystem::logger = Logger::getLogger("de.hackcraft.world.sub.chat.ChatSystem");

ChatSystem* ChatSystem::instance = NULL;


ChatSystem* ChatSystem::getInstance() {
    assert(instance != NULL);
    return instance;
}


ChatSystem::ChatSystem() {
    instance = this;
}


ChatSystem::~ChatSystem() {
}


OID ChatSystem::getGroup(std::string name) {
    
    for (rChatGroup* group : groups) {
        if (group->name.compare(name) == 0) {
            return group->getId();
        }
    }
    
    rChatGroup* group = new rChatGroup();
    group->name = name;
    
    groups.push_back(group);
    groupsIndex[group->getId()] = group;
    
    logger->debug() << "Chat group created: " << group->id << " as " << group->name << "\n";
    
    return group->getId();
}


void ChatSystem::addToGroup(OID gid, Entity* member) {
    
    if (member == NULL) {
        logger->error() << "No object given while trying to add object to chat group.\n";
        return;
    }
    
    if (groupsIndex[gid] == NULL) {
        logger->error() << "No such chat group " << gid << " while trying to add object " << member->oid << " to group.\n";
        return;
    }
    
    rChatMember* chatMember = findChatMemberByEntity(member->oid);
    
    if (chatMember == NULL) {
        logger->error() << "No member for entity to add to chat group.\n";
        return;
    }
    
    logger->debug() << "Adding object " << chatMember->id << " to chat group " << gid << " " << groupsIndex[gid]->name << ".\n";
    
    groupsIndex[gid]->members.push_back(chatMember->id);
}


void ChatSystem::add(rChatMember* member) {
    members.push_back(member);
    membersIndex[member->getId()] = member;
}


rChatMember* ChatSystem::findChatMemberByEntity(OID entityID) {
    
    if (membersByEntity.find(entityID) != membersByEntity.end()) {
        return membersByEntity.at(entityID);
    }
    
    for(rChatMember* member : members) {
        
        if (member->object->oid == entityID) {
            membersByEntity[entityID] = member;
            return member;
        }
    }
    
    return NULL;
}


void ChatSystem::sendMessage(OID delay, OID sender /* = 0 */, OID recvid /* = 0 */, std::string type, std::string text, void* blob) {
    
    //logger->trace() << "sendMessageT(dly=" << delay << ", sdr=" << sender << ", rid=" << recvid << ", txt=" << text << ")\n";
    Timing* timing = World::getInstance()->getTiming();
    timing->advanceDelta();
    
    rChatMessage* message = new rChatMessage(sender, recvid, timing->getTimekey() + delay, 0, type, text, blob);
    
    messagesByTimestamp.push(message);
}


void ChatSystem::dispatchMessages() {
    
    // TODO: Remove return statement to enable dispatchMessages.
    //return;
    
    OID now = World::getInstance()->getOID();
    
    while (!messagesByTimestamp.empty()) {
        
        rChatMessage* message = messagesByTimestamp.top();
        
        if (message->getTimestamp() <= now) {
            
            // Remove from to-send-list.
            messagesByTimestamp.pop();
            // Keep record of sent messages.
            messagesAsDispatched.push_front(message);
            
            // Deliver info about new message to individual group members.
            if (message->getReceiver() == 0) continue;

            rChatGroup* group = groupsIndex[message->getReceiver()];
            
            if (group == NULL) continue;

            for(OID i: group->members) {
                
                if (membersIndex.find(i) != membersIndex.end()) {
                    rChatMember* member = membersIndex[i];
                    member->recvMessage(message);
                }
            }
            
        } else {
            // There are no more overdue messages to be dispatched.
            break;
        }
    }
}

