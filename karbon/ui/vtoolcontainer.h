/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <qwidget.h>

class QButtonGroup;
class QPushButton;

class VToolContainer : public QWidget
{
	Q_OBJECT
public:
	VToolContainer( QObject* parent = 0L, const char* name = 0L );
	~VToolContainer();
private:
	QButtonGroup *btngroup;
	enum ButtonChoice { Select, Scale, Rotate, Shear, Ellipse, Rectangle, Roundrect, Polygon, Star, Sinus, Spiral };
private slots:
	void slotButton( int ID );
};

#endif
