/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers
   Copyright (C) 2005-2006 Jan Hambrecht <jaham@gmx.net>

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

#include "karbon_view.h"

#include <q3dragobject.h>
#include <qicon.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <q3popupmenu.h>
#include <qpainter.h>
#include <QResizeEvent>
#include <QPixmap>
#include <Q3ValueList>
#include <QEvent>
#include <QDropEvent>
#include <QGridLayout>
#include <QToolBar>
#include <QTimer>

#include <kaction.h>
#include <kcolormimedata.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdeversion.h>
#include <kprinter.h>
#include <kcomponentdata.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kcommand.h>
#include <kicon.h>
#include <KoMainWindow.h>
#include <KoToolBox.h>
#include <KoCreateShapesTool.h>
#include <KoFilterManager.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kstandardaction.h>
#include <KoContextHelp.h>
#include <KoUnitWidgets.h>
#include <KoPageLayoutDia.h>
#include <KoRuler.h>
#include <Kolinestyleaction.h>
#include <KoToolManager.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>
#include <KoShapeContainer.h>
#include <KoShapeGroup.h>
#include <KoShapeDeleteCommand.h>
#include <KoUngroupShapesCommand.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeDeleteCommand.h>
#include <KoShapeReorderCommand.h>
#include <KoSelection.h>
#include <KoZoomAction.h>
#include <KoShapeSelector.h>
#include <KoPathShape.h>
#include <KoPathCommand.h>
#include <KoToolBoxFactory.h>
#include <KoShapeSelectorFactory.h>
#include <KoShapeController.h>

// Commands.
#include "vclipartcmd.h"
#include "vfillcmd.h"
#include "vstrokecmd.h"
#include "vtransformcmd.h"
#include "vinsertcmd.h"

// Dialogs.
#include "vconfiguredlg.h"

// Dockers.
#include "vcolordocker.h"
#include "vdocumentdocker.h"
#include "vstrokedocker.h"
#include "vstyledocker.h"
#include "vtransformdocker.h"
#include "vlayerdocker.h"
#include <KoToolDocker.h>
#include <KoToolDockerFactory.h>
// ToolBars
//#include "vselecttoolbar.h"

// Statusbar
#include "vsmallpreview.h"

// The rest.
#include "karbon_factory.h"
#include "karbon_part.h"
#include "vglobal.h"
#include "vselection.h"
#include "vtool.h"
#include "vtoolcontroller.h"
#include "vcomposite.h"
#include "vpainterfactory.h"
#include "vqpainter.h"
#include "vstrokefillpreview.h"
#include "vtypebuttonbox.h"
#include "vstatebutton.h"
#include "vcanvas.h"
#include "KoCanvasController.h"
#include "karbon_drag.h"

#include <unistd.h>

// Only for debugging.
#include <kdebug.h>

// Uncomment the #define below to print lots of debug information about the view.
// Or use the -DKARBON_DEBUG_VIEW flag when using cmake, so the code stays the same.
#define KARBON_DEBUG_VIEW

#ifdef KARBON_DEBUG_VIEW
#define debugView(text) kDebug() << "KARBON_DEBUG_VIEW: " << text << endl
#else
#define debugView(text)
#endif

KarbonView::KarbonView( KarbonPart* p, QWidget* parent )
		: KoView( p, parent ), KXMLGUIBuilder( shell() ), m_part( p ), m_canvas( 0L )
{
	debugView("KarbonView::KarbonView");

	m_toolController = new VToolController( this );
	m_toolController->init();

	setComponentData( KarbonFactory::componentData(), true );

	setClientBuilder( this );

	if( !p->isReadWrite() )
		setXMLFile( QString::fromLatin1( "karbon_readonly.rc" ) );
	else
		setXMLFile( QString::fromLatin1( "karbon.rc" ) );

	// set up status bar message
	m_status = new KStatusBarLabel( QString::null, 0, statusBar() );
	m_status->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
	m_status->setMinimumWidth( 300 );
	addStatusBarItem( m_status, 1 );
	m_cursorCoords = new KStatusBarLabel( QString::null, 0, statusBar() );
	m_cursorCoords->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
	m_cursorCoords->setMinimumWidth( 50 );
	addStatusBarItem( m_cursorCoords, 0 );
	m_smallPreview = new VSmallPreview( this );
	addStatusBarItem( m_smallPreview );

	initActions();

	m_DocumentTab = 0L;
	m_strokeFillPreview = 0L;
	m_ColorManager = 0L;
	m_strokeDocker = 0L;
	m_styleDocker = 0L;
	m_TransformDocker = 0L;
	m_layerDocker = 0L;

	unsigned int max = part()->maxRecentFiles();
	setNumberOfRecentFiles( max );

    connect( p, SIGNAL( unitChanged( KoUnit ) ), this, SLOT( setUnit( KoUnit ) ) );

	// layout:
	QGridLayout *layout = new QGridLayout();
	layout->setMargin(0);

	// widgets:
    m_canvas = new KarbonCanvas( p );
    m_canvas->setParent( this );

    connect( m_canvas->shapeManager()->selection(), SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );

    m_canvasView = new KoCanvasController(this);
    m_canvasView->setCanvas(m_canvas);
    m_canvasView->centerCanvas( false );
    layout->addWidget(m_canvasView, 1, 1);
    m_canvasView->show();

    m_horizRuler = new KoRuler( this, Qt::Horizontal, m_canvas->viewConverter() );
    m_horizRuler->setShowMousePosition(true);
    m_horizRuler->setUnit(p->unit());
    layout->addWidget( m_horizRuler, 0, 1 );
    connect( p, SIGNAL( unitChanged( KoUnit ) ), m_horizRuler, SLOT( setUnit( KoUnit ) ) );
    connect(m_canvasView, SIGNAL(canvasOffsetXChanged(int)), this, SLOT(pageOffsetChanged()));

    m_vertRuler = new KoRuler( this, Qt::Vertical, m_canvas->viewConverter() );
    m_vertRuler->setShowMousePosition(true);
    m_vertRuler->setUnit(p->unit());
    layout->addWidget( m_vertRuler, 1, 0 );
    connect( p, SIGNAL( unitChanged( KoUnit ) ), m_horizRuler, SLOT( setUnit( KoUnit ) ) );

    connect(m_canvas, SIGNAL(documentOriginChanged( const QPoint &)), this , SLOT(pageOffsetChanged()));
    connect(m_canvasView, SIGNAL(canvasOffsetYChanged(int)), this, SLOT(pageOffsetChanged()));
    connect(m_canvasView, SIGNAL(canvasMousePositionChanged(const QPoint &)),
            this, SLOT(mousePositionChanged(const QPoint&)));

    updateRuler();
// 
	// set up factory
	m_painterFactory = new VPainterFactory;
	//m_painterFactory->setPainter( m_canvas->pixmap(), m_canvas->contentsWidth(), m_canvas->contentsHeight() );
	//m_painterFactory->setEditPainter( m_canvas->canvasWidget(), m_canvas->canvasWidget()->width(), m_canvas->canvasWidget()->height() );

	if( shell() )
	{
		KoToolManager::instance()->addController( m_canvasView );
        // set the first layer active
        m_canvasView->canvas()->shapeManager()->selection()->setActiveLayer( part()->document().layers().first() );

		//Create Dockers
		createColorDock();
		createStrokeDock();
		createTransformDock();
		//createDocumentTabDock();
		createLayersTabDock();
		//createResourceDock();

		KoToolBoxFactory toolBoxFactory( "Karbon" );
		createDockWidget( &toolBoxFactory );
		KoShapeSelectorFactory shapeSelectorFactory;
		createDockWidget( &shapeSelectorFactory );

		m_strokeFillPreview = new VStrokeFillPreview( part() );
        //createDock(i18n("Stroke/Fills"), m_strokeFillPreview);

		KoToolDockerFactory toolDockerFactory;
		KoToolDocker * toolDocker =  dynamic_cast<KoToolDocker*>( createDockWidget( &toolDockerFactory ) );
		connect(m_canvasView, SIGNAL(toolOptionWidgetChanged(QWidget*)), toolDocker, SLOT(newOptionWidget(QWidget*)));

	//	m_typeButtonBox = new VTypeButtonBox( part(), m_toolbox );

		//connect( m_strokeFillPreview, SIGNAL( fillSelected() ), m_typeButtonBox, SLOT( setFill() ) );
		//connect( m_strokeFillPreview, SIGNAL( strokeSelected() ), m_typeButtonBox, SLOT( setStroke() ) );

		connect( m_strokeFillPreview, SIGNAL( strokeChanged( const VStroke & ) ), this, SLOT( slotStrokeChanged( const VStroke & ) ) );
		connect( m_strokeFillPreview, SIGNAL( fillChanged( const VFill & ) ), this, SLOT( slotFillChanged( const VFill & ) ) );

		connect( m_strokeFillPreview, SIGNAL( strokeSelected() ), m_ColorManager, SLOT( setStrokeDocker() ) );
		connect( m_strokeFillPreview, SIGNAL( fillSelected( ) ), m_ColorManager, SLOT( setFillDocker() ) );

			//create toolbars
// 			m_selectToolBar = new VSelectToolBar( this, "selecttoolbar" );
// 			mainWindow()->addToolBar( m_selectToolBar );

		if( m_showRulerAction->isChecked() )
		{
			m_horizRuler->show();
			m_vertRuler->show();
		}
		else
		{
			m_horizRuler->hide();
			m_vertRuler->hide();
		}
	}

	setLayout(layout);

	zoomChanged();
	reorganizeGUI();
}

KarbonView::~KarbonView()
{
	//kDebug(38000) << "Handling KarbonView dtor" << endl;
	debugView("KarbonView::~KarbonView()");

	// widgets:
	delete m_smallPreview;
	delete m_status;
	delete m_cursorCoords;

	delete m_painterFactory;

	delete m_toolController;
}

QWidget*
KarbonView::canvas() const
{
	debugView("KarbonView::canvas()");

	return m_canvas;
}

void
KarbonView::resizeEvent( QResizeEvent* /*event*/ )
{
	debugView("KarbonView::resizeEvent()");

	if(!m_showRulerAction)
		return;

	if(!m_canvas)
		return;

	QSize maxSize = m_canvasView->maximumViewportSize();
	m_canvas->setVisibleSize( maxSize.width(), maxSize.height() );
	zoomChanged();
	reorganizeGUI();
}

void
KarbonView::dropEvent( QDropEvent *e )
{
	debugView("KarbonView::dropEvent()");

	//Accepts QColor - from Color Manager's KColorPatch
	VColor realcolor;
	VObjectList selection;

	QColor color = KColorMimeData::fromMimeData( e->mimeData() );
	if ( color.isValid() )
	{
		float r = color.red() / 255.0;
		float g = color.green() / 255.0;
		float b = color.blue() / 255.0;

		realcolor.set( r, g, b );

		if( part() )
			if( m_strokeFillPreview->strokeIsSelected() )
				part()->addCommand( new VStrokeCmd( &part()->document(), realcolor ), true );
			else
				part()->addCommand( new VFillCmd( &part()->document(), realcolor ), true );
	}
	else if( KarbonDrag::decode( e->mimeData(), selection, m_part->document() ) )
	{
		VObject *clipart = selection.first();
		QPointF p( e->pos() );
		p = m_canvas->viewConverter()->viewToDocument( p ); // TODO: or documentToView ?
		QMatrix mat( 1, 0, 0, 1, p.x(), p.y() );

		VTransformCmd trafo( 0L, mat );
		trafo.visit( *clipart );
		VClipartCmd* cmd = new VClipartCmd( &m_part->document(), i18n( "Insert Clipart" ), clipart );

		m_part->addCommand( cmd, true );
	}
}

void
KarbonView::print( KPrinter &printer )
{
	debugView("KarbonView::print(KPrinter)");

	// TODO : ultimately use plain QPainter here as that is better suited to print system
	//kDebug(38000) << "KarbonView::print" << endl;

	printer.setFullPage( true );

	// we are using 72 dpi internally
	double zoom = printer.logicalDpiX() / 72.0;

	QMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0, -part()->document().height()*zoom );

	double w = zoom*part()->document().width();
	double h = zoom*part()->document().height();

	QRectF rect( 0, 0, w, h );

	QPixmap img( static_cast<int>( w ), static_cast<int>( h ) );

	// first use the painter to draw into the pixmap
	VQPainter kop( ( QPaintDevice * )&img, static_cast<int>( w ), static_cast<int>( h ) );

	kop.setZoomFactor( zoom );
	kop.setMatrix( mat );

	kop.begin();

	//part()->document().draw( &kop, &rect );

	kop.end();

	QPainter p;

	// us qpainter to draw the pixmap
	// note that it is looking unsmooth when previewing,
	// but the print is actually ok as we are printing at 100% zoom anyway
	p.begin( &printer );
	p.drawPixmap( 0, 0, img );
	p.end();
}

void
KarbonView::fileImportGraphic()
{
	debugView("KarbonView::fileImportGraphic()");

	QStringList filter;
	filter << "application/x-karbon" << "image/svg+xml" << "image/x-wmf" << "image/x-eps" << "application/postscript";
	KFileDialog *dialog = new KFileDialog(KUrl("foo"), "", 0);
	dialog->setCaption("Choose Graphic to Add");
	dialog->setModal(true);
	dialog->setMimeFilter( filter, "application/x-karbon" );
	if(dialog->exec()!=QDialog::Accepted) {
		delete dialog;
		return;
	}
	QString fname = dialog->selectedFile();
	//kDebug(38000) << "in : " << fname.latin1() << endl;
	//kDebug(38000) << "part()->document()->nativeFormatMimeType().latin1() : " << part()->nativeFormatMimeType() << endl;
	//kDebug(38000) << "dialog->currentMimeFilter().latin1() : " << dialog->currentMimeFilter().latin1() << endl;
	if( part()->nativeFormatMimeType() == dialog->currentMimeFilter().toLatin1() )
		part()->mergeNativeFormat( fname );
	else
	{
		KoFilterManager man( part() );
		KoFilter::ConversionStatus status;
		QString importedFile = man.import( fname, status );
		part()->mergeNativeFormat( importedFile );
		if( !importedFile.isEmpty() )
			unlink( QFile::encodeName( importedFile ) );
	}
	delete dialog;
	part()->repaintAllViews();
}

void
KarbonView::editCut()
{
	debugView("KarbonView::editCut()");

	addSelectionToClipboard();
	// remove selection
	editDeleteSelection();
}

void
KarbonView::editCopy()
{
	debugView("KarbonView::editCopy()");

	addSelectionToClipboard();
}

void
KarbonView::addSelectionToClipboard() const
{
	debugView("KarbonView::addSelectionToClipboard()");

	if( part()->document().selection()->objects().count() <= 0 )
		return;

	KarbonDrag* kd = new KarbonDrag();
	kd->setObjectList( part()->document().selection()->objects() );
	QApplication::clipboard()->setMimeData( kd->mimeData() );
}

void
KarbonView::editPaste()
{
	debugView("KarbonView::editPaste()");

	KarbonDrag kd;
	VObjectList objects;

	if( !kd.decode( QApplication::clipboard()->mimeData(), objects, part()->document() ) )
		return;

	// Paste with a small offset.
	double copyOffset = part()->componentData().config()->readEntry( "CopyOffset", 10 );
	part()->addCommand( new VInsertCmd( &part()->document(),
										objects.count() == 1
											? i18n( "Paste Object" )
											: i18n( "Paste Objects" ),
										&objects, copyOffset ),
						true );

	part()->repaintAllViews();
	selectionChanged();
}

void
KarbonView::editSelectAll()
{
    debugView("KarbonView::editSelectAll()");

    KoSelection* selection = m_canvas->shapeManager()->selection();
    if( ! selection )
        return;

    QList<KoShape*> shapes = part()->document().shapes();
    kDebug(38000) << "shapes.size() = " << shapes.size() << endl;

    foreach( KoShape* shape, shapes )
    {
        selection->select( shape );
        shape->repaint();
    }

    selectionChanged();
}

void
KarbonView::editDeselectAll()
{
	debugView("KarbonView::editDeselectAll()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( selection )
		selection->deselectAll();

	selectionChanged();
}

void
KarbonView::editDeleteSelection()
{
	debugView("KarbonView::editDeleteSelection()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	QList<KoShape*> selectedShapes = selection->selectedShapes();
	if( selectedShapes.count() < 1)
		return;
	selection->deselectAll();

	KoShapeDeleteCommand *cmd = new KoShapeDeleteCommand( part(), selectedShapes );
	part()->KoDocument::addCommand( cmd );
}

void
KarbonView::editPurgeHistory()
{
	debugView("KarbonView::editPurgeHistory()");

	// TODO: check for history size != 0
    // TODO needs porting
/*	if( KMessageBox::warningContinueCancel( this,
			i18n( "This action cannot be undone later. Do you really want to continue?" ),
			i18n( "Purge History" ),
			KStandardGuiItem::del(),
			"edit_purge_history" ) )
	{
		part()->commandHistory()->clear();
	}*/
}

void
KarbonView::selectionAlignHorizontalLeft()
{
	debugView("KarbonView::selectionAlignHorizontalLeft()");

	selectionAlign(KoShapeAlignCommand::HorizontalLeftAlignment);
}
void
KarbonView::selectionAlignHorizontalCenter()
{
	debugView("KarbonView::selectionAlignHorizontalCenter()");

	selectionAlign(KoShapeAlignCommand::HorizontalCenterAlignment);
}

void
KarbonView::selectionAlignHorizontalRight()
{
	debugView("KarbonView::selectionAlignHorizontalRight()");

	selectionAlign(KoShapeAlignCommand::HorizontalRightAlignment);
}

void
KarbonView::selectionAlignVerticalTop()
{
	debugView("KarbonView::selectionAlignVerticalTop()");

	selectionAlign(KoShapeAlignCommand::VerticalTopAlignment);
}

void
KarbonView::selectionAlignVerticalCenter()
{
	debugView("KarbonView::selectionAlignVerticalCenter()");

	selectionAlign(KoShapeAlignCommand::VerticalCenterAlignment);
}

void
KarbonView::selectionAlignVerticalBottom()
{
	debugView("KarbonView::selectionAlignVerticalBottom()");

	selectionAlign(KoShapeAlignCommand::VerticalBottomAlignment);
}

void
KarbonView::selectionAlign(KoShapeAlignCommand::Align align)
{
	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	if( selectedShapes.count() < 1)
		return;

	QRectF bRect;
	bRect= (selectedShapes.count() == 1) ? part()->document().boundingRect() : selection->boundingRect();
	KoShapeAlignCommand *cmd = new KoShapeAlignCommand( selectedShapes, align, bRect);

	part()->KoDocument::addCommand( cmd );
}


void
KarbonView::selectionDistributeHorizontalCenter()
{
	debugView("KarbonView::selectionDistributeHorizontalCenter()");

	selectionDistribute(KoShapeDistributeCommand::HorizontalCenterDistribution);
}

void
KarbonView::selectionDistributeHorizontalGap()
{
	debugView("KarbonView::selectionDistributeHorizontalGap()");

	selectionDistribute(KoShapeDistributeCommand::HorizontalGapsDistribution);
}

void
KarbonView::selectionDistributeHorizontalLeft()
{
	debugView("KarbonView::selectionDistributeHorizontalLeft()");

	selectionDistribute(KoShapeDistributeCommand::HorizontalLeftDistribution);
}

void
KarbonView::selectionDistributeHorizontalRight()
{
	debugView("KarbonView::selectionDistributeHorizontalRight()");

	selectionDistribute(KoShapeDistributeCommand::HorizontalRightDistribution);
}

void
KarbonView::selectionDistributeVerticalCenter()
{
	debugView("KarbonView::selectionDistributeVerticalCenter()");

	selectionDistribute(KoShapeDistributeCommand::VerticalCenterDistribution);
}

void
KarbonView::selectionDistributeVerticalGap()
{
	debugView("KarbonView::selectionDistributeVerticalGap()");

	selectionDistribute(KoShapeDistributeCommand::VerticalGapsDistribution);
}

void
KarbonView::selectionDistributeVerticalBottom()
{
	debugView("KarbonView::selectionDistributeVerticalBottom()");

	selectionDistribute(KoShapeDistributeCommand::VerticalBottomDistribution);
}

void
KarbonView::selectionDistributeVerticalTop()
{
	debugView("KarbonView::selectionDistributeVerticalTop()");

	selectionDistribute(KoShapeDistributeCommand::VerticalTopDistribution);
}

void
KarbonView::selectionDistribute(KoShapeDistributeCommand::Distribute distribute)
{
	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	if( selectedShapes.count() < 2) return;

	KoShapeDistributeCommand *cmd = new KoShapeDistributeCommand( selectedShapes, distribute, selection->boundingRect());

	part()->KoDocument::addCommand( cmd );
}

void
KarbonView::selectionDuplicate()
{
	debugView("KarbonView::selectionDuplicate()");

	if ( !part()->document().selection()->objects().count() )
		return;

	VObjectList  objects;

	// Create copies of all the objects and not just the list.
	VObjectListIterator itr( part()->document().selection()->objects() );
	for ( ; itr.current() ; ++itr )	{
		objects.append( itr.current()->clone() );
	}

	// Paste with a small offset.
	double copyOffset = part()->componentData().config()->readEntry( "CopyOffset", 10 );
	part()->addCommand( new VInsertCmd( &part()->document(),
										objects.count() == 1
											? i18n( "Duplicate Object" )
											: i18n( "Duplicate Objects" ),
										&objects, copyOffset ),
						true );

	part()->repaintAllViews();
	selectionChanged();
}

void
KarbonView::selectionBringToFront()
{
    debugView("KarbonView::selectionBringToFront()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    part()->KoDocument::addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::BringToFront ) );
}

void
KarbonView::selectionMoveUp()
{
    debugView("KarbonView::selectionMoveUp()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    part()->KoDocument::addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::RaiseShape ) );
}

void
KarbonView::selectionMoveDown()
{
    debugView("KarbonView::selectionMoveDown()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    part()->KoDocument::addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::LowerShape ) );
}

void
KarbonView::selectionSendToBack()
{
    debugView("KarbonView::selectionSendToBack()");

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes( KoFlake::TopLevelSelection );
    part()->KoDocument::addCommand( KoShapeReorderCommand::createCommand( selectedShapes, m_canvas->shapeManager(), KoShapeReorderCommand::SendToBack ) );
}

void
KarbonView::groupSelection()
{
	debugView("KarbonView::groupSelection()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	KoShapeGroup *group = new KoShapeGroup();
	QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	QList<KoShape*> groupedShapes;

	// only group shapes with an unselected parent
	foreach( KoShape* shape, selectedShapes )
	{
		if( selectedShapes.contains( shape->parent() ) )
			continue;
		groupedShapes << shape;
	}
    // add group to parent of first shape to be grouped
    group->setParent( groupedShapes.first()->parent() );
	QUndoCommand *cmd = new QUndoCommand( i18n("Group shapes") );
	new KoShapeCreateCommand( m_part, group, cmd );
	new KoGroupShapesCommand( group, groupedShapes, cmd );
	part()->KoDocument::addCommand( cmd );
}

void
KarbonView::ungroupSelection()
{
	debugView("KarbonView::ungroupSelection()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	QList<KoShape*> containerSet;

	// only ungroup shape containers with an unselected parent
	foreach( KoShape* shape, selectedShapes )
	{
		if( selectedShapes.contains( shape->parent() ) )
			continue;
		containerSet << shape;
	}

	QUndoCommand *cmd = new QUndoCommand( i18n("Ungroup shapes") );

	// add a ungroup command for each found shape container to the macro command
	foreach( KoShape* shape, containerSet )
	{
		KoShapeContainer *container = dynamic_cast<KoShapeContainer*>( shape );
		if( container )
        {
			new KoUngroupShapesCommand( container, container->iterator(), cmd );
            new KoShapeDeleteCommand( m_part, container, cmd );
        }
	}
	part()->KoDocument::addCommand( cmd );
}

void
KarbonView::closePath()
{
    debugView("KarbonView::closePath()");

    // TODO add the new close path command here
}

void
KarbonView::combinePath()
{
	debugView("KarbonView::combinePath()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	QList<KoShape*> selectedShapes = selection->selectedShapes();
	QList<KoPathShape*> paths;

	foreach( KoShape* shape, selectedShapes )
	{
		KoPathShape *path = dynamic_cast<KoPathShape*>( shape );
		if( path )
		{
			paths << path;
			selection->deselect( shape );
		}
	}

	if( paths.size() )
		m_canvas->addCommand( new KoPathCombineCommand( part(), paths ) );
}

void
KarbonView::separatePath()
{
	debugView("KarbonView::separatePath()");
	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	QList<KoShape*> selectedShapes = selection->selectedShapes();
	QList<KoPathShape*> paths;

	foreach( KoShape* shape, selectedShapes )
	{
		KoPathShape *path = dynamic_cast<KoPathShape*>( shape );
		if( path )
		{
			paths << path;
			selection->deselect( shape );
		}
	}

	if( paths.size() )
		m_canvas->addCommand( new KoPathSeparateCommand( part(), paths ) );
}

void
KarbonView::viewModeChanged()
{
	debugView("KarbonView::viewModeChanged()");

	// TODO: port: canvasWidget()->pixmap()->fill();

	/* TODO: port: if( m_viewAction->currentItem() == 1 )
		m_painterFactory->setWireframePainter( canvasWidget()->pixmap(), width(), height() );
	else
		m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );*/

	m_canvas->updateCanvas(m_canvas->canvasWidget()->rect());
}

void
KarbonView::setZoomAt( double zoom, const QPointF &p )
{
	debugView(QString("KarbonView::setZoomAt(%1, QPointF(%2, %3)").arg(zoom).arg(p.x()).arg(p.y()));

	QString zoomText = QString( "%1%" ).arg( zoom * 100.0, 0, 'f', 2 );
	QStringList stl = m_zoomAction->items();
	if( stl.first() == "25%" )
	{
		stl.prepend( zoomText.toLatin1() );
		m_zoomAction->setItems( stl );
		m_zoomAction->setCurrentItem( 0 );
	}
	else
	{
		m_zoomAction->setCurrentItem( 0 );
		m_zoomAction->changeItem( m_zoomAction->currentItem(), zoomText.toLatin1() );
	}
	zoomChanged( p );
}

void
KarbonView::viewZoomIn()
{
	debugView("KarbonView::viewZoomIn()");

	setZoomAt( zoom() * 1.50 );
}

void
KarbonView::viewZoomOut()
{
	debugView("KarbonView::viewZoomOut()");

	setZoomAt( zoom() * 0.75 );
}

void
KarbonView::zoomChanged( KoZoomMode::Mode mode, int zoom )
{
	debugView(QString("KarbonView::zoomChanged( mode = %1, zoom = %2) )").arg(mode).arg(zoom));

	KoZoomHandler *zoomHandler = (KoZoomHandler*)m_canvas->viewConverter();

	if( mode == KoZoomMode::ZOOM_CONSTANT )
	{
		double zoomF = zoom / 100.0;
		if( zoomF == 0.0 ) return;
		KoView::setZoom( zoomF );
		zoomHandler->setZoom( zoomF );
	}

	zoomHandler->setZoomMode( mode );
	m_canvas->adjustSize();
	if( mode == KoZoomMode::ZOOM_PAGE || mode == KoZoomMode::ZOOM_WIDTH )
		QTimer::singleShot(500, this, SLOT(centerCanvas()));
}

void
KarbonView::centerCanvas()
{
	m_canvasView->ensureVisible( QRectF(0,0,part()->document().width(),part()->document().height() ) );
}

void
KarbonView::zoomChanged( const QPointF &p )
{
	debugView(QString("KarbonView::zoomChanged( QPointF(%1, %2) )").arg(p.x()).arg(p.y()));

	if( m_canvas )
	{
		double zoomFactor = m_zoomAction->currentText().remove( '%' ).toDouble() / 100.0;
		if( zoomFactor == 0.0 ) return;

		KoZoomHandler *zoomHandler = (KoZoomHandler*)m_canvas->viewConverter();
		zoomHandler->setZoom( zoomFactor );
		KoView::setZoom( zoomFactor );
		m_canvas->adjustSize();
	}
	else
		KoView::setZoom( 1.0 );

// TODO: this should be done by the cavasview and the canvas itself

	/*double centerX;
	double centerY;
	double zoomFactor;

	if( !p.isNull() )
	{
		centerX = ( ( p.x() ) * zoom() + m_canvasView->canvasOffsetX() ) / double( m_canvas->canvasWidget()->width() );
		centerY = 1 - ( ( p.y() ) * zoom() + m_canvasView->canvasOffsetY() ) / double( m_canvas->canvasWidget()->height() );
		zoomFactor = m_zoomAction->currentText().remove( '%' ).toDouble() / 100.0;
	}
	else if( m_zoomAction->currentText() == i18n("Zoom Width") )
	{
		centerX = 0.5;
		centerY = double( m_canvas->canvasWidget()->y() + 0.5 * m_canvas->visibleHeight() ) / double( m_canvas->contentsHeight() );
		zoomFactor = double( m_canvas->canvasWidget()->x() ) / double( part()->document().width() );
	}
	else if( m_zoomAction->currentText() == i18n("Whole Page") )
	{
		centerX = 0.5;
		centerY = 0.5;
		double zoomFactorX = double( m_canvas->visibleWidth() ) / double( part()->document().width() );
		double zoomFactorY = double( m_canvas->visibleHeight() ) / double( part()->document().height() );

		if(zoomFactorX < 0 && zoomFactorY > 0)
			zoomFactor = zoomFactorY;
		else if(zoomFactorX > 0 && zoomFactorY < 0)
			zoomFactor = zoomFactorX;
		else if(zoomFactorX < 0 && zoomFactorY < 0)
			zoomFactor = 0.0001;
		else
			zoomFactor = qMin( zoomFactorX, zoomFactorY );
	}
	else
	{
		if( m_canvas->contentsWidth() > m_canvas->visibleWidth() )
			centerX = double( m_canvas->contentsX() + 0.5 * m_canvas->visibleWidth() ) / double( m_canvas->contentsWidth() );
		else
			centerX = 0.5;
		if( m_canvas->contentsHeight() > m_canvas->visibleHeight() )
			centerY = double( m_canvas->contentsY() + 0.5 * m_canvas->visibleHeight() ) / double( m_canvas->contentsHeight() );
		else
			centerY = 0.5;
		zoomFactor = m_zoomAction->currentText().remove( '%' ).toDouble() / 100.0;
	}
	kDebug(38000) << "centerX : " << centerX << endl;
	kDebug(38000) << "centerY : " << centerY << endl;
	kDebug(38000) << "zoomFactor : " << zoomFactor << endl;
	if( zoomFactor == 0.0 ) return;

	// above 2000% probably doesn't make sense... (Rob)
	if( zoomFactor > 20 )
	{
		zoomFactor = 20;
		m_zoomAction->changeItem( m_zoomAction->currentItem(), " 2000%" );
	}

	KoZoomHandler *zoomHandler = (KoZoomHandler*)m_canvas->viewConverter();
	zoomHandler->setZoom( zoomFactor );
	KoView::setZoom( zoomFactor );

	m_canvas->viewport()->setUpdatesEnabled( false );

	m_canvas->resizeContents( int( ( part()->pageLayout().ptWidth + 300 ) * zoomFactor ),
							  int( ( part()->pageLayout().ptHeight + 460 ) * zoomFactor ) );


	VPainter *painter = painterFactory()->editpainter();
	painter->setZoomFactor( zoomFactor );

	m_canvas->setViewport( centerX, centerY );
	m_canvas->repaintAll();
	m_canvas->viewport()->setUpdatesEnabled( true );


	if( shell() && m_showRulerAction->isChecked() )
	{
		m_horizRuler->setZoom( zoomFactor );
		m_vertRuler->setZoom( zoomFactor );
		m_canvas->setGeometry( rulerWidth, rulerHeight, width() - rulerWidth, height() - rulerHeight );
		updateRuler();
	}
	else
	{
		m_horizRuler->hide();
		m_vertRuler->hide();
	}
	m_canvas->viewport()->setFocus();

	emit zoomChanged( zoomFactor );*/
}

void
KarbonView::setLineStyle( int style )
{
	debugView(QString("KarbonView::setLineStyle(%1)").arg(style));

	Q3ValueList<float> dashes;
	if( style == Qt::NoPen )
		part()->addCommand( new VStrokeCmd( &part()->document(), dashes << 0 << 20 ), true );
	else if( style == Qt::SolidLine )
		part()->addCommand( new VStrokeCmd( &part()->document(), dashes ), true );
	else if( style == Qt::DashLine )
		part()->addCommand( new VStrokeCmd( &part()->document(), dashes << 12 << 6 ), true );
	else if( style == Qt::DotLine )
		part()->addCommand( new VStrokeCmd( &part()->document(), dashes << 2 << 2 ), true );
	else if( style == Qt::DashDotLine )
		part()->addCommand( new VStrokeCmd( &part()->document(), dashes << 12 << 2 << 2 << 2 ), true );
	else if( style == Qt::DashDotDotLine )
		part()->addCommand( new VStrokeCmd( &part()->document(), dashes << 12 << 2 << 2 << 2 << 2 << 2 ), true );
}

void
KarbonView::slotStrokeChanged( const VStroke &c )
{
	debugView("KarbonView::slotStrokeChanged(VStroke)");

	part()->document().selection()->setStroke( c );
	selectionChanged();
}

void
KarbonView::slotFillChanged( const VFill &f )
{
	debugView("KarbonView::slotFillChanged(VFill)");

	part()->document().selection()->setFill( f );
	selectionChanged();
}

void
KarbonView::setLineWidth()
{
	debugView("KarbonView::setLineWidth()");

	setLineWidth( m_setLineWidth->value() );
	selectionChanged();
}

//necessary for dcop call !
void
KarbonView::setLineWidth( double val )
{
	debugView(QString("KarbonView::setLineWidth(%1)").arg(val));

	part()->addCommand( new VStrokeCmd( &part()->document(), val ), true );
}

void
KarbonView::initActions()
{
	debugView("KarbonView::initActions()");

	// view ----->
    m_viewAction  = new KSelectAction(i18n("View &Mode"), this);
    actionCollection()->addAction("view_mode", m_viewAction );
	connect(m_viewAction, SIGNAL(triggered()), this, SLOT(viewModeChanged()));

	m_zoomAction = new KoZoomAction( KoZoomMode::ZOOM_CONSTANT|KoZoomMode::ZOOM_PAGE|KoZoomMode::ZOOM_WIDTH,
	i18n("Zoom"), KIcon("14_zoom"), KShortcut(), actionCollection(), "view_zoom");
	connect(m_zoomAction, SIGNAL(zoomChanged(KoZoomMode::Mode, int)),
          this, SLOT(zoomChanged(KoZoomMode::Mode, int)));

    QToolBar *tbar = new QToolBar( statusBar() );
    statusBar()->insertWidget( 2, tbar);
    tbar->addAction(m_zoomAction);

    actionCollection()->addAction(KStandardAction::ZoomIn,  "view_zoom_in", this, SLOT( viewZoomIn() ));
    actionCollection()->addAction(KStandardAction::ZoomOut,  "view_zoom_out", this, SLOT( viewZoomOut() ));

    m_showPageMargins  = new KToggleAction(KIcon("view_margins"), i18n("Show Page Margins"), this);
    actionCollection()->addAction("view_show_margins", m_showPageMargins );
	connect( m_showPageMargins, SIGNAL(toggled(bool)), SLOT(togglePageMargins(bool)));
	m_showPageMargins->setCheckedState(KGuiItem(i18n("Hide Page Margins")));

	// No need for the other actions in read-only (embedded) mode
	if( !shell() )
		return;

	// edit ----->
    actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", this, SLOT(editCut()));
    actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", this, SLOT(editCopy()));
    actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", this, SLOT(editPaste()));
    actionCollection()->addAction(KStandardAction::SelectAll,  "edit_select_all", this, SLOT(editSelectAll()));
    actionCollection()->addAction(KStandardAction::Deselect,  "edit_deselect_all", this, SLOT(editDeselectAll()));

    KAction *actionImportGraphic  = new KAction(i18n("&Import Graphic..."), this);
    actionCollection()->addAction("file_import", actionImportGraphic );
	connect(actionImportGraphic, SIGNAL(triggered()), this, SLOT(fileImportGraphic()));

    m_deleteSelectionAction  = new KAction(KIcon("editdelete"), i18n("D&elete"), this);
    actionCollection()->addAction("edit_delete", m_deleteSelectionAction );
	m_deleteSelectionAction->setShortcut(QKeySequence("Del"));
	connect(m_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));

    KAction *actionPurgeHistory  = new KAction(i18n("&History"), this);
    actionCollection()->addAction("edit_purge_history", actionPurgeHistory );
	connect(actionPurgeHistory, SIGNAL(triggered()), this, SLOT(editPurgeHistory()));
	// edit <-----

	// object ----->
    KAction *actionDuplicate  = new KAction(KIcon("duplicate"), i18n("&Duplicate"), this);
    actionCollection()->addAction("object_duplicate", actionDuplicate );
	actionDuplicate->setShortcut(QKeySequence("Ctrl+D"));
	connect(actionDuplicate, SIGNAL(triggered()), this, SLOT(selectionDuplicate()));

    KAction *actionBringToFront  = new KAction(KIcon("bring_forward"), i18n("Bring to &Front"), this);
    actionCollection()->addAction("object_move_totop", actionBringToFront );
	actionBringToFront->setShortcut(QKeySequence("Ctrl+Shift+]"));
	connect(actionBringToFront, SIGNAL(triggered()), this, SLOT(selectionBringToFront()));

    KAction *actionRaise  = new KAction(KIcon("raise"), i18n("&Raise"), this);
    actionCollection()->addAction("object_move_up", actionRaise );
	actionRaise->setShortcut(QKeySequence("Ctrl+]"));
	connect(actionRaise, SIGNAL(triggered()), this, SLOT(selectionMoveUp()));

    KAction *actionLower  = new KAction(KIcon("lower"), i18n("&Lower"), this);
    actionCollection()->addAction("object_move_down", actionLower );
	actionLower->setShortcut(QKeySequence("Ctrl+["));
	connect(actionLower, SIGNAL(triggered()), this, SLOT(selectionMoveDown()));

    KAction *actionSendToBack  = new KAction(KIcon("send_backward"), i18n("Send to &Back"), this);
    actionCollection()->addAction("object_move_tobottom", actionSendToBack );
	actionSendToBack->setShortcut(QKeySequence("Ctrl+Shift+["));
	connect(actionSendToBack, SIGNAL(triggered()), this, SLOT(selectionSendToBack()));

    KAction *actionAlignLeft  = new KAction(KIcon("aoleft"), i18n("Align Left"), this);
    actionCollection()->addAction("object_align_horizontal_left", actionAlignLeft );
	connect(actionAlignLeft, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalLeft()));

    KAction *actionAlignCenter  = new KAction(KIcon("aocenterh"), i18n("Align Center (Horizontal)"), this);
    actionCollection()->addAction("object_align_horizontal_center", actionAlignCenter );
	connect(actionAlignCenter, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalCenter()));

    KAction *actionAlignRight  = new KAction(KIcon("aoright"), i18n("Align Right"), this);
    actionCollection()->addAction("object_align_horizontal_right", actionAlignRight );
	connect(actionAlignRight, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalRight()));

    KAction *actionAlignTop  = new KAction(KIcon("aotop"), i18n("Align Top"), this);
    actionCollection()->addAction("object_align_vertical_top", actionAlignTop );
	connect(actionAlignTop, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalTop()));

    KAction *actionAlignMiddle  = new KAction(KIcon("aocenterv"), i18n("Align Middle (Vertical)"), this);
    actionCollection()->addAction("object_align_vertical_center", actionAlignMiddle );
	connect(actionAlignMiddle, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalCenter()));

    KAction *actionAlignBottom  = new KAction(KIcon("aobottom"), i18n("Align Bottom"), this);
    actionCollection()->addAction("object_align_vertical_bottom", actionAlignBottom );
	connect(actionAlignBottom, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalBottom()));

    KAction *actionDistributeHorizontalCenter  = new KAction(i18n("Distribute Center (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_center", actionDistributeHorizontalCenter );
	connect(actionDistributeHorizontalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalCenter()));

    KAction *actionDistributeHorizontalGap  = new KAction(i18n("Distribute Gaps (Horizontal)"), this);
    actionCollection()->addAction("object_distribute_horizontal_gap", actionDistributeHorizontalGap );
	connect(actionDistributeHorizontalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalGap()));

    KAction *actionDistributeLeft  = new KAction(i18n("Distribute Left Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_left", actionDistributeLeft );
	connect(actionDistributeLeft, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalLeft()));

    KAction *actionDistributeRight  = new KAction(i18n("Distribute Right Borders"), this);
    actionCollection()->addAction("object_distribute_horizontal_right", actionDistributeRight );
	connect(actionDistributeRight, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalRight()));

    KAction *actionDistributeVerticalCenter  = new KAction(i18n("Distribute Center (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_center", actionDistributeVerticalCenter );
	connect(actionDistributeVerticalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalCenter()));

    KAction *actionDistributeVerticalGap  = new KAction(i18n("Distribute Gaps (Vertical)"), this);
    actionCollection()->addAction("object_distribute_vertical_gap", actionDistributeVerticalGap );
	connect(actionDistributeVerticalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalGap()));

    KAction *actionDistributeBottom  = new KAction(i18n("Distribute Bottom Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_bottom", actionDistributeBottom );
	connect(actionDistributeBottom, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalBottom()));

    KAction *actionDistributeTop  = new KAction(i18n("Distribute Top Borders"), this);
    actionCollection()->addAction("object_distribute_vertical_top", actionDistributeTop );
	connect(actionDistributeTop, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalTop()));

    m_showRulerAction  = new KToggleAction(i18n("Show Rulers"), this);
    actionCollection()->addAction("view_show_ruler", m_showRulerAction );
	m_showRulerAction->setCheckedState(KGuiItem(i18n("Hide Rulers")));
	m_showRulerAction->setToolTip(i18n("Shows or hides rulers"));
	m_showRulerAction->setChecked(false);
	connect( m_showRulerAction, SIGNAL(triggered()), this, SLOT(showRuler()));

    m_showGridAction  = new KToggleAction(KIcon("grid"), i18n("Show Grid"), this);
    actionCollection()->addAction("view_show_grid", m_showGridAction );
	m_showGridAction->setCheckedState(KGuiItem(i18n("Hide Grid")));
	m_showGridAction->setToolTip(i18n("Shows or hides grid"));
	//m_showGridAction->setChecked(true);
	connect(m_showGridAction, SIGNAL(triggered()), this, SLOT(showGrid()));

    m_snapGridAction  = new KToggleAction(i18n("Snap to Grid"), this);
    actionCollection()->addAction("view_snap_to_grid", m_snapGridAction );
	m_snapGridAction->setToolTip(i18n( "Snaps to grid"));
	//m_snapGridAction->setChecked(true);
	connect(m_snapGridAction, SIGNAL(triggered()), this, SLOT(snapToGrid()));

    m_groupObjects  = new KAction(KIcon("group"), i18n("&Group Objects"), this);
    actionCollection()->addAction("selection_group", m_groupObjects );
	m_groupObjects->setShortcut(QKeySequence("Ctrl+G"));
	connect(m_groupObjects, SIGNAL(triggered()), this, SLOT(groupSelection()));

    m_ungroupObjects  = new KAction(KIcon("ungroup"), i18n("&Ungroup Objects"), this);
    actionCollection()->addAction("selection_ungroup", m_ungroupObjects );
	m_ungroupObjects->setShortcut(QKeySequence("Ctrl+Shift+G"));
	connect(m_ungroupObjects, SIGNAL(triggered()), this, SLOT(ungroupSelection()));
	// object <-----

	// path ------->
    m_closePath  = new KAction(i18n("&Close Path"), this);
    actionCollection()->addAction("close_path", m_closePath );
	m_closePath->setShortcut(QKeySequence("Ctrl+U"));
	m_closePath->setEnabled( false );
	connect(m_closePath, SIGNAL(triggered()), this, SLOT(closePath()));

    m_combinePath  = new KAction(i18n("Com&bine Path"), this);
    actionCollection()->addAction("combine_path", m_combinePath );
	m_combinePath->setShortcut(QKeySequence("Ctrl+K"));
	m_combinePath->setEnabled( false );
	connect(m_combinePath, SIGNAL(triggered()), this, SLOT(combinePath()));

    m_separatePath  = new KAction(i18n("Se&parate Path"), this);
    actionCollection()->addAction("separate_path", m_separatePath );
	m_separatePath->setShortcut(QKeySequence("Shift+Ctrl+K"));
	m_separatePath->setEnabled( false );
	connect(m_separatePath, SIGNAL(triggered()), this, SLOT(separatePath()));
	// path <-----

	// line style (dashes)
	// TODO: KoLineStyleAction isn't ported yet.
	//m_lineStyleAction = new KoLineStyleAction(i18n("Line Style"), "linestyle", this, SLOT(setLineStyle(int)), actionCollection(), "setLineStyle");

	// line width
	m_setLineWidth = new KoUnitDoubleSpinComboBox( this, 0.0, 1000.0, 0.5, 1.0, KoUnit(KoUnit::Point), 1 );
	/* TODO: port
new KWidgetAction( m_setLineWidth, i18n( "Set Line Width" ), 0, this, SLOT( setLineWidth() ), actionCollection(), "setLineWidth" );
*/
	m_setLineWidth->insertItem( 0.25 );
	m_setLineWidth->insertItem( 0.5 );
	m_setLineWidth->insertItem( 0.75 );
	m_setLineWidth->insertItem( 1.0 );
	m_setLineWidth->insertItem( 2.0 );
	m_setLineWidth->insertItem( 3.0 );
	m_setLineWidth->insertItem( 4.0 );
	m_setLineWidth->insertItem( 5.0 );
	m_setLineWidth->insertItem( 10.0 );
	m_setLineWidth->insertItem( 20.0 );
	connect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( setLineWidth() ) );

    m_configureAction  = new KAction(KIcon("configure"), i18n("Configure Karbon..."), this);
    actionCollection()->addAction("configure", m_configureAction );
	connect(m_configureAction, SIGNAL(triggered()), this, SLOT(configure()));

    KAction *actionPageLayout  = new KAction(i18n("Page &Layout..."), this);
    actionCollection()->addAction("page_layout", actionPageLayout );
	connect(actionPageLayout, SIGNAL(triggered()), this, SLOT(pageLayout()));

	m_contextHelpAction = new KoContextHelpAction( actionCollection(), this );
}

void
KarbonView::paintEverything( QPainter& /*p*/, const QRect& /*rect*/, bool /*transparent*/ )
{
	debugView("KarbonView::paintEverything(...)");
}

void
KarbonView::mousePositionChanged( const QPoint &position )
{
    QPoint viewPos = position - m_canvas->documentOrigin();
    m_horizRuler->updateMouseCoordinate( viewPos.x() );
    m_vertRuler->updateMouseCoordinate( viewPos.y() );

    QPointF documentPos = m_canvas->viewConverter()->viewToDocument( viewPos );
    double x = KoUnit::toUserValue(documentPos.x(), part()->unit());
    double y = KoUnit::toUserValue(documentPos.y(), part()->unit());

    m_cursorCoords->setText( QString( "%1, %2" ).arg(KGlobal::locale()->formatNumber(x, 2)).arg(KGlobal::locale()->formatNumber(y, 2)) );
}

void
KarbonView::reorganizeGUI()
{
	debugView("KarbonView::reorganizeGUI()");

	if( statusBar() )
	{
		if( part()->showStatusBar() )
			statusBar()->show();
		else
			statusBar()->hide();
	}
}

void
KarbonView::setNumberOfRecentFiles( unsigned int number )
{
	debugView(QString("KarbonView::setNumberOfRecentFiles(%1)").arg(number));

	if( shell() )	// 0L when embedded into konq !
		shell()->setMaxRecentItems( number );
}

void
KarbonView::showRuler()
{
	debugView("KarbonView::showRuler()");

	if( shell() && m_showRulerAction->isChecked() )
	{
		m_horizRuler->show();
		m_vertRuler->show();
		updateRuler();
	}
	else
	{
		m_horizRuler->hide();
		m_vertRuler->hide();
	}

	zoomChanged();
}

bool
KarbonView::showPageMargins()
{
	debugView("KarbonView::showPageMargins()");

	return ((KToggleAction*)actionCollection()->action("view_show_margins"))->isChecked();
}

void
KarbonView::togglePageMargins(bool b)
{
    debugView(QString("KarbonView::togglePageMargins(%1)").arg(b));

    ((KToggleAction*)actionCollection()->action("view_show_margins"))->setChecked(b);
    m_canvas->setShowMargins( b );
    m_canvas->update();
}

void
KarbonView::pageOffsetChanged()
{
    m_horizRuler->setOffset( m_canvasView->canvasOffsetX() + m_canvas->documentOrigin().x() );
    m_vertRuler->setOffset( m_canvasView->canvasOffsetY() + m_canvas->documentOrigin().y() );
}

void
KarbonView::updateRuler()
{
    m_horizRuler->setRulerLength( part()->document().width() );
    m_vertRuler->setRulerLength( part()->document().height() );
}

void
KarbonView::showGrid()
{
	debugView("KarbonView::showGrid()");

	m_part->gridData().setShowGrid( m_showGridAction->isChecked() );
	m_canvas->update();
}

void
KarbonView::snapToGrid()
{
	debugView("KarbonView::snapToGrid()");

	m_part->gridData().setSnapToGrid( m_snapGridAction->isChecked() );
	m_canvas->update();
}

void
KarbonView::showSelectionPopupMenu( const QPoint &pos )
{
	debugView(QString("KarbonView::showSelectionPopupMenu(QPoint(%1, %2))").arg(pos.x()).arg(pos.y()));

	QList<QAction*> actionList;
	if( m_groupObjects->isEnabled() )
		actionList.append( m_groupObjects );
	else if( m_ungroupObjects->isEnabled() )
		actionList.append( m_ungroupObjects );
	if( m_closePath->isEnabled() )
		actionList.append( m_closePath );
	if( m_combinePath->isEnabled() )
		actionList.append( m_combinePath );
	plugActionList( "selection_type_action", actionList );
	((Q3PopupMenu *)factory()->container( "selection_popup", this ) )->exec( pos );
	unplugActionList( "selection_type_action" );
}

void
KarbonView::configure()
{
	debugView("KarbonView::configure()");

	VConfigureDlg dialog( this );
	dialog.exec();
}

void
KarbonView::pageLayout()
{
	debugView("KarbonView::pageLayout()");

	KoHeadFoot hf;
	KoPageLayout layout = part()->pageLayout();
	KoUnit unit = part()->unit();
	if( KoPageLayoutDia::pageLayout( layout, hf, FORMAT_AND_BORDERS | DISABLE_UNIT, unit ) )
	{
		part()->setPageLayout( layout, unit );
		m_horizRuler->setUnit( unit );
		m_vertRuler->setUnit( unit );
		m_canvas->canvasWidget()->resize( int( ( part()->pageLayout().ptWidth + 300 ) * zoom() ),
								  int( ( part()->pageLayout().ptHeight + 460 ) * zoom() ) );
        m_canvas->adjustSize();
		part()->repaintAllViews();

		emit pageLayoutChanged();
	}
}

void
KarbonView::selectionChanged()
{
	debugView("KarbonView::selectionChanged()");

	KoSelection *selection = m_canvas->shapeManager()->selection();
	int count = selection->selectedShapes( KoFlake::TopLevelSelection ).count();

	m_groupObjects->setEnabled( count > 1 );
	m_ungroupObjects->setEnabled( false );
	m_closePath->setEnabled( false );
	m_combinePath->setEnabled( false );
	m_deleteSelectionAction->setEnabled( count > 0 );

	kDebug(38000) << count << " shapes selected" << endl;

	if( count > 0 )
	{
		KoShape *shape = *selection->selectedShapes().begin();
		if( shape )
			m_strokeDocker->setStroke( shape->border() );
		/** TODO needs porting to flake
		VObject *obj = part()->document().selection()->objects().getFirst();

		if ( shell() ) {
			//if ( this == shell()->rootView() || koDocument()->isEmbedded() ) {
				m_strokeFillPreview->update( *obj->stroke(), *obj->fill() );
				m_smallPreview->update( *obj->stroke(), *obj->fill() );
			//}
		}
		m_strokeDocker->setStroke( *( obj->stroke() ) );

		if( count == 1 )
		{
			VPath *path = dynamic_cast<VPath *>( part()->document().selection()->objects().getFirst() );
			m_closePath->setEnabled( path && !path->isClosed() );
		}

		part()->document().selection()->setStroke( *obj->stroke() );
		part()->document().selection()->setFill( *obj->fill() );
		m_setLineWidth->setEnabled( true );
		m_setLineWidth->updateValue( obj->stroke()->lineWidth() );
		// dashes
  		m_lineStyleAction->setEnabled( true );
		if( obj->stroke()->dashPattern().array().isEmpty() )
  			m_lineStyleAction->setCurrentSelection( Qt::SolidLine );
		else if( obj->stroke()->dashPattern().array()[ 0 ] == 0. )
  			m_lineStyleAction->setCurrentSelection( Qt::NoPen );
		else if( obj->stroke()->dashPattern().array()[ 0 ]  == 2. )
  			m_lineStyleAction->setCurrentSelection( Qt::DotLine );
		else if( obj->stroke()->dashPattern().array().count() == 2 )
  			m_lineStyleAction->setCurrentSelection( Qt::DashLine );
		else if( obj->stroke()->dashPattern().array().count() == 4 )
  			m_lineStyleAction->setCurrentSelection( Qt::DashDotLine );
		else if( obj->stroke()->dashPattern().array().count() == 6 )
  			m_lineStyleAction->setCurrentSelection( Qt::DashDotDotLine );
		*/

		uint selectedPaths = 0;
		uint selectedGroups = 0;
		// check for different shape types for enabling specific actions
		foreach( KoShape* shape, selection->selectedShapes() )
		{
			if( dynamic_cast<KoShapeGroup*>( shape->parent() ) )
				selectedGroups++;
			if( dynamic_cast<KoPathShape*>( shape ) )
				selectedPaths++;
		}
		m_ungroupObjects->setEnabled( selectedGroups > 0 );
		//TODO enable action when the ClosePath command is ported
		//m_closePath->setEnabled( selectedPaths > 0 );
		m_combinePath->setEnabled( selectedPaths > 1 );
		m_separatePath->setEnabled( selectedPaths > 0 );
	}
	else
	{
		if ( shell() )
			//if ( this == shell()->rootView() || koDocument()->isEmbedded() && m_strokeFillPreview )
			m_strokeFillPreview->update( *( part()->document().selection()->stroke() ),
									 *( part()->document().selection()->fill() ) );
		// TODO: activate the line below when KoLineStyleAction is ported.
		// m_lineStyleAction->setEnabled( false );
	}
	emit selectionChange();
}
void
KarbonView::setCursor( const QCursor &c )
{
	debugView("KarbonView::setCursor(QCursor)");

	m_canvas->setCursor( c );
}

void
KarbonView::repaintAll( const QRectF &r )
{
	debugView(QString("KarbonView::repaintAll(QRectF(%1, %2, %3, %4))").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));

	m_canvas->updateCanvas( r );
}

void
KarbonView::repaintAll( bool repaint )
{
	debugView(QString("KarbonView::repaintAll(%1)").arg(repaint));

	m_canvas->updateCanvas(m_canvas->canvasWidget()->rect());
}
void
KarbonView::setPos( const QPointF& p )
{
	debugView(QString("KarbonView::setPos(QPointF(%1, %2))").arg(p.x()).arg(p.y()));

	// TODO: port: m_canvas->setPos( p );
}

void
KarbonView::setViewportRect( const QRectF &rect )
{
	debugView(QString("KarbonView::setViewportRect(QRectF(%1, %2, %3, %4))").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()));

	// TODO: port: m_canvas->setViewportRect( rect );
}

void
KarbonView::setUnit( KoUnit /*_unit*/ )
{
	debugView("KarbonView::setUnit(KoUnit)");
}

void KarbonView::createDocumentTabDock()
{
	debugView("KarbonView::createDocumentTabDock()");

	m_DocumentTab = new VDocumentTab(this, this);
	m_DocumentTab->setWindowTitle(i18n("Document"));
    createDock(i18n("Document"), m_DocumentTab);
	connect( m_part, SIGNAL( unitChanged( KoUnit ) ), m_DocumentTab, SLOT( updateDocumentInfo() ) );
}

void KarbonView::createLayersTabDock()
{
	debugView("KarbonView::createLayersTabDock()");

	VLayerDockerFactory layerFactory( m_part, &part()->document() );
	m_layerDocker = qobject_cast<VLayerDocker*>(createDockWidget(&layerFactory));
	connect( this, SIGNAL( selectionChange() ), m_layerDocker, SLOT( updateView() ) );
}

void KarbonView::createStrokeDock()
{
	debugView("KarbonView::createStrokeDock()");

	VStrokeDockerFactory strokeFactory;
	m_strokeDocker = qobject_cast<VStrokeDocker*>(createDockWidget(&strokeFactory));
	connect( part(), SIGNAL( unitChanged( KoUnit ) ), m_strokeDocker, SLOT( setUnit( KoUnit ) ) );
}

void KarbonView::createColorDock()
{
	debugView("KarbonView::createColorDock()");

	VColorDockerFactory colorFactory;
	m_ColorManager = qobject_cast<VColorDocker*>(createDockWidget(&colorFactory));

	connect( this, SIGNAL( selectionChange() ), m_ColorManager, SLOT( update() ) );
}

void KarbonView::createTransformDock()
{
	debugView("KarbonView::createTransformDock()");

	VTransformDockerFactory transformFactory;
	m_TransformDocker = qobject_cast<VTransformDocker*>(createDockWidget(&transformFactory));
	connect( this, SIGNAL( selectionChange() ), m_TransformDocker, SLOT( update() ) );
	connect( part(), SIGNAL( unitChanged( KoUnit ) ), m_TransformDocker, SLOT( setUnit( KoUnit ) ) );
}

void KarbonView::createResourceDock()
{
	debugView("KarbonView::createResourceDock()");

	m_styleDocker = new VStyleDocker( part(), this );
	m_styleDocker->setWindowTitle(i18n("Resources"));
    createDock(i18n("Resources"), m_styleDocker);
}

VToolController *
KarbonView::toolController()
{
	debugView("KarbonView::toolController()");

	return m_toolController;
}

#include "karbon_view.moc"

