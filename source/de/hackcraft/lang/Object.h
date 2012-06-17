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
    virtual bool equals(const Object* other) {
        return (this == other);
    }
    
    virtual int hashCode() {
        return 0;
    }
    
    virtual String* toString() {
        return (String*) 0;
    }
    
protected:
    virtual Object* clone() {
        throw "Cloning is not supported by this object.";
    }
};

#endif
