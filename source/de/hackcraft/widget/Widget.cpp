#include "Widget.h"


#include "de/hackcraft/widget/Border.h"
#include "de/hackcraft/widget/Layout.h"

#include "de/hackcraft/util/GapBuffer.h"


Widget::Widget() {
    text = new GapBuffer();
    
    dirty = false;
    parent = NULL;
    widgets = new std::list<Widget*>();
    layout = new Layout();
    border = new Border();
    
    size = new double[2];
    minimumSize = new double[2];
    maximumSize = new double[2];
    preferredSize = new double[2];
    position = new double[2];
    
    setSize(50, 50);
    setMinimumSize(0, 0);
    setMaximumSize(100, 100);
    setPreferredSize(50, 50);
    setPosition(0, 0);
}


Widget::~Widget() {
    delete text;
    
    delete[] size;
    delete[] minimumSize;
    delete[] maximumSize;
    delete[] preferredSize;
    delete[] position;
}


void Widget::invalidate() {
    
    dirty = true;

    if (parent != NULL) {
        parent->invalidate();
    }
}


void Widget::validate() {
    
    if (layout != NULL) {
        layout->applyLayout(this);
    }
    
    dirty = false;
}

double* Widget::getPosition() {
    return position;
}

double* Widget::getSize() {
    return size;
}

double* Widget::getPreferredSize() {
    return preferredSize;
}

double* Widget::getMaximumSize() {
    return maximumSize;
}

double* Widget::getMinimumSize() {
    return minimumSize;
}

GapBuffer* Widget::getText() const {
    return text;
}

Border* Widget::getBorder() const {
    return border;
}

Layout* Widget::getLayout() const {
    return layout;
}

std::list<Widget*>* Widget::getWidgets() const {
    return widgets;
}

Widget* Widget::getParent() const {
    return parent;
}

bool Widget::isDirty() const {
    return dirty;
}

void Widget::setPosition(double* position) {
    this->position = position;
}

void Widget::setPosition(double positionX, double positionY) {
    this->position[0] = positionX;
    this->position[1] = positionY;
}

void Widget::setSize(double* size) {
    this->size = size;
}

void Widget::setSize(double sizeX, double sizeY) {
    this->size[0] = sizeX;
    this->size[1] = sizeY;
}

void Widget::setPreferredSize(double* preferredSize) {
    this->preferredSize = preferredSize;
}

void Widget::setPreferredSize(double preferredSizeX, double preferredSizeY) {
    this->preferredSize[0] = preferredSizeX;
    this->preferredSize[1] = preferredSizeY;
}

void Widget::setMaximumSize(double* maximumSize) {
    this->maximumSize = maximumSize;
}

void Widget::setMaximumSize(double maximumSizeX, double maximumSizeY) {
    this->maximumSize[0] = maximumSizeX;
    this->maximumSize[1] = maximumSizeY;
}

void Widget::setMinimumSize(double* minimumSize) {
    this->minimumSize = minimumSize;
}

void Widget::setMinimumSize(double minimumSizeX, double minimumSizeY) {
    this->minimumSize[0] = minimumSizeX;
    this->minimumSize[1] = minimumSizeY;
}

void Widget::setText(GapBuffer* text) {
    this->text = text;
}

void Widget::setBorder(Border* border) {
    this->border = border;
}

void Widget::setLayout(Layout* layout) {
    this->layout = layout;
}

void Widget::setWidgets(std::list<Widget*>* widgets) {
    this->widgets = widgets;
}

void Widget::setParent(Widget* parent) {
    this->parent = parent;
}

void Widget::setDirty(bool dirty) {
    this->dirty = dirty;
}

