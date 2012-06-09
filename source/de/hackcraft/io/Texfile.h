/* 
 * File:   Texfile.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on June 3, 2011, 11:22 PM
 */

#ifndef TEXFILE_H
#define	TEXFILE_H

/**
 * Class for loading (uncompressed tga) textures.
 */
struct Texfile {

    /**
     * Utility function that generates a warning image.
     * Used by loaders in case of failure.
     *
     * @param fname
     * @param w
     * @param h
     * @param bpp
     * @return
     */
    static unsigned char* warningTexture(const char *fname, int *w, int* h, int* bpp);

    /**
     * Utility function to load uncompressed lower-left 24 or 32 bit tga bitmaps.
     * Note that zipped tga files may be smaller than other common compressed
     * formats when zipped => Distribution size is less than or similar to
     * when png or jpg is used, the installation size is larger but
     * the quality is as good as it gets - assuming 8 bits per pixel.
     * Besides tga is one of the simplest and fastest to load.
     * An enhancement may be to have *.tga.gz if at all.
     *
     * @param w
     * @param h
     * @param bpp
     * @return
     */
    static unsigned char* loadTGA(const char *fname, int *w, int* h, int* bpp);

    /**
     * Utility function to save uncompressed lower-left 24 or 32 bit tga bitmaps.
     * Note that zipped tga files may be smaller than other common compressed
     * formats when zipped => Distribution size is less than or similar to
     * when png or jpg is used, the installation size is larger but
     * the quality is as good as it gets - assuming 8 bits per pixel.
     * Besides tga is one of the simplest and fastest to load.
     * An enhancement may be to have *.tga.gz if at all.
     *
     * @param w
     * @param h
     * @param bpp
     * @param image
     * @return
     */
    static int saveTGA(const char *fname, int w, int h, int bpp, unsigned char* image);
};

#endif	/* TEXFILE_H */

