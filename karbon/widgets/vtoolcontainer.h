/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <qptrdict.h>
#include <qtoolbar.h>

#include <ksharedptr.h>

class KarbonPart;
class KDualColorButton;
class KoMainWindow;
class KoView;
class QButtonGroup;
class VStrokeFillPreview;

class VToolContainer : public QToolBar, public KShared
{
	Q_OBJECT

public:
	static VToolContainer* instance( KarbonPart* part, KoView* parent = 0L,
		const char* name = 0L );
	~VToolContainer();

// TODO: why are these public?
	QButtonGroup* btngroup;
	QButtonGroup* dlggroup;
	KDualColorButton* m_dualColorButton;
	VStrokeFillPreview* strokeFillPreview() { return m_strokeFillPreview; }

signals:
	// shape tools:
	void ellipseToolActivated();
	void fillColorChanged( const QColor& );
	void polygonToolActivated();
	void rectangleToolActivated();
	void rotateToolActivated();
	void roundRectToolActivated();
	void selectToolActivated();
	void shearToolActivated();
	void sinusToolActivated();
	void solidFillActivated();
	void spiralToolActivated();
	void starToolActivated();
	void strokeActivated();
	void strokeColorChanged( const QColor& );
	void textToolActivated();
	void gradToolActivated();

private:
	VStrokeFillPreview* m_strokeFillPreview;
	enum ButtonChoice
	{
		Select, Rotate, Shear, Ellipse, Rectangle,
		Roundrect, Polygon, Star, Sinus, Spiral, Text, Grad
	};

	enum DlgChoice { Outline, SolidFill, Gradient };

	static QPtrDict< VToolContainer > m_containers;

	VToolContainer( KoView* parent = 0L, const char* name = 0L );
};

#endif

