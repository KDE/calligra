/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <qtoolbar.h>

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
	QButtonGroup *dlggroup;
private:
	enum ButtonChoice { Select, Scale, Rotate, Shear, Ellipse, Rectangle, Roundrect, Polygon, Star, Sinus, Spiral, Text };
	enum DlgChoice { Outline, SolidFill, Gradient };
signals:
	// shape-tools:
	void ellipseToolActivated();
	void polygonToolActivated();
	void rectangleToolActivated();
	void roundRectToolActivated();
	void selectToolActivated();
	void rotateToolActivated();
	void scaleToolActivated();
	void shearToolActivated();
	void sinusToolActivated();
	void spiralToolActivated();
	void starToolActivated();
	void textToolActivated();
	void solidFillActivated();
	void strokeActivated();
};

#endif
