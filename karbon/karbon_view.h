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

#include "karbon_view_base.h"
#include <ksharedptr.h>
#include <kxmlguibuilder.h>
#include <koUnit.h>

class DCOPObject;
class QLabel;

class KAction;
class KarbonPart;
class KFontAction;
class KFontSizeAction;
class KSelectAction;
class KSelectColorAction;
class KToggleAction;
class KoContextHelpAction;

class KoUnitDoubleSpinComboBox;
class KoRect;

class VColorDocker;
class VDocumentDocker;
class VToolOptionsDocker;

class VFill;
class VPainterFactory;
class VSelectToolBar;
class VStateButton;
class VStroke;
class VStrokeDocker;
class VStrokeFillPreview;
class VTransformDocker;
class VCanvas;

class VTool;
class VToolFactory;
class VToolBox;

class KarbonView : public KarbonViewBase, public KXMLGUIBuilder
{
	Q_OBJECT

public:
	KarbonView( KarbonPart* part, QWidget* parent = 0L,
				const char* name = 0L );
	virtual ~KarbonView();

	virtual DCOPObject* dcopObject();

	virtual void paintEverything( QPainter &p, const QRect &rect,
								  bool transparent = false );

	bool mouseEvent( QMouseEvent* event, const KoPoint & );
	bool keyEvent( QEvent* event );

	virtual QWidget* canvas();

	VCanvas* canvasWidget() const { return m_canvas; }

	virtual VPainterFactory* painterFactory() const { return m_painterFactory; }

	KarbonPart* part() const { return (KarbonPart *)m_part; }

	// printing support, override from KoView
	virtual void setupPrinter( KPrinter &printer );
	virtual void print( KPrinter& printer );

	KoContextHelpAction* contextHelpAction() const { return m_contextHelpAction; }

	VToolOptionsDocker* toolOptionsDocker() const { return m_toolOptionsDocker; }

	void reorganizeGUI();
	void setNumberOfRecentFiles( int number );
	void setLineWidth( double val );

	QLabel* statusMessage() const { return m_status; }

	void setUnit( KoUnit::Unit _unit );

	// manage tools
	virtual void registerTool( VTool * );

	void setCursor( const QCursor & );

	void repaintAll( const KoRect & );
	void repaintAll( bool = true );

	void setPos( const KoPoint& p );

	void setViewportRect( const KoRect &rect );
	virtual void setZoomAt( double zoom, const KoPoint & = KoPoint() );

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

	void pageLayout();

	void setLineWidth();
	void selectionChanged();

	void slotActiveToolChanged( VTool * );

protected slots:
	// Object related operations.
	void objectTrafoTranslate();
	void objectTrafoScale();
	void objectTrafoRotate();
	void objectTrafoShear();

	// View.
	void viewModeChanged();
	void zoomChanged( const KoPoint & = KoPoint() );
	void viewColorManager();
	void viewToolOptions();
	void viewStrokeDocker();

	// Toolbox dialogs.
	void slotStrokeChanged( const VStroke& );
	void slotFillChanged( const VFill & );

signals:
	void zoomChanged( double );
	void selectionChange();

protected:
	virtual void updateReadWrite( bool rw );
	virtual void resizeEvent( QResizeEvent* event );
	void dragEnterEvent( QDragEnterEvent *e );
	void dropEvent( QDropEvent *e );

	//KXMLGUIBuilder
	virtual QWidget *createContainer( QWidget *parent, int index, const QDomElement &element, int &id );
	virtual void removeContainer( QWidget *container, QWidget *parent, QDomElement &element, int id );
	void addSelectionToClipboard() const;

private:
	void initActions();

	VCanvas					*m_canvas;

	VPainterFactory			*m_painterFactory;
	VStrokeFillPreview		*m_strokeFillPreview;

	VToolBox				*m_toolbox;
	VToolFactory			*m_toolFactory;
	VTool					*m_currentTool;

	KAction					*m_groupObjects;
	KAction					*m_ungroupObjects;

	// actions:
	KSelectAction			*m_zoomAction;
	KSelectAction			*m_viewAction;
	KAction					*m_configureAction;
	KoContextHelpAction		*m_contextHelpAction;
	// line width
	KoUnitDoubleSpinComboBox *m_setLineWidth;

	//dockers
	VColorDocker			*m_ColorManager;
	VDocumentDocker			*m_documentDocker;
	VStrokeDocker			*m_strokeDocker;
	VToolOptionsDocker		*m_toolOptionsDocker;
	VTransformDocker		*m_TransformDocker;

	VSelectToolBar			*m_selectToolBar;
	QLabel					*m_status;

	// dcop
	DCOPObject				*m_dcop;
};

#endif

