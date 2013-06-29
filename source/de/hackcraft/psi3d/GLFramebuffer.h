/* 
 * File:   GLFramebuffer.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on June 29, 2013, 4:24 PM
 */

#ifndef GLFRAMEBUFFER_H
#define	GLFRAMEBUFFER_H

/**
 * Framebuffer-Object for offscreen and texture rendering according to:
 * https://www.opengl.org/wiki/Framebuffer_Object_Examples
 * 
 * May change because it's said to deliver higher performance
 * to recycle one frame-buffer and just exchange the textures.
 * That is given they have the same size - which may/not be the case.
 * 
 * Basically it just works by creating textures for buffers and binding
 * them to the frame-buffer-object which is just a container for the bindings.
 * Once that is done the frame-buffer-object can be bound and unbound
 * to enable and disable rendering to that one instead of the usual
 * screen-frame-buffer.
 * 
 * There isn't much configuration of the buffer properties right now -
 * just using sensible usual quality values.
 */
class GLFramebuffer {
private:
    unsigned int width;
    unsigned int height;
    unsigned int framebuffer;
    unsigned int colorbuffer;
    unsigned int depthbuffer;
    
public:
    GLFramebuffer();
    
    bool isBuiltin();
    
    unsigned int getFramebuffer();

    unsigned int getColorbuffer();
    unsigned int getDepthbuffer();
    
    unsigned int getWidth();
    unsigned int getHeight();
    
    void initBuiltin(int w, int h);
    void initBuffers(int w, int h);
    void initBuffers(int w, int h, bool repeat_x, bool repeat_y);
    void cleanBuffers();
    
    void bindBuffers();
    void unbindBuffers();
};

#endif	/* GLFRAMEBUFFER_H */

