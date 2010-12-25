/* 
 * File:   cMessage.h
 * Author: benben
 *
 * Created on 23. Dezember 2010, 15:41
 */

#ifndef CMESSAGE_H
#define	CMESSAGE_H

class cMessage;

#include "OID.h"
#include <string>

/**
 *  Sent event messages.
 */
class cMessage {
private:
    /// Who sent this message (object id).
    OID mSender;
    /// To whom this message is/was sent (object id).
    OID mReceiver;
    /// To whom this message is/was sent (group id).
    OID mGroup;
    /// When was this message sent / valid only after this due date.
    OID mTimestamp;
    /// Until when is this message meant to be valid (0 == forever).
    OID mBestbefore;
    /// Message type.
    std::string mType;
    /// Message String (lets say it's the subject).
    std::string mText;
    /// Binary large object - fresh allocated pointer to custom data (delete yourself).
    void* mBlob;
public:
    cMessage(OID sender = 0, OID receiver = 0, OID group = 0, OID timestamp = 0, OID bestbefore = 0, std::string type = "", std::string text = "", void* blob = NULL);
    cMessage(const cMessage& orig);
    virtual ~cMessage();

    OID getSender();
    OID getReceiver();
    OID getGroup();
    OID getTimestamp();
    OID getBestbefore();
    std::string getType();
    std::string getText();
    void* getBlob();

    /// Compare time for message priority queue, fifo.
    bool operator()(cMessage const* a, cMessage const* b) const;
};


#endif	/* CMESSAGE_H */

