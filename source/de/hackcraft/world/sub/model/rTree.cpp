#include "rTree.h"

#include "de/hackcraft/psi3d/macros.h"
#include "de/hackcraft/psi3d/Primitive.h"
#include "de/hackcraft/psi3d/Particle.h"

#include "de/hackcraft/io/Texfile.h"

#include <cmath>
#include <cassert>

#include <iostream>
using std::cout;

#include <list>
using std::list;

#include <string>
using std::string;

#define ran(seed) (seed = ((seed * 1234567) % 65535))

std::string rTree::cname = "TREE";
unsigned int rTree::cid = 4715;

int rTree::sInstances = 0;
std::map<OID, rTree::TreeType*> rTree::sTrees;
std::vector<long> rTree::sLeaftexs;
std::vector<long> rTree::sBarktexs;

rTree::rTree(Entity* obj, float* pos, float* rot, int seed, int type, int age) {
    this->object = obj;
    
    sInstances++;
    if (sInstances == 1) {
        string foliagepath = string("data/base/landscape/foliage/");
        string foliagenames[] = {
            string("foliage_tree.tga"),
            string("foliage_strange.tga"),
            string("foliage_widow.tga"),
            string("foliage_pine.tga"),
        };

        loopi(4) {
            string name = string(foliagepath).append(foliagenames[i]);
            cout << "Loading leaf [" << name << "] ...\n";
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = Texfile::loadTGA(name.c_str(), &w, &h, &bpp);
            texname = GLS::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete[] texels;
            sLeaftexs.push_back(texname);
        }
        
        string barkpath = string("data/base/landscape/bark/");
        string barknames[] = {
            string("bark_oak.tga"),
            string("bark_palm.tga"),
            string("bark_birch.tga"),
            string("bark_ash.tga"),
        };
        
        loopi(4) {
            string name = string(barkpath).append(barknames[i]);
            cout << "Loading bark [" << name << "] ...\n";
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = Texfile::loadTGA(name.c_str(), &w, &h, &bpp);
            texname = GLS::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            delete[] texels;
            sBarktexs.push_back(texname);
        }
    }

    if (pos) vector_cpy(this->pos0, pos);
    
    if (rot) {
        float axis[] = {0, 1, 0};
        quat_rotaxis(this->ori0, rot[1] * PI_OVER_180, axis);
    }
    
    if (age == 0) {
        tree = NULL;
    } else {
        tree = getCompiledTree(seed, type, age);
    }
    
    seconds = 0;
}

void rTree::animate(float spf) {
}

void rTree::drawSolid() {
    glPushAttrib(GL_ENABLE_BIT);
    {
        GLS::glUseProgram_fglitcolor();
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);

        glPushMatrix();
        {
            glTranslatef(this->pos0[0], this->pos0[1], this->pos0[2]);
            //glRotatef(this->ori0[1], 0, 1, 0);
            GLS::glRotateq(this->ori0);
            glCallList(tree->list);
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void rTree::drawEffect() {
    glPushMatrix();
    {
        glTranslatef(this->pos0[0], this->pos0[1], this->pos0[2]);
        GLS::glRotateq(this->ori0);

        // Construct Billboarding Matrix.
        float n[16];
        GLS::glGetTransposeInverseRotationMatrix(n);
        //vector_set(&n[4], 0,1,0);
        vector_cross(&n[0], &n[4], &n[8]);
        vector_norm(&n[0], &n[0]);
        vector_cross(&n[8], &n[0], &n[4]);

        float color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);
        //cout << color[3] << "\n";

        glPushAttrib(GL_ENABLE_BIT);
        {
            GLS::glUseProgram_fglittexture();
            glDisable(GL_CULL_FACE);
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.58f);

            glBindTexture(GL_TEXTURE_2D, sLeaftexs[tree->type % sLeaftexs.size()]);
            int m = tree->leaves.size() / 3;
            //std::cout << m << std::endl;

            loopi(m) {
                float x = tree->leaves[i * 3 + 0];
                float y = tree->leaves[i * 3 + 1];
                float z = tree->leaves[i * 3 + 2];

                float light = 0.5f + 0.4f * (1.0f - exp(-0.1f * y));
                float shake = 0.4f * (1.0f - exp(-0.1f * y));
                float swirl = 1.0f + 10.0f * (1.0f - exp(-0.2f * y));
                float size = 0.5f + 0.8f * ( log(tree->age)*0.26f + 0.18f * tree->height + 0.03f * sin(seconds * 0.21f + i * 0.1f) );

                float dx = 1.0f * sin(seconds * 0.41f + i * 0.1f);
                float dy = 0.3f * cos(seconds * 0.43f + i * 0.2f);
                float dz = 1.0f * cos(seconds * 0.47f + i * 0.3f);
                x += dx * shake;
                y += dy * shake;
                z += dz * shake;
                glNormal3f(0, 1, 0);
                glColor4f(light, light, light, color[3]);
                //float v[] = { x, y, z };
                //vector_print(v);
                glPushMatrix();
                {
                    glTranslatef(x, y, z);
                    //glTranslatef(x, y, z);
                    //glColor4f(1,0,0,1);
                    glMultMatrixf(n);
                    glRotatef(sin(seconds * 0.5f + i * 0.3f) * swirl, 0, 0, 1);
                    Primitive::glXYCenteredTextureSquare(size);
                    //cPrimitives::glDisk();
                    //cPrimitives::glAxis(1.1f);
                }
                glPopMatrix();
            }
        }
        glPopAttrib();
    }
    glPopMatrix();
}

float rTree::constrain(float* worldpos, float radius, float* localpos, Entity* enactor) {
    float base[] = {this->pos0[0], this->pos0[1] - 0.0f - radius, this->pos0[2]};
    float radius_ = 0.1 + radius;
    float height = 3 + 2 * radius;
    float depth = Particle::constraintParticleByCylinder(worldpos, base, radius_, height, worldpos);
    if (depth > 0 && localpos != NULL) {
        vector_sub(localpos, worldpos, this->pos0);
    }
    return depth;
}

rTree::TreeType* rTree::getCompiledTree(int seed, int type, int age) {
    // Try to find and return a cached version of the given tree.

    OID key = (seed << 16) ^ ((type & 0xFF) << 8) ^ (age & 0xFF);

    TreeType* tree = sTrees[key];
    if (tree != NULL) {
        //cout << "found cached tree seed: " << seed << " type: " << type << " age: " << age << "\n";
        return tree;
    }

    // There was no such tree cached - so lets compile a displaylist.

    static GLint leaf_displaylists[] = {-1, -1, -1, -1, -1, -1, -1, -1};
    if (type < 5) {
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
    }

    static GLuint trunk_displaylists[] = {-1, -1, -1, -1, -1, -1, -1, -1};
    if ((int) trunk_displaylists[type] == -1) {
        trunk_displaylists[type] = glGenLists(1);
        glNewList(trunk_displaylists[type], GL_COMPILE);
        //glColor3f(0.1f, 0.1f, 0.0f);
        glColor3f(0.2f, 0.2f, 0.2f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sBarktexs[type % sBarktexs.size()]);
        //Primitive::glCenterUnitCylinder(5, 5);
        //Primitive::glCenterUnitBlock();
        drawTrunk(5, 5);
        glEndList();
    }

    TreeType* t = new TreeType();
    assert(t != NULL);
    t->seed = seed;
    t->type = type;
    t->age = age;
    t->list = glGenLists(1);
    t->height = log(age);
    glPushAttrib(GL_ENABLE_BIT);
    {
        //GLS::glUseProgram_fglitcolor();
        //glColor4f(0.3f, 0.3f, 0.1f, 1.0f);
        GLS::glUseProgram_fglittexture();
        glColor3f(0.9f, 0.9f, 0.9f);
        glPushMatrix();
        {
            glLoadIdentity();
            glNewList(t->list, GL_COMPILE_AND_EXECUTE);
            float s = 2.5f + age * 0.3;
            glScalef(s, s, s);
            drawTreePart(0, age, 0.3f + age * 0.1, t->seed, trunk_displaylists[type], leaf_displaylists[type], &t->leaves, &t->height);
            glEndList();
        }
        glPopMatrix();
    }
    glPopAttrib();

    //cache.push_front(t);
    sTrees[key] = t;

    std::cout << "Cached Tree " << sTrees.size() << std::endl;
    return t;
}

int rTree::drawTreePart(int depth, int maxdepth, float length, int seed, GLuint trunk_displaylist, GLuint leaf_displaylist, std::vector<float>* leaves, float* totalheight) {
    //
    const float height = length * (maxdepth - depth);
    const float width = 1.25f * (0.06 * length + (maxdepth - depth)*(maxdepth - depth) * length * 0.02f);
    bool draw_poly_leaves = !true;
    // Draw Trunk and leaf if at max depth.
    glPushMatrix();
    {
        glScalef(width * 0.5f, height * 0.5f, width * 0.5f);
        glTranslatef(0, 1, 0);
        glCallList(trunk_displaylist);
        if (depth > 0) {
            const float s = 3.6f;
            if (draw_poly_leaves) {
                glScalef(s * height / width, s * 0.5f, s * height / width);
                glTranslatef(0, 0.5f, 0);
                glCallList(leaf_displaylist);
            } else if (leaves) {
                glScalef(s * height / width, s * 0.5f, s * height / width);
                glTranslatef(0, 0.5f, 0);
                mat4 M;
                glGetFloatv(GL_MODELVIEW_MATRIX, M);
                float v[] = { 0.0f, 0.0f, 0.0f };
                matrix_apply2(M, v);
                //std::cout << "Leaf at: " << std::endl;
                //matrix_print(M);
                //vector_print(v);
                leaves->push_back(v[0]);
                leaves->push_back(v[1]);
                leaves->push_back(v[2]);
                if (totalheight != NULL) {
                    *totalheight = fmax(*totalheight, v[1]);
                }
            }
        }
    }
    glPopMatrix();
    // Draw Offsprings

    // Recursion end?
    if (depth + 1 >= maxdepth) return seed;

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
        angley = 130 * i - yvar + rnums[j++] % (2 * yvar);
        const float xvar = 27.0f;
        anglex = int(60.0f - xvar + (rnums[j++] % 100)*0.01f * (2.0f * xvar)*(1.0f - float(maxdepth - depth) / float(maxdepth)));
        glPushMatrix();
        {
            glTranslatef(0, (i + 2) * height / float(n + 1) - width * 0.5f, 0);
            glRotatef(angley, 0, 1, 0);
            glRotatef(anglex, 1, 0, 0);
            r = drawTreePart(depth + 1, maxdepth, length, r, trunk_displaylist, leaf_displaylist, leaves, totalheight);
        }
        glPopMatrix();
    };
    return r;
}

void rTree::drawRubberTreeLeaf() {
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

void rTree::drawCaribeanTreeLeaf() {
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

void rTree::drawHalmTreeLeaf() {
    return;
    glPushMatrix();
    {
        glScalef(0.2, 0.6, 1.7);
        drawRubberTreeLeaf();
    }
    glPopMatrix();
}

void rTree::drawButterflyTreeLeaf() {
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

void rTree::drawTrunk(float segments, float vrepeat) {
    float step = 1.0f / (float) segments;
    float anglestep = 2.0f * M_PI *step;
    float angle = 0.0f;
    float r1 = 0.75f;
    float r2 = 1.20f;
    glBegin(GL_TRIANGLE_STRIP);
    for (int i=0; i<=segments; i++) {
        float r1_ = r1 * (1.00f + (rand()%100) * 0.002f);
        float r2_ = r2 * (1.00f + (rand()%100) * 0.004f);
        glTexCoord3f(0.5f+0.499f*sin(angle), 0.999f * vrepeat, 0.5f+0.499f*cos(angle));
        glVertex3f(sin(angle)*r1_, +1.0f, cos(angle)*r1_);
        glTexCoord3f(0.5f+0.499f*sin(angle), 0.001f, 0.5f+0.499f*cos(angle));
        glVertex3f(sin(angle)*r2_, -1.0f, cos(angle)*r2_);
        angle += anglestep;
    };
    glEnd();
}

