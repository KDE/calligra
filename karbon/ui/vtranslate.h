/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

/* vtranslate.h */
#ifndef VTRANSLATE_H
#define VTRANSLATE_H

#include <qwidget.h>

class QCheckBox;
class QLabel;
class QPushButton;
class QString;
class KDoubleNumInput;

class VTranslate : public QWidget
{
    Q_OBJECT
public:
    VTranslate(QWidget *parent=0, const char *name=0);
    ~VTranslate();
public slots:
    void setUnits( const QString& units ); //sets the unit labels do display correct text (mm, cm, pixels etc);
private:
    QLabel *labelx;
    KDoubleNumInput *inputx; //X coordinate
    QLabel *labely;
    KDoubleNumInput *inputy; //Y coordinate
    QLabel *unit1;
    QLabel *unit2;
    QCheckBox *positionbox; //If checked, displays coordinates of selected object
    QPushButton *apdbutton; //duplicate (makes a copy of selected object(s) with a new position)
    QPushButton *apbutton; //apply new position
};

#endif
