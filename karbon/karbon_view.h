/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include <koView.h>

#include "vcanvas.h"

class QPaintEvent;

class KAction;
class KSelectAction;
class KToggleAction;

class KarbonPart;
class VTool;
class VToolContainer;

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

public slots:
	void activateTool( int ID ); //tool chooser

protected slots:
	// editing:
	void editCut();
	void editCopy();
	void editPaste();
	void editSelectAll();
	void editDeselectAll();
	void editDeleteSelection();
	void editPurgeHistory();

	// object related operations:
	void objectMoveToTop();
	void objectMoveUp();
	void objectMoveDown();
	void objectMoveToBottom();
	void objectTrafoTranslate();
	void objectTrafoScale();
	void objectTrafoRotate();
	void objectTrafoShear();

	// shape-tools:
	void ellipseTool();
	void polygonTool();
	void rectangleTool();
	void roundRectTool();
	void selectTool();
	void rotateTool();
	void scaleTool();
	void sinusTool();
	void spiralTool();
	void starTool();

	// handle-tool:
	void handleTool();

	// zooming:
	void zoomChanged();

protected:
	virtual void updateReadWrite( bool rw );
	virtual void resizeEvent( QResizeEvent* event );

private:
	void initActions();

	KarbonPart* m_part;
	VCanvas* m_canvas;

	VTool* s_currentTool;

	// zoom action:
	KSelectAction* m_zoomAction;
	// shape actions:
	KToggleAction* m_ellipseToolAction;
	KToggleAction* m_polygonToolAction;
	KToggleAction* m_rectangleToolAction;
	KToggleAction* m_roundRectToolAction;
	KToggleAction* m_selectToolAction;
	KToggleAction* m_rotateToolAction;
	KToggleAction* m_scaleToolAction;
	KToggleAction* m_sinusToolAction;
	KToggleAction* m_spiralToolAction;
	KToggleAction* m_starToolAction;

	//toolbox
	VToolContainer *toolbox;
};

#endif
