/* 
 * File:    Layout.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 14, 2012, 7:40 PM
 */


#ifndef LAYOUT_H
#define	LAYOUTH

class Layout;

#include <list>

class Widget;

class Layout {
public:
    Layout();
    virtual ~Layout();
    
    void applyLayout(Widget* widget);
private:
    void applyLayout(std::list<Widget*>* widgets);
};

#endif	/* LAYOUT_H */

