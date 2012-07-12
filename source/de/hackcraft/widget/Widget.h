/* 
 * File:    Widget.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 12, 2012, 10:11 PM
 */


#ifndef WIDGET_H
#define	WIDGET_H

class GapBuffer;

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
private:
    GapBuffer* text;
};

#endif	/* WIDGET_H */

