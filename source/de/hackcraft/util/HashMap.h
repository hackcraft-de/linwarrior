/* 
 * File:   HashMap.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on August 7, 2011, 1:42 PM
 */

#ifndef HASHMAP_H
#define	HASHMAP_H

#include "de/hackcraft/lang/Object.h"

#include <map>
#include <vector>

template <class T1, class T2>
class HashMap : public Object {
private:
    std::vector<T2> values;
    std::vector<T1> keys;
    int inside;
    T2 nil;
public:

    HashMap() {
        nil = NULL;
        clear();
    }

    //~HashMap() { std::cout << "~HashMap\n"; }

    void clear() {
        keys.reserve(16);
        keys.resize(16);
        values.reserve(16);
        values.resize(16);
        for (unsigned int i = 0; i < values.size(); i++) {
            keys[i] = NULL;
            values[i] = NULL;
        }
        //cout << "size: " << values.size() << "\n";
        inside = 0;
    }

    bool containsKey(const T1& key) {
        return (get(key) != nil);
    }

    T2& get(const T1& k) {
        // Look at hash location and compare key.
        int n = keys.size();
        int hash = k->hashCode() % n;
        T1& key = keys[hash];
        //std::cout << "hash: " << hash << " " << key << "\n";
        if (key == NULL) {
            // No such key.
            return nil;
        } else if (key->equals(k)) {
            // Key found.
            return values[hash];
        }
        // Not found at first location => search further.
        int i = 1;
        while (i < n) {
            hash = (hash + 1) % n;
            key = keys[hash];
            if (key->equals(k)) return values[hash];
            i++;
        }
        return nil;
    }

    bool isEmpty() {
        return (inside == 0);
    }

    void put(const T1& k, const T2& v) {
        //std::cout << "put: " << k << " => " << v << "\n";
        int n = keys.size();
        // Look at hash location and compare key.
        int hashcode = k->hashCode();
        int hash = hashcode % n;
        T1& key = keys[hash];
        //std::cout << "hashcode: " << hashcode << "  hash: " << hash << "  key: " << key << "\n";
        if (key == NULL) {
            set(hash, k, v);
            inside++;
            // Grow container?
            if (inside > (n - (n >> 2))) {
                grow(n << 1);
            }
            return;
        } else if (key->equals(k)) {
            //std::cout << "X1\n";
            set(hash, k, v);
            return;
        }
        //std::cout << "X2\n";
        //std::cout << "n = " << n << "\n";
        // Not found at first location => insert new / search further.
        int i = 0;
        while (i < n) {
            hash = (hash + 1) % n;
            key = keys[hash];
            //std::cout << "hash: " << hash << " " << key << "  (" << i << ")" << "\n";
            if (key == NULL) {
                set(hash, k, v);
                break;
            } else if (key->equals(k)) {
                set(hash, k, v);
                return;
            }
            i++;
        }
        // The key was inserted the first time.
        inside++;
        return;
    }

    void remove(const T1& k) {
        // Look at hash location and compare key.
        int n = keys.size();
        int hash = k->hashCode() % n;
        T1& key = keys[hash];
        //std::cout << "hash: " << hash << " " << key << "\n";
        if (key == NULL) {
            // No such key.
            return;
        } else if (key->equals(k)) {
            // Key found.
            set(hash, nil, nil);
            inside--;
            return;
        }
        // Not found at first location => search further.
        int i = 1;
        while (i < n) {
            hash = (hash + 1) % n;
            key = keys[hash];
            if (key == NULL) {
                // No such key.
                return;
            } else if (key->equals(k)) {
                // Key found.
                set(hash, nil, nil);
                inside--;
                return;
            }
            i++;
        }
        return;
    }

    int size() {
        return inside;
    }

private:
    
    void set(int index, const T1& k, const T2& v) {
        //std::cout << "HashMap::set " << index << ": " << k << " => " << v << "\n";
        keys[index] = k;
        values[index] = v;
    }

    void grow(int size) {
        //std::cout << "HashMap::grow start >>>>>>" << size << "\n";
        std::vector<T1> old_keys = keys;
        std::vector<T2> old_values = values;
        
        keys.reserve(size);
        keys.resize(size);
        values.reserve(size);
        values.resize(size);
        for (int i = 0; i < size; i++) {
            keys[i] = NULL;
            values[i] = NULL;
        }
        inside = 0;
        
        int n = old_keys.size();
        for (int i = 0; i < n; i++) if (old_keys[i] != NULL) {
            put(old_keys[i], old_values[i]);
        }
        //std::cout << "HashMap::grow end <<<<<<" << size << "\n";
    }
};

#endif	/* HASHMAP_H */

