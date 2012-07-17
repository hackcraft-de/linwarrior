#include "Layout.h"

#include "de/hackcraft/widget/Widget.h"


Layout::Layout() {
}


Layout::~Layout() {
}


void Layout::applyLayout(Widget* widget) {
    
    for ( Widget* subwidget : *widget->getWidgets() ) {
        
        double* pref = subwidget->getPreferredSize();
        subwidget->setSize(pref[0], pref[1]);
    }
}


void Layout::applyLayout(std::list<Widget*>* widgets) {
    
}

