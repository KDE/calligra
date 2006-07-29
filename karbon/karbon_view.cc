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

#include "karbon_view.h"

#include <q3dragobject.h>
#include <qicon.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <q3popupmenu.h>
#include <q3paintdevicemetrics.h>
#include <qpainter.h>
#include <QResizeEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <Q3ValueList>
#include <QEvent>
#include <QDropEvent>
#include <Q3PtrList>
#include <QGridLayout>

#include <kaction.h>
#include <kcolormimedata.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdeversion.h>
#include <kprinter.h>
#include <kinstance.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kcommand.h>

#include <KoMainWindow.h>
#include <KoFilterManager.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kstdaction.h>
#include <KoContextHelp.h>
#include <KoUnitWidgets.h>
#include <KoPageLayoutDia.h>
#include <vruler.h>
#include <Kolinestyleaction.h>
#include <KoToolManager.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>
#include <KoShapeContainer.h>
#include <KoShapeGroup.h>
#include <KoCommand.h>
#include <KoSelection.h>

// Commands.
#include "vcleanupcmd.h"
#include "vclipartcmd.h"
#include "vclosepathcmd.h"
#include "vfillcmd.h"
#include "vstrokecmd.h"
#include "vtransformcmd.h"
#include "vinsertcmd.h"
#include "vzordercmd.h"
#include "vlayer.h"

// Dialogs.
#include "vconfiguredlg.h"

// Dockers.
#include "kopalettemanager.h"
#include "vcolordocker.h"
#include "vdocumentdocker.h"
#include "vstrokedocker.h"
#include "vstyledocker.h"
#include "vtransformdocker.h"

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
#include "vtoolbox.h"
#include "karbon_drag.h"
// #include "vselectnodestool.h"

#include <unistd.h>

// Only for debugging.
#include <kdebug.h>

// Uncomment the #define below to print lots of debug information about the view.
// Or use the -DKARBON_DEBUG_VIEW flag when using cmake, so the code stays the same.
// #define KARBON_DEBUG_VIEW

#ifdef KARBON_DEBUG_VIEW
#define debugView(text) kDebug() << "KARBON_DEBUG_VIEW: " << text << endl
#else
#define debugView(text)
#endif

const int rulerWidth = 20;  // vertical ruler width
const int rulerHeight = 20; // horizontal ruler height

KarbonView::KarbonView( KarbonPart* p, QWidget* parent, const char* name )
		: KoView( p, parent, name ), KXMLGUIBuilder( shell() ), m_part( p )
{
	debugView("KarbonView::KarbonView");

	m_toolbox = 0L;
	m_toolController = new VToolController( this );
	m_toolController->init();

	setInstance( KarbonFactory::instance(), true );

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
	m_smallPreview = new VSmallPreview( this, name );
	addStatusBarItem( m_smallPreview );

	initActions();

	m_DocumentTab = 0L;
	m_LayersTab = 0L;
	m_HistoryTab = 0L;
	m_strokeFillPreview = 0L;
	m_ColorManager = 0L;
	m_strokeDocker = 0L;
	m_styleDocker = 0L;
	m_TransformDocker = 0L;

	// set selectTool by default
	//m_toolbox->slotPressButton( 0 );

	m_pPaletteManager = new KoPaletteManager(this, actionCollection(), "karbon palette manager");

	unsigned int max = part()->maxRecentFiles();
	setNumberOfRecentFiles( max );

        connect( p, SIGNAL( unitChanged( KoUnit::Unit ) ), this, SLOT( setUnit( KoUnit::Unit ) ) );

	// layout:
	QGridLayout *layout = new QGridLayout();
	layout->setMargin(0);

	// widgets:
	m_horizRuler = new VRuler( Qt::Horizontal, this );
	m_horizRuler->setUnit(p->unit());
	connect( p, SIGNAL( unitChanged( KoUnit::Unit ) ), m_horizRuler, SLOT( setUnit( KoUnit::Unit ) ) );

	m_vertRuler = new VRuler( Qt::Vertical, this );
	m_vertRuler->setUnit(p->unit());
	connect( p, SIGNAL( unitChanged( KoUnit::Unit ) ), m_vertRuler, SLOT( setUnit( KoUnit::Unit ) ) );

	m_canvas = new KarbonCanvas( p->document().shapes() ); //, this, p );
	m_canvas->setCommandHistory( p->commandHistory() );
	connect( m_canvas->shapeManager()->selection(), SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );
	//connect( m_canvas, SIGNAL( contentsMoving( int, int ) ), this, SLOT( canvasContentsMoving( int, int ) ) );

	m_canvasView = new KoCanvasController(this);
	m_canvasView->setCanvas(m_canvas);
	m_canvasView->centerCanvas( true );

	layout->addWidget(m_canvasView, 0, 0);

	m_canvasView->show();

	// set up factory
	m_painterFactory = new VPainterFactory;
	//m_painterFactory->setPainter( m_canvas->pixmap(), m_canvas->contentsWidth(), m_canvas->contentsHeight() );
	m_painterFactory->setEditPainter( m_canvas->canvasWidget(), m_canvas->canvasWidget()->width(), m_canvas->canvasWidget()->height() );

	if( shell() )
	{
		//Create Dockers
		createColorDock();
		createStrokeDock();
		createTransformDock();
		createDocumentTabDock();
		createLayersTabDock();
		createHistoryTabDock();
		createResourceDock();

		// TODO: proper use of the toolbox once it is ready
		// plug the toolbox as a docker for now to have something
		KoToolManager::instance()->addControllers( m_canvasView, p );
		QWidget *tb = KoToolManager::instance()->toolBox();
		tb->setWindowTitle( "Toolbox" );
		paletteManager()->addWidget( tb, "ToolBox", "ToolBox" );

		// for testing: manually set a shape id of the shape to be created
		KoCreateShapesTool *createTool = KoToolManager::instance()->shapeCreatorTool( m_canvas );
		if( createTool )
		{
			QList<KoID> shapeIdList = KoShapeRegistry::instance()->listKeys();
			if( shapeIdList.count() > 0 )
				createTool->setShapeId( shapeIdList.first().id() );
			//createTool->setShapeId( "KoRegularPolygonShape" );
			//createTool->setShapeId( "43751" );
			//createTool->setShapeId( "TextShapeID" );
			createTool->setShapeController( p );
		}

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
	
		m_horizRuler->installEventFilter(m_canvas);
		m_vertRuler->installEventFilter(m_canvas);
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

	delete m_canvas;

	delete m_toolController;
}

static Qt::ToolBarArea stringToDock( const QString& attrPosition )
{
	debugView(QString("Qt::ToolBarArea stringToDock(%1)").arg(attrPosition));
	/* Port to KDE/Qt 4

	KToolBar::Dock dock = KToolBar::DockTop;
	if ( !attrPosition.isEmpty() ) {
		if ( attrPosition == "top" )
			dock = Qt::DockTop;
		else if ( attrPosition == "left" )
			dock = Qt::DockLeft;
		else if ( attrPosition == "right" )
			dock = Qt::DockRight;
		else if ( attrPosition == "bottom" )
			dock = Qt::DockBottom;
		else if ( attrPosition == "floating" )
			dock = Qt::DockTornOff;
		else if ( attrPosition == "flat" )
			dock = Qt::DockMinimized;
	}

	return dock;
	*/
	
	return Qt::TopToolBarArea;

}

QWidget *
KarbonView::createContainer( QWidget *parent, int index, const QDomElement &element, int &id )
{
	debugView(QString("KarbonView::createContainer(parent = QWidget, index = %1, element = %2, id = %3)").arg(index).arg(element.tagName()).arg(id));

	if( element.attribute( "name" ) == "Tools" )
	{
		m_toolbox = new VToolBox( mainWindow(), "Tools", KarbonFactory::instance() );
		toolController()->setUp( actionCollection(), m_toolbox );

		kDebug() << "Toolbox position: " << element.attribute( "position" ) << "\n";
	        Qt::ToolBarArea dock = stringToDock( element.attribute( "position" ).toLower() );

	        /* TODO: Port to KDE/Qt 4
		
		mainWindow()->addDockWindow( m_toolbox, dock, false);
	        mainWindow()->moveDockWindow( m_toolbox, dock, false, 0, 0 );
		*/


		//connect( m_toolbox, SIGNAL( activeToolChanged( VTool * ) ), this, SLOT( slotActiveToolChanged( VTool * ) ) );

		if( shell() )
		{
			m_strokeFillPreview = new VStrokeFillPreview( part(), m_toolbox );
			m_typeButtonBox = new VTypeButtonBox( part(), m_toolbox );

			connect( m_strokeFillPreview, SIGNAL( fillSelected() ), m_typeButtonBox, SLOT( setFill() ) );
			connect( m_strokeFillPreview, SIGNAL( strokeSelected() ), m_typeButtonBox, SLOT( setStroke() ) );

			connect( m_strokeFillPreview, SIGNAL( strokeChanged( const VStroke & ) ), this, SLOT( slotStrokeChanged( const VStroke & ) ) );
			connect( m_strokeFillPreview, SIGNAL( fillChanged( const VFill & ) ), this, SLOT( slotFillChanged( const VFill & ) ) );

			connect( m_strokeFillPreview, SIGNAL( strokeSelected() ), m_ColorManager, SLOT( setStrokeDocker() ) );
			connect( m_strokeFillPreview, SIGNAL( fillSelected( ) ), m_ColorManager, SLOT( setFillDocker() ) );
			selectionChanged();

			//create toolbars
// 			m_selectToolBar = new VSelectToolBar( this, "selecttoolbar" );
// 			mainWindow()->addToolBar( m_selectToolBar );
		}
	}

	return KXMLGUIBuilder::createContainer( parent, index, element, id );
}

void
KarbonView::removeContainer( QWidget *container, QWidget *parent,
							 QDomElement &element, int id )
{
	debugView(QString("KarbonView::removeContainer(container = QWidget, parent = QWidget, element = %1, id = %2)").arg(element.tagName()).arg(id));

	if( container )
		kDebug(38000) << container << endl;

	if( shell() && container == m_toolbox )
	{
		delete m_toolbox;
		m_toolbox = 0L;
		m_toolController->youAintGotNoToolBox();
//		delete m_strokeFillPreview;
		m_strokeFillPreview = 0;
//		delete m_typeButtonBox;
		m_typeButtonBox = 0;
// 		delete m_selectToolBar;
// 		m_selectToolBar = 0L;
	}
	else
		KXMLGUIBuilder::removeContainer( container, parent, element, id );
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
	
	Q3PaintDeviceMetrics metrics( ( QPaintDevice * ) & printer );
	printer.setFullPage( true );
	
	// we are using 72 dpi internally
	double zoom = metrics.logicalDpiX() / 72.0;

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

	part()->document().draw( &kop, &rect );

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
	double copyOffset = part()->instance()->config()->readEntry( "CopyOffset", 10 );
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

	part()->document().selection()->append();

	if( part()->document().selection()->objects().count() > 0 )
		part()->repaintAllViews();

	selectionChanged();
}

void
KarbonView::editDeselectAll()
{
	debugView("KarbonView::editDeselectAll()");

	if( part()->document().selection()->objects().count() > 0 )
	{
		part()->document().selection()->clear();
		part()->repaintAllViews();
	}

	selectionChanged();
}

void
KarbonView::editDeleteSelection()
{
	debugView("KarbonView::editDeleteSelection()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	KoSelectionSet selectedShapes = selection->selectedShapes();
	if( selectedShapes.count() < 1)
		return;

	KoShapeDeleteCommand *cmd = new KoShapeDeleteCommand( part(), selectedShapes );
	part()->commandHistory()->addCommand( cmd, true );
}

void
KarbonView::editPurgeHistory()
{
	debugView("KarbonView::editPurgeHistory()");

	// TODO: check for history size != 0

	if( KMessageBox::warningContinueCancel( this,
			i18n( "This action cannot be undone later. Do you really want to continue?" ),
			i18n( "Purge History" ),
			KStdGuiItem::del(),
			"edit_purge_history" ) )
	{
		// Use the VCleanUp command to remove "deleted"
		// objects from all layers.
		VCleanUpCmd cmd( &part()->document() );
		cmd.execute();

		part()->clearHistory();
	}
}

void
KarbonView::selectionAlignHorizontalLeft()
{
	debugView("KarbonView::selectionAlignHorizontalLeft()");

	selectionAlign(KoShapeAlignCommand::ALIGN_HORIZONTAL_LEFT);
}
void
KarbonView::selectionAlignHorizontalCenter()
{
	debugView("KarbonView::selectionAlignHorizontalCenter()");

	selectionAlign(KoShapeAlignCommand::ALIGN_HORIZONTAL_CENTER);
}

void
KarbonView::selectionAlignHorizontalRight()
{
	debugView("KarbonView::selectionAlignHorizontalRight()");

	selectionAlign(KoShapeAlignCommand::ALIGN_HORIZONTAL_RIGHT);
}

void
KarbonView::selectionAlignVerticalTop()
{
	debugView("KarbonView::selectionAlignVerticalTop()");

	selectionAlign(KoShapeAlignCommand::ALIGN_VERTICAL_TOP);
}

void
KarbonView::selectionAlignVerticalCenter()
{
	debugView("KarbonView::selectionAlignVerticalCenter()");

	selectionAlign(KoShapeAlignCommand::ALIGN_VERTICAL_CENTER);
}

void
KarbonView::selectionAlignVerticalBottom()
{
	debugView("KarbonView::selectionAlignVerticalBottom()");

	selectionAlign(KoShapeAlignCommand::ALIGN_VERTICAL_BOTTOM);
}

void
KarbonView::selectionAlign(KoShapeAlignCommand::Align align)
{
	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	KoSelectionSet selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	if( selectedShapes.count() < 2) return;  // TODO: handle case count = 1

	KoShapeAlignCommand *cmd = new KoShapeAlignCommand( selectedShapes, align, selection->boundingRect());

	part()->commandHistory()->addCommand( cmd, true );
}


void
KarbonView::selectionDistributeHorizontalCenter()
{
	debugView("KarbonView::selectionDistributeHorizontalCenter()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_HORIZONTAL_CENTER);
}

void
KarbonView::selectionDistributeHorizontalGap()
{
	debugView("KarbonView::selectionDistributeHorizontalGap()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_HORIZONTAL_GAP);
}

void
KarbonView::selectionDistributeHorizontalLeft()
{
	debugView("KarbonView::selectionDistributeHorizontalLeft()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_HORIZONTAL_LEFT);
}

void
KarbonView::selectionDistributeHorizontalRight()
{
	debugView("KarbonView::selectionDistributeHorizontalRight()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_HORIZONTAL_RIGHT);
}

void
KarbonView::selectionDistributeVerticalCenter()
{
	debugView("KarbonView::selectionDistributeVerticalCenter()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_VERTICAL_CENTER);
}

void
KarbonView::selectionDistributeVerticalGap()
{
	debugView("KarbonView::selectionDistributeVerticalGap()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_VERTICAL_GAP);
}

void
KarbonView::selectionDistributeVerticalBottom()
{
	debugView("KarbonView::selectionDistributeVerticalBottom()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_VERTICAL_BOTTOM);
}

void
KarbonView::selectionDistributeVerticalTop()
{
	debugView("KarbonView::selectionDistributeVerticalTop()");

	selectionDistribute(KoShapeDistributeCommand::DISTRIBUTE_VERTICAL_TOP);
}

void
KarbonView::selectionDistribute(KoShapeDistributeCommand::Distribute distribute)
{
	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	KoSelectionSet selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	if( selectedShapes.count() < 2) return;

	KoShapeDistributeCommand *cmd = new KoShapeDistributeCommand( selectedShapes, distribute, selection->boundingRect());

	part()->commandHistory()->addCommand( cmd, true );
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
	double copyOffset = part()->instance()->config()->readEntry( "CopyOffset", 10 );
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

	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::bringToFront ), true );
}

void
KarbonView::selectionMoveUp()
{
	debugView("KarbonView::selectionMoveUp()");

	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::up ), true );
}

void
KarbonView::selectionMoveDown()
{
	debugView("KarbonView::selectionMoveDown()");

	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::down ), true );
}

void
KarbonView::selectionSendToBack()
{
	debugView("KarbonView::selectionSendToBack()");

	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::sendToBack ), true );
}

void
KarbonView::groupSelection()
{
	debugView("KarbonView::groupSelection()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	KoShapeGroup *group = new KoShapeGroup();
	KoSelectionSet selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	
	QList<KoShape*> groupedShapes;

	// only group shapes with an unselected parent
	foreach( KoShape* shape, selectedShapes )
	{
		if( selectedShapes.contains( shape->parent() ) )
			continue;
		groupedShapes << shape;
	}
	
	KMacroCommand *cmd = new KMacroCommand( i18n("Group shapes") );
	cmd->addCommand( new KoShapeCreateCommand( m_part, group ) );
	cmd->addCommand( new KoGroupShapesCommand( group, groupedShapes ) );
		
	part()->commandHistory()->addCommand( cmd, true );
}

void
KarbonView::ungroupSelection()
{
	debugView("KarbonView::ungroupSelection()");

	KoSelection* selection = m_canvas->shapeManager()->selection();
	if( ! selection )
		return;

	KoSelectionSet selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
	KoSelectionSet containerSet;

	// only ungroup shape containers with an unselected parent
	foreach( KoShape* shape, selectedShapes )
	{
		if( selectedShapes.contains( shape->parent() ) )
			continue;
		containerSet << shape;
	}

	KMacroCommand *cmd = new KMacroCommand( i18n("Ungroup shapes") );

	// add a ungroup command for each found shape container to the macro command
	foreach( KoShape* shape, containerSet )
	{
		KoShapeContainer *container = dynamic_cast<KoShapeContainer*>( shape );
		if( container )
			cmd->addCommand( new KoUngroupShapesCommand( container, container->iterator() ) );
	}
	part()->commandHistory()->addCommand( cmd, true );
}

void
KarbonView::closePath()
{
	debugView("KarbonView::closePath()");

	part()->addCommand( new VClosePathCmd( &part()->document() ), true );
}

void
KarbonView::slotActiveToolChanged( VTool *tool )
{
	debugView(QString("KarbonView::slotActiveToolChanged(%1)").arg(tool->uiname()));

	toolController()->setCurrentTool( tool );

	m_canvas->updateCanvas(m_canvas->canvasWidget()->rect());
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
KarbonView::zoomChanged( const QPointF &p )
{
	debugView(QString("KarbonView::zoomChanged( QPointF(%1, %2) )").arg(p.x()).arg(p.y()));

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
	m_viewAction = new KSelectAction(i18n("View &Mode"), actionCollection(), "view_mode");
	connect(m_viewAction, SIGNAL(triggered()), this, SLOT(viewModeChanged()));

	m_zoomAction = new KSelectAction(KIcon("viewmag"), i18n("&Zoom"), actionCollection(), "view_zoom");
	connect(m_zoomAction, SIGNAL(triggered()), this, SLOT(zoomChanged()));

	QStringList mstl;
	mstl << i18n( "Normal" ) << i18n( "Wireframe" );
	m_viewAction->setItems( mstl );
	m_viewAction->setCurrentItem( 0 );
	m_viewAction->setEditable( false );

	QStringList stl;
        // xgettext:no-c-format
	stl << i18n( "25%" );
        // xgettext:no-c-format
	stl << i18n( "50%" );
        // xgettext:no-c-format
	stl << i18n( "100%" );
        // xgettext:no-c-format
	stl << i18n( "200%" );
        // xgettext:no-c-format
	stl << i18n( "300%" );
        // xgettext:no-c-format
	stl << i18n( "400%" );
        // xgettext:no-c-format
	stl << i18n( "800%" );
	stl << i18n( "Whole Page" )
            << i18n( "Zoom Width" );

	m_zoomAction->setItems( stl );
	m_zoomAction->setEditable( true );
	m_zoomAction->setCurrentItem( 7 );

	KStdAction::zoomIn( this, SLOT( viewZoomIn() ), actionCollection(), "view_zoom_in" );
	KStdAction::zoomOut( this, SLOT( viewZoomOut() ), actionCollection(), "view_zoom_out" );

	m_showPageMargins = new KToggleAction(KIcon("view_margins"), i18n("Show Page Margins"), actionCollection(), "view_show_margins");
	connect( m_showPageMargins, SIGNAL(toggled(bool)), SLOT(togglePageMargins(bool)));
	m_showPageMargins->setCheckedState(i18n("Hide Page Margins"));

	// No need for the other actions in read-only (embedded) mode
	if( !shell() )
		return;

	// edit ----->
	KStdAction::cut(this, SLOT(editCut()), actionCollection(), "edit_cut");
	KStdAction::copy(this, SLOT(editCopy()), actionCollection(), "edit_copy");
	KStdAction::paste(this, SLOT(editPaste()), actionCollection(), "edit_paste");
	KStdAction::selectAll(this, SLOT(editSelectAll()), actionCollection(), "edit_select_all");
	KStdAction::deselect(this, SLOT(editDeselectAll()), actionCollection(), "edit_deselect_all");

	KAction *actionImportGraphic = new KAction(i18n("&Import Graphic..."), actionCollection(), "file_import");
	connect(actionImportGraphic, SIGNAL(triggered()), this, SLOT(fileImportGraphic()));

	m_deleteSelectionAction = new KAction(KIcon("editdelete"), i18n("D&elete"), actionCollection(), "edit_delete");
	m_deleteSelectionAction->setShortcut(QKeySequence("Del"));
	connect(m_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));

	KAction *actionPurgeHistory = new KAction(i18n("&History"), actionCollection(), "edit_purge_history");
	connect(actionPurgeHistory, SIGNAL(triggered()), this, SLOT(editPurgeHistory()));
	// edit <-----

	// object ----->
	KAction *actionDuplicate = new KAction(KIcon("duplicate"), i18n("&Duplicate"), actionCollection(), "object_duplicate");
	actionDuplicate->setShortcut(QKeySequence("Ctrl+D"));
	connect(actionDuplicate, SIGNAL(triggered()), this, SLOT(selectionDuplicate()));

	KAction *actionBringToFront = new KAction(KIcon("bring_forward"), i18n("Bring to &Front"), actionCollection(), "object_move_totop");
	actionBringToFront->setShortcut(QKeySequence("Ctrl+Shift+]"));
	connect(actionBringToFront, SIGNAL(triggered()), this, SLOT(selectionBringToFront()));

	KAction *actionRaise = new KAction(KIcon("raise"), i18n("&Raise"), actionCollection(), "object_move_up");
	actionRaise->setShortcut(QKeySequence("Ctrl+]"));
	connect(actionRaise, SIGNAL(triggered()), this, SLOT(selectionMoveUp()));

	KAction *actionLower = new KAction(KIcon("lower"), i18n("&Lower"), actionCollection(), "object_move_down");
	actionLower->setShortcut(QKeySequence("Ctrl+["));
	connect(actionLower, SIGNAL(triggered()), this, SLOT(selectionMoveDown()));

	KAction *actionSendToBack = new KAction(KIcon("send_backward"), i18n("Send to &Back"), actionCollection(), "object_move_tobottom");
	actionSendToBack->setShortcut(QKeySequence("Ctrl+Shift+["));
	connect(actionSendToBack, SIGNAL(triggered()), this, SLOT(selectionSendToBack()));

	KAction *actionAlignLeft = new KAction(KIcon("aoleft"), i18n("Align Left"), actionCollection(), "object_align_horizontal_left");
	connect(actionAlignLeft, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalLeft()));

	KAction *actionAlignCenter = new KAction(KIcon("aocenterh"), i18n("Align Center (Horizontal)"), actionCollection(), "object_align_horizontal_center");
	connect(actionAlignCenter, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalCenter()));

	KAction *actionAlignRight = new KAction(KIcon("aoright"), i18n("Align Right"), actionCollection(), "object_align_horizontal_right");
	connect(actionAlignRight, SIGNAL(triggered()), this, SLOT(selectionAlignHorizontalRight()));

	KAction *actionAlignTop = new KAction(KIcon("aotop"), i18n("Align Top"), actionCollection(), "object_align_vertical_top");
	connect(actionAlignTop, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalTop()));

	KAction *actionAlignMiddle = new KAction(KIcon("aocenterv"), i18n("Align Middle (Vertical)"), actionCollection(), "object_align_vertical_center");
	connect(actionAlignMiddle, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalCenter()));

	KAction *actionAlignBottom = new KAction(KIcon("aobottom"), i18n("Align Bottom"), actionCollection(), "object_align_vertical_bottom");
	connect(actionAlignBottom, SIGNAL(triggered()), this, SLOT(selectionAlignVerticalBottom()));

	KAction *actionDistributeHorizontalCenter = new KAction(i18n("Distribute Center (Horizontal)"), actionCollection(), "object_distribute_horizontal_center");
	connect(actionDistributeHorizontalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalCenter()));

	KAction *actionDistributeHorizontalGap = new KAction(i18n("Distribute Gaps (Horizontal)"), actionCollection(), "object_distribute_horizontal_gap");
	connect(actionDistributeHorizontalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalGap()));

	KAction *actionDistributeLeft = new KAction(i18n("Distribute Left Borders"), actionCollection(), "object_distribute_horizontal_left");
	connect(actionDistributeLeft, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalLeft()));

	KAction *actionDistributeRight = new KAction(i18n("Distribute Right Borders"), actionCollection(), "object_distribute_horizontal_right");
	connect(actionDistributeRight, SIGNAL(triggered()), this, SLOT(selectionDistributeHorizontalRight()));

	KAction *actionDistributeVerticalCenter = new KAction(i18n("Distribute Center (Vertical)"), actionCollection(), "object_distribute_vertical_center");
	connect(actionDistributeVerticalCenter, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalCenter()));

	KAction *actionDistributeVerticalGap = new KAction(i18n("Distribute Gaps (Vertical)"), actionCollection(), "object_distribute_vertical_gap");
	connect(actionDistributeVerticalGap, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalGap()));

	KAction *actionDistributeBottom = new KAction(i18n("Distribute Bottom Borders"), actionCollection(), "object_distribute_vertical_bottom");
	connect(actionDistributeBottom, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalBottom()));

	KAction *actionDistributeTop = new KAction(i18n("Distribute Top Borders"), actionCollection(), "object_distribute_vertical_top");
	connect(actionDistributeTop, SIGNAL(triggered()), this, SLOT(selectionDistributeVerticalTop()));

	m_showRulerAction = new KToggleAction(i18n("Show Rulers"), actionCollection(), "view_show_ruler");
	m_showRulerAction->setCheckedState(i18n("Hide Rulers"));
	m_showRulerAction->setToolTip(i18n("Shows or hides rulers"));
	m_showRulerAction->setChecked(false);
	connect( m_showRulerAction, SIGNAL(triggered()), this, SLOT(showRuler()));

	m_showGridAction = new KToggleAction(KIcon("view_grid"), i18n("Show Grid"), actionCollection(), "view_show_grid");
	m_showGridAction->setCheckedState(i18n("Hide Grid"));
	m_showGridAction->setToolTip(i18n("Shows or hides grid"));
	//m_showGridAction->setChecked(true);
	connect(m_showGridAction, SIGNAL(triggered()), this, SLOT(showGrid()));

	m_snapGridAction = new KToggleAction(i18n("Snap to Grid"), actionCollection(), "view_snap_to_grid");
	m_snapGridAction->setToolTip(i18n( "Snaps to grid"));
	//m_snapGridAction->setChecked(true);
	connect(m_snapGridAction, SIGNAL(triggered()), this, SLOT(snapToGrid()));

	m_groupObjects = new KAction(KIcon("group"), i18n("&Group Objects"), actionCollection(), "selection_group");
	m_groupObjects->setShortcut(QKeySequence("Ctrl+G"));
	connect(m_groupObjects, SIGNAL(triggered()), this, SLOT(groupSelection()));

	m_ungroupObjects = new KAction(KIcon("ungroup"), i18n("&Ungroup Objects"), actionCollection(), "selection_ungroup");
	m_ungroupObjects->setShortcut(QKeySequence("Ctrl+Shift+G"));
	connect(m_ungroupObjects, SIGNAL(triggered()), this, SLOT(ungroupSelection()));

	m_closePath = new KAction(i18n("&Close Path"), actionCollection(), "close_path");
	m_closePath->setShortcut(QKeySequence("Ctrl+U"));
	connect(m_closePath, SIGNAL(triggered()), this, SLOT(closePath()));
	// object <-----

	// line style (dashes)
	// TODO: KoLineStyleAction isn't ported yet.
	//m_lineStyleAction = new KoLineStyleAction(i18n("Line Style"), "linestyle", this, SLOT(setLineStyle(int)), actionCollection(), "setLineStyle");

	// line width
	m_setLineWidth = new KoUnitDoubleSpinComboBox( this, 0.0, 1000.0, 0.5, 1.0, KoUnit::U_PT, 1 );
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

	m_configureAction = new KAction(KIcon("configure"), i18n("Configure Karbon..."), actionCollection(), "configure");
	connect(m_configureAction, SIGNAL(triggered()), this, SLOT(configure()));

	KAction *actionPageLayout = new KAction(i18n("Page &Layout..."), actionCollection(), "page_layout");
	connect(actionPageLayout, SIGNAL(triggered()), this, SLOT(pageLayout()));

	m_contextHelpAction = new KoContextHelpAction( actionCollection(), this );
}

void
KarbonView::paintEverything( QPainter& /*p*/, const QRect& /*rect*/, bool /*transparent*/ )
{
	debugView("KarbonView::paintEverything(...)");
}

bool
KarbonView::mouseEvent( QMouseEvent* event, const QPointF &p )
{
	debugView(QString("KarbonView::mouseEvent(event, QPointF(%1, %2))").arg(p.x()).arg(p.y()));

	int mx = event->pos().x();
	int my = event->pos().y();

	int px;
	int py;
	if( m_canvasView->horizontalScrollBar()->isVisible() && ((m_canvasView->horizontalScrollBar()->value() - m_canvasView->canvasOffsetX()) > 0))
		px = mx;
	else
		px = (mx + m_canvas->canvasWidget()->x() - m_canvasView->canvasOffsetX()); // TODO: needs some checking

	if( m_canvasView->verticalScrollBar()->isVisible() && ((m_canvasView->verticalScrollBar()->value() - m_canvasView->canvasOffsetY()) > 0))
		py = my;
	else
		py = (my + m_canvas->canvasWidget()->y() - m_canvasView->canvasOffsetY());

	m_horizRuler->updatePointer(px, py);
	m_vertRuler->updatePointer(px, py);

	QPointF xy;
	xy.setX((mx + m_canvas->canvasWidget()->x() - m_canvasView->canvasOffsetX()) / zoom());
	xy.setY( qRound(m_part->document().height()) - (my + m_canvas->canvasWidget()->y() - m_canvasView->canvasOffsetY()) / zoom());

	xy.setX(KoUnit::toUserValue(xy.x(), part()->unit()));
	xy.setY(KoUnit::toUserValue(xy.y(), part()->unit()));

	m_cursorCoords->setText( QString( "%1, %2" ).arg(KGlobal::_locale->formatNumber(xy.x(), 2)).arg(KGlobal::_locale->formatNumber(xy.y(), 2)) );

	if( toolController() )
		return toolController()->mouseEvent( event, p );
	else
		return false;
}

bool
KarbonView::keyEvent( QEvent* event )
{
	debugView("KarbonView::keyEvent(event)");

	if( toolController() )
		return toolController()->keyEvent( event );
	else
		return false;
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
		m_canvas->setGeometry( rulerWidth, rulerHeight, width() - rulerWidth, height() - rulerHeight );
		updateRuler();
	}
	else
	{
		m_horizRuler->hide();
		m_vertRuler->hide();
		m_canvas->setGeometry( 0, 0, width(), height() );
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
	m_canvas->updateCanvas(m_canvas->canvasWidget()->rect());
}

void
KarbonView::updateRuler()
{
	debugView("KarbonView::updateRuler()");

	/* TODO: port: if(!m_canvas->horizontalScrollBar()->isVisible())
	{
		if( (1 + m_canvas->pageOffsetX() - m_canvas->contentsX()) >= 0 )
		{
			m_horizRuler->setGeometry( 1 + rulerWidth + m_canvas->pageOffsetX() - m_canvas->contentsX(), 0, qRound( 1 + part()->document().width() * zoom() ), rulerHeight );
			m_horizRuler->updateVisibleArea(0,0);
		}
		else
		{
			m_horizRuler->setGeometry( rulerWidth, 0, qRound( 1 + part()->document().width() * zoom() ) - m_canvas->contentsX() + m_canvas->pageOffsetX(), rulerHeight );
			m_horizRuler->updateVisibleArea((m_canvas->contentsX() - m_canvas->pageOffsetX()),0);
		}
	}

	if(!m_canvas->verticalScrollBar()->isVisible())
	{
		if( (1 + m_canvas->pageOffsetY() - m_canvas->contentsY()) >= 0 )
		{
			m_vertRuler->setGeometry( 0, 1 + rulerHeight + m_canvas->pageOffsetY() - m_canvas->contentsY(), rulerWidth, 1 + qRound( part()->document().height() * zoom() ));
			m_vertRuler->updateVisibleArea(0,0);
		}
		else
		{
			m_vertRuler->setGeometry( 0, 1 + rulerHeight, rulerWidth, 1 + qRound( part()->document().height() * zoom() ) + m_canvas->contentsY() - m_canvas->pageOffsetY() );
			m_vertRuler->updateVisibleArea(0, (m_canvas->contentsY() - m_canvas->pageOffsetY()));
		}
	}*/
}

void
KarbonView::showGrid()
{
	debugView("KarbonView::showGrid()");

	m_part->document().grid().isShow = m_showGridAction->isChecked();
}

void
KarbonView::snapToGrid()
{
	debugView("KarbonView::snapToGrid()");

	m_part->document().grid().isSnap = m_snapGridAction->isChecked();
}

void
KarbonView::showSelectionPopupMenu( const QPoint &pos )
{
	debugView(QString("KarbonView::showSelectionPopupMenu(QPoint(%1, %2))").arg(pos.x()).arg(pos.y()));

	QList<KAction*> actionList;
	if( m_groupObjects->isEnabled() )
		actionList.append( m_groupObjects );
	else if( m_ungroupObjects->isEnabled() )
		actionList.append( m_ungroupObjects );
	if( m_closePath->isEnabled() )
		actionList.append( m_closePath );
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
	KoUnit::Unit unit = part()->unit();
	if( KoPageLayoutDia::pageLayout( layout, hf, FORMAT_AND_BORDERS | DISABLE_UNIT, unit ) )
	{
		part()->setPageLayout( layout, unit );
		m_horizRuler->setUnit( unit );
		m_vertRuler->setUnit( unit );
		m_canvas->canvasWidget()->resize( int( ( part()->pageLayout().ptWidth + 300 ) * zoom() ),
								  int( ( part()->pageLayout().ptHeight + 460 ) * zoom() ) );
		part()->repaintAllViews();

		emit pageLayoutChanged();
	}
}

void
KarbonView::canvasContentsMoving( int x, int y )
{
	debugView(QString("KarbonView::canvasContentsMoving(x = %1, y = %2)").arg(x).arg(y));

	/* TODO: port: if( m_canvas->horizontalScrollBar()->isVisible() )
	{
		if( shell() && m_showRulerAction->isChecked() )
		{
			if( (1 + m_canvas->pageOffsetX() - x) >= 0)
			{
				m_horizRuler->setGeometry( 1 + rulerWidth + m_canvas->pageOffsetX() - x, 0, qRound( 1 + 	part()->document().width() * zoom() ), rulerHeight );
				m_horizRuler->updateVisibleArea(0,0);
			}
			else
			{
				m_horizRuler->setGeometry( rulerWidth, 0, qRound( 1 + part()->document().width() * zoom() ) - x + m_canvas->pageOffsetX(), rulerHeight );
				m_horizRuler->updateVisibleArea((x - m_canvas->pageOffsetX()),0);
			}
		}
	}

	if( m_canvas->verticalScrollBar()->isVisible() )
	{
		if( shell() && m_showRulerAction->isChecked() )
		{
			if( (1 + m_canvas->pageOffsetY() - y) >= 0)
			{
				m_vertRuler->setGeometry( 0, 1 + rulerHeight + m_canvas->pageOffsetY() - y , rulerWidth, 1 + qRound( part()->document().height() * zoom() ));
				m_vertRuler->updateVisibleArea(0,0);
			}
			else
			{
				m_vertRuler->setGeometry( 0, 1 + rulerHeight, rulerWidth, 1 + qRound( part()->document().height() * zoom() ) - y + m_canvas->pageOffsetY() );
				m_vertRuler->updateVisibleArea(0, (y - m_canvas->pageOffsetY()));
			}
		}
	}*/
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
	m_deleteSelectionAction->setEnabled( count > 0 );

	kDebug(38000) << count << " shapes selected" << endl;

	if( count > 0 )
	{
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
		
		// check all selected shapes if they are grouped
		// and enable the ungroup action if at least one is found
		foreach( KoShape* shape, selection->selectedShapes() )
		{
			if( dynamic_cast<KoShapeGroup*>( shape->parent() ) )
			{
				m_ungroupObjects->setEnabled( true );
				break;
			}
		}
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

	m_canvas->updateCanvas(r);
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
KarbonView::setUnit( KoUnit::Unit /*_unit*/ )
{
	debugView("KarbonView::setUnit(KoUnit::Unit)");
}

void KarbonView::createDocumentTabDock()
{
	debugView("KarbonView::createDocumentTabDock()");

	m_DocumentTab = new VDocumentTab(this, this);
	m_DocumentTab->setWindowTitle(i18n("Document"));
	paletteManager()->addWidget(m_DocumentTab, "DocumentTabDock", "DocumentPanel");
	connect( m_part, SIGNAL( unitChanged( KoUnit::Unit ) ), m_DocumentTab, SLOT( updateDocumentInfo() ) );
}

void KarbonView::createLayersTabDock()
{
	debugView("KarbonView::createLayersTabDock()");

	m_LayersTab = new VLayersTab(this, this);
	m_LayersTab->setWindowTitle(i18n("Layers"));
	paletteManager()->addWidget(m_LayersTab, "LayersTabDock", "DocumentPanel");
}

void KarbonView::createHistoryTabDock()
{
	debugView("KarbonView::createHistoryTabDock()");

	m_HistoryTab = new VHistoryTab(part(), this);
	m_HistoryTab->setWindowTitle(i18n("History"));
	paletteManager()->addWidget(m_HistoryTab, "HistoryTabDock", "DocumentPanel");
}

void KarbonView::createStrokeDock()
{
	debugView("KarbonView::createStrokeDock()");

	m_strokeDocker = new VStrokeDocker(part(), this);
	m_strokeDocker->setWindowTitle(i18n("Stroke Properties"));
	paletteManager()->addWidget(m_strokeDocker, "StrokeTabDock", "StrokePanel");

	connect( part(), SIGNAL( unitChanged( KoUnit::Unit ) ), m_strokeDocker, SLOT( setUnit( KoUnit::Unit ) ) );
}

void KarbonView::createColorDock()
{
	debugView("KarbonView::createColorDock()");

	m_ColorManager = new VColorDocker(part(),this);
	//m_ColorManager->setWindowTitle(i18n("Stroke Properties"));
	paletteManager()->addWidget(m_ColorManager, "ColorTabDock", "ColorPanel");

	connect( this, SIGNAL( selectionChange() ), m_ColorManager, SLOT( update() ) );
}

void KarbonView::createTransformDock()
{
	debugView("KarbonView::createTransformDock()");

	m_TransformDocker = new VTransformDocker(part(), this);
	m_TransformDocker->setWindowTitle(i18n("Transform"));
	paletteManager()->addWidget(m_TransformDocker, "TransformTabDock", "TransformPanel");

	connect( this, SIGNAL( selectionChange() ), m_TransformDocker, SLOT( update() ) );
	connect( part(), SIGNAL( unitChanged( KoUnit::Unit ) ), m_TransformDocker, SLOT( setUnit( KoUnit::Unit ) ) );
}

void KarbonView::createResourceDock()
{
	debugView("KarbonView::createResourceDock()");

	m_styleDocker = new VStyleDocker( part(), this );
	m_styleDocker->setWindowTitle(i18n("Resources"));
	paletteManager()->addWidget(m_styleDocker, "ResourceTabDock", "ResourcePanel");
}

VToolController *
KarbonView::toolController()
{
	debugView("KarbonView::toolController()");

	return m_toolController;
}

#include "karbon_view.moc"

