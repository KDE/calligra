/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <qtoolbar.h>

class KarbonView;
class QButtonGroup;

class VToolContainer : public QToolBar
{
	Q_OBJECT
public:
	VToolContainer( KarbonView* parent = 0L, const char* name = 0L );
	~VToolContainer();
private:
	QButtonGroup *btngroup;
	enum ButtonChoice { Select, Scale, Rotate, Shear, Ellipse, Rectangle, Roundrect, Polygon, Star, Sinus, Spiral };
private slots:
	void slotButton( int ID );
};

#endif
