/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <koView.h>
#include <ksharedptr.h>
#include <kxmlguibuilder.h>
#include <koUnit.h>
#include "vcanvas.h"


class DCOPObject;
class QDockArea;
class QLabel;
class QPaintEvent;

class KAction;
class KarbonPart;
class KFontAction;
class KFontSizeAction;
class KSelectAction;
class KSelectColorAction;
class KToggleAction;

class TKUFloatSpinBoxAction;

class VColorDocker;
class VLayersDocker;
class VContextHelpDocker;
class VHistoryDocker;
class VToolOptionsDocker;

class VClipartTool;
class VEllipseTool;
class VFill;
class VFlattenDlg;
class VGradientTool;
class VInsertKnotsDlg;
class VPainterFactory;
class VPatternTool;
class VPolygonTool;
class VPolylineTool;
class VRectangleTool;
class VRotateTool;
class VRoundCornersDlg;
class VRoundRectTool;
class VScaleTool;
class VSelectNodesTool;
class VSelectTool;
class VSelectToolBar;
class VShearTool;
class VSinusTool;
class VSpiralTool;
class VStarTool;
class VStateButton;
class VStroke;
class VStrokeDocker;
class VStrokeFillPreview;

#ifdef HAVE_KARBONTEXT
class VTextTool;
#endif

class VTool;
class VToolBox;
class VTransformDlg;
class VWhirlPinchDlg;


class KarbonView : public KoView, public KXMLGUIBuilder
{
	Q_OBJECT

public:
	KarbonView( KarbonPart* part, QWidget* parent = 0L,
				const char* name = 0L );
	virtual ~KarbonView();

	virtual DCOPObject* dcopObject();

	virtual void paintEverything( QPainter &p, const QRect &rect,
								  bool transparent = false );

	virtual bool eventFilter( QObject* object, QEvent* event );

	virtual QWidget* canvas()
	{
		return m_canvas;
	}

	VCanvas* canvasWidget() const
	{
		return m_canvas;
	}

	VPainterFactory* painterFactory() const
	{
		return m_painterFactory;
	}

	// printing support, override from KoView
	virtual void setupPrinter( KPrinter &printer );
	virtual void print( KPrinter& printer );

	KarbonPart* part() const
	{
		return m_part;
	}

	VLayersDocker* layersDocker() const
	{
		return m_layersDocker;
	}

	VContextHelpDocker* contextHelpDocker() const
	{
		return m_contextHelpDocker;
	}

	VToolOptionsDocker* toolOptionsDocker() const
	{
		return m_toolOptionsDocker;
	}

	void reorganizeGUI();
	void setNumberOfRecentFiles( int number );
	void setLineWidth( double val );

	QLabel* statusMessage() const
	{
		return m_status;
	}

	void setUnit( KoUnit::Unit _unit );

public slots:
	// editing:
	void editCut();
	void editCopy();
	void editPaste();
	void editSelectAll();
	void editDeselectAll();
	void editDeleteSelection();
	void editPurgeHistory();

	void selectionBringToFront();
	void selectionSendToBack();
	void selectionMoveUp();
	void selectionMoveDown();

	void groupSelection();
	void ungroupSelection();

	// TODO: remove this someday:
	void dummyForTesting();

	void configure();

	void setLineWidth();
	void selectionChanged();

	// Path.
	void pathInsertKnots();
	void pathFlatten();
	void pathRoundCorners();
	void pathWhirlPinch();

protected slots:
	// Object related operations.
	void objectTrafoTranslate();
	void objectTrafoScale();
	void objectTrafoRotate();
	void objectTrafoShear();

	// Shape Tools.
	void ellipseTool();
	void polygonTool();
	void rectangleTool();
	void roundRectTool();
	void selectTool();
	void selectNodesTool();
	void rotateTool();
	void shearTool();
	void sinusTool();
	void spiralTool();
	void starTool();
	void textTool();
	void gradTool();
	void polylineTool();
	void clipartTool();
	void patternTool();

	// View.
	void viewModeChanged();
	void zoomChanged();
	void viewColorManager();
	void viewLayersDocker();
	void viewToolOptions();
	void viewStrokeDocker();
	void viewContextHelp();
	void viewHistory();

	// Toolbox dialogs.
	void solidFillClicked();
	void strokeClicked();
	void slotStrokeChanged( const VStroke& );
	void slotFillChanged( const VFill & );
	void slotJoinStyleClicked();
	void slotCapStyleClicked();

protected:
	virtual void updateReadWrite( bool rw );
	virtual void resizeEvent( QResizeEvent* event );
	void dragEnterEvent( QDragEnterEvent *e );
	void dropEvent ( QDropEvent *e );

	//KXMLGUIBuilder
	virtual QWidget *createContainer( QWidget *parent, int index, const QDomElement &element, int &id );
	virtual void removeContainer( QWidget *container, QWidget *parent, QDomElement &element, int id );

private:
	void initActions();

	KarbonPart* m_part;
	VCanvas* m_canvas;

	VPainterFactory* m_painterFactory;

	VTool* m_currentTool;

	// tools:
	VEllipseTool* m_ellipseTool;
	VPolygonTool* m_polygonTool;
	VRectangleTool* m_rectangleTool;
	VRotateTool* m_rotateTool;
	VRoundRectTool* m_roundRectTool;
	VSelectTool* m_selectTool;
	VSelectNodesTool* m_selectNodesTool;
	VShearTool* m_shearTool;
	VSinusTool* m_sinusTool;
	VSpiralTool* m_spiralTool;
	VStarTool* m_starTool;
	VGradientTool* m_gradTool;
	VPolylineTool* m_polylineTool;
	VClipartTool* m_clipartTool;
	VPatternTool* m_patternTool;
#ifdef HAVE_KARBONTEXT
	VTextTool* m_textTool;
#endif

	// text:
	KFontAction *m_setFontFamily;
	KFontSizeAction *m_setFontSize;
	KToggleAction *m_setFontBold;
	KToggleAction *m_setFontItalic;
	KToggleAction *m_setUnderline;
	KAction *m_groupObjects;
	KAction *m_ungroupObjects;
	//KSelectColorAction *m_setTextColor;

	// view:
	KSelectAction* m_zoomAction;
	KSelectAction* m_viewAction;

	// line width
	TKUFloatSpinBoxAction *m_setLineWidth;

	// line join style
	VStateButton *m_joinStyle;
	VStateButton *m_capStyle;

	// settings:
	KAction* m_configureAction;

	// dialogs:
	VInsertKnotsDlg* m_insertKnotsDlg;
	VFlattenDlg* m_flattenDlg;
	VRoundCornersDlg* m_roundCornersDlg;
	VWhirlPinchDlg* m_whirlPinchDlg;
	VTransformDlg* m_TransformDlg;

	//dockers
	VColorDocker* m_ColorManager;
	VLayersDocker* m_layersDocker;
	VContextHelpDocker* m_contextHelpDocker;
	VToolOptionsDocker* m_toolOptionsDocker;
	VStrokeDocker* m_strokeDocker;
	VHistoryDocker* m_historyDocker;

	//toolbox
	DCOPObject* m_dcop;

	VToolBox *m_toolbox;

	//toolbars
	VSelectToolBar* m_selectToolBar;

	//status bar message
	QLabel* m_status;

	VStrokeFillPreview *m_strokeFillPreview;
};

#endif

