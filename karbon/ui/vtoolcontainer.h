/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <qtoolbar.h>
//#include <koMainWindow.h>
//#include <koView.h>

class KoMainWindow;
class KoView;
class QButtonGroup;

class VToolContainer : public QToolBar
{
	Q_OBJECT
public:
	VToolContainer( KoView* parent = 0L, const char* name = 0L );
	~VToolContainer();
	QButtonGroup *btngroup;
private:
	enum ButtonChoice { Select, Scale, Rotate, Shear, Ellipse, Rectangle, Roundrect, Polygon, Star, Sinus, Spiral };
private slots:
	void slotButton( int ID );
};

#endif
