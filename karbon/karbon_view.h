/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#include <koView.h>
#include <ksharedptr.h>

#include "vcanvas.h"

class DCOPObject;
class KAction;
class KarbonPart;
class KFontAction;
class KFontSizeAction;
class VPainterFactory;
class KSelectAction;
class KSelectColorAction;
class KToggleAction;
class QPaintEvent;
class QLabel;
class VInsertKnotsDlg;
class VPolygonizeDlg;
class VTool;
class VToolContainer;
class VWhirlPinchDlg;
class TKUFloatSpinBoxAction;
class VColorDlg;


class KarbonView : public KoView
{
	Q_OBJECT

public:
	KarbonView( KarbonPart* part, QWidget* parent = 0, const char* name = 0 );
	virtual ~KarbonView();

	virtual DCOPObject* dcopObject();

	virtual void paintEverything( QPainter &p, const QRect &rect,
		bool transparent = false );

	virtual bool eventFilter( QObject* object, QEvent* event );

	virtual QWidget* canvas() { return m_canvas; }
	// this is the kword-solution at least:
	VCanvas* canvasWidget() { return m_canvas; }

	//const double& zoomFactor() { return m_canvas->zoomFactor(); }

	VPainterFactory* painterFactory() { return m_painterFactory; }

	// printing support, override from KoView
	virtual void setupPrinter( KPrinter &printer );
	virtual void print( KPrinter& printer );

	KarbonPart* part() { return m_part; }

	void reorganizeGUI();
	void setNumberOfRecentFiles( int number );

public slots:
	// editing:
	void editCut();
	void editCopy();
	void editPaste();
	void editSelectAll();
	void editDeselectAll();
	void editDeleteSelection();
	void editPurgeHistory();

	void selectionMoveToTop();
	void selectionMoveToBottom();
	void selectionMoveUp();
	void selectionMoveDown();

	void groupSelection();
	void ungroupSelection();

	// TODO: remove this someday:
	void dummyForTesting();

	void configure();

	void setLineWidth();

protected slots:
	// object related operations:
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
	void shearTool();
	void sinusTool();
	void spiralTool();
	void starTool();
	void textTool();

	// path:
	void pathInsertKnots();
	void pathPolygonize();
	void pathWhirlPinch();

	// view:
	void viewModeChanged();
	void zoomChanged();
	void viewColorManager();
	void refreshView();

	//toolbox dialogs - slots
	void solidFillClicked();
	void strokeClicked();
	void slotStrokeColorChanged( const QColor & );
	void slotFillColorChanged( const QColor & );

protected:
	virtual void updateReadWrite( bool rw );
	virtual void resizeEvent( QResizeEvent* event );

private:
	void initActions();

	KarbonPart* m_part;
	VCanvas* m_canvas;

	VPainterFactory* m_painterFactory;

	VTool* m_currentTool;

	// tools:
	KToggleAction* m_ellipseToolAction;
	KToggleAction* m_polygonToolAction;
	KToggleAction* m_rectangleToolAction;
	KToggleAction* m_rotateToolAction;
	KToggleAction* m_roundRectToolAction;
	KToggleAction* m_scaleToolAction;
	KToggleAction* m_selectToolAction;
	KToggleAction* m_shearToolAction;
	KToggleAction* m_sinusToolAction;
	KToggleAction* m_spiralToolAction;
	KToggleAction* m_starToolAction;
	KToggleAction* m_textToolAction;

	// text:
	KFontAction *m_setFontFamily;
	KFontSizeAction *m_setFontSize;
	KToggleAction *m_setFontBold;
	KToggleAction *m_setFontItalic;
	KToggleAction *m_setUnderline;
	//KSelectColorAction *m_setTextColor;

	// view:
	KSelectAction* m_zoomAction;
	KSelectAction* m_viewAction;

	// line width
	TKUFloatSpinBoxAction *m_setLineWidth;

	// settings:
	KAction* m_configureAction;

	// dialogs:
	VInsertKnotsDlg* m_insertKnotsDlg;
	VPolygonizeDlg* m_polygonizeDlg;
	VWhirlPinchDlg* m_whirlPinchDlg;
	VColorDlg* m_ColorManager;

	//toolbox
	KSharedPtr< VToolContainer > m_toolbox;
	DCOPObject* m_dcop;

	//status bar message
	QLabel* m_status;
};

#endif

