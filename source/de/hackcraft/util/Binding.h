/* 
 * File:     Binding.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on October 30, 2011, 5:40 PM
 */

#ifndef BINDING_H
#define	BINDING_H

#include <string.h>

/**
 *  Represents a binding of a variable src to a variable dst of a given size.
 */
class Binding {
private:
    void* dst;
    void* src;
    unsigned int size;
    
public:
    Binding(void* dst, void* src, unsigned int size) {
        this->dst = dst;
        this->src = src;
        this->size = size;
    }
    
    /// Store from destination to source.
    void write() {
        memcpy(src, dst, size);
    }
    
    /// Load from source to destination.
    void read() {
        memcpy(dst, src, size);
    }
};


#endif	/* BINDING_H */

