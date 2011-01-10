#include "cObject.h"

#include "psi3d/macros.h"

#include <cassert>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;


int cObject::ENABLE_TEXTURE_3D = 1;
std::map<std::string, OID> cObject::roleids;
std::map<OID, rRole*> cObject::roletypes;


unsigned char* loadTGA(const char *fname, int *w, int* h, int* bpp) {
    typedef unsigned char BYTE;

    struct {
        BYTE idlength;
        BYTE colormaptype;
        BYTE imagetype;
        BYTE colormapspec[5];
        BYTE xorigin[2];
        BYTE yorigin[2];
        BYTE width[2];
        BYTE height[2];
        BYTE depth;
        BYTE descriptor;
    } tgahead;

    FILE *file;

    file = fopen(fname, "rb");
    if (file == NULL) {
        return NULL;
    };

    size_t r = fread(&tgahead, sizeof (tgahead), 1, file);
    if (r != 1) throw "Could not read tga header.";

    if (tgahead.imagetype != 2) {
        fclose(file);
        throw "Only uncompressed true color tga images supported.";
        return NULL;
    };

    if (tgahead.depth != 24 && tgahead.depth != 32) {
        throw "Only 24 or 32 bit per pixel tga images supported.";
        return NULL;
    }

    for (int j = 0; j < tgahead.idlength; j++) fgetc(file);

#ifdef __BIG_ENDIAN__
    *w = (tgahead.width[0] << 8L)+(tgahead.width[1]);
    *h = (tgahead.height[0] << 8L)+(tgahead.height[1]);
#else
    *w = (tgahead.width[1] << 8L)+(tgahead.width[0]);
    *h = (tgahead.height[1] << 8L)+(tgahead.height[0]);
#endif
    *bpp = tgahead.depth >> 3;

    unsigned long size = (*w) * (*h) * (*bpp);
    unsigned char* bmp = new unsigned char[size];
    assert(bmp != NULL);
    r = fread(bmp, size, 1, file);
    if (r != 1) throw "Could not read tga bitmap data.";

    fclose(file);
    return bmp;
}


int saveTGA(const char *fname, int w, int h, int bpp, unsigned char* image) {
    typedef unsigned char BYTE;

    struct {
        BYTE idlength;
        BYTE colormaptype;
        BYTE imagetype;
        BYTE colormapspec[5];
        BYTE xorigin[2];
        BYTE yorigin[2];
        BYTE width[2];
        BYTE height[2];
        BYTE depth;
        BYTE descriptor;
    } tgahead;

    FILE *file;

    file = fopen(fname, "wb");
    if (file == NULL) {
        return -1;
    };

    tgahead.idlength = 0;
    tgahead.colormaptype = 0;
    tgahead.colormapspec[0] = 0;
    tgahead.colormapspec[1] = 0;
    tgahead.colormapspec[2] = 0;
    tgahead.colormapspec[3] = 0;
    tgahead.colormapspec[4] = 0;
    
    // Only uncompressed true color tga images supported.
    tgahead.imagetype = 2;

    tgahead.depth = bpp << 3;
    if (tgahead.depth != 24 && tgahead.depth != 32) {
        throw "Only 24 or 32 bit per pixel tga images supported.";
        return -1;
    }

    tgahead.xorigin[0] = 0;
    tgahead.xorigin[1] = 0;
    tgahead.yorigin[0] = 0;
    tgahead.yorigin[1] = 0;

#ifdef __BIG_ENDIAN__
    tgahead.width[0] = (w & 0xFF00) >> 8;
    tgahead.width[1] = (w & 0x00FF);
    tgahead.height[0] = (h & 0xFF00) >> 8;
    tgahead.height[1] = (h & 0x00FF);
#else
    tgahead.width[1] = (w & 0xFF00) >> 8;
    tgahead.width[0] = (w & 0x00FF);
    tgahead.height[1] = (h & 0xFF00) >> 8;
    tgahead.height[0] = (h & 0x00FF);
#endif

    size_t r = fwrite(&tgahead, sizeof (tgahead), 1, file);
    if (r != 1) throw "Could not write tga header.";

    unsigned long size = (w) * (h) * (bpp);
    if (image == NULL) {
        throw "Can't write NULL image.";
        return -1;
    }
    r = fwrite(image, size, 1, file);
    if (r != 1) throw "Could not write tga bitmap data.";

    fclose(file);
    return 0;
}






