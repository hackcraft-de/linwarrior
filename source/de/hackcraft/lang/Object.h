/* 
 * File:   Object.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on June 3, 2011, 9:59 PM
 */

#ifndef HCOBJECT_H
#define HCOBJECT_H

class String;

class Object {
public:
    virtual Object* clone() {
        return new Object();
    }
    
    virtual bool equals(const Object* other) {
        return (this == other);
    }
    
    virtual int hashCode() {
        return 0;
    }
    
    virtual String* toString() {
        return (String*) 0;
    }
};

#endif
