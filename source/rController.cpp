#include "rController.h"

#include "cObject.h"
#include "cWorld.h"
#include "cMech.h"
#include "rComcom.h"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;

#define debug_state !true
#define debug_transitions !true
#define debug_step !true

// -------------------------------------------------------------

rController::rController(cObject* entity, bool enable) {
    object = entity;
    enabled = enable;
    role = "CONTROLLER";

    lastDisturbedBy = 0;
    disturbedBy = 0;
    enemyNearby = 0;

    targetAim = 0;
    targetAimActive = false;
    targetAimFire = false;
    vector_zero(targetGoto);
    targetGotoActive = false;
    targetGotoAim = false;
    idling = false;

    aimrange = 0;
    walkrange = 0;
}

rController::~rController() {
    while (!commandStack.empty()) commandStack.pop_back();
}

void rController::doit(OID aim, float* go, bool fire) {
    targetAim = aim;
    targetAimActive = (aim != 0);
    targetAimFire = fire;
    if (go != NULL) {
        vector_cpy(targetGoto, go);
        targetGotoActive = true;
    } else {
        targetGotoActive = false;
    }
    targetGotoAim = (aim != 0) && (go == NULL);
    idling = !targetAimFire && !targetAimActive && !targetGotoActive;

    if (true) {
        object->do_moveFor(go);
        object->do_aimFor(aim);
        object->do_aimAt();
        if (go) {
            object->do_moveTowards();
        } else if (aim) {
            object->do_moveNear();
        }
        if (fire) object->do_fireAt();
        if (idling) object->do_idle();
    }
}

void rController::printState() {
    int framesize = getFrameSize();
    string framename = getFrameName();
    cout << "CtrlSt of " << object->name.c_str() << "#" << object->oid << "  Frame: " << framename << "  Framesize:" << framesize << "  Stack: " << commandStack.size() << endl;
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

void rController::pop() {
    if (debug_transitions) {
        cout << object->name.c_str() << "#" << object->oid << ".pop()\n";
    }
    int size = getFrameSize(); // Important: eval outside loop-condition!
    loopi(size) commandStack.pop_back();
    if (debug_transitions) {
        printState();
    }
}

void rController::animate(float spf) {
    if (debug_state) cout << "cController::process()\n";

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
        default: cout << "Invalid Instruction Request!\n";
            break;
    }
    if (debug_step) {
        cout << "=> ";
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
                cWorld::instance->sendMessage(0, self, 0, "DEBUG", s.str());
            }
            this->doit(enemy, NULL, false);
            pushAttackEnemy(enemy);
        }
    }

    if (mseconds > 0) {
        mseconds -= 1000 / 40;
        setParameter(1, mseconds);
        if (mseconds <= 0) {
            pop();
            return;
        }
    }
}

// -------------------------------------------------------------

void rController::pushAttackEnemy(OID entity) {
    if (debug_transitions) {
        cout << object->name.c_str() << "#" << object->oid << ".pushAttackEnemy( " << entity << " )\n";
    }
    {
        //OID self = mDevice->mSerial;
        //cWorld::instance->sendMessage("@%llu: All ur base belongs to us.\n", entity);
    }
    push(entity);
    push(ATTACK);
}

void rController::attackEnemy() {
    //OID opcode = getParameter(0);
    OID entity = getParameter(1);

    {
        //((cMech*)mDevice)->Pattern(tf, "nrnlln");
        this->doit(entity, NULL, aimrange < 50.0f);
    }

    // FIXME: Depends on Mech/tarcom.
    cMech* mech = (cMech*) object;

    cObject* target = cWorld::instance->getObject(entity);
    if (target == NULL) {
        // Target disappeared (removed from world: fragged).
        this->doit(0, NULL, false);
        pop();
        return;
    } else if (!mech->tarcom->isEnemy(&target->tags)) {
        // Not an enemy anymore (maybe dead or not interesting anymore).
        this->doit(0, NULL, false);
        pop();
        return;
    } else if (aimrange > 60.0f) {
        // Target is out of targeting range.
        this->doit(0, NULL, false);
        pop();
        return;
    }
}

// -------------------------------------------------------------

void rController::pushFollowLeader(OID entity, bool patrol) {
    if (debug_transitions) {
        cout << object->name.c_str() << "#" << object->oid << ".pushFollowLeader( " << entity << ", " << patrol << " )\n";
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
        this->doit(entity, NULL, false);
    }

    if (patrol) {
        OID nearby = enemyNearby; //controlledDevice->enemyNearby();
        if (nearby) {
            this->doit(nearby, NULL, false);
            pushAttackEnemy(nearby);
            return;
        }
    }
}

// -------------------------------------------------------------

void rController::pushGotoDestination(float* v, bool patrol) {
    if (v == NULL) return;
    if (debug_transitions) {
        cout << object->name.c_str() << "#" << object->oid << ".pushGotoDestination( <" << v[0] << "," << v[1] << "," << v[2] << ">, " << patrol << " )\n";
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
    unsigned long p[] = {
        v0, v1, v2
    };
    float* v = (float*) p;

    {
        if (debug_state) cout << "going " << ((patrol > 0) ? "patrolling" : "directly") << " to <" << v[0] << "," << v[1] << "," << v[2] << " >\n";
        this->doit(0, v, false);
    }
    
    float range = walkrange;
    if (debug_state) cout << "DestinationRange " << range << endl;
    if (range < 8.0f) {
        this->doit(0, NULL, false);
        pop();
        return;
    }

    if (patrol) {
        OID nearby = enemyNearby;
        if (nearby) {
            this->doit(nearby, NULL, false);
            pushAttackEnemy(nearby);
            return;
        }
    }
}

// -------------------------------------------------------------

void rController::pushRepeatInstructions(int n) {
    if (debug_transitions) {
        cout << object->name.c_str() << "#" << object->oid << ".pushRepeatInstructions( " << n << " )\n";
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
    if (debug_state) cout << "first " << first << " -> opcode " << opcode << endl;
    unsigned int last = first;

    loopi(n) {
        int opcode = getParameter(last);
        last += getFrameSizeOf(opcode);
        if (debug_state) cout << "last " << last << " -> opcode " << opcode << endl;
    }

    int size = last - first;
    if (debug_state) cout << size << " = " << last << " - " << first << endl;

    loopi(size) {
        push(getParameter(last - 1));
    }
}


