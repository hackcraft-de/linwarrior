/* 
 * File:     rChatMessage.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 23. Dezember 2010, 15:41
 */

#ifndef RCHATMESSAGE_H
#define	RCHATMESSAGE_H

class rChatMessage;

#include "de/hackcraft/lang/OID.h"

#include "de/hackcraft/world/Component.h"

#include <string>

/**
 *  Sent event messages.
 */
class rChatMessage : public Component {
private:
    /** Who sent this message (object id). */
    OID mSender;
    /** To whom this message is/was sent (object id). */
    OID mReceiver;
    /** When was this message sent / valid only after this due date. */
    OID mTimestamp;
    /** Until when is this message meant to be valid (0 == forever). */
    OID mBestbefore;
    /** Message type. */
    std::string mType;
    /** Message String (lets say it's the subject). */
    std::string mText;
    /** Binary large object - fresh allocated pointer to custom data (delete yourself). */
    void* mBlob;
public:
    rChatMessage(OID sender = 0, OID receiver = 0, OID timestamp = 0, OID bestbefore = 0, std::string type = "", std::string text = "", void* blob = NULL);
    rChatMessage(const rChatMessage* orig);
    virtual ~rChatMessage();

    OID getSender();
    OID getReceiver();
    OID getTimestamp();
    OID getBestbefore();
    std::string getType();
    std::string getText();
    void* getBlob();

    /** Compare time for message priority queue, fifo. */
    bool operator()(rChatMessage const* a, rChatMessage const* b) const;
};


#endif	/* RCHATMESSAGE_H */

