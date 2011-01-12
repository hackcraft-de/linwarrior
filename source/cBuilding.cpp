// cBuilding.cpp

#include "cBuilding.h"

#include "psi3d/macros.h"
#include "psi3d/snippetsgl.h"
#include "psi3d/noise.h"

#include <cassert>

#include <iostream>
using std::cout;
using std::endl;



static void getVStrings(float x, float y, float* color4f, unsigned char seed = 131) {
    float x_ = fmod(x,1.0f);
    float sinx = cNoise::simplex3(((x_ < 0.5f)?(x_):(1.0f-x_))*16.0, 0, 0, seed);
    float sx = 0.8 + 0.2 * sinx;
    float grain = 0.9f + 0.1f * (0.5f + 0.5f * cNoise::simplex3(x * 256.0f, y * 256.0f, 0, seed));
    color4f[0] = color4f[1] = color4f[2] = grain * sx;
    color4f[3] = 1.0f;
}

static void getHStrings(float x, float y, float* color4f, unsigned char seed = 131) {
    float y_ = fmod(y,1.0f);
    float siny = cNoise::simplex3(0, y_ * 8.0f, 0, seed);
    float sy = 0.8 + 0.2 * siny;
    float grain = 0.9f + 0.1f * (0.5f + 0.5f * cNoise::simplex3(x * 256.0f, y * 256.0f, 0, seed) );
    color4f[0] = color4f[1] = color4f[2] = grain * sy;
    color4f[3] = 1.0f;
}

static void getInterrior(float x, float y, float* color4fv, unsigned char seed = 131) {
    float alpha = 0.3f + 0.7f * (0.5f + 0.5f * cNoise::samplex3(x*63, y*63, 0, seed));
    float dark[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float lite[] = { 0.99f, 0.99f, 0.7f, 1.0f };
    cDistortion::fade4(alpha, dark, lite, color4fv);
}

static void getExterrior(float x, float y, float* color4fv, unsigned char seed = 131) {
    float alpha = 0.3f + 0.7f * (0.5f + 0.5f * cNoise::simplex3(x,y,0,seed));
    float dark[] = { 0.30f, 0.30f, 0.40f, 1.0f };
    float lite[] = { 0.50f, 0.50f, 0.70f, 1.0f };
    cDistortion::fade4(alpha, dark, lite, color4fv);
}

static void getConcrete(float x, float y, float z, float* color4fv, unsigned char seed = 131) {
    float grain = 0.15f + 0.15f * (0.5f + 0.5f * cNoise::simplex3(x, y, z, seed) );
    color4fv[0] = color4fv[1] = color4fv[2] = grain;
    color4fv[3] = 1.0f;
}


static inline void getFacade(float x, float y, int gx, int gy, float age, float* c3f, unsigned char seed = 131) {

    // Border Levels from outside to inside.
    enum Levels {
        MARGIN, BORDER, PADDING, CONTENT, MAX_LEVELS
    };

    float dims[4 * MAX_LEVELS];

    unsigned char r = cNoise::LFSR8(seed);
    r = cNoise::LFSR8(r);
    r = cNoise::LFSR8(r);
    r = cNoise::LFSR8(r);
    r = cNoise::LFSR8(r);
    const float inv256 = 0.003906f;

    r = cNoise::LFSR8(r);
    bool rowmajor = (r & 1) == 0;

    float* margin = &dims[0];
    {
        r = cNoise::LFSR8(r);
        margin[0] = 0.20f * r * inv256 + 0.006f;
        margin[2] = margin[0];
        r = cNoise::LFSR8(r);
        margin[1] = 0.14f * r * inv256 + 0.006f;
        // should be related
        r = cNoise::LFSR8(r);
        margin[3] = 0.36f * r * inv256 + 0.006f;
    }

    float* border = &dims[4];
    {
        float w = 1.0f - (margin[2] + margin[0]);
        float h = 1.0f - (margin[3] + margin[1]);
        r = cNoise::LFSR8(r);
        border[0] = r * 0.0002f * w + 0.0002f * w;
        border[2] = border[0];
        r = cNoise::LFSR8(r);
        border[1] = r * 0.0002f * h + 0.0002f * h;
        border[3] = border[1];
    }

    float* padding = &dims[8];
    {
        float w = 1.0f - (margin[2] + border[2] + margin[0] + border[0]);
        float h = 1.0f - (margin[3] + border[3] + margin[1] + border[1]);
        r = cNoise::LFSR8(r);
        padding[0] = r * 0.0002f * w + 0.0002f * w;
        padding[2] = padding[0];
        r = cNoise::LFSR8(r);
        padding[1] = r * 0.0002f * h + 0.0002f * h;
        padding[3] = padding[1];
    }

    float* content = &dims[12];
    {
        float w = 1.0f - (margin[2] + border[2] + padding[2] + margin[0] + border[0] + padding[0]);
        float h = 1.0f - (margin[3] + border[3] + padding[3] + margin[1] + border[1] + padding[1]);
        r = cNoise::LFSR8(r);
        content[0] = r * 0.0002f * w + 0.0002f * w;
        content[2] = content[0];
        r = cNoise::LFSR8(r);
        content[1] = r * 0.0001f * h + 0.0001f * h;
        r = cNoise::LFSR8(r);
        content[3] = r * 0.0001f * h + 0.0001f * h;
    }

    float sum[MAX_LEVELS+1][4] = { {0, 0, 0, 0} };
    loopi(MAX_LEVELS) {
        sum[i+1][0] = sum[i][0] + dims[4*i + 0];
        sum[i+1][1] = sum[i][1] + dims[4*i + 1];
        sum[i+1][2] = sum[i][2] + dims[4*i + 2];
        sum[i+1][3] = sum[i][3] + dims[4*i + 3];
    }
    
    float pyr[MAX_LEVELS+1];
    loopi(MAX_LEVELS+1) {
        pyr[i] = cDistortion::rampPyramid(sum[i][0],sum[i][1], 1.0f-sum[i][2],1.0f-sum[i][3], x,y);
    }

    float inside[MAX_LEVELS+1];
    loopi(MAX_LEVELS+1) {
        inside[i] = (pyr[i] >= 0) ? 1.0f : 0.0f;
    }

    float colors[][4] = {
        { 1,0,0,1 },
        { 0,1,0,1 },
        { 0,0,1,1 },
        { 0,1,1,1 },
        { 1,1,1,1 },
    };

    loopi(MAX_LEVELS+1) {
        if (pyr[i] >= 0) {
            c3f[0] = colors[i][0];
            c3f[1] = colors[i][1];
            c3f[2] = colors[i][2];
        }
    }

    float chma = 2.0f * 0.1f;
    float cr = r = cNoise::LFSR8(r);
    float cg = r = cNoise::LFSR8(r);
    float cb = r = cNoise::LFSR8(r);
    float cl = r = cNoise::LFSR8(r);
    cl = 0.6f + 0.4f * inv256*cl;
    cl *= (1.0f - chma);
    chma *= inv256;
    float c0[] = { cl + chma*cr, cl + chma*cg, cl + chma*cb, 1.0f };

    chma = 2.0f * 0.1f;
    cr = r = cNoise::LFSR8(r);
    cg = r = cNoise::LFSR8(r);
    cb = r = cNoise::LFSR8(r);
    cl = r = cNoise::LFSR8(r);
    cl = 0.6f + 0.4f * inv256*cl;
    cl *= (1.0f - chma);
    chma *= inv256;
    float c1[] = { cl + chma*cr, cl + chma*cg, cl + chma*cb, 1.0f };

    //return;


    // Vertical Column Pattern.
    float col[4];
    getVStrings(x, y, col, seed+21);
    loopi(4) {
        col[i] *= c0[i];
    }

    // Horizontal Row Pattern.
    float row[4];
    getHStrings(x, y, row, seed+11);
    loopi(4) {
        row[i] *= c1[i];
    }

    // Glass distortion
    float dx = 0.01f * cNoise::simplex3((gx+x)*63,(gy+y)*63,0,seed+43);
    float dy = 0.01f * cNoise::simplex3((gx+x)*63,(gy+y)*63,0,seed+31);

    // Interrior Room Pattern.
    float interrior[4];
    getInterrior(gx+x+dx, gy+y+dy, interrior);
    float innerwalls = 0.6f + fmin(0.4, fmax(0.0f, cDistortion::rampPyramid(0,0.1,1,0.9, x+dx,y+dy)));
    loopi(4) { interrior[i] *= innerwalls; }

    // Exterrior Scene Reflection Pattern.
    float exterrior[4];
    getExterrior(gx+x+2*dx, gy+y+2*dy, exterrior);

    // Light smearing
    float smear = 0.8 + 0.2f * cNoise::simplex3((gx+x)*8+(gy+y)*8,(gx+x)*1-(gy+y)*1,0,seed+41);


    //float glass[] = { 0.90f, 0.90f, 0.99f, 1.0f };
    //float window[] = { 0.3f, 0.35f, 0.3f, 1.0f };

    int u = (x < margin[0]) ? 0 : ((x < 1.0f - margin[2]) ? 1 : 2);
    int v = (y < margin[1]) ? 0 : ((y < 1.0f - margin[3]) ? 1 : 2);

    int colpart = ((!rowmajor || v == 1) && u != 1) ? 1 : 0;
    int rowpart = (( rowmajor || u == 1) && v != 1) ? 1 : 0;

    float mirror = 0.8f;
    mirror *= smear;

    float shade[4];
    cDistortion::fade4(mirror, interrior, exterrior, shade);
    cDistortion::mix3(0.56f*colpart, col, 0.60f*rowpart, row, c3f);
    
    int winpart = inside[BORDER+1];
    cDistortion::mix3(1.0f-winpart, c3f, winpart, shade, c3f);

    return;

    float dirt[] = { 0.3, 0.3, 0.25, 1.0f };
    float dirtf = 1.0f + pyr[BORDER+1];
    dirtf = (dirtf > 1.0f) ? 0.0f : dirtf;
    dirtf *= 0.99f;
    cDistortion::mix3(1.0f-dirtf, c3f, dirtf, dirt, c3f);
}


int cBuilding::sInstances = 0;
std::map<int, long> cBuilding::sTextures;

cBuilding::cBuilding(int x, int y, int z, int rooms_x, int rooms_y, int rooms_z) {
    sInstances++;
    if (sInstances == 1) {
        unsigned int texname;

        {
            cout << "Generating Rooftops..." << endl;
            int w = 1<<5;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float c4f[4];
                    //getConcrete(256*(float)i/(float)w, 256*(float)j/(float)h, 0, c4f);
                    getConcrete(1*(float)i/(float)w, 1*(float)j/(float)h, 0, c4f);
                    *p++ = c4f[2] * 255;
                    *p++ = c4f[1] * 255;
                    *p++ = c4f[0] * 255;
                }
            }
            texname = SGL::glBindTexture2D(0, true, false, true, true, w, h, bpp, texels);
            delete texels;
            sTextures[0] = texname;
        }
        
        {
            //glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
            cout << "Generating Facades..." << endl;
                loopk(17) {
                int w = 1<<7;
                int h = w;
                int bpp = 3;
                unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
                unsigned char* p = texels;
                loopj(h) {
                    loopi(w) {
                        float a = (0.0f + ((float)i / (float)w)) * 3.0f;
                        float b = (1.0f - ((float)j / (float)h)) * 3.0f;
                        int gx = a;
                        int gy = b;
                        float x = a - gx;
                        float y = b - gy;
                        float c[3];
                        getFacade(x, y, gx, gy, 0, c, 131 + k* 31);
                        *p++ = c[2] * 255;
                        *p++ = c[1] * 255;
                        *p++ = c[0] * 255;
                    }
                }

                texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
                //texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
                sTextures[k+1] = texname;

                if (0) {
                    try {
                        char numb[3] = { '0' + (k/10), '0' + (k%10), '\0' };
                        std::string fname = std::string("data/urban/scraper2/facade") + std::string(numb) + std::string(".tga");
                        if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                            cout << "Could not save image" << endl;
                        }
                    } catch(char const* s) {
                        cout << "ERROR: " << s << endl;
                    }
                }
                delete texels;
            }
        }
    }
    
    damageable = new rDamageable;
    socialised = new rSocialised;

    quat_set(traceable->ori, 0, 0, 0, 1);
    buildingRooms[0] = 3 * rooms_x;
    buildingRooms[1] = 3 * rooms_y;
    buildingRooms[2] = 3 * rooms_z;
    vector_set(traceable->pos, x + buildingRooms[0]*0.5f, y, z + buildingRooms[2]*0.5f);
    this->traceable->radius = sqrt(0.25f * buildingRooms[0] * buildingRooms[0] + 0.25f * buildingRooms[1] * buildingRooms[1] + 0.25f * buildingRooms[2] * buildingRooms[2]);
    dirtyBase = true;
}

void cBuilding::damageByParticle(float* localpos, float damage, cObject* enactor) {
    if (!damageable->alife) return;
    
    if (damage > 0) {
        int body = rDamageable::BODY;
        damageable->hp[body] -= damage;
        if (damageable->hp[body] < 0) damageable->hp[body] = 0;
        if (damageable->hp[body] <= 0) {
            damageable->alife = false;
            cout << "cBuilding::damageByParticle(): DEAD\n";
            explosionObject.fire(0);
        }
        if (damageable->hp[body] <= 75) addTag(WOUNDED);
        if (damageable->hp[body] <= 50) addTag(SERIOUS);
        if (damageable->hp[body] <= 25) addTag(CRITICAL);
        if (damageable->hp[body] <= 0) addTag(DEAD);
    }
}

float cBuilding::constrainParticle(float* worldpos, float radius, float* localpos, cObject* enactor) {
    float localpos_[3];

    {
        float ori_inv[4];
        quat_cpy(ori_inv, traceable->ori);
        quat_conj(ori_inv);

        vector_cpy(localpos_, worldpos);
        vector_sub(localpos_, localpos_, traceable->pos);
        quat_apply(localpos_, ori_inv, localpos_);
    }

    float* w = this->buildingRooms;
    float mins[3] = {-w[0]*0.5 - radius, -0.0 - radius, -w[2]*0.5 - radius};
    float maxs[3] = {+w[0]*0.5 + radius, +w[1] + radius, +w[2]*0.5 + radius};
    float localprj[3];

    //cout << endl;
    //cout << mins[0] << " " << mins[1] << " " << mins[2] << endl;
    //cout << localpos[0] << " " << localpos[1] << " " << localpos[2] << endl;
    //cout << maxs[0] << " " << maxs[1] << " " << maxs[2] << endl;

    float depth = 0;
    depth = cParticle::constraintParticleByBox(localpos_, mins, maxs, localprj);

    //cout << depth << endl;

    if (depth <= 0) return 0;

    if (localpos != NULL) vector_cpy(localpos, localprj);

    {
        quat_apply(worldpos, traceable->ori, localprj);
        vector_add(worldpos, worldpos, traceable->pos);
    }

    return depth;
}

void cBuilding::onSpawn() {
    explosionObject.weaponOwner = this;
}

void cBuilding::animate(float spf) {
    explosionObject.animate(spf);
}

void cBuilding::transform() {
    if (dirtyBase) {
        dirtyBase = false;
        glPushMatrix();
        {
            float* p = this->traceable->pos.data();
            float r[] = {0, 0, 0};
            float* w = this->buildingRooms;
            glTranslatef(p[0], p[1], p[2]);
            if (r[1] < -0.00001f || r[1] > 0.00001f) glRotatef(r[1] * 0.017453f, 0, 1, 0);
            glTranslatef(0, 0.125f * 3.5f * w[1], 0);
            glGetFloatv(GL_MODELVIEW_MATRIX, baseMatrix);
            explosionObject.weaponBasefv = baseMatrix;
        }
        glPopMatrix();
    }
    explosionObject.transform();
}

void cBuilding::drawSolid() {
    explosionObject.drawSolid();

    float* p = this->traceable->pos.data();
    float r[] = {0, 0, 0};
    float* w = this->buildingRooms;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        long roof = sTextures[0];
        long wall = 0;
        if (this->damageable->hp[rDamageable::BODY] <= 0) {
            glColor3f(0.6f, 0.5f, 0.4f);
        } else if (this->damageable->hp[rDamageable::BODY] <= 50) {
            glColor3f(0.6f, 0.7f, 0.8f);
        } else {
            glColor3f(0.7f, 0.8f, 0.9f);
        }

        wall = sTextures[1 + (((int)(p[0]*1.234567+p[2]*7.654321))%(sTextures.size()-1))];

        float hp = this->damageable->hp[rDamageable::BODY] * 0.01;
        hp = hp < 0 ? 0 : hp;
        float d = 0.5f + hp * 0.5f;
        glColor3f(d, d, d);

        glPushMatrix();
        {
            if (r[1] < -0.00001f || r[1] > 0.00001f) glRotatef(r[1] * 0.017453f, 0, 1, 0);
            glTranslatef(p[0] - w[0]*0.5f, p[1], p[2] - w[2]*0.5f);
            //cPrimitives::glBlockFlat(w[0], w[1], w[2], wall, wall, wall, wall, roof, roof);
            const float n2 = 0.707107f; // 1.0f / sqrt(2);
            const float n3 = 0.577350f; // 1.0f / sqrt(3);
            glBindTexture(GL_TEXTURE_2D, roof);
            glBegin(GL_QUADS);
            {
                glNormal3f(0, 1, 0);

                glTexCoord2f(0, 0);
                glVertex3f(0, w[1], 0);

                glTexCoord2f(0, w[2]);
                glVertex3f(0, w[1], w[2]);

                glTexCoord2f(w[0], w[2]);
                glVertex3f(w[0], w[1], w[2]);

                glTexCoord2f(w[0], 0);
                glVertex3f(w[0], w[1], 0);
            }
            glEnd();
            glBindTexture(GL_TEXTURE_2D, wall);
            glBegin(GL_TRIANGLE_STRIP);
            {
                float tx = 0;
                float f = 1.0f / 9.0f * 2.0f;
                float ty = w[1] * f;

                glNormal3f(-n2, 0, -n2);
                glTexCoord2f(tx, 0);
                glVertex3f(0, 0, 0);

                glNormal3f(-n3, n3, -n3);
                glTexCoord2f(tx, ty);
                glVertex3f(0, w[1], 0);

                tx += w[0] * f;

                glNormal3f(+n2, 0, -n2);
                glTexCoord2f(tx, 0);
                glVertex3f(w[0], 0, 0);

                glNormal3f(+n3, n3, -n3);
                glTexCoord2f(tx, ty);
                glVertex3f(w[0], w[1], 0);

                tx += w[2] * f;

                glNormal3f(+n2, 0, +n2);
                glTexCoord2f(tx, 0);
                glVertex3f(w[0], 0, w[2]);

                glNormal3f(+n3, n3, +n3);
                glTexCoord2f(tx, ty);
                glVertex3f(w[0], w[1], w[2]);

                tx += w[0] * f;

                glNormal3f(-n2, 0, +n2);
                glTexCoord2f(tx, 0);
                glVertex3f(0, 0, w[2]);

                glNormal3f(-n3, n3, +n3);
                glTexCoord2f(tx, ty);
                glVertex3f(0, w[1], w[2]);

                tx += w[2] * f;

                glNormal3f(-n2, 0, -n2);
                glTexCoord2f(tx, 0);
                glVertex3f(0, 0, 0);

                glNormal3f(-n3, n3, -n3);
                glTexCoord2f(tx, ty);
                glVertex3f(0, w[1], 0);
            }
            glEnd();

            if (!true) cPrimitives::glAxis();
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void cBuilding::drawEffect() {
    explosionObject.drawEffect();
}


// ###############################################



#include "cParticle.h"

int cScatterfield::sInstances = 0;
std::map<int, long> cScatterfield::sTextures;

cScatterfield::cScatterfield(float x, float y, float z, float radius, float density) {
    sInstances++;
    if (sInstances == 1) {
        {
            unsigned int texname;
            int w, h, bpp;
            unsigned char* texels = loadTGA("data/base/decals/desertplant.tga", &w, &h, &bpp);
            texname = SGL::glBindTexture2D(0, true, true, false, false, w, h, bpp, texels);
            delete texels;
            sTextures[0] = texname;
        }
    }

    vector_set(traceable->pos, x, y, z);

    int amount = density * 4 * radius * M_PI;
    for (int i = 0; i < amount; i++) {
        float x_ = x + (rand() % 1000) * 0.001 * 2 * radius - radius;
        float y_ = y;
        float z_ = z + (rand() % 1000) * 0.001 * 2 * radius - radius;
        float r_ = (rand() % 360);
        cParticle* p = new cParticle();
        assert(p != NULL);
        vector_set(p->pos, x_, y_, z_);
        p->spawn = r_;
        decalParticles.push_back(p);
    }
}

void cScatterfield::drawEffect() {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glColor4f(0.3, 0.7, 0.3, 1);
        glBindTexture(GL_TEXTURE_2D, sTextures[0]);
        
        foreachNoInc(i, decalParticles) {
            cParticle* p = *i++;
            glPushMatrix();
            {
                glTranslatef(p->pos[0], p->pos[1], p->pos[2]);
                glRotatef(p->spawn, 0, 1, 0);
                //glAxis(0.9);
                cPrimitives::glXYCenteredTextureSquare();
            }
            glPopMatrix();
        }
    }
    glPopAttrib();
}


// ###############################################

#include "cSolid.h"
#include "cSurface.h"


static void getBasicRoad(float x, float y, float z, float* color, unsigned char seed = 131) {
    cSolid::concrete_cracked(x, y, z, color, seed);
    
    float marks = 0.6f + 0.05f * cos(2.0f*4.0f * x * 2.0f * M_PI);
    color[0] *= marks;
    color[1] *= marks;
    color[2] *= marks;

    float walk4fv[4];
    cSurface::stone_plates(x, y, z, walk4fv, seed);

    unsigned char pattern = 0x66;
    int bitshift = (int) (y * 7.999999f);
    bool bit = (pattern >> bitshift) & 1;

    if (x >  0.495f && x < 0.505f && bit) {
        color[0] = color[0] + 0.35;
        color[1] = color[1] + 0.35;
        color[2] = color[2] + 0.1;
    }

    if ((x > 0.25 && x < 0.26) || (x > (1.0f-0.26f) && x < (1.0f-0.25f))) {
        color[0] = color[0] + 0.35;
        color[1] = color[1] + 0.35;
        color[2] = color[2] + 0.35;
    }

    // sidewalk
    if (x < 0.23 || x > (1.0f-0.23f)) {
        float f = 1;
        if (x < 0.22 || x > (1.0f-0.22f)) {
        } else {
            f = 0.7;
        }
        color[0] = f * walk4fv[0];
        color[1] = f * walk4fv[1];
        color[2] = f * walk4fv[2];
    }
}

static void getBasicCrossRoad(float x, float y, float z, float* color, unsigned char seed = 131) {
    float ns[4];
    float ew[4];
    getBasicRoad(x,y,z, ns, seed + 0);
    getBasicRoad(y,x,z, ew, seed + 1);
    float alpha = (fabs(0.5f - fmod(x,1)) < fabs(0.5f - fmod(y,1))) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

static void getBasicNERoad(float x, float y, float z, float* color, unsigned char seed = 131) {
    float ns[4];
    float ew[4];
    getBasicRoad(x,y,z, ns, seed + 0);
    getBasicRoad(y,x,z, ew, seed + 1);
    float alpha = (fmod(y,1) > fmod(x,1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

static void getBasicNWRoad(float x, float y, float z, float* color, unsigned char seed = 131) {
    float ns[4];
    float ew[4];
    getBasicRoad(x,y,z, ns, seed + 0);
    getBasicRoad(y,x,z, ew, seed + 1);
    float alpha = (fmod(y,1) > 1.0f-fmod(x,1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

static void getBasicSERoad(float x, float y, float z, float* color, unsigned char seed = 131) {
    float ns[4];
    float ew[4];
    getBasicRoad(x,y,z, ns, seed + 0);
    getBasicRoad(y,x,z, ew, seed + 1);
    float alpha = (1.0f-fmod(y,1) > fmod(x,1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

static void getBasicSWRoad(float x, float y, float z, float* color, unsigned char seed = 131) {
    float ns[4];
    float ew[4];
    getBasicRoad(x,y,z, ns, seed + 0);
    getBasicRoad(y,x,z, ew, seed + 1);
    float alpha = (1.0f-fmod(y,1) > 1.0f-fmod(x,1)) ? 1.0f : 0.0f;
    color[0] = alpha * ns[0] + (1.0f - alpha) * ew[0];
    color[1] = alpha * ns[1] + (1.0f - alpha) * ew[1];
    color[2] = alpha * ns[2] + (1.0f - alpha) * ew[2];
    color[3] = alpha * ns[3] + (1.0f - alpha) * ew[3];
}

int cTile::sInstances = 0;
std::map<int, long> cTile::sTextures;

cTile::cTile(int x, int y, int z, int kind) {
    sInstances++;
    if (sInstances == 1) {
        cout << "Generating Roads..." << endl;
        std::string basepath = std::string("data/base/textures/");
        bool save = false;

        unsigned int texname;
        int w_ = 1 << 8;

        // NEWS-Road
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    getBasicCrossRoad(1*(float)i/(float)w, 1*(float)j/(float)h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNEWS.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_NEWS] = texname;
        }
        // NS road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    getBasicRoad(1*(float)i/(float)w, 1*(float)j/(float)h, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNS.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_NS] = texname;
        }
        // EW road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    getBasicRoad(1*(float)j/(float)h, 1*(float)i/(float)w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadEW.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_EW] = texname;
        }
        // NE road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    getBasicNERoad(1*(float)i/(float)h, 1*(float)j/(float)w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNE.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_NE] = texname;
        }
        // SE road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    getBasicSERoad(1*(float)i/(float)h, 1*(float)j/(float)w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadSE.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }
        // SW road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    getBasicSWRoad(1*(float)i/(float)h, 1*(float)j/(float)w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadSW.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }
        // NW road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    getBasicNWRoad(1*(float)i/(float)h, 1*(float)j/(float)w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadNW.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }
        // PLAZA road.
        {
            int w = w_;
            int h = w;
            int bpp = 3;
            unsigned char* texels = new unsigned char[((unsigned long)w)*h*bpp];
            unsigned char* p = texels;
            loopj(h) {
                loopi(w) {
                    float color[16];
                    cSurface::stone_plates(1*(float)i/(float)h, 1*(float)j/(float)w, 0, color);
                    *p++ = color[2] * 255;
                    *p++ = color[1] * 255;
                    *p++ = color[0] * 255;
                }
            }
            //texname = SGL::glBindTextureMipmap2D(0, true, true, true, true, w, h, bpp, texels);
            texname = SGL::glBindTexture2D(0, true, true, true, true, w, h, bpp, texels);
            std::string fname = basepath + std::string("roadPLAZA.tga");
            if (save) {
                if (saveTGA(fname.c_str(), w, h, bpp, texels)) {
                    cout << "Could not save image: " << fname << endl;
                }
            }
            delete texels;
            sTextures[KIND_ROAD_SE] = texname;
        }

    } // if sInstances == 1

    //    addRole(COLLIDEABLE); // !!
    vector_set(traceable->pos, x + 4.5f, y, z + 4.5f);
    tileKind = kind;
}

void cTile::drawSolid() {
    float* p = this->traceable->pos.data();
    float r[] = {0, 0, 0};

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    {
        long ground = sTextures[tileKind];
        glEnable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, ground);
        
        glPushMatrix();
        {
            float x = p[0] - 4.50f;
            float y = p[1] + 0.08f;
            float z = p[2] - 4.50f;
            if (r[1] < -0.01f || r[1] > 0.01f) glRotatef(r[1], 0, 1, 0);
            glTranslatef(x, y, z);
            GLfloat p[][3] = {
                {0, 0, 0},
                {9, 0, 0},
                {0, 0, 9},
                {9, 0, 9}
            };
            const float a = 0.01;
            const float b = 0.99;
            GLfloat t[][2] = {
                {a, a},
                {b, a},
                {b, b},
                {a, b},
            };
            glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);
            cPrimitives::glVertex3fv2fv(p[1], t[0]);
            cPrimitives::glVertex3fv2fv(p[0], t[1]);
            cPrimitives::glVertex3fv2fv(p[2], t[2]);
            cPrimitives::glVertex3fv2fv(p[3], t[3]);
            glEnd();
        }
        glPopMatrix();
    }
    glPopAttrib();
}


// ###############################################

