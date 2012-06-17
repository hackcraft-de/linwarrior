/* 
 * File:     String.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on August 7, 2011, 1:39 PM
 */

#ifndef STRING_H
#define	STRING_H

#include "de/hackcraft/lang/Object.h"

#include <string>

class String : public Object {
    std::string value;
public:
    String();
    
    String(const char* s);
    
    ~String();
    
    bool equals(String* str);
    
    int hashCode();
    
    String* toString();
    
    const char* c_str();
};

#endif	/* STRING_H */

