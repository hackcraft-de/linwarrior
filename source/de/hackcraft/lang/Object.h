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
    virtual bool equals(const Object* other);
    
    virtual int hashCode();
    
    virtual String* toString();
    
protected:
    virtual Object* clone();
};

#endif
