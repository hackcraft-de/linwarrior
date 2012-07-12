#include "Widget.h"

#include "de/hackcraft/util/GapBuffer.h"


Widget::Widget() {
    text = new GapBuffer();
}


Widget::~Widget() {
    delete text;
}

