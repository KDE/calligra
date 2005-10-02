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

#include <qdragobject.h>
#include <qiconset.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qpopupmenu.h>

#include <kaction.h>
#include <kcolordrag.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdeversion.h>

#include <koMainWindow.h>
#include <koFilterManager.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kstdaction.h>
#include <kocontexthelp.h>
#include <koUnitWidgets.h>
#include <koPageLayoutDia.h>
#include <vruler.h>

// Commands.
#include "valigncmd.h"
#include "vcleanupcmd.h"
#include "vclipartcmd.h"
#include "vclosepathcmd.h"
#include "vdeletecmd.h"
#include "vfillcmd.h"
#include "vgroupcmd.h"
#include "vstrokecmd.h"
#include "vtransformcmd.h"
#include "vungroupcmd.h"
#include "vzordercmd.h"

// Dialogs.
#include "vconfiguredlg.h"

// Dockers.
#include <kopalettemanager.h>
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
#include "karbon_view_iface.h"
#include "vglobal.h"
#include "vselection.h"
#include "vtool.h"
#include "vtoolcontroller.h"
#include "vcomposite.h"
#include "vgroup.h"
#include "vpainterfactory.h"
#include "vqpainter.h"
#include "vstrokefillpreview.h"
#include "vstatebutton.h"
#include "vcanvas.h"
#include "vtoolbox.h"
#include "karbon_drag.h"
// #include "vselectnodestool.h"

#include <unistd.h>

// Only for debugging.
#include <kdebug.h>


KarbonView::KarbonView( KarbonPart* p, QWidget* parent, const char* name )
		: KarbonViewBase( p, parent, name ), KXMLGUIBuilder( shell() )
{
	m_toolbox = 0L;

	setInstance( KarbonFactory::instance(), true );

	setClientBuilder( this );

	if( !p->isReadWrite() )
		setXMLFile( QString::fromLatin1( "karbon_readonly.rc" ) );
	else
		setXMLFile( QString::fromLatin1( "karbon.rc" ) );

	m_dcop = 0L;
	dcopObject(); // build it

	// set up status bar message
	m_status = new KStatusBarLabel( QString::null, 0, statusBar() );
	m_status->setAlignment( AlignLeft | AlignVCenter );
	m_status->setMinimumWidth( 300 );
	addStatusBarItem( m_status, 1 );
	m_cursorCoords = new KStatusBarLabel( QString::null, 0, statusBar() );
	m_cursorCoords->setAlignment( AlignLeft | AlignVCenter );
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

	m_pPaletteManager = new KoPaletteManager(this, actionCollection(), "kivio palette manager");

	if( shell() )
	{
		//Create Dockers
		createColorDock();
		createStrokeDock();
		createTransformDock();
	}

	unsigned int max = part()->maxRecentFiles();
	setNumberOfRecentFiles( max );

	reorganizeGUI();

	// widgets:
	m_horizRuler = new VRuler( Qt::Horizontal, this );
	m_horizRuler->setUnit(p->unit());
	connect( p, SIGNAL( unitChanged( KoUnit::Unit ) ), m_horizRuler, SLOT( setUnit( KoUnit::Unit ) ) );

	m_vertRuler = new VRuler( Qt::Vertical, this );
	m_vertRuler->setUnit(p->unit());
	connect( p, SIGNAL( unitChanged( KoUnit::Unit ) ), m_vertRuler, SLOT( setUnit( KoUnit::Unit ) ) );

	m_canvas = new VCanvas( this, this, p );
	connect( m_canvas, SIGNAL( contentsMoving( int, int ) ), this, SLOT( canvasContentsMoving( int, int ) ) );

	m_canvas->show();

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

	// set up factory
	m_painterFactory = new VPainterFactory;
	m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );
	m_painterFactory->setEditPainter( canvasWidget()->viewport(), width(), height() );

	zoomChanged();
}

KarbonView::~KarbonView()
{
	kdDebug(38000) << "Handling KarbonView dtor" << endl;
	if( shell() )
	{
		delete( m_ColorManager );
		disconnect( m_ColorManager, SIGNAL( colorChanged() ), this, SLOT( colorDockerChanged() ) );
		delete( m_strokeDocker );
		delete( m_styleDocker );
		disconnect( this, SIGNAL( selectionChange() ), m_TransformDocker, SLOT( update() ) );
		delete( m_TransformDocker );
	}

	// widgets:
	delete( m_smallPreview );
	delete( m_status );
	delete( m_cursorCoords );

	delete( m_painterFactory );

	delete( m_canvas );

	delete( m_dcop );

	//delete( m_toolbox );
}

QWidget *
KarbonView::createContainer( QWidget *parent, int index, const QDomElement &element, int &id )
{
	if( element.attribute( "name" ) == "Tools" )
	{
		if( !m_toolbox )
			m_toolbox = new VToolBox( (KarbonPart *)m_part, mainWindow(), "Tools" );
		else
		{
			m_toolbox = dynamic_cast<VToolBox *>( shell()->toolBar( "Tools" ) );
			mainWindow()->moveDockWindow( m_toolbox, Qt::DockLeft, false, 0 );
			return m_toolbox;
		}

		m_toolbox->setupTools();
		connect( m_toolbox, SIGNAL( activeToolChanged( VTool * ) ), this, SLOT( slotActiveToolChanged( VTool * ) ) );

		if( shell() )
		{
			m_strokeFillPreview = m_toolbox->strokeFillPreview();
			connect( m_strokeFillPreview, SIGNAL( strokeChanged( const VStroke & ) ), this, SLOT( slotStrokeChanged( const VStroke & ) ) );
			connect( m_strokeFillPreview, SIGNAL( fillChanged( const VFill & ) ), this, SLOT( slotFillChanged( const VFill & ) ) );

			connect( m_strokeFillPreview, SIGNAL( strokeSelected() ), m_ColorManager, SLOT( setStrokeDocker() ) );
			connect( m_strokeFillPreview, SIGNAL( fillSelected( ) ), m_ColorManager, SLOT( setFillDocker() ) );
			selectionChanged();

			//create toolbars
// 			m_selectToolBar = new VSelectToolBar( this, "selecttoolbar" );
// 			mainWindow()->addToolBar( m_selectToolBar );

			createDocumentTabDock();
			createLayersTabDock();
			createHistoryTabDock();
		}

		mainWindow()->moveDockWindow( m_toolbox, Qt::DockLeft, false, 0 );
		part()->toolController()->setActiveView( this );
		return m_toolbox;
	}

	return KXMLGUIBuilder::createContainer( parent, index, element, id );
}

void
KarbonView::removeContainer( QWidget *container, QWidget *parent,
							 QDomElement &element, int id )
{
	if( container )
		kdDebug(38000) << container << endl;

	if( shell() && container == m_toolbox )
	{
		delete m_toolbox;
		m_toolbox = 0L;
// 		delete m_selectToolBar;
// 		m_selectToolBar = 0L;
		delete m_DocumentTab;
		delete m_LayersTab;
		delete m_HistoryTab;
	}
	else
		KXMLGUIBuilder::removeContainer( container, parent, element, id );
}


DCOPObject *
KarbonView::dcopObject()
{
	if( !m_dcop )
		m_dcop = new KarbonViewIface( this );

	return m_dcop;
}

QWidget*
KarbonView::canvas()
{
	return m_canvas;
}

void
KarbonView::resizeEvent( QResizeEvent* /*event*/ )
{
    int hSpace = 20;
    int vSpace = 20;

	if( m_showRulerAction->isChecked())
	{
		updateRuler();
		m_canvas->setGeometry( hSpace, vSpace, width() - hSpace, height() - vSpace );
	}
	else
	{
		m_horizRuler->hide();
		m_vertRuler->hide();
		m_canvas->setGeometry( 0, 0, width(), height() );
	}

	zoomChanged();
	reorganizeGUI();
}

void
KarbonView::dropEvent( QDropEvent *e )
{
	//Accepts QColor - from Color Manager's KColorPatch
	QColor color;
	VColor realcolor;
	VObjectList selection;

	if( KColorDrag::decode( e, color ) )
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
	else if( KarbonDrag::decode( e, selection, m_part->document() ) )
	{
		VObject *clipart = selection.first();
		KoPoint p( e->pos() );
		p = m_canvas->toContents( p );
		QWMatrix mat( 1, 0, 0, 1, p.x(), p.y() );

		VTransformCmd trafo( 0L, mat );
		trafo.visit( *clipart );
		VClipartCmd* cmd = new VClipartCmd( &m_part->document(), i18n( "Insert Clipart" ), clipart );

		m_part->addCommand( cmd, true );
	}
}

void
KarbonView::print( KPrinter &printer )
{
	// TODO : ultimately use plain QPainter here as that is better suited to print system
	kdDebug(38000) << "KarbonView::print" << endl;
	VQPainter p( ( QPaintDevice * ) & printer, width(), height() );
	p.begin();
	p.setZoomFactor( 1.0 );
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0, -part()->document().height() );
	p.setWorldMatrix( mat );

	// print the doc using QPainter at zoom level 1
	// TODO : better use eps export?
	// TODO : use real page layout stuff
	KoRect rect( 0, 0, width(), height() );
	p.setPen(Qt::NoPen);
	p.setBrush(Qt::white);
	p.drawRect(rect);

	part()->document().draw( &p, &rect );

	p.end();
}

void
KarbonView::fileImportGraphic()
{
	QStringList filter;
	filter << "application/x-karbon" << "image/svg+xml" << "image/x-wmf" << "image/x-eps" << "application/postscript";
	KFileDialog *dialog = new KFileDialog( "foo", QString::null, 0L, "Choose Graphic to Add", true);
	dialog->setMimeFilter( filter, "application/x-karbon" );
	if(dialog->exec()!=QDialog::Accepted) {
		delete dialog;
		return;
	}
	QString fname = dialog->selectedFile();
	//kdDebug(38000) << "in : " << fname.latin1() << endl;
	//kdDebug(38000) << "part()->document()->nativeFormatMimeType().latin1() : " << part()->nativeFormatMimeType() << endl;
	//kdDebug(38000) << "dialog->currentMimeFilter().latin1() : " << dialog->currentMimeFilter().latin1() << endl;
	if( part()->nativeFormatMimeType() == dialog->currentMimeFilter().latin1() )
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
	addSelectionToClipboard();
	// remove selection
	editDeleteSelection();
}

void
KarbonView::editCopy()
{
	addSelectionToClipboard();
}

void
KarbonView::addSelectionToClipboard() const
{
	if( part()->document().selection()->objects().count() <= 0 )
		return;

	KarbonDrag* kd = new KarbonDrag();
	kd->setObjectList( part()->document().selection()->objects() );
	QApplication::clipboard()->setData( kd );
}

void
KarbonView::editPaste()
{
	KarbonDrag kd;
	VObjectList selection;

	if( kd.decode( QApplication::clipboard()->data(), selection, part()->document() ) ) {
		part()->document().selection()->clear();

		// Calc new selection
		VObjectListIterator itr( selection );
		double copyOffset = m_part->instance()->config()->readNumEntry( "CopyOffset", 10 );

		for( ; itr.current() ; ++itr )
		{
			VObject *obj = itr.current();
			part()->document().selection()->append( obj );
			part()->insertObject( obj );
 			VTranslateCmd cmd( 0L, copyOffset, -copyOffset );
 			cmd.visit( *obj );
		}

		part()->repaintAllViews();
		selectionChanged();
	}
}

void
KarbonView::editSelectAll()
{
	part()->document().selection()->append();

	if( part()->document().selection()->objects().count() > 0 )
		part()->repaintAllViews();

	selectionChanged();
}

void
KarbonView::editDeselectAll()
{
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
	kdDebug(38000) << "*********" << endl;

	if( part()->document().selection()->objects().count() > 0 )
	{
		part()->addCommand(
			new VDeleteCmd( &part()->document() ),
			true );
	}
}

void
KarbonView::editPurgeHistory()
{
	// TODO: check for history size != 0

	if( KMessageBox::warningContinueCancel( this,
			i18n( "This action cannot be undone later. Do you really want to continue?" ),
			i18n( "Purge History" ),
			KStdGuiItem::cont(),
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
	part()->addCommand(
		new VAlignCmd( &part()->document(), VAlignCmd::ALIGN_HORIZONTAL_LEFT ), true );
}
void
KarbonView::selectionAlignHorizontalCenter()
{
	part()->addCommand(
		new VAlignCmd( &part()->document(), VAlignCmd::ALIGN_HORIZONTAL_CENTER ), true );
}

void
KarbonView::selectionAlignHorizontalRight()
{
	part()->addCommand(
		new VAlignCmd( &part()->document(), VAlignCmd::ALIGN_HORIZONTAL_RIGHT ), true );
}

void
KarbonView::selectionAlignVerticalTop()
{
	part()->addCommand(
		new VAlignCmd( &part()->document(), VAlignCmd::ALIGN_VERTICAL_TOP ), true );
}

void
KarbonView::selectionAlignVerticalCenter()
{
	part()->addCommand(
		new VAlignCmd( &part()->document(), VAlignCmd::ALIGN_VERTICAL_CENTER ), true );
}

void
KarbonView::selectionAlignVerticalBottom()
{
	part()->addCommand(
		new VAlignCmd( &part()->document(), VAlignCmd::ALIGN_VERTICAL_BOTTOM ), true );
}

void
KarbonView::selectionBringToFront()
{
	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::bringToFront ), true );
}

void
KarbonView::selectionMoveUp()
{
	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::up ), true );
}

void
KarbonView::selectionMoveDown()
{
	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::down ), true );
}

void
KarbonView::selectionSendToBack()
{
	part()->addCommand(
		new VZOrderCmd( &part()->document(), VZOrderCmd::sendToBack ), true );
}

void
KarbonView::groupSelection()
{
	part()->addCommand( new VGroupCmd( &part()->document() ), true );
}

void
KarbonView::ungroupSelection()
{
	part()->addCommand( new VUnGroupCmd( &part()->document() ), true );
}

void
KarbonView::closePath()
{
	part()->addCommand( new VClosePathCmd( &part()->document() ), true );
}

// TODO: remove this one someday:
void
KarbonView::dummyForTesting()
{
	kdDebug(38000) << "KarbonView::dummyForTesting()" << endl;
}

void
KarbonView::slotActiveToolChanged( VTool *tool )
{
	part()->toolController()->setActiveTool( tool );

	m_canvas->repaintAll();
}

void
KarbonView::viewModeChanged()
{
	canvasWidget()->pixmap()->fill();

	if( m_viewAction->currentItem() == 1 )
		m_painterFactory->setWireframePainter( canvasWidget()->pixmap(), width(), height() );
	else
		m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );

	m_canvas->repaintAll();
}

void
KarbonView::setZoomAt( double zoom, const KoPoint &p )
{
	QString zoomText = QString( "%1%" ).arg( zoom * 100.0, 0, 'f', 2 );
	QStringList stl = m_zoomAction->items();
	if( stl.first() == "25%" )
	{
		stl.prepend( zoomText.latin1() );
		m_zoomAction->setItems( stl );
		m_zoomAction->setCurrentItem( 0 );
	}
	else
	{
		m_zoomAction->setCurrentItem( 0 );
		m_zoomAction->changeItem( m_zoomAction->currentItem(), zoomText.latin1() );
	}
	zoomChanged( p );
}

void
KarbonView::viewZoomIn()
{
	setZoomAt( zoom() * 1.50 );
}

void
KarbonView::viewZoomOut()
{
	setZoomAt( zoom() * 0.75 );
}

void
KarbonView::zoomChanged( const KoPoint &p )
{
	double centerX;
	double centerY;
	double zoomFactor;

	int hSpace = 20;
	int vSpace = 20;

	if( !p.isNull() )
	{
		centerX = ( ( p.x() ) * zoom() + m_canvas->pageOffsetX() ) / double( m_canvas->contentsWidth() );
		centerY = 1 - ( ( p.y() ) * zoom() + m_canvas->pageOffsetY() ) / double( m_canvas->contentsHeight() );
		zoomFactor = m_zoomAction->currentText().remove( '%' ).toDouble() / 100.0;
	}
	else if( m_zoomAction->currentText() == i18n("Zoom Width") )
	{
		centerX = 0.5;
		centerY = double( m_canvas->contentsY() + ( height() - vSpace ) / 2 ) / double( m_canvas->contentsHeight() );
		zoomFactor = double( width() - hSpace ) / double( part()->document().width() + ( hSpace / 2 ) );
	}
	else if( m_zoomAction->currentText() == i18n("Whole Page") )
	{
		centerX = 0.5;
		centerY = 0.5;
		double zoomFactorX = double( width() - hSpace ) / double( part()->document().width() + ( hSpace / 2 ) );
		double zoomFactorY = double( height() - vSpace ) / double( part()->document().height() + ( vSpace / 2 ) );
		zoomFactor = kMin( zoomFactorX, zoomFactorY );
	}
	else
	{
		if( m_canvas->contentsWidth() > width() - hSpace )
			centerX = double( m_canvas->contentsX() + ( width() - hSpace ) / 2 ) / double( m_canvas->contentsWidth() );
		else
			centerX = 0.5;
		if( m_canvas->contentsHeight() > height() - vSpace )
			centerY = double( m_canvas->contentsY() + ( height() - vSpace ) / 2 ) / double( m_canvas->contentsHeight() );
		else
			centerY = 0.5;
		zoomFactor = m_zoomAction->currentText().remove( '%' ).toDouble() / 100.0;
	}
	kdDebug(38000) << "centerX : " << centerX << endl;
	kdDebug(38000) << "centerY : " << centerY << endl;
	kdDebug(38000) << "zoomFactor : " << zoomFactor << endl;
	if( zoomFactor == 0.0 ) return;

	// above 2000% probably doesn't make sense... (Rob)
	if( zoomFactor > 20 )
	{
		zoomFactor = 20;
		m_zoomAction->changeItem( m_zoomAction->currentItem(), " 2000%" );
	}

	KoView::setZoom( zoomFactor );
	m_horizRuler->setZoom( zoomFactor );
	m_vertRuler->setZoom( zoomFactor );

	m_canvas->viewport()->setUpdatesEnabled( false );

	m_canvas->resizeContents( int( ( part()->pageLayout().ptWidth + 300 ) * zoomFactor ),
							  int( ( part()->pageLayout().ptHeight + 460 ) * zoomFactor ) );


	VPainter *painter = painterFactory()->editpainter();
	painter->setZoomFactor( zoomFactor );

	m_canvas->setViewport( centerX, centerY );
	m_canvas->repaintAll();
	m_canvas->viewport()->setUpdatesEnabled( true );


	if( m_showRulerAction->isChecked() )
	{
		updateRuler();
	}
	else
	{
		m_horizRuler->hide();
		m_vertRuler->hide();
	}
	m_canvas->viewport()->setFocus();

	emit zoomChanged( zoomFactor );
}

void
KarbonView::slotStrokeChanged( const VStroke &c )
{
	part()->document().selection()->setStroke( c );
	selectionChanged();
}

void
KarbonView::slotFillChanged( const VFill &f )
{
	part()->document().selection()->setFill( f );
	selectionChanged();
}

void
KarbonView::setLineWidth()
{
	setLineWidth( m_setLineWidth->value() );
	selectionChanged();
}

//necessary for dcop call !
void
KarbonView::setLineWidth( double val )
{
	part()->addCommand( new VStrokeCmd( &part()->document(), val ), true );
}

void
KarbonView::initActions()
{
	// view ----->
	m_viewAction = new KSelectAction(
					   i18n( "View &Mode" ), 0, this,
					   SLOT( viewModeChanged() ), actionCollection(), "view_mode" );

	m_zoomAction = new KSelectAction(
					   i18n( "&Zoom" ), "viewmag", 0, this,
					   SLOT( zoomChanged() ), actionCollection(), "view_zoom" );

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

	// No need for the other actions in read-only (embedded) mode
	if( !shell() )
		return;

	// edit ----->
	KStdAction::cut( this,
					 SLOT( editCut() ), actionCollection(), "edit_cut" );
	KStdAction::copy( this,
					  SLOT( editCopy() ), actionCollection(), "edit_copy" );
	KStdAction::paste( this,
					   SLOT( editPaste() ), actionCollection(), "edit_paste" );
	KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(), "edit_select_all" );
	KStdAction::deselect( this, SLOT( editDeselectAll() ), actionCollection(), "edit_deselect_all" );

	new KAction(
		i18n( "&Import Graphic..." ), 0, 0, this,
		SLOT( fileImportGraphic() ), actionCollection(), "file_import" );
	m_deleteSelectionAction = new KAction(
		i18n( "D&elete" ), "editdelete", QKeySequence( "Del" ), this,
		SLOT( editDeleteSelection() ), actionCollection(), "edit_delete" );
	new KAction(
		i18n( "&History" ), 0, 0, this,
		SLOT( editPurgeHistory() ), actionCollection(), "edit_purge_history" );
	// edit <-----

	// object ----->
	new KAction(
		i18n( "Bring to &Front" ), "bring_forward", QKeySequence( "Ctrl+Shift+]" ), this,
		SLOT( selectionBringToFront() ), actionCollection(), "object_move_totop" );
	new KAction(
		i18n( "&Raise" ), "raise", QKeySequence( "Ctrl+]" ), this,
		SLOT( selectionMoveUp() ), actionCollection(), "object_move_up" );
	new KAction(
		i18n( "&Lower" ), "lower", QKeySequence( "Ctrl+[" ), this,
		SLOT( selectionMoveDown() ), actionCollection(), "object_move_down" );
	new KAction(
		i18n( "Send to &Back" ), "send_backward", QKeySequence( "Ctrl+Shift+[" ), this,
		SLOT( selectionSendToBack() ), actionCollection(), "object_move_tobottom" );

	new KAction(
		i18n( "Align Left" ), "aoleft", 0, this,
		SLOT( selectionAlignHorizontalLeft() ),
		actionCollection(), "object_align_horizontal_left" );
	new KAction(
		i18n( "Align Center (Horizontal)" ), "aocenterh", 0, this,
		SLOT( selectionAlignHorizontalCenter() ),
		actionCollection(), "object_align_horizontal_center" );
	new KAction(
		i18n( "Align Right" ), "aoright", 0, this,
		SLOT( selectionAlignHorizontalRight() ),
		actionCollection(), "object_align_horizontal_right" );
	new KAction(
		i18n( "Align Top" ), "aotop", 0, this,
		SLOT( selectionAlignVerticalTop() ),
		actionCollection(), "object_align_vertical_top" );
	new KAction(
		i18n( "Align Middle (Vertical)" ), "aocenterv", 0, this,
		SLOT( selectionAlignVerticalCenter() ),
		actionCollection(), "object_align_vertical_center" );
	new KAction(
		i18n( "Align Bottom" ), "aobottom", 0, this,
		SLOT( selectionAlignVerticalBottom() ),
		actionCollection(), "object_align_vertical_bottom" );

	m_showRulerAction = new KToggleAction( i18n( "Show Rulers" ), 0, this, SLOT( showRuler() ), actionCollection(), "view_show_ruler" );
#if KDE_IS_VERSION(3,2,90)
	m_showRulerAction->setCheckedState(i18n("Hide Rulers"));
#endif
	m_showRulerAction->setToolTip( i18n( "Shows or hides rulers." ) );
	m_showRulerAction->setChecked( false );
	m_showGridAction = new KToggleAction( i18n( "Show Grid" ), "view_grid", this, SLOT( showGrid() ), actionCollection(), "view_show_grid" );
#if KDE_IS_VERSION(3,2,90)
	m_showGridAction->setCheckedState(i18n("Hide Grid"));
#endif
	m_showGridAction->setToolTip( i18n( "Shows or hides grid." ) );
	//m_showGridAction->setChecked( true );
	m_snapGridAction = new KToggleAction( i18n( "Snap to Grid" ), 0, this, SLOT( snapToGrid() ), actionCollection(), "view_snap_to_grid" );
	m_snapGridAction->setToolTip( i18n( "Snaps to grid." ) );
	//m_snapGridAction->setChecked( true );
	m_groupObjects = new KAction(
		i18n( "&Group Objects" ), "group", QKeySequence( "Ctrl+G" ), this,
		SLOT( groupSelection() ), actionCollection(), "selection_group" );
	m_ungroupObjects = new KAction(
		i18n( "&Ungroup Objects" ), "ungroup", QKeySequence( "Ctrl+Shift+G" ), this,
		SLOT( ungroupSelection() ), actionCollection(), "selection_ungroup" );
	m_closePath = new KAction(
		i18n( "&Close Path" ), QKeySequence( "Ctrl+U" ), this,
		SLOT( closePath() ), actionCollection(), "close_path" );
	// object <-----

	// line width
	m_setLineWidth = new KoUnitDoubleSpinComboBox( this, 0.0, 1000.0, 0.5, 1.0, KoUnit::U_PT, 1 );
	new KWidgetAction( m_setLineWidth, i18n( "Set Line Width" ), 0, this, SLOT( setLineWidth() ), actionCollection(), "setLineWidth" );
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

	m_configureAction = new KAction(
				i18n( "Configure Karbon..." ), "configure", 0, this,
				SLOT( configure() ), actionCollection(), "configure" );

	new KAction( i18n( "Page &Layout..." ), 0, this,
			SLOT( pageLayout() ), actionCollection(), "page_layout" );
	m_contextHelpAction = new KoContextHelpAction( actionCollection(), this );
}

void
KarbonView::paintEverything( QPainter& /*p*/, const QRect& /*rect*/, bool /*transparent*/ )
{
	kdDebug(38000) << "view->paintEverything()" << endl;
}

bool
KarbonView::mouseEvent( QMouseEvent* event, const KoPoint &p )
{
	int mx = event->pos().x();
	int my = event->pos().y();

	int px;
	int py;
	if( m_canvas->horizontalScrollBar()->isVisible() && ((m_canvas->horizontalScrollBar()->value() - m_canvas->pageOffsetX()) > 0))
		px = mx;
	else
		px = (mx + canvasWidget()->contentsX() - canvasWidget()->pageOffsetX());

	if( m_canvas->verticalScrollBar()->isVisible() && ((m_canvas->verticalScrollBar()->value() - m_canvas->pageOffsetY()) > 0))	
		py = my;
	else
		py = (my + canvasWidget()->contentsY() - canvasWidget()->pageOffsetY());

	m_horizRuler->updatePointer(px, py); 
	m_vertRuler->updatePointer(px, py); 

	KoPoint xy;
	xy.setX((mx + canvasWidget()->contentsX() - canvasWidget()->pageOffsetX())/zoom());
	xy.setY((my + canvasWidget()->contentsY() - canvasWidget()->pageOffsetY())/zoom());

	xy.setX(KoUnit::toUserValue(xy.x(), part()->unit()));
	xy.setY(KoUnit::toUserValue(xy.y(), part()->unit()));
	
	m_cursorCoords->setText( QString( "%1, %2" ).arg(KGlobal::_locale->formatNumber(xy.x(), 2)).arg(KGlobal::_locale->formatNumber(xy.y(), 2)) );
	
	part()->toolController()->setActiveView( this );
	if( part()->toolController() )
		return part()->toolController()->mouseEvent( event, p );
	else
		return false;
}

bool
KarbonView::keyEvent( QEvent* event )
{
	if( part()->toolController() )
		return part()->toolController()->keyEvent( event );
	else
		return false;
}

void
KarbonView::reorganizeGUI()
{
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
	if( shell() )	// 0L when embedded into konq !
		shell()->setMaxRecentItems( number );
}

void
KarbonView::showRuler()
{
    int hSpace = 20;
    int vSpace = 20;

	if( m_showRulerAction->isChecked() )
	{
		m_horizRuler->show();
		m_vertRuler->show();
		if( (1 + hSpace + m_canvas->pageOffsetX() - m_canvas->contentsX()) >= hSpace)
		{
			m_horizRuler->setGeometry( 1 + hSpace + m_canvas->pageOffsetX() - m_canvas->contentsX(), 0, qRound( 1 + part()->document().width() * zoom() ), vSpace );
			m_horizRuler->updateVisibleArea(0,0);
		}
		else
		{
			m_horizRuler->setGeometry( hSpace, 0, qRound( 1 + part()->document().width() * zoom() ) - m_canvas->contentsX() + m_canvas->pageOffsetX(), vSpace );
			m_horizRuler->updateVisibleArea((m_canvas->contentsX() - m_canvas->pageOffsetX()),0);
		}

		if( (1 + vSpace + m_canvas->pageOffsetY() - m_canvas->contentsY()) >= vSpace)
		{
			m_vertRuler->setGeometry( 0, 1 + vSpace - m_canvas->pageOffsetY() + m_canvas->contentsY(), hSpace, qRound( 1 + part()->document().height() * zoom() ));
			m_vertRuler->updateVisibleArea(0,0);
		}
		else
		{
			m_vertRuler->setGeometry( 0, vSpace, hSpace, qRound( 1 + part()->document().height() * zoom() ) - m_canvas->contentsY() + m_canvas->pageOffsetY() );
			m_vertRuler->updateVisibleArea(0, (m_canvas->contentsY() - m_canvas->pageOffsetY()));
		}
		m_canvas->setGeometry( hSpace, vSpace, width() - hSpace, height() - vSpace );
	}
	else
	{
		m_horizRuler->hide();
		m_vertRuler->hide();
		m_canvas->setGeometry( 0, 0, width(), height() );
	}

	zoomChanged();
}

void
KarbonView::updateRuler()
{
    int hSpace = 20;
    int vSpace = 20;

	if(!m_canvas->horizontalScrollBar()->isVisible())
	{
		if( (1 + hSpace + m_canvas->pageOffsetX() - m_canvas->contentsX()) >= hSpace)
		{
			m_horizRuler->setGeometry( 1 + hSpace + m_canvas->pageOffsetX() - m_canvas->contentsX(), 0, qRound( 1 + part()->document().width() * zoom() ), vSpace );
			m_horizRuler->updateVisibleArea(0,0);
		}
		else
		{
			m_horizRuler->setGeometry( hSpace, 0, qRound( 1 + part()->document().width() * zoom() ) - m_canvas->contentsX() + m_canvas->pageOffsetX(), vSpace );
			m_horizRuler->updateVisibleArea((m_canvas->contentsX() - m_canvas->pageOffsetX()),0);
		}
	}

	if(!m_canvas->verticalScrollBar()->isVisible())
	{
		if( (1 + vSpace + m_canvas->pageOffsetY() - m_canvas->contentsY()) >= vSpace)
		{
			m_vertRuler->setGeometry( 0, 1 + vSpace - m_canvas->pageOffsetY() + m_canvas->contentsY(), hSpace, qRound( 1 + part()->document().height() * zoom() ));
			m_vertRuler->updateVisibleArea(0,0);
		}
		else
		{
			m_vertRuler->setGeometry( 0, vSpace, hSpace, qRound( 1 + part()->document().height() * zoom() ) - m_canvas->contentsY() + m_canvas->pageOffsetY() );
			m_vertRuler->updateVisibleArea(0, (m_canvas->contentsY() - m_canvas->pageOffsetY()));
		}
	}
}

void
KarbonView::showGrid()
{
	m_part->document().grid().isShow = m_showGridAction->isChecked();
}

void
KarbonView::snapToGrid()
{
	m_part->document().grid().isSnap = m_snapGridAction->isChecked();
}

void
KarbonView::showSelectionPopupMenu( const QPoint &pos )
{
	QPtrList<KAction> actionList;
	if( m_groupObjects->isEnabled() )
		actionList.append( m_groupObjects );
	else if( m_ungroupObjects->isEnabled() )
		actionList.append( m_ungroupObjects );
	if( m_closePath->isEnabled() )
		actionList.append( m_closePath );
	plugActionList( "selection_type_action", actionList );
	((QPopupMenu *)factory()->container( "selection_popup", this ) )->exec( pos );
	unplugActionList( "selection_type_action" );
}

void
KarbonView::configure()
{
	VConfigureDlg dialog( this );
	dialog.exec();
}

void
KarbonView::pageLayout()
{
	KoHeadFoot hf;
	KoPageLayout layout = part()->pageLayout();
	KoUnit::Unit unit = part()->unit();
	if( KoPageLayoutDia::pageLayout( layout, hf, FORMAT_AND_BORDERS, unit ) )
	{
		part()->setPageLayout( layout, unit );
		m_horizRuler->setUnit( unit );
		m_vertRuler->setUnit( unit );
		m_canvas->resizeContents( int( ( part()->pageLayout().ptWidth + 300 ) * zoom() ),
								  int( ( part()->pageLayout().ptHeight + 460 ) * zoom() ) );
		part()->repaintAllViews();

		emit pageLayoutChanged();
	}
}

void
KarbonView::canvasContentsMoving( int x, int y )
{
	int hSpace = 20;
	int vSpace = 20;

	if( m_canvas->horizontalScrollBar()->isVisible() )
	{
		if( m_showRulerAction->isChecked() )
		{
			if( (1 + hSpace + m_canvas->pageOffsetX() - x) >= hSpace)
			{
				m_horizRuler->setGeometry( 1 + hSpace + m_canvas->pageOffsetX() - x, 0, qRound( 1 + 	part()->document().width() * zoom() ), vSpace );
				m_horizRuler->updateVisibleArea(0,0);
			}
			else
			{
				m_horizRuler->setGeometry( hSpace, 0, qRound( 1 + part()->document().width() * zoom() ) - x + m_canvas->pageOffsetX(), vSpace ); 
				m_horizRuler->updateVisibleArea((x - m_canvas->pageOffsetX()),0);
			}
		}
	}

	if( m_canvas->verticalScrollBar()->isVisible() )
	{
		if( m_showRulerAction->isChecked() )
		{
			if( (1 + vSpace + m_canvas->pageOffsetY() - y) >= vSpace)
			{
				m_vertRuler->setGeometry( 0, 1 + vSpace + m_canvas->pageOffsetY() - y , hSpace, qRound( 1 + part()->document().height() * zoom() ));
				m_vertRuler->updateVisibleArea(0,0);
			}
			else
			{
				m_vertRuler->setGeometry( 0, vSpace, hSpace, qRound( 1 + part()->document().height() * zoom() ) - y + m_canvas->pageOffsetY() );
				m_vertRuler->updateVisibleArea(0, (y - m_canvas->pageOffsetY()));
			}
		}
	}
}

void KarbonView::colorDockerChanged()
{
	if ( m_ColorManager->isStrokeDocker() && m_part && m_part->document().selection() )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), m_ColorManager->color() ), true );
	else if( m_part && m_part->document().selection() )
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( m_ColorManager->color() ) ), true );
}

void
KarbonView::selectionChanged()
{
	int count = part()->document().selection()->objects().count();
	m_groupObjects->setEnabled( false );
	m_closePath->setEnabled( false );
	m_ungroupObjects->setEnabled( false );

	if( count > 0 )
	{
		VObject *obj = part()->document().selection()->objects().getFirst();

		if ( shell() ) {
			if ( this == shell()->rootView() || koDocument()->isEmbedded() ) {
				m_strokeFillPreview->update( *obj->stroke(), *obj->fill() );
				m_smallPreview->update( *obj->stroke(), *obj->fill() );
			}
		}
		m_strokeDocker->setStroke( *( obj->stroke() ) );

		if( count == 1 )
		{
			VGroup *group = dynamic_cast<VGroup *>( part()->document().selection()->objects().getFirst() );
			m_ungroupObjects->setEnabled( group );
			VPath *path = dynamic_cast<VPath *>( part()->document().selection()->objects().getFirst() );
			m_closePath->setEnabled( path && !path->isClosed() );
		}
		else
			m_groupObjects->setEnabled( true );

		part()->document().selection()->setStroke( *obj->stroke() );
		part()->document().selection()->setFill( *obj->fill() );
		m_setLineWidth->setEnabled( true );
		m_setLineWidth->updateValue( obj->stroke()->lineWidth() );

		VColor *c = new VColor( m_ColorManager->isStrokeDocker() ? obj->stroke()->color() : obj->fill()->color() );
		m_ColorManager->setColor( c );
		m_deleteSelectionAction->setEnabled( true );
	}
	else
	{
		if ( shell() )
			if ( this == shell()->rootView() || koDocument()->isEmbedded() )
			m_strokeFillPreview->update( *( part()->document().selection()->stroke() ),
									 *( part()->document().selection()->fill() ) );
		m_setLineWidth->setEnabled( false );
		m_deleteSelectionAction->setEnabled( false );
	}
	emit selectionChange();
}
void
KarbonView::setCursor( const QCursor &c )
{
	m_canvas->setCursor( c );
}

void
KarbonView::repaintAll( const KoRect &r )
{
	m_canvas->repaintAll( r );
}

void
KarbonView::repaintAll( bool repaint )
{
	m_canvas->repaintAll( repaint );
}
void
KarbonView::setPos( const KoPoint& p )
{
	m_canvas->setPos( p );
}

void
KarbonView::setViewportRect( const KoRect &rect )
{
	m_canvas->setViewportRect( rect );
}

void
KarbonView::setUnit( KoUnit::Unit /*_unit*/ )
{
}

void KarbonView::createDocumentTabDock()
{
    m_DocumentTab = new VDocumentTab(this, this);
    m_DocumentTab->setCaption(i18n("Document"));
    paletteManager()->addWidget(m_DocumentTab, "DocumentTabDock", "DocumentPanel");
	connect( m_part, SIGNAL( unitChanged( KoUnit::Unit ) ), m_DocumentTab, SLOT( updateDocumentInfo() ) );
}

void KarbonView::createLayersTabDock()
{
    m_LayersTab = new VLayersTab(this, this);
    m_LayersTab->setCaption(i18n("Layers"));
    paletteManager()->addWidget(m_LayersTab, "LayersTabDock", "DocumentPanel");
}

void KarbonView::createHistoryTabDock()
{
    m_HistoryTab = new VHistoryTab(part(), this);
    m_HistoryTab->setCaption(i18n("History"));
    paletteManager()->addWidget(m_HistoryTab, "HistoryTabDock", "DocumentPanel");
}

void KarbonView::createStrokeDock()
{
    m_strokeDocker = new VStrokeDocker(part(), this);
    m_strokeDocker->setCaption(i18n("Stroke Properties"));
    paletteManager()->addWidget(m_strokeDocker, "StrokeTabDock", "StrokePanel");

	connect( part(), SIGNAL( unitChanged( KoUnit::Unit ) ), m_strokeDocker, SLOT( setUnit( KoUnit::Unit ) ) );
}

void KarbonView::createColorDock()
{
    m_ColorManager = new VColorDocker(this);
    //m_ColorManager->setCaption(i18n("Stroke Properties"));
    paletteManager()->addWidget(m_ColorManager, "ColorTabDock", "ColorPanel");

    connect( m_ColorManager, SIGNAL( colorChanged() ), this, SLOT( colorDockerChanged() ) );
}

void KarbonView::createTransformDock()
{
    m_TransformDocker = new VTransformDocker(part(), this);
    m_TransformDocker->setCaption(i18n("Transform"));
    paletteManager()->addWidget(m_TransformDocker, "TransformTabDock", "TransformPanel");

    connect( this, SIGNAL( selectionChange() ), m_TransformDocker, SLOT( update() ) );
	connect( part(), SIGNAL( unitChanged( KoUnit::Unit ) ), m_TransformDocker, SLOT( setUnit( KoUnit::Unit ) ) );
}

#include "karbon_view.moc"

