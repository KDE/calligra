/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <qtoolbar.h>
#include <qptrdict.h>
#include <ksharedptr.h>

class KoMainWindow;
class KarbonPart;
class KoView;
class QButtonGroup;
class KDualColorButton;

class VToolContainer : public QToolBar, public KShared
{
	Q_OBJECT

public:
	static VToolContainer* instance( KarbonPart* part, KoView* parent = 0L,
		const char* name = 0L );
	~VToolContainer();

	QButtonGroup *btngroup;
	QButtonGroup *dlggroup;
	KDualColorButton *m_dualColorButton;

private:
	enum ButtonChoice
	{
		Select, Scale, Rotate, Shear, Ellipse, Rectangle,
		Roundrect, Polygon, Star, Sinus, Spiral, Text
	};

	enum DlgChoice { Outline, SolidFill, Gradient };

	static QPtrDict< VToolContainer > m_containers;

	VToolContainer( KoView* parent = 0L, const char* name = 0L );

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
	void strokeColorChanged( const QColor& );
	void fillColorChanged( const QColor& );
};

#endif

