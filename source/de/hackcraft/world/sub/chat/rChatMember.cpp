#include "rChatMember.h"


#include "de/hackcraft/psi3d/Console.h"
#include "de/hackcraft/psi3d/GLS.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/world/sub/chat/rChatMessage.h"

#include <cassert>


std::string rChatMember::cname = "COMCOM";
unsigned int rChatMember::cid = 5007;


rChatMember::rChatMember(Entity* obj) {
    object = obj;
    
    id = (OID) this;

    int w = 60;
    int h = 10;
    mConsole = new Console(w * h, w);
    assert(mConsole != NULL);
    //mConsole->print("* Welcome to LinWarrior3D\n");
    mConsole->print("ComCOM(1) - MENU\n\n");
    mConsole->print(" [1] Formation\n");
    mConsole->print(" [2] Engage\n");
    mConsole->print(" [3] Report\n");
    mConsole->print(" [4] Objectives\n");
    mConsole->print("\n\n > ");
    mLastMessage = 0;
}


void rChatMember::animate(float spf) {
    rChatMessage* message = NULL; //World::getInstance()->recvMessage(0, mLastMessage);
    if (message != NULL) {
        mLastMessage = message->getTimestamp();
    }
}


void rChatMember::drawHUD() {
    if (!active) return;
    /*
    GL::glColor4f(0,0,0,0.8);
    GL::glBegin(GL_QUADS);
    GL::glVertex3f(3, 1, 0);
    GL::glVertex3f(0, 1, 0);
    GL::glColor4f(0,0,0,0.2);
    GL::glVertex3f(0, 0, 0);
    GL::glVertex3f(3, 0, 0);
    GL::glEnd();
    */
    
    GL::glPushMatrix();
    {
        float w = mConsole->getWidth();
        float h = mConsole->getHeight();
        GL::glTranslatef(0, 1.0f + (1.0f / h), 0);
        GL::glScalef(3.0f / w, 1.0f / h, 1.0f);
        
        for (int i = 0; i < h; i++) {
            GL::glTranslatef(0, -1, 0);
            GL::glBegin(GL_LINES);
            GL::glColor4f(0.99, 0.99, 0.99, 0.02 + 0.1 * (h - i) / h);
            GL::glVertex3f(0,0,0);
            GL::glColor4f(0.99, 0.99, 0.99, 0.0);
            GL::glVertex3f(w,0,0);
            GL::glEnd();
        }
        
    }
    GL::glPopMatrix();
    
    GL::glPushMatrix();
    {
        GL::glColor4f(0.9, 0.9, 0.9, 0.5);
        //GL::glColor4f(0.99, 0.99, 0.19, 1);
        float w = mConsole->getWidth();
        float h = mConsole->getHeight();
        GL::glTranslatef(0, 1.0f + (1.0f / h), 0);
        GL::glScalef(3.0f / w, 1.0f / h, 1.0f);
        mConsole->draw();
    }
    GL::glPopMatrix();
}


void rChatMember::recvMessage(rChatMessage* msg) {
    
    mConsole->print(msg->getText().c_str());
}
