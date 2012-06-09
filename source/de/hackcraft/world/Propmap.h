/* 
 * File:     Propmap.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on January 22, 2012, 7:27 PM
 */

#ifndef PROPMAP_H
#define	PROPMAP_H

class Propmap;

#include <map>
#include <string>
#include <stdlib.h>

class Propmap {
    std::map<std::string, std::string> contents;
public:
    
    void load(const char* filename);

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
        return (contents.find(*s) != contents.end());
    }

    std::string getProperty(const char* k, const char* v) {
        return getProperty(std::string(k), std::string(v));
    }

    std::string getProperty(std::string k, std::string v) {
        return getProperty(&k, &v);
    }
    
    std::string getProperty(std::string* k, std::string* v) {
        if (!contains(*k)) {
            return *v;
        } else {
            return contents.at(*k);
        }
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
            return atof(contents.at(*s).c_str());
        }
    }
    
    void put(const char* k, const char* v) {
        put(std::string(k), std::string(v));
    }
    
    void put(char* k, char* v) {
        put(std::string(k), std::string(v));
    }
    
    void put(std::string k, std::string v) {
        put(&k, &v);
    }
    
    void put(std::string* k, std::string* v) {
        contents[*k] = *v;
    }
};

#endif	/* PROPMAP_H */

