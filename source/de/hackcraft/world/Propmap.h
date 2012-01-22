/* 
 * File:     Propmap.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on January 22, 2012, 7:27 PM
 */

#ifndef PROPMAP_H
#define	PROPMAP_H

#include <string>

class Propmap : public std::map<std::string, std::string> {
public:

    bool contains(const char* s) {
        return contains(std::string(s));
    }

    bool contains(char* s) {
        return contains(std::string(s));
    }

    bool contains(std::string s) {
        return contains(&s);
    }
    
    bool contains(std::string* s) {
        return (find(*s) != end());
    }

    
    double getProperty(const char* s, double defaultValue) {
        return getProperty(std::string(s), defaultValue);
    }
    
    double getProperty(char* s, double defaultValue) {
        return getProperty(std::string(s), defaultValue);
    }

    double getProperty(std::string s, double defaultValue) {
        return getProperty(&s, defaultValue);
    }
    
    double getProperty(std::string *s, double defaultValue) {
        if (!contains(s)) {
            return defaultValue;
        } else {
            return atof(at(*s).c_str());
        }
    }
};

#endif	/* PROPMAP_H */

