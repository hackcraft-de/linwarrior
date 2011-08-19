/* 
 * File:   String.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
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
    String() {
    }
    
    String(const char* s) {
        value = s;
    }
    
    ~String() {
        //cout << "~String()";
    }
    
    bool equals(String* str) {
        if (str == NULL) return false;
        //std::cout << value << " ?= " << str->value << "\n";
        return (value.compare(str->value) == 0);
    }
    
    int hashCode() {
        int code = 0;
        int n = value.size();
        for (int i = 0; i < n; i++) {
                unsigned char c = value.at(i);
                code += ((code << 5) - code) + c;
        }
        return code;
    }
};

#endif	/* STRING_H */

