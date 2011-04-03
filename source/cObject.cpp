#include "cObject.h"

#include <cassert>

#include <iostream>
using std::cout;
using std::endl;

// OBJECT

std::map<std::string, rComponent*> cObject::roleprotos;
std::map<std::string, OID> cObject::roleoffsets;
//std::map<std::string, OID> cObject::roleids;
//std::map<OID, rRole*> cObject::roletypes;



// MISC FUNCTIONAL CODE

unsigned char* warningTexture(const char *fname, int *w, int* h, int* bpp) {
    *w = 8;
    *h = 8;
    *bpp = 3;
    unsigned char* bmp = new unsigned char[(*w)*(*h)*(*bpp)];
    int i, j;
    for (j = 0; j < *h; j++) {
        for (i = 0; i < *w; i++) {
            bmp[(j * (*w) + i) * 3 + 0] = ((i+j)&1) * 255;
            bmp[(j * (*w) + i) * 3 + 1] = ((i+j)&1) * 255;
            bmp[(j * (*w) + i) * 3 + 2] = 255;
        }
    }
    return bmp;
}

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
        cout << "WARNING: Could not open tga file.\n";
        return warningTexture(fname, w, h, bpp);
    };

    size_t r = fread(&tgahead, sizeof (tgahead), 1, file);

    if (r != 1) {
        fclose(file);
        cout << "WARNING: Could not read tga header.\n";
        return warningTexture(fname, w, h, bpp);
    }

    if (tgahead.imagetype != 2) {
        fclose(file);
        cout << "WARNING: Only uncompressed true color tga images supported.\n";
        return warningTexture(fname, w, h, bpp);
    };

    if (tgahead.depth != 24 && tgahead.depth != 32) {
        fclose(file);
        cout << "WARNING: Only 24 or 32 bit per pixel tga images supported.\n";
        return warningTexture(fname, w, h, bpp);
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
    if (r != 1) {
        fclose(file);
        cout << "WARNING: Could not read tga bitmap data.\n";
        return warningTexture(fname, w, h, bpp);
    }

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

