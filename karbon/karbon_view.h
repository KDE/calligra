/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include <koView.h>

#include "vcanvas.h"	// necessary because of canvas()

class QPaintEvent;

class KAction;
class KSelectAction;

class KarbonPart;
class VTool;

class KarbonView : public KoView
{
	Q_OBJECT
public:
	KarbonView( KarbonPart* part, QWidget* parent = 0, const char* name = 0 );
	virtual ~KarbonView();

	virtual void paintEverything( QPainter &p, const QRect &rect,
		bool transparent = false );

	virtual bool eventFilter( QObject* object, QEvent* event );

	virtual QWidget* canvas() { return m_canvas; }
	// this is the kword-solution at least:
	VCanvas* canvasWidget() { return m_canvas; }

	const double& zoomFactor() { return m_canvas->zoomFactor(); }

protected slots:
	void editCut();
	void editCopy();
	void editPaste();
	void editSelectAll();

	// shape-tools:
	void ellipseTool();
	void polygonTool();
	void rectangleTool();
	void roundRectTool();
	void selectTool();
	void sinusTool();
	void spiralTool();
	void starTool();

protected:
	virtual void updateReadWrite( bool rw );
	virtual void resizeEvent( QResizeEvent* event );

private:
	void initActions();

	KarbonPart* m_part;
	VCanvas* m_canvas;

	// i currently think that all views should be in the same state (static):
	static VTool* s_currentTool;

	// actions:
	KSelectAction* m_zoomAction;
};

#endif
