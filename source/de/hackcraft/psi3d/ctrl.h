/* 
 * File:   ctrl.h
 * Author: Benjamin Pickhardt
 * Home: hackcraft.de
 *
 * Created on September 15, 2008, 12:49 PM
 */

#ifndef _CTRL_H
#define	_CTRL_H

#define loop(variable, count) for (int variable = 0; variable < (int)(count); variable++)

#define loopi(n) loop(i, n)
#define loopj(n) loop(j, n)
#define loopk(n) loop(k, n)
#define loopx(n) loop(x, n)
#define loopy(n) loop(y, n)

#define loop3i(iteration) loopi(3) { iteration; }

#define loopij(in, jn) loopi(in) loopj(jn)

#define loopijk(in, jn, kn) loopi(in) loopj(jn) loopk(kn)

//#define looplist(loopvar, itemtype, listvar) \
//for (std::list<itemtype>::iterator loopvar = (listvar).begin(); loopvar != (listvar).end(); loopvar++)

//#define foreach(loopvar, collection) \
for (auto loopvar = (collection).begin(); loopvar != (collection).end(); loopvar++)

//for (typeof((collection).begin()) loopvar = (collection).begin(); loopvar != (collection).end(); loopvar++)

#define foreachNoInc(loopvar, collection) \
for (auto loopvar = (collection).begin(); loopvar != (collection).end();)

//for (typeof((collection).begin()) loopvar = (collection).begin(); loopvar != (collection).end();)

// Shortcut for a simple integer loop with counter i
// through the size of a STL-vector.
//#define loopiv(v) for (int i = 0; i < (int)(v).size(); i++)

// foldl - folding from the left
// foldl(0.0f, listvector, sumfunc)
// varname = 0 (+) listvector[0] (+) ... (+) listvector[n-1]
#define foldl(first, listvector, binaryfunc) \
({ \
    auto result = first; \
    foreach(i, listvector) { \
        if (i == listvector.begin()) result = binaryfunc(first, (*i)); \
        else result = binaryfunc(result, (*i)); \
    } \
    result; \
})

//    typeof(first) result = first; \

#define setbit(var, bit) var |= (1UL << (unsigned long) bit)
#define rstbit(var, bit) var &= ~(1UL << (unsigned long) bit)
#define togbit(var, bit) var ^= (1UL << (unsigned long) bit)
#define getbit(var, bit) (var & (1UL << (unsigned long) bit))

#define declareArray(typ, name, size, expr)  typ name[size]; loopi(size) name[i] = expr;
#define declareValue(typ, name, size, expr)  typ name = 0; loopi(size) name += expr;


#endif	/* _CTRL_H */

