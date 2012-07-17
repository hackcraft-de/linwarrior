/* 
 * File:    Widget.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 12, 2012, 10:11 PM
 */


#ifndef WIDGET_H
#define	WIDGET_H

class Widget;

#include <list>

class Border;
class GapBuffer;
class Layout;

/*
 * Models an all-in-one widget:
 * Button, Toggle-Button, Label, TextArea, TextField, Console, Panel
 * Basically a clickable and editable text.
 * Should render to a texture/screen.
 */
class Widget {
public:
    Widget();
    virtual ~Widget();
    
    
    /** Mark this widget and parents dirty (need validation/re-layout) */
    void invalidate();
    /** Layout this widget if dirty (and all dirty sub-widgets as necessary). */
    void validate();
    
    double* getPosition();
    double* getSize();
    double* getPreferredSize();
    double* getMaximumSize();
    double* getMinimumSize();
    GapBuffer* getText() const;
    Border* getBorder() const;
    Layout* getLayout() const;
    std::list<Widget*>* getWidgets() const;
    Widget* getParent() const;
    bool isDirty() const;
    
    void setPosition(double* position);
    void setPosition(double positionX, double positionY);
    void setSize(double* size);
    void setSize(double sizeX, double sizeY);
    void setPreferredSize(double* preferredSize);
    void setPreferredSize(double preferredSizeX, double preferredSizeY);
    void setMaximumSize(double* maximumSize);
    void setMaximumSize(double maximumSizeX, double maximumSizeY);
    void setMinimumSize(double* minimumSize);
    void setMinimumSize(double minimumSizeX, double minimumSizeY);
    void setText(GapBuffer* text);
    void setBorder(Border* border);
    void setLayout(Layout* layout);
    void setWidgets(std::list<Widget*>* widgets);
    void setParent(Widget* parent);
    void setDirty(bool dirty);
private:
    /** Marks this widget as dirty - ie. need re-validation (re-layout) and repainting. */
    bool dirty;
    /** Parent container component containing this component. */
    Widget* parent;
    /** Sub-widgets */
    std::list<Widget*>* widgets;
    /** Layout-Manager responsible for sub-widget layout. */
    Layout* layout;
    /** Visual border of this widget within the bounds of the total area - defines insets. */
    Border* border;
    /** Text-buffer with editing capabilities. */
    GapBuffer* text;
    /** Layout input: 2d Minimum size for this widget. */
    double* minimumSize;
    /** Layout input: 2d Maximum size for this widget. */
    double* maximumSize;
    /** Layout input: 2d Preferred size for this widget. */
    double* preferredSize;
    /** Layout output: 2d Actual size of this widget. */
    double* size;
    /** Layout output: 2d Actual absolute position of this widget. */
    double* position;
};

#endif	/* WIDGET_H */

