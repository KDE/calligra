/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

/* This is the color slider widget that is used to select color or color components */

/* vcolorslider.h */
#ifndef VCOLORSLIDER_H
#define VCOLORSLIDER_H

#include <qwidget.h>

class QLabel;
class QSpinBox;
class KGradientSelector;

class VColorSlider : public QWidget
{
    Q_OBJECT
public:
    VColorSlider(QWidget *parent=0, const char *name=0);
    ~VColorSlider();

public slots:
    virtual void setLabel(const QString &);
    virtual void setColors(const QColor &, const QColor &);
    virtual void setValue( int & );
    virtual void setMinValue ( int & );
    virtual void setMaxValue ( int & );

private:
	void init();
	QLabel *mLabel;
	QSpinBox *mSpinBox;
	KGradientSelector *mSelector;

};

#endif
