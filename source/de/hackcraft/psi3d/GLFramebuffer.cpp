#include "GLFramebuffer.h"

#include "de/hackcraft/opengl/GL.h"


GLFramebuffer::GLFramebuffer() {
    initBuiltin(0, 0);
}


unsigned int GLFramebuffer::getFramebuffer() {
    return framebuffer;
}


unsigned int GLFramebuffer::getColorbuffer() {
    return colorbuffer;
}


unsigned int GLFramebuffer::getDepthbuffer() {
    return depthbuffer;
}


unsigned int GLFramebuffer::getWidth() {
    return width;
}


unsigned int GLFramebuffer::getHeight() {
    return height;
}


void GLFramebuffer::initBuiltin(int w, int h) {
    
    width = w;
    height = h;
    framebuffer = 0;
    colorbuffer = 0;
    depthbuffer = 0;
}


void GLFramebuffer::initBuffers(int w, int h) {

    this->width = w;
    this->height = h;

    unsigned int colorbuffer;
    unsigned int depthbuffer;
    unsigned int framebuffer;

    void* undefinedTexels = NULL;

    // Generate and initialize color-buffer.
    GL::glGenTextures(1, &colorbuffer);
    GL::glBindTexture(GL_TEXTURE_2D, colorbuffer);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, undefinedTexels);
    GL::glGenerateMipmapEXT(GL_TEXTURE_2D);

    // Generate and initialize depth-buffer.
    GL::glGenTextures(1, &depthbuffer);
    GL::glBindTexture(GL_TEXTURE_2D, depthbuffer);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    GL::glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, undefinedTexels);
    GL::glGenerateMipmapEXT(GL_TEXTURE_2D);

    // Generate frame-buffer and attach color- and depth-buffer.
    GL::glGenFramebuffersEXT(1, &framebuffer);
    GL::glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
    GL::glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorbuffer, 0/*mipmap level*/);
    GL::glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthbuffer, 0/*mipmap level*/);

    this->colorbuffer = colorbuffer;
    this->depthbuffer = depthbuffer;
    this->framebuffer = framebuffer;
    
    //Does the GPU support current FBO configuration?
    GL::GLenum status;
    status = GL::glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        throw "Could not initialize frame-buffer.";
    }
    
    // Switch back to normal builtin frame buffer for now.
    GL::glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void GLFramebuffer::bindBuffers() {
    GL::glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
}


void GLFramebuffer::unbindBuffers() {
    GL::glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void GLFramebuffer::cleanBuffers() {
    //Delete resources
    GL::glDeleteTextures(1, &colorbuffer);
    GL::glDeleteTextures(1, &depthbuffer);
    //Bind 0, which means render to back buffer, as a result, fb is unbound
    GL::glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    GL::glDeleteFramebuffersEXT(1, &framebuffer);
}

