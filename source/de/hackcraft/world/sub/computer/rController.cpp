#include "rController.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/world/Entity.h"
#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/object/cMech.h"

#include "de/hackcraft/world/sub/weapon/rTarcom.h"

#include <cstdlib>
#include <cassert>
#include <sstream>

using std::string;

#define debug_state !true
#define debug_transitions !true
#define debug_step !true

// -------------------------------------------------------------

#define NEARTO 23

Logger* rController::logger = Logger::getLogger("de.hackcraft.world.sub.computer.rController");

std::string rController::cname = "CONTROLLER";
unsigned int rController::cid = 3637;

rController::rController(Entity* entity, bool enable) {
    object = entity;
    enabled = enable;

    lastDisturbedBy = 0;
    disturbedBy = 0;
    enemyNearby = 0;

    aimtarget = 0;
    firetarget = false;
    walktargetdist = 0.0f;
    vector_zero(walktarget);
    idling = false;

    aimrange = 0;
    walkrange = 0;
}

rController::~rController() {
    while (!commandStack.empty()) commandStack.pop_back();
}

void rController::doit(OID aim, float* go, bool fire, float distance) {
    aimtarget = aim;
    firetarget = fire;
    if (go != NULL) {
        vector_cpy(walktarget, go);
    } else if (aimtarget == 0) {
        vector_set(walktarget, float_NAN, float_NAN, float_NAN);
    } else {
        Entity* tgt = World::getInstance()->getObject(aimtarget);
        if (tgt != NULL) {
            vector_cpy(walktarget, tgt->pos0);
        } else {
            vector_set(walktarget, float_NAN, float_NAN, float_NAN);
        }
    }
    walktargetdist = distance;
    idling = !firetarget && (aimtarget == 0) && (go == NULL);

    /*
    if (!true) {
        //object->do_moveFor(go);
        //object->do_aimFor(aim);
        object->do_aimAt();
        if (go) {
            object->do_moveTowards();
        } else if (aim) {
            object->do_moveNear();
        }
        if (fire) object->do_fireAt();
        if (idling) object->do_idle();
    }
    */
}

void rController::printState() {
    int framesize = getFrameSize();
    string framename = getFrameName();
    logger->debug() << "CtrlSt of " << object->name.c_str() << "#" << object->oid << "  Frame: " << framename << "  Framesize:" << framesize << "  Stack: " << commandStack.size() << "\n";
}

string rController::getFrameName() {
    //cout << "getFrameName()\n";
    const char* names[] = {
        "WAIT", "ATTACK", "FOLLOW", "GOTO", "REPEAT",
        "NOSTATENAME"
    };
    unsigned int i = commandStack.back();
    i = (i < OPCODE_MAX) ? i : OPCODE_MAX - 1;
    string s = string(names[i]);
    return s;
};

unsigned int rController::getFrameSizeOf(int opcode) {
    switch (opcode) {
        case WAIT: return 3;
        case ATTACK: return 2;
        case FOLLOW: return 3;
        case GOTO: return 5;
        case REPEAT: return 2;
        default: return 2;
    }
}

unsigned int rController::getFrameSize() {
    return getFrameSizeOf(commandStack.back());
}

OID rController::getParameter(int offset) {
    return commandStack[commandStack.size() - offset - 1];
}

void rController::setParameter(int offset, OID value) {
    commandStack[commandStack.size() - offset - 1] = value;
}

void rController::push(OID value) {
    commandStack.push_back(value);
}

void rController::pop(std::string reason) {
    if (debug_transitions) {
        logger->debug() << object->name.c_str() << "#" << object->oid << ".pop(" << reason << ")\n";
    }
    int size = getFrameSize(); // Important: eval outside loop-condition!
    loopi(size) commandStack.pop_back();
    if (debug_transitions) {
        printState();
    }
}

void rController::animate(float spf) {
    if (debug_state) logger->debug() << "cController::process()\n";

    if (!active || !enabled || object == NULL) return;

    if (commandStack.empty()) pushWaitEvent();

    if (debug_step) printState();

    switch (commandStack.back()) {
        case WAIT: waitEvent();
            break;
        case ATTACK: attackEnemy();
            break;
        case FOLLOW: followLeader();
            break;
        case GOTO: gotoDestination();
            break;
        case REPEAT: repeatInstructions();
            break;
        default: logger->error() << "Invalid Instruction Request!\n";
            break;
    }
    if (debug_step) {
        logger->debug() << "=> ";
        printState();
    }
}


// -------------------------------------------------------------

void rController::pushWaitEvent(long mseconds, bool patrol) {
    push(patrol);
    push(mseconds);
    push(WAIT);
}

void rController::waitEvent() {
    //OID opcode = getParameter(0);
    long mseconds = getParameter(1);
    bool patrol = getParameter(2);

    {
        this->doit(0, NULL, false);
    }

    if (patrol) {
        OID enemy = 0;
        if (enemy == 0 && lastDisturbedBy != disturbedBy) {
            // Ignore next time - probably destroyed.
            // FIXME: Find better solution to prevent jumpy/locked behavior.
            lastDisturbedBy = disturbedBy;
            enemy = disturbedBy;
            //cout << "DISTURBER !!!!!!!!!!!!!\n";
        }
        if (enemy == 0) {
            enemy = enemyNearby;
            if (enemy != 0) {
                //cout << "INTRUDER !!!!!!!!!!!!!\n";
            }
        }
        if (enemy) {
            {
                OID self = object->oid;
                std::stringstream s;
                s << self << ": Intruder!\n";
                //World::getInstance()->sendMessage(0, self, 0, "DEBUG", s.str());
            }
            this->doit(enemy, NULL, false, NEARTO);
            pushAttackEnemy(enemy);
        }
    }

    if (mseconds > 0) {
        mseconds -= 1000 / 40;
        setParameter(1, mseconds);
        if (mseconds <= 0) {
            pop("Timeout for wait was reached.");
            return;
        }
    }
}

// -------------------------------------------------------------

void rController::pushAttackEnemy(OID entity) {
    if (debug_transitions) {
        logger->debug() << object->name.c_str() << "#" << object->oid << ".pushAttackEnemy( " << entity << " )\n";
    }
    {
        //OID self = mDevice->mSerial;
        //World::getInstance()->sendMessage("@%llu: All ur base belongs to us.\n", entity);
    }
    push(entity);
    push(ATTACK);
}

void rController::attackEnemy() {
    //OID opcode = getParameter(0);
    OID entity = getParameter(1);

    {
        //((cMech*)mDevice)->Pattern(tf, "nrnlln");
        this->doit(entity, NULL, aimrange < 50.0f, NEARTO);
    }

    // FIXME: Depends on Mech/tarcom.
    //cMech* mech = (cMech*) object;
    
    if (disturbedBy != 0 && disturbedBy != entity) {
        pop("Changing target (disturbed by another)");
        pushAttackEnemy(disturbedBy);
        return;
    }

    //Entity* target = World::getInstance()->getObject(entity);
    rTarget* target = WeaponSystem::getInstance()->findTargetByEntity(entity);
    rTarcom* tarcom = WeaponSystem::getInstance()->findTarcomByEntity(this->object->oid);
            
    if (target == NULL) {
        this->doit(0, NULL, false);
        pop("Target disappeared (removed from world: fragged).");
        return;
    } else if (tarcom == NULL) {
        this->doit(0, NULL, false);
        pop("Tarcom disappeared (removed from world: fragged).");
        return;
    //} else if (!mech->tarcom->isEnemy(&target->tags)) { // TODO: Change dependency.
    } else if (!target->isEnemy(tarcom)) {
        this->doit(0, NULL, false);
        pop("Not an enemy anymore (maybe dead or not interesting anymore).");
        return;
    } else if (aimrange > 60.0f && disturbedBy == 0 ) {
        this->doit(0, NULL, false);
        pop("Target is out of targeting range.");
        return;
    }
}

// -------------------------------------------------------------

void rController::pushFollowLeader(OID entity, bool patrol) {
    if (debug_transitions) {
        logger->debug() << object->name.c_str() << "#" << object->oid << ".pushFollowLeader( " << entity << ", " << patrol << " )\n";
    }
    push(patrol ? 1 : 0);
    push(entity);
    push(FOLLOW);
}

void rController::followLeader() {
    //OID opcode = getParameter(0);
    OID entity = getParameter(1);
    OID patrol = getParameter(2);

    {
        this->doit(entity, NULL, false, NEARTO);
    }

    if (patrol) {
        OID nearby = enemyNearby; //controlledDevice->enemyNearby();
        if (nearby) {
            this->doit(nearby, NULL, false, NEARTO);
            pushAttackEnemy(nearby);
            return;
        }
    }
}

// -------------------------------------------------------------

void rController::pushGotoDestination(float* v, bool patrol) {
    if (v == NULL) return;
    if (debug_transitions) {
        logger->debug() << object->name.c_str() << "#" << object->oid << ".pushGotoDestination( <" << v[0] << "," << v[1] << "," << v[2] << ">, " << patrol << " )\n";
    }
    unsigned long *p = (unsigned long*) v;
    push(patrol ? !0 : 0);
    push(p[2]);
    push(p[1]);
    push(p[0]);
    push(GOTO);
}

void rController::gotoDestination() {
    //OID opcode = getParameter(0);
    OID v0 = getParameter(1);
    OID v1 = getParameter(2);
    OID v2 = getParameter(3);
    OID patrol = getParameter(4);
    
    // Store data in integer array that really holds binary float data.
    unsigned long p[] = {
        (unsigned long)v0, (unsigned long)v1, (unsigned long)v2
    };
    // Now re-interprete as a float array.
    float* v = (float*)((void*) p);

    {
        if (debug_state) logger->debug() << "going " << ((patrol > 0) ? "patrolling" : "directly") << " to <" << v[0] << "," << v[1] << "," << v[2] << " >\n";
        float* u = new float[3];
        vector_set(u, v[0],v[1],v[2]);
        this->doit(0, u, false);
        delete[] u;
    }
    
    float range = walkrange;
    if (debug_state) logger->debug() << "DestinationRange " << range << "\n";
    if (range < 8.0f) {
        this->doit(0, NULL, false);
        pop("Destination was reached (within destination range).");
        return;
    }

    if (patrol) {
        OID nearby = enemyNearby;
        if (nearby) {
            this->doit(nearby, NULL, false, NEARTO);
            pushAttackEnemy(nearby);
            return;
        }
    }
}

// -------------------------------------------------------------

void rController::pushRepeatInstructions(int n) {
    if (debug_transitions) {
        logger->debug() << object->name.c_str() << "#" << object->oid << ".pushRepeatInstructions( " << n << " )\n";
    }
    push(n);
    push(REPEAT);
}

void rController::repeatInstructions() {
    OID opcode = getParameter(0);
    OID n = getParameter(1);

    if (debug_state) {
        printState();
    }

    unsigned int first = getFrameSize();
    if (debug_state) logger->debug() << "first " << first << " -> opcode " << opcode << "\n";
    unsigned int last = first;

    loopi(n) {
        int opcode = getParameter(last);
        last += getFrameSizeOf(opcode);
        if (debug_state) logger->debug() << "last " << last << " -> opcode " << opcode << "\n";
    }

    int size = last - first;
    if (debug_state) logger->debug() << size << " = " << last << " - " << first << "\n";

    loopi(size) {
        push(getParameter(last - 1));
    }
}


