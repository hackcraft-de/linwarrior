#include "cPadmap.h"

#include "psi3d/snippetsgl.h"


static void stepSpiral(int& x, int& y, int& i, int& length, bool& horizontal, int& increment) {
    if (i >= length) {
        i = 0;
        if (!horizontal) {
            length++;
            increment = -increment; // +1 / -1
        }
        horizontal = !horizontal;
    }
    i++;
    if (horizontal) {
        x += increment;
    } else {
        y += increment;
    }
}

int cPadmap::sInstances = 0;
std::map<int, long> cPadmap::sTextures;
std::vector<int> cPadmap::sSpiral[2];

cPadmap::cPadmap(float x, float y, float z) {
    sInstances++;
    if (sInstances == 1) {
        unsigned char* texels = NULL;
        unsigned int texname;
        int w, h, bpp;

        texels = loadTGA("data/urban/street/junction.tga", &w, &h, &bpp);
        texname = SGL::glBindTexture2D(0, true, false, true, true, w, h, bpp, texels);
        sTextures[KIND_ROAD_JUNKTION] = texname;

        texels = loadTGA("data/urban/street/ns_road.tga", &w, &h, &bpp);
        texname = SGL::glBindTexture2D(0, true, false, true, true, w, h, bpp, texels);
        sTextures[KIND_ROAD_NORTHSOUTH] = texname;

        texels = loadTGA("data/urban/street/we_road.tga", &w, &h, &bpp);
        texname = SGL::glBindTexture2D(0, true, false, true, true, w, h, bpp, texels);
        sTextures[KIND_ROAD_EASTWEST] = texname;

        // Generate a spiral path outwards from the neutral center (0,0).
        {
            int maxlen = 32;
            int x = 0;
            int y = 0;
            int i = 0;
            int length = 1;
            bool horizontal = true;
            int increment = +1;
            //cout << "spiral:" << endl;
            while (true) {
                //cout << x << " " << y << endl;
                sSpiral[0].push_back(x);
                sSpiral[1].push_back(y);
                stepSpiral(x, y, i, length, horizontal, increment);
                if (length >= maxlen) break;
            }
        }
    }

    vector_set(traceable->pos, x, y, z);
    quat_set(traceable->ori, 0, 0, 0, 1);

    float h[16 * 16] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 1, 1, 4, 4, 3, 3, 2, 2, 2, 2, 0, 0, 0,
        0, 0, 0, 1, 1, 4, 4, 3, 3, 2, 2, 2, 2, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 2, 2, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 3, 3, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 3, 3, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 4, 4, 0, 0, 0, 0,

        0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    dims[0] = 16;
    dims[1] = 16;
    long size = dims[0] * dims[1];
    //memcpy(heights, h, sizeof (float) *size);

    loopi(size) {
        heights[i] = 0.5f * h[i];
    }

    mapscale[0] = 2;
    mapscale[1] = 4.2;
    mapscale[2] = 2;
}


float cPadmap::getHeight(float x, float z) {
    int a = dims[0];
    int b = dims[1];

    float u = x;
    float v = z;

    if (u < 0) return -INFINITY;
    if (v < 0) return -INFINITY;
    if (u >= a / 2) return -INFINITY;
    if (v >= b / 2) return -INFINITY;

    int u0 = int(u) * 2;
    float du = u - int(u);

    int v0 = int(v) * 2;
    float dv = v - int(v);

    //cout << u0 << " " << v0 << " / " << du << " " << dv << endl;

    float h00 = heights[(v0 + 0) * a + u0 + 0];
    float h10 = heights[(v0 + 1) * a + u0 + 0];
    float h11 = heights[(v0 + 1) * a + u0 + 1];
    float h01 = heights[(v0 + 0) * a + u0 + 1];

    float h0 = (1 - dv) * h00 + dv * h10;
    float h1 = (1 - dv) * h01 + dv * h11;
    float h = (1 - du) * h0 + du * h1;

    return h;
}


void cPadmap::drawSolid() {
    //return; // !!

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        glEnable(GL_TEXTURE_2D);
        //glDisable(GL_CULL_FACE);
        glColor4f(1, 1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, sTextures[KIND_ROAD_JUNKTION]);

        glPushMatrix();
        {
            glTranslatef(traceable->pos[0], traceable->pos[1], traceable->pos[2]);
            glScalef(mapscale[0], mapscale[1], mapscale[2]);

            int a = dims[0];
            int b = dims[1];

            /*
            glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, b);
            glVertex3f(a, 0, 0);
            glVertex3f(a, 0, b);
            glEnd();
             */


            // Draw pad tops
            {
                glColor4f(0, 0, 1, 1);
                glNormal3f(0, 1, 0);
                int z = 0;
                for (int j = 0; j < b - 1; j += 2) {
                    glBegin(GL_TRIANGLE_STRIP);
                    int x = 0;
                    for (int i = 0; i < a - 1; i += 2) {
                        bool stumble = (i == 0);
                        if (!stumble) {
                            stumble = (heights[(j + 0) * a + i - 1] != heights[(j + 0) * a + i])
                                    || (heights[(j + 1) * a + i - 1] != heights[(j + 1) * a + i]);
                        }
                        if (stumble) {
                            glEnd();
                            glBegin(GL_TRIANGLE_STRIP);
                            glTexCoord2f(x, z);
                            glVertex3f(x, heights[(j + 0) * a + i], z);
                            glTexCoord2f(x, z + 1);
                            glVertex3f(x, heights[(j + 1) * a + i], z + 1);
                        }
                        glTexCoord2f(x + 1, z);
                        glVertex3f(x + 1, heights[(j + 0) * a + i + 1], z);
                        glTexCoord2f(x + 1, z + 1);
                        glVertex3f(x + 1, heights[(j + 1) * a + i + 1], z + 1);
                        x++;
                    }
                    glEnd();
                    z++;
                }
            } // Draw pad tops

            //glDisable(GL_TEXTURE_2D);
            // Draw east-west cliffs
            {
                glColor4f(1, 0, 1, 1);
                glNormal3f(0, 0, 1);
                int x = 0;
                for (int i = 0; i < a; i += 2) {
                    int z = 1;
                    for (int j = 2; j < b - 1; j += 2) {
                        bool stumble = (heights[(j - 1) * a + (i + 0)] != heights[(j + 0) * a + (i + 0)])
                                || (heights[(j - 1) * a + (i + 1)] != heights[(j + 0) * a + (i + 1)]);
                        if (stumble) {
                            glBegin(GL_TRIANGLE_STRIP);
                            glTexCoord2f(x + 1, heights[(j - 1) * a + i + 1]);
                            glVertex3f(x + 1, heights[(j - 1) * a + i + 1], z);
                            glTexCoord2f(x + 0, heights[(j - 1) * a + i + 0]);
                            glVertex3f(x + 0, heights[(j - 1) * a + i + 0], z);
                            glTexCoord2f(x + 1, heights[(j - 0) * a + i + 1]);
                            glVertex3f(x + 1, heights[(j - 0) * a + i + 1], z);
                            glTexCoord2f(x + 0, heights[(j - 0) * a + i + 0]);
                            glVertex3f(x + 0, heights[(j - 0) * a + i + 0], z);
                            glEnd();
                        }
                        z++;
                    }
                    x++;
                }
            } // Draw east-west cliffs

            //glDisable(GL_TEXTURE_2D);
            // Draw north-south cliffs
            {
                glColor4f(0, 1, 1, 1);
                glNormal3f(1, 0, 0);
                int z = 0;
                for (int j = 0; j < b; j += 2) {
                    int x = 1;
                    for (int i = 2; i < a - 1; i += 2) {
                        bool stumble = (heights[(j + 0) * a + i - 1] != heights[(j + 0) * a + i])
                                || (heights[(j + 1) * a + i - 1] != heights[(j + 1) * a + i]);
                        if (stumble) {
                            glBegin(GL_TRIANGLE_STRIP);
                            glTexCoord2f(z + 0, heights[(j + 0) * a + i - 1]);
                            glVertex3f(x, heights[(j + 0) * a + i - 1], z + 0);
                            glTexCoord2f(z + 1, heights[(j + 1) * a + i - 1]);
                            glVertex3f(x, heights[(j + 1) * a + i - 1], z + 1);
                            glTexCoord2f(z + 0, heights[(j + 0) * a + i - 0]);
                            glVertex3f(x, heights[(j + 0) * a + i + 0], z + 0);
                            glTexCoord2f(z + 1, heights[(j + 1) * a + i - 0]);
                            glVertex3f(x, heights[(j + 1) * a + i + 0], z + 1);
                            glEnd();
                        }
                        x++;
                    }
                    z++;
                }
            } // Draw north-south cliffs

        }
        glPopMatrix();
    }
    glPopAttrib();
}


float cPadmap::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    return 0.0f; // !!!!!!!!!!

    //if (enactor == NULL) return 0.0;
    float localpos_[3];
    vector_cpy(localpos_, worldpos);
    vector_sub(localpos_, localpos_, traceable->pos);
    
    //localpos_[0] /= mapscale[0];
    //localpos_[1] /= mapscale[1];
    //localpos_[2] /= mapscale[2];

    //assert(finitef(localpos_[0]));
    //assert(finitef(localpos_[1]));
    //assert(finitef(localpos_[2]));

    const int iterations = 24;
    const int samples = 16;
    float relax = 0.1;
    float maxdelta = 0;
    //float color3f[3];
    int runs = 0;
    loopj(iterations) {
        runs++;
        //relax = 0.05 + 0.1 * (iterations - j) / (float)(iterations);
        relax = 0.01;
        //const float a = 2.0 * M_PI / (float) samples;
        //const float b = 0.5 * M_PI / (float) samples;
        float nearest2 = radius*radius;
        float near[] = {0, 0, 0};

        loopi(samples) {
            float x_ = localpos_[0];
            float y_ = localpos_[1];
            float z_ = localpos_[2];
            float alpha = 0;
            float beta = 0;
            if (i != 0) {
                //alpha = i * a;
                //beta = i * b;
                alpha = (rand()%62831)*0.0001f; // 2PI 0-360
                beta =  (rand()%15707)*0.0001f; // 0.5PI 0-90
            }
            x_ += sin(alpha) * sin(beta) * radius;
            y_ += -cos(beta) * radius;
            z_ += cos(alpha) * sin(beta) * radius;

            float h00 = getHeight(x_ / mapscale[0], z_ / mapscale[2]) * mapscale[1];
            float delta = y_ - h00;
            if (delta > 0) continue;
            if (h00 > localpos_[1]) {
                h00 = localpos_[1];
            }
            float v[] = {localpos_[0] - x_, localpos_[1] - h00, localpos_[2] - z_};
            float dist2 = vector_dot(v, v);
            if (dist2 < nearest2) {
                nearest2 = dist2;
                vector_cpy(near, v);
            }
        }
        float nearest = sqrt(nearest2);
        float delta = radius - nearest;
        if (delta > 0.01) {
            //vector_print(near);
            //cout << radius << " " << nearest << " " << delta << endl;

            vector_scale(near, near, 1.0f / (nearest+0.000001f));
            vector_scale(near, near, delta * relax);
            vector_add(localpos_, localpos_, near);
        } else if (j > 2) {
            break;
        }
        maxdelta = fmax(maxdelta, delta);
    }
    //cout << "it " << runs << endl;

    if (maxdelta > 0) {
        //localpos_[0] *= mapscale[0];
        //localpos_[1] *= mapscale[1];
        //localpos_[2] *= mapscale[2];
        vector_add(localpos_, localpos_, traceable->pos);
        vector_cpy(worldpos, localpos_);
        if (localpos != NULL) vector_cpy(localpos, localpos_);
    }
    return maxdelta;
}


