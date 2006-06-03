/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __KARBON_VIEW__
#define __KARBON_VIEW__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <KoView.h>
#include <QPointF>
#include <ksharedptr.h>
#include <kxmlguibuilder.h>
#include <KoUnit.h>
#include <koffice_export.h>
//Added by qt3to4:
#include <QDropEvent>
#include <QResizeEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QEvent>
class DCOPObject;
class QLabel;

class KAction;
class KarbonPart;
class KSelectAction;
class KToggleAction;
class KoContextHelpAction;
class KoLineStyleAction;

class KoUnitDoubleSpinComboBox;
class QRectF;
class VRuler;

class KoPaletteManager;
class VDocumentTab;
class VLayersTab;
class VHistoryTab;
class VStrokeDocker;
class VColorDocker;
class VStyleDocker;
class VTransformDocker;

class VFill;
class VPainterFactory;
class VSelectToolBar;
class VSmallPreview;
class VStateButton;
class VStroke;
class VStrokeFillPreview;
class KarbonCanvas;
class VStrokeFillPreview;
class VTypeButtonBox;

class VTool;
class VToolBox;
class VToolController;

class KARBONCOMMON_EXPORT KarbonView : public KoView, public KXMLGUIBuilder
{
	Q_OBJECT

public:
	KarbonView( KarbonPart* part, QWidget* parent = 0L,
				const char* name = 0L );
	virtual ~KarbonView();

	virtual DCOPObject* dcopObject();

	KarbonPart *part() const { return m_part; }

	virtual void paintEverything( QPainter &p, const QRect &rect, bool transparent = false );

	bool mouseEvent( QMouseEvent* event, const QPointF & );
	bool keyEvent( QEvent* event );
	void dropEvent( QDropEvent *e );

	virtual QWidget* canvas() const;

	KarbonCanvas* canvasWidget() const { return m_canvas; }

	virtual VPainterFactory* painterFactory() const { return m_painterFactory; }

	KoPaletteManager* paletteManager() { return m_pPaletteManager; };

	// printing support, override from KoView
	virtual void setupPrinter( KPrinter &/*printer*/ ) {}
	virtual void print( KPrinter& printer );

	KoContextHelpAction* contextHelpAction() const { return m_contextHelpAction; }

	void reorganizeGUI();
	void setNumberOfRecentFiles( unsigned int number );
	void setLineWidth( double val );

	QLabel* statusMessage() const { return m_status; }

	void setCursor( const QCursor & );

	void repaintAll( const QRectF & );
	void repaintAll( bool = true );

	void setPos( const QPointF& p );

	void setViewportRect( const QRectF &rect );
	void setZoomAt( double zoom, const QPointF & = QPointF() );

	VToolController *toolController();

	VStrokeFillPreview* strokeFillPreview()
		{ return m_strokeFillPreview; }

public slots:
	// editing:
	void editCut();
	void editCopy();
	void editPaste();
	void editSelectAll();
	void editDeselectAll();
	void editDeleteSelection();
	void editPurgeHistory();

	void selectionDuplicate();
	void selectionBringToFront();
	void selectionSendToBack();
	void selectionMoveUp();
	void selectionMoveDown();
	void selectionAlignHorizontalLeft();
	void selectionAlignHorizontalCenter();
	void selectionAlignHorizontalRight();
	void selectionAlignVerticalTop();
	void selectionAlignVerticalCenter();
	void selectionAlignVerticalBottom();

	void selectionDistributeHorizontalCenter();
	void selectionDistributeHorizontalGap();
	void selectionDistributeHorizontalLeft();
	void selectionDistributeHorizontalRight();
	void selectionDistributeVerticalCenter();
	void selectionDistributeVerticalGap();
	void selectionDistributeVerticalBottom();
	void selectionDistributeVerticalTop();

	void fileImportGraphic();

	void groupSelection();
	void ungroupSelection();

	void closePath();

	//View:
	void viewZoomIn();
	void viewZoomOut();

	void setUnit( KoUnit::Unit _unit );

	void configure();

	void pageLayout();

	void setLineWidth();
	void selectionChanged();

	void slotActiveToolChanged( VTool * );

	void togglePageMargins(bool);
	void showRuler();
	void showGrid();
	bool showPageMargins();
	void snapToGrid();

	void showSelectionPopupMenu( const QPoint &pos );

protected slots:
	// Object related operations.

	// View.
	void viewModeChanged();
	void zoomChanged( const QPointF & = QPointF() );
	void setLineStyle( int );

	// Toolbox dialogs.
	void slotStrokeChanged( const VStroke& );
	void slotFillChanged( const VFill & );

	void canvasContentsMoving( int x, int y );

signals:
	void zoomChanged( double );
	void selectionChange();
	void pageLayoutChanged();

protected:
	virtual void updateReadWrite( bool ) {}
	virtual void resizeEvent( QResizeEvent* event );

	void createDocumentTabDock();
	void createLayersTabDock();
	void createHistoryTabDock();
	void createStrokeDock();
	void createColorDock();
	void createTransformDock();
	void createResourceDock();

	//KXMLGUIBuilder
	virtual QWidget *createContainer( QWidget *parent, int index, const QDomElement &element, int &id );
	virtual void removeContainer( QWidget *container, QWidget *parent, QDomElement &element, int id );
	void addSelectionToClipboard() const;

private:
	void initActions();
	void updateRuler();

	KarbonPart		*m_part;
	KarbonCanvas		*m_canvas;
	VRuler			*m_horizRuler;
	VRuler			*m_vertRuler;

	VPainterFactory		*m_painterFactory;
	VStrokeFillPreview	*m_strokeFillPreview;
	VTypeButtonBox		*m_typeButtonBox;

	VToolBox		*m_toolbox;

	KAction			*m_groupObjects;
	KAction			*m_ungroupObjects;

	KAction			*m_closePath;

	// actions:
	KSelectAction		*m_zoomAction;
	KSelectAction		*m_viewAction;
	KAction				*m_configureAction;
	KToggleAction		*m_showRulerAction;
	KToggleAction		*m_showGridAction;
	KToggleAction		*m_snapGridAction;
	KToggleAction		*m_showPageMargins;
	KoContextHelpAction	*m_contextHelpAction;
	KAction				*m_deleteSelectionAction;
	// line width
	KoUnitDoubleSpinComboBox *m_setLineWidth;
	KoLineStyleAction	*m_lineStyleAction;

	//dockers
	KoPaletteManager	*m_pPaletteManager;
	VDocumentTab		*m_DocumentTab;
	VLayersTab			*m_LayersTab;
	VHistoryTab			*m_HistoryTab;
	VColorDocker		*m_ColorManager;
	VStrokeDocker		*m_strokeDocker;
	VStyleDocker		*m_styleDocker;
	VTransformDocker	*m_TransformDocker;

	VSelectToolBar		*m_selectToolBar;

	// dcop
	DCOPObject		*m_dcop;

	//Status Bar
	QLabel				*m_status;       // Ordinary status
	QLabel				*m_cursorCoords; // Cursor coordinates
	VSmallPreview		*m_smallPreview; // Stroke and Fill
	VToolController	*m_toolController;
};

#endif

