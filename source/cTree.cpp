#include "cTree.h"
#include "psi3d/macros.h"
#include "psi3d/snippetsgl.h"

#include <cmath>
#include <cassert>

#include <iostream>
using std::cout;

#include <list>
using std::list;


#define ran(seed) (seed = ((seed * 1234567) % 65535))


//std::list<cTree::rTree*> cTree::cache;
std::map<OID,cTree::rTree*> cTree::cache;


cTree::cTree(float* pos, float* rot, int seed, int type, int age) {
    if (pos) vector_cpy(traceable->pos, pos);
    tree.seed = seed;
    tree.type = type;
    tree.age = age;
    tree.list = -1;
}

void cTree::drawSolid() {
    if ((int) tree.list == -1) tree.list = compileTreeDisplaylist(tree.seed, tree.type, tree.age);
    glPushAttrib(GL_ENABLE_BIT);
    {
        SGL::glUseProgram_fglitcolor();
        glDisable(GL_CULL_FACE);

        glPushMatrix();
        {
            glTranslatef(traceable->pos[0], traceable->pos[1], traceable->pos[2]);
            glCallList(tree.list);
        }
        glPopMatrix();
    }
    glPopAttrib();
}

float cTree::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float base[] = {traceable->pos[0], traceable->pos[1] - 0.0 - radius, traceable->pos[2]};
    float radius_ = 0.1 + radius;
    float height = 3 + 2 * radius;
    float depth = cParticle::constraintParticleByCylinder(worldpos, base, radius_, height, worldpos);
    if (depth > 0 && localpos != NULL) {
        vector_sub(localpos, worldpos, traceable->pos);
    }
    return depth;
}

GLuint cTree::compileTreeDisplaylist(int seed, int type, int age) {
    // Try to find and return a cached displaylist version of the given tree.

    OID key = (seed << 16) | ((type & 0xFF) << 8) | (age & 0xFF);

    {
        rTree* t = cache[key];
        if (t != NULL) {
            //cout << "found cached tree seed: " << seed << " type: " << type << " age: " << age << "\n";
            return t->list;
        }
    }

    /*
    foreach(i, cache) {
        rTree* t = *i;
        if (t->seed == seed && t->type == type && t->age == age) {
            //cout << "found cached tree seed: " << seed << " type: " << type << "\n";
            return t->list;
        }
    }
    */

    // There was no such tree cached - so lets compile a displaylist.

    static GLint leaf_displaylists[] = {-1, -1, -1, -1, -1};
    if (leaf_displaylists[type] == -1) {
        leaf_displaylists[type] = glGenLists(1);
        glNewList(leaf_displaylists[type], GL_COMPILE);
        switch (type) {
            case 0: drawRubberTreeLeaf();
                break;
            case 1: drawCaribeanTreeLeaf();
                break;
            case 2: drawHalmTreeLeaf();
                break;
            case 3: drawButterflyTreeLeaf();
                break;
        };
        glEndList();
    }

    static GLuint trunk_displaylist = -1;
    if ((int) trunk_displaylist == -1) {
        trunk_displaylist = glGenLists(1);
        glNewList(trunk_displaylist, GL_COMPILE);
        glColor3f(0.1f, 0.1f, 0.0f);
        cPrimitives::glCenterUnitBlock();
        glEndList();
    }

    rTree* t = new rTree();
    assert(t != NULL);
    t->seed = seed;
    t->type = type;
    t->age = age;
    t->list = glGenLists(1);
    glNewList(t->list, GL_COMPILE);
    float s = 2.5f + age * 0.3;
    glScalef(s, s, s);
    drawTreePart(0, age, 0.3f + age * 0.1, t->seed, trunk_displaylist, leaf_displaylists[type]);
    glEndList();

    //cache.push_front(t);
    cache[key] = t;
    
    std::cout << "Cached Tree " << cache.size() << std::endl;
    return t->list;
}


int cTree::drawTreePart(int depth, int maxdepth, float length, int seed, GLuint trunk_displaylist, GLuint leaf_displaylist) {
    // Recursion end?
    if (depth >= maxdepth) return seed;
    //
    const float height = length * (maxdepth - depth);
    const float width = 0.035 * length + (maxdepth - depth)*(maxdepth - depth) * length * 0.0125f;
    bool leaves = true;
    // Draw Trunk and leaf if at max depth.
    glPushMatrix();
    {
        glScalef(width * 0.5f, height * 0.5f, width * 0.5f);
        glTranslatef(0, 1, 0);
        glCallList(trunk_displaylist);
        if ((depth == maxdepth - 1) && leaves) {
            const float s = 3.6;
            //glTranslatef(0, 0.5f, 0);
            glScalef(s * height / width, s * 0.5, s * height / width);
            glTranslatef(0, 0.5, 0);
            glCallList(leaf_displaylist);
        }
    }
    glPopMatrix();
    // Draw Offsprings

    const int n = 3;
    int rnums[2 * n];
    int r = seed;
    int j = 0;

    loopi(n) {
        rnums[j++] = ran(r);
        rnums[j++] = ran(r);
    }

    int anglex, angley;
    j = 0;

    loopi(n) {
        const int yvar = 15;
        angley = int(i * 360.0f / float(n)) - yvar + rnums[j++] % (2 * yvar);
        const float xvar = 27.0f;
        anglex = int(60.0f - xvar + (rnums[j++] % 100)*0.01f * (2.0f * xvar)*(1.0f - float(maxdepth - depth) / float(maxdepth)));
        glPushMatrix();
        {
            glTranslatef(0, (i + 2) * height / float(n + 1) - width * 0.5f, 0);
            glRotatef(angley, 0, 1, 0);
            glRotatef(anglex, 1, 0, 0);
            r = drawTreePart(depth + 1, maxdepth, length, r, trunk_displaylist, leaf_displaylist);
        }
        glPopMatrix();
    };
    return r;
}

void cTree::drawRubberTreeLeaf() {
    glBegin(GL_TRIANGLE_FAN);
    // 0 back
    glNormal3f(0, +1, 0);
    glColor3f(0.2f, 0.1f, 0.0f);
    glVertex3f(0.0, 0.0, 0.0);
    // 1 right
    glNormal3f(-1, +1, 0);
    glColor3f(0.0f, 0.4f, 0.0f);
    glVertex3f(+0.3, +0.1, +0.1);
    // 2 right
    glColor3f(0.0f, 0.3f, 0.0f);
    glVertex3f(+0.4, +0.2, +0.2);
    // 3 right
    glColor3f(0.0f, 0.4f, 0.0f);
    glVertex3f(+0.35, +0.2, +0.5);
    // 4 right
    glColor3f(0.0f, 0.3f, 0.0f);
    glVertex3f(+0.15, +0.1, +0.8);
    // 5 tip
    glNormal3f(+1, +1, 0);
    glColor3f(0.0f, 0.2f, 0.0f);
    glVertex3f(0.0, 0.0, +1.1);
    // 4 left
    glColor3f(0.0f, 0.3f, 0.0f);
    glVertex3f(-0.15, +0.1, +0.8);
    // 3 left
    glColor3f(0.0f, 0.4f, 0.0f);
    glVertex3f(-0.35, +0.2, +0.5);
    // 2 left
    glColor3f(0.0f, 0.3f, 0.0f);
    glVertex3f(-0.4, +0.2, +0.2);
    // 1 left
    glColor3f(0.0f, 0.4f, 0.0f);
    glVertex3f(-0.3, +0.1, +0.0);
    // 0 back
    glNormal3f(0, +1, 0);
    glColor3f(0.2f, 0.1f, 0.0f);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();
}

void cTree::drawCaribeanTreeLeaf() {
    const float step = 6 * M_PI / 180.0f;
    float alpha = step * 0.5;
    glBegin(GL_TRIANGLES);

    loopi(5) {
        float l[] = {1.0, 0.7, 0.5, 0.3, 0.2};
        //float length = 1-sin(10*i*0.6*i * 0.017453); //3.0 / (1.0f + i);
        //length *= 3;
        float length = 2.5 * l[i];
        float ulength = 1.7 * length;
        //cout << "len" << i << " = " << length << "\n";
        float tips = 0.7;
        // 0 back
        glNormal3f(0, +1, 0);
        glColor3f(0.0f, 0.3f, 0.0f);
        glVertex3f(0.0, 0.0, 0.0);
        glColor3f(0.5f, 0.4f, 0.2f);
        // 1
        glNormal3f(0, +1, 0);
        glVertex3f(length * sin(alpha), ulength * cos(alpha), 0.0);
        // 2
        glNormal3f(0, +1, 0);
        glVertex3f(tips * length * sin(alpha + step), tips * ulength * cos(alpha + step), 0.0);
        // 0 back
        glNormal3f(0, +1, 0);
        glColor3f(0.0f, 0.3f, 0.0f);
        glVertex3f(0.0, 0.0, 0.0);
        glColor3f(0.5f, 0.4f, 0.2f);
        // 1
        glNormal3f(0, +1, 0);
        glVertex3f(length * sin(-alpha), ulength * cos(-alpha), 0.0);
        // 2
        glNormal3f(0, +1, 0);
        glVertex3f(tips * length * sin(-alpha - step), tips * ulength * cos(-alpha - step), 0.0);
        //
        alpha += 2 * step;
    }
    glEnd();
}

void cTree::drawHalmTreeLeaf() {
    return;
    glPushMatrix();
    {
        glScalef(0.2, 0.6, 1.7);
        drawRubberTreeLeaf();
    }
    glPopMatrix();
}

void cTree::drawButterflyTreeLeaf() {
    glPushMatrix();
    {
        glRotatef(-30, 0, 1, 0);
        drawHalmTreeLeaf();
        glRotatef(30, 0, 1, 0);
        drawHalmTreeLeaf();
        glRotatef(30, 0, 1, 0);
        drawHalmTreeLeaf();
    }
    glPopMatrix();
}
