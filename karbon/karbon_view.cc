/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qiconset.h>
#include <qpainter.h>
#include <qbuttongroup.h>

#include <kaction.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstdaction.h>

#include "karbon_factory.h"
#include "karbon_part.h"
#include "karbon_view.h"
#include "vctool_ellipse.h"
#include "vctool_polygon.h"
#include "vctool_rectangle.h"
#include "vctool_roundrect.h"
#include "vctool_sinus.h"
#include "vctool_spiral.h"
#include "vctool_star.h"
#include "vmtool_handle.h"
#include "vmtool_select.h"
#include "vmtool_rotate.h"
#include "vmtool_scale.h"
#include "vmtool_shear.h"
#include "vmcmd_delete.h"
#include "vtoolcontainer.h"
#include "vpainterfactory.h"
#include "vpainter.h"
#include "vmdlg_solidfill.h"
#include "vccmd_text.h"
#include "vtext.h"

#include <kdebug.h>
#include <koMainWindow.h>
#include <koFontDia.h>

#include "KArbonViewIface.h"


KarbonView::KarbonView( KarbonPart* part, QWidget* parent, const char* name )
	: KoView( part, parent, name ), m_part( part )
{
	setInstance( KarbonFactory::instance() );

	setXMLFile( QString::fromLatin1( "karbon.rc" ) );
	initActions();
	m_dcop = 0;
	dcopObject(); // build it


	m_canvas = new VCanvas( this, part );
	m_canvas->viewport()->installEventFilter( this );
	m_canvas->setGeometry( 0, 0, width(), height() );

	// initial tool is select-tool:
	selectTool();

	// set up factory
	m_painterFactory = new VPainterFactory;
	m_painterFactory->setPainter( canvasWidget()->viewport(), width(), height() );
	m_painterFactory->setEditPainter( canvasWidget()->viewport(), width(), height() );
}

KarbonView::~KarbonView()
{
	delete m_canvas;
	m_canvas = 0L;
	delete m_dcop;
	delete m_painterFactory;
}

DCOPObject* KarbonView::dcopObject()
{
    if ( !m_dcop )
	m_dcop = new KArbonViewIface( this );

    return m_dcop;
}


void
KarbonView::updateReadWrite( bool /*rw*/ )
{
}

void
KarbonView::resizeEvent( QResizeEvent* /*event*/ )
{
	m_painterFactory->painter()->resize( width(), height() );
	m_painterFactory->editpainter()->resize( width(), height() );
	m_canvas->resize( width(), height() );
}

void
KarbonView::editCut()
{
}

void
KarbonView::editCopy()
{
}

void
KarbonView::editPaste()
{
}

void
KarbonView::editSelectAll()
{
	m_part->selectAllObjects();
	//handleTool();
	if( m_part->selection().count() > 0 )
		m_part->repaintAllViews();
}

void
KarbonView::editDeselectAll()
{
	if( m_part->selection().count() > 0 )
	{
		m_part->deselectAllObjects();
		m_part->repaintAllViews();
	}
}

void
KarbonView::editDeleteSelection()
{
	m_part->addCommand( new VMCmdDelete( m_part ), true );

	selectTool();
}

void
KarbonView::editPurgeHistory()
{
// TODO: check for history size != 0
	if( KMessageBox::warningContinueCancel( this,
		i18n( "This action cannot be undone later. Do you really want to continue?" ),
		i18n( "Purge History" ),
		i18n( "C&ontinue" ),	// TODO: is there a constant for this?
		"edit_purge_history" ) )
	{
		m_part->purgeHistory();
	}
}



void
KarbonView::objectMoveToTop()
{
}

void
KarbonView::objectMoveUp()
{
}

void
KarbonView::objectMoveDown()
{
}

void
KarbonView::objectMoveToBottom()
{
}


void
KarbonView::objectTrafoTranslate()
{
}

void
KarbonView::objectTrafoScale()
{
}

void
KarbonView::objectTrafoRotate()
{
}

void
KarbonView::objectTrafoShear()
{
}



void
KarbonView::ellipseTool()
{
	s_currentTool = VCToolEllipse::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::polygonTool()
{
	s_currentTool = VCToolPolygon::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::rectangleTool()
{
	s_currentTool = VCToolRectangle::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::roundRectTool()
{
	s_currentTool = VCToolRoundRect::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::selectTool()
{
	s_currentTool = VMToolSelect::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_selectToolAction->setChecked( true );
}

void
KarbonView::rotateTool()
{
	s_currentTool = VMToolRotate::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_rotateToolAction->setChecked( true );
}

void
KarbonView::scaleTool()
{
	kdDebug() << "KarbonView::scaleTool()" << endl;
	s_currentTool = VMToolScale::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_scaleToolAction->setChecked( true );
}

void
KarbonView::textTool()
{
    kdDebug() << "KarbonView::textTool()" << endl;
	QFont f;

	KoFontDia *fontDia = new KoFontDia( this, "", f, false, false, Qt::black, Qt::white );
	fontDia->exec();
	int flags = fontDia->changedFlags();
	//kdDebug() << "KWView::formatFont changedFlags = " << flags << endl;
	if( flags )
	{
		f = fontDia->getNewFont();
		//fontDia->getSubScript(), fontDia->getSuperScript(),
		//fontDia->color(),fontDia->backGroundColor(), flags);
	}

	delete fontDia;

	// TODO : find a way to edit the text, no predefined strings
	m_part->addCommand( new VCCmdText( m_part, f, "KARBON" ) );
}

void
KarbonView::shearTool()
{
	kdDebug() << "KarbonView::shearTool()" << endl;
	s_currentTool = VMToolShear::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_shearToolAction->setChecked( true );
}

void
KarbonView::sinusTool()
{
	s_currentTool = VCToolSinus::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::spiralTool()
{
	s_currentTool = VCToolSpiral::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::starTool()
{
	s_currentTool = VCToolStar::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}


void
KarbonView::handleTool()
{
	s_currentTool = VMToolHandle::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
}

void
KarbonView::viewModeChanged()
{
	if( m_viewAction->currentItem() == 1 )
	{
		m_painterFactory->setWireframePainter( canvasWidget()->viewport(), width(), height() );
	}
	else
	{
		m_painterFactory->setPainter( canvasWidget()->viewport(), width(), height() );
	}
	m_canvas->repaintContents( true );
}

void
KarbonView::zoomChanged()
{
	bool bOK;
	double zoomFactor = m_zoomAction->currentText().toDouble( &bOK ) / 100.0;
	m_canvas->setZoomFactor( zoomFactor );
	m_canvas->repaintContents( true );
}

void
KarbonView::solidFillClicked()
{
	VMDlgSolidFill* m_solidFillDialog = new VMDlgSolidFill( m_part );
	m_solidFillDialog->show();
}

void
KarbonView::initActions()
{
	// edit ----->
	KStdAction::cut( this,
		SLOT( editCut() ), actionCollection(), "edit_cut" );
	KStdAction::copy( this,
		SLOT( editCopy() ), actionCollection(), "edit_copy");
	KStdAction::paste( this,
		SLOT( editPaste() ), actionCollection(), "edit_paste" );
  	KStdAction::selectAll( this,
		SLOT( editSelectAll() ), actionCollection(), "edit_select_all" );
	new KAction(
		i18n( "&Deselect All" ), QKeySequence( "Ctrl+D" ), this,
		SLOT( editDeselectAll() ), actionCollection(), "edit_deselect_all" );
  	new KAction(
		i18n( "D&elete" ), "editdelete", QKeySequence( "Del" ), this,
		SLOT( editDeleteSelection() ), actionCollection(), "edit_delete" );
	new KAction(
		i18n( "&History" ), 0, 0, this,
		SLOT( editPurgeHistory() ), actionCollection(), "edit_purge_history" );
	// edit <-----

	// object ----->
	new KAction(
		i18n( "Bring To &Front" ), 0, QKeySequence("Shift+PgUp"), this,
		SLOT( objectMoveToTop() ), actionCollection(), "object_move_totop" );
	new KAction(
		i18n( "&Raise" ), 0, QKeySequence("Ctrl+PgUp"), this,
		SLOT( objectMoveUp() ), actionCollection(), "object_move_up" );
	new KAction(
		i18n( "&Lower" ), 0, QKeySequence("Ctrl+PgDown"), this,
		SLOT( objectMoveDown() ), actionCollection(), "object_move_down" );
	new KAction(
		i18n( "Send To &Back" ), 0, QKeySequence("Shift+PgDown"), this,
		SLOT( objectMoveToBottom() ), actionCollection(), "object_move_tobottom" );
	new KAction(
		i18n( "&Translate" ), "translate", 0, this,
		SLOT( objectTrafoTranslate() ), actionCollection(), "object_trafo_translate" );
	new KAction(
		i18n( "&Scale" ), "scale", 0, this,
		SLOT( objectTrafoScale() ), actionCollection(), "object_trafo_scale" );
	new KAction(
		i18n( "&Rotate" ), "rotate", 0, this,
		SLOT( objectTrafoRotate() ), actionCollection(), "object_trafo_rotate" );
	new KAction(
		i18n( "S&hear" ), "shear", 0, this,
		SLOT( objectTrafoShear() ), actionCollection(), "object_trafo_shear" );
	// object <-----

	// tools ----->
	m_ellipseToolAction = new KToggleAction(
		i18n( "&Ellipse" ), "ellipse", 0, this,
		SLOT( ellipseTool() ), actionCollection(), "tool_ellipse" );
	m_polygonToolAction = new KToggleAction(
		i18n( "&Polygon" ), "polygon", 0, this,
		SLOT( polygonTool() ), actionCollection(), "tool_polygon" );
	m_rectangleToolAction = new KToggleAction(
		i18n( "&Rectangle" ), "rectangle", 0, this,
		SLOT( rectangleTool() ), actionCollection(), "tool_rectangle" );
	m_roundRectToolAction = new KToggleAction(
		i18n( "&Round Rectangle" ), "roundrect", 0, this,
		SLOT( roundRectTool() ), actionCollection(), "tool_roundrect" );
	m_sinusToolAction = new KToggleAction(
		i18n( "S&inus" ), "sinus", 0, this,
		SLOT( sinusTool() ), actionCollection(), "tool_sinus" );
	m_selectToolAction = new KToggleAction(
		i18n( "&Select Objects" ), "select", 0, this,
		SLOT( selectTool() ), actionCollection(), "tool_select" );
	m_rotateToolAction = new KToggleAction(
		i18n( "&Rotate Objects" ), "rotate", 0, this,
		SLOT( rotateTool() ), actionCollection(), "tool_rotate" );
	m_scaleToolAction = new KToggleAction(
		i18n( "&Scale Objects" ), "scale", 0, this,
		SLOT( scaleTool() ), actionCollection(), "tool_scale" );
	m_shearToolAction = new KToggleAction(
		i18n( "&Shear Objects" ), "shear", 0, this,
		SLOT( shearTool() ), actionCollection(), "tool_shear" );
	m_spiralToolAction = new KToggleAction(
		i18n( "S&piral" ), "spiral", 0, this,
		SLOT( spiralTool() ), actionCollection(), "tool_spiral" );
	m_starToolAction = new KToggleAction(
		i18n( "S&tar" ), "star", 0, this,
		SLOT( starTool() ), actionCollection(), "tool_star" );
	m_textToolAction = new KToggleAction(
		i18n( "Text" ), "text", 0, this,
		SLOT( textTool() ), actionCollection(), "tool_text" );

	m_ellipseToolAction->setExclusiveGroup( "Tools" );
	m_polygonToolAction->setExclusiveGroup( "Tools" );
	m_rectangleToolAction->setExclusiveGroup( "Tools" );
	m_roundRectToolAction->setExclusiveGroup( "Tools" );
	m_sinusToolAction->setExclusiveGroup( "Tools" );
	m_selectToolAction->setExclusiveGroup( "Tools" );
	m_rotateToolAction->setExclusiveGroup( "Tools" );
	m_scaleToolAction->setExclusiveGroup( "Tools" );
	m_spiralToolAction->setExclusiveGroup( "Tools" );
	m_starToolAction->setExclusiveGroup( "Tools" );
	m_textToolAction->setExclusiveGroup( "Tools" );
	// tools <-----

	// view ----->
	m_viewAction = new KSelectAction(
		i18n( "View &Mode" ), 0, this,
		SLOT( viewModeChanged() ), actionCollection(), "view_mode" );

	m_zoomAction = new KSelectAction(
		i18n( "&Zoom" ), 0, this,
		SLOT( zoomChanged() ), actionCollection(), "view_zoom" );

	QStringList mstl;
	mstl
		<< i18n( "Normal" )
		<< i18n( "Wireframe" );
	m_viewAction->setItems( mstl );
	m_viewAction->setCurrentItem( 0 );
	m_viewAction->setEditable( false );

	QStringList stl;
	stl
		<< i18n( "25%" )
		<< i18n( "50%" )
		<< i18n( "100%" )
		<< i18n( "200%" )
		<< i18n( "300%" )
		<< i18n( "400%" )
		<< i18n( "800%" );
	m_zoomAction->setItems( stl );
	m_zoomAction->setCurrentItem( 2 );
	m_zoomAction->setEditable( true );
	// view <-----

	m_toolbox = new VToolContainer( this );
	connect( m_toolbox, SIGNAL(selectToolActivated()),		this, SLOT(selectTool()) );
	connect( m_toolbox, SIGNAL(scaleToolActivated()),		this, SLOT(scaleTool()) );
	connect( m_toolbox, SIGNAL(rotateToolActivated()),		this, SLOT(rotateTool()) );
	connect( m_toolbox, SIGNAL(shearToolActivated()),		this, SLOT(shearTool()) );
	connect( m_toolbox, SIGNAL(ellipseToolActivated()),	this, SLOT(ellipseTool()) );
	connect( m_toolbox, SIGNAL(rectangleToolActivated()),	this, SLOT(rectangleTool()) );
	connect( m_toolbox, SIGNAL(roundRectToolActivated()),	this, SLOT(roundRectTool()) );
	connect( m_toolbox, SIGNAL(polygonToolActivated()),	this, SLOT(polygonTool()) );
	connect( m_toolbox, SIGNAL(starToolActivated()),		this, SLOT(starTool()) );
	connect( m_toolbox, SIGNAL(sinusToolActivated()),		this, SLOT(sinusTool()) );
	connect( m_toolbox, SIGNAL(spiralToolActivated()),		this, SLOT(spiralTool()) );
	connect( m_toolbox, SIGNAL(textToolActivated()),		this, SLOT(textTool()) );
	connect( m_toolbox, SIGNAL(solidFillActivated()), this, SLOT(solidFillClicked()) );
	shell()->moveDockWindow( m_toolbox, Qt::DockLeft );
	m_toolbox->show();
}

void
KarbonView::paintEverything( QPainter& /*p*/, const QRect& /*rect*/,
	bool /*transparent*/)
{
	kdDebug() << "view->paintEverything()" << endl;
}

bool
KarbonView::eventFilter( QObject* object, QEvent* event )
{
	if ( object == m_canvas->viewport() )
		return s_currentTool->eventFilter( this, event );
	else
		return false;
}

#include "karbon_view.moc"

