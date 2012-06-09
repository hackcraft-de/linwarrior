/* 
 * File:    Geomap.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on April 21, 2012, 6:08 PM
 */


#ifndef GEOMAP_H
#define	GEOMAP_H

#include <unordered_map>
#include <list>
#include <stdlib.h>

/**
 * Generic Geo-Hash-Table for spatial indexing.
 */
template <class T>
class Geomap {
public:
    void clear() { 
        unsigned long geosize = geomap.size();
        geomap.clear();
        geomap.reserve(geosize * 2);
    }


    void put(long x, long z, T value) {
        Geokey key = getGeokey(x, z);
        geomap[key].push_back(value);
    }
    
    
    std::list<T>* getGeoInterval(float* min2f, float* max2f) {
        std::list<T>* found = new std::list<T>();
        const long f = 1 << 5;

        long ax = ((long) min2f[0] / f) * f;
        long az = ((long) min2f[1] / f) * f;
        long bx = ((long) max2f[0] / f) * f;
        long bz = ((long) max2f[1] / f) * f;

        int n = 0;
        for (long j = az - f * 1; j <= bz + f * 1; j += f) {
            for (long i = ax - f * 1; i <= bx + f * 1; i += f) {
                std::list<T>* l = &(geomap[getGeokey(i, j)]);
                if (l != NULL) {
                    //cout << "found " << l->size() << endl;
                    found->insert(found->begin(), l->begin(), l->end());
                }
                n++;
            }
        }
        //cout << min2f[0] << ":" << min2f[1] << " - " << max2f[0] << ":" << max2f[1] << " " << n <<  " FOUND " << found->size() << endl;
        //cout << ax << ":" << az << " - " << bx << ":" << bz << " " << n <<  " FOUND " << found->size() << endl;

        return found;
    }

private:
    typedef unsigned long long Geokey;
    std::unordered_map<Geokey,std::list<T> > geomap;
    
    Geokey getGeokey(long x, long z) {
        Geokey xpart = ((Geokey) ((long(x))&0xFFFFFFFF)) >> 5;
        //cout << x << " ~ " << xpart << endl;
        Geokey zpart = ((Geokey) ((long(z))&0xFFFFFFFF)) >> 5;
        Geokey key = (xpart << 32) | zpart;
        //cout << key << endl;
        return key;
    };
};

#endif	/* GEOMAP_H */

