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
#include <koMainWindow.h>
#include <kstatusbar.h>
#include <kstdaction.h>

#include "karbon_factory.h"
#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_view_iface.h"

// tools:
#include "vellipsetool.h"
#include "vhandletool.h"
#include "vpolygontool.h"
#include "vrectangletool.h"
#include "vrotatetool.h"
#include "vroundrecttool.h"
#include "vscaletool.h"
#include "vselecttool.h"
#include "vsheartool.h"
#include "vsinustool.h"
#include "vspiraltool.h"
#include "vstartool.h"

// commands:
//#include "vtextcmd.h"
#include "vdeletecmd.h"
#include "vfillcmd.h"
#include "vgroupcmd.h"
#include "vinsertknotscmd.h"
#include "vpolygonizecmd.h"
#include "vstrokecmd.h"

// dialogs:
#include "vcolordlg.h"
#include "vconfiguredlg.h"
#include "vfilldlg.h"
#include "vinsertknotsdlg.h"
#include "vpolygonizedlg.h"
#include "vstrokedlg.h"

#include "vpainter.h"
#include "vpainterfactory.h"
#include "vqpainter.h"

//#include "vtext.h"
#include "vgroup.h"
#include "vtoolcontainer.h"

#include <kdebug.h>

// TODO: only for testing:
#include "vwhirlpinch.h"

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

	// set up factory
	m_painterFactory = new VPainterFactory;
	m_painterFactory->setPainter( canvasWidget()->viewport(), width(), height() );
	m_painterFactory->setEditPainter( canvasWidget()->viewport(), width(), height() );

	// set up status bar message
	m_status = new KStatusBarLabel( QString::null, 0, statusBar() );
	m_status->setAlignment( AlignLeft | AlignVCenter );
	m_status->setMinimumWidth( 300 );
	addStatusBarItem( m_status, 0 );

	if( shell() )
	{
		setNumberOfRecentFiles( m_part->maxRecentFiles() );
	}

	// initial tool is select-tool:
	selectTool();
	reorganizeGUI();
}

KarbonView::~KarbonView()
{
	delete m_dcop;
	delete m_painterFactory;
	delete m_canvas;
	m_canvas = 0L;
	//delete m_toolbox;
	delete m_status;
}

DCOPObject* KarbonView::dcopObject()
{
    if ( !m_dcop )
	m_dcop = new KarbonViewIface( this );

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
        reorganizeGUI();
}

void
KarbonView::setupPrinter( KPrinter& /*printer*/ )
{
}

void
KarbonView::print( KPrinter &printer )
{
	VQPainter p( (QPaintDevice *)&printer, width(), height() );
	p.begin();
	p.setZoomFactor( 1.0 );

	// print the doc using QPainter at zoom level 1
	// TODO : better use eps export?
	// TODO : use real page layout stuff
	QPtrListIterator<VLayer> i = m_part->document().layers();
	for ( ; i.current(); ++i )
		if ( i.current()->visible() )
			i.current()->draw( &p, KoRect::fromQRect( QRect( 0, 0, width(), height() ) ) );

	p.end();
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
	VObjectListIterator itr( m_part->document().selection() );
	VObjectList selection;
	for ( ; itr.current() ; ++itr )
	{
		VShape *temp = itr.current()->clone();
		temp->transform( QWMatrix().translate( VGlobal::copyOffset, VGlobal::copyOffset ) );
		selection.append( temp );
	}
	m_part->document().deselectAllObjects();
	// calc new selection
	VObjectListIterator itr2( selection );
	for ( ; itr2.current() ; ++itr2 )
	{
		m_part->insertObject( itr2.current() );
		m_part->document().selectObject( *( itr2.current() ) );
	}
	m_part->repaintAllViews();
}

void
KarbonView::editSelectAll()
{
	m_part->document().selectAllObjects();
	//handleTool();
	if( m_part->document().selection().count() > 0 )
		m_part->repaintAllViews();
}

void
KarbonView::editDeselectAll()
{
	if( m_part->document().selection().count() > 0 )
	{
		m_part->document().deselectAllObjects();
		m_part->repaintAllViews();
	}
}

void
KarbonView::editDeleteSelection()
{
	m_part->addCommand( new VDeleteCmd( &m_part->document() ), true );

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
KarbonView::selectionMoveToTop()
{
	m_part->document().moveSelectionToTop();
	m_part->repaintAllViews();
}

void
KarbonView::selectionMoveUp()
{
	m_part->document().moveSelectionUp();
	m_part->repaintAllViews();
}

void
KarbonView::selectionMoveDown()
{
	m_part->document().moveSelectionDown();
	m_part->repaintAllViews();
}

void
KarbonView::selectionMoveToBottom()
{
	m_part->document().moveSelectionToBottom();
	m_part->repaintAllViews();
}

void
KarbonView::groupSelection()
{
	m_part->addCommand( new VGroupCmd( &m_part->document() ), true );
}

void
KarbonView::ungroupSelection()
{
	if( m_part->document().selection().count() == 1 )
	{
		VGroup *grp = dynamic_cast<VGroup *>( m_part->document().selection().getFirst() );
		if( grp )
		{
			grp->ungroup();
			delete grp;
			m_part->document().deselectAllObjects();
		}
	}
}

// TODO: remove this one someday:
void
KarbonView::dummyForTesting()
{
kdDebug() << "KarbonView::dummyForTesting()" << endl;

	const KoRect& rect = m_part->document().selection().boundingBox();

	VWhirlPinch op(
		rect.center(),
		90.0,
		0.5,
		200.0 );

	VObjectListIterator itr( m_part->document().selection() );
	for ( ; itr.current() ; ++itr )
	{
		op.visit( *itr.current() );
	}

	m_part->repaintAllViews();
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
	m_status->setText( i18n( "Ellipse" ) );
	m_currentTool = VEllipseTool::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::polygonTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == VPolygonTool::instance( m_part ) )
			( (VShapeTool *) m_currentTool )->showDialog();
		else
		{
			m_currentTool = VPolygonTool::instance( m_part );
			m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
		}
	}
}

void
KarbonView::rectangleTool()
{
	m_currentTool = VRectangleTool::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
}

void
KarbonView::roundRectTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == VRoundRectTool::instance( m_part ) )
			( (VShapeTool *) m_currentTool )->showDialog();
		else
		{
			m_currentTool = VRoundRectTool::instance( m_part );
			m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
		}
	}
}

void
KarbonView::selectTool()
{
	m_status->setText( i18n( "Selection" ) );
	m_currentTool = VSelectTool::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_selectToolAction->setChecked( true );
}

void
KarbonView::rotateTool()
{
	m_currentTool = VRotateTool::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_rotateToolAction->setChecked( true );
}

void
KarbonView::scaleTool()
{
	kdDebug() << "KarbonView::scaleTool()" << endl;
	m_currentTool = VScaleTool::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_scaleToolAction->setChecked( true );
}

void
KarbonView::textTool()
{
    /*kdDebug() << "KarbonView::textTool()" << endl;
	QFont f;

	f.setFamily( m_setFontFamily->font() );
	f.setPointSize( m_setFontSize->fontSize() );
	f.setBold( m_setFontBold->isChecked() );
	f.setItalic( m_setFontItalic->isChecked() );

	// TODO : find a way to edit the text, no predefined strings
	m_part->addCommand( new VTextCmd( m_part, f, "KARBON" ), true );*/
}

void
KarbonView::shearTool()
{
	kdDebug() << "KarbonView::shearTool()" << endl;
	m_currentTool = VShearTool::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
	m_shearToolAction->setChecked( true );
}

void
KarbonView::sinusTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == VSinusTool::instance( m_part ) )
			( (VShapeTool *) m_currentTool )->showDialog();
		else
		{
			m_currentTool = VSinusTool::instance( m_part );
			m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
		}
	}
}

void
KarbonView::spiralTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == VSpiralTool::instance( m_part ) )
			( (VShapeTool *) m_currentTool )->showDialog();
		else
		{
			m_currentTool = VSpiralTool::instance( m_part );
			m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
		}
	}
}

void
KarbonView::starTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == VStarTool::instance( m_part ) )
			( (VShapeTool *) m_currentTool )->showDialog();
		else
		{
			m_currentTool = VStarTool::instance( m_part );
			m_canvas->viewport()->setCursor( QCursor( crossCursor ) );
		}
	}
}


void
KarbonView::handleTool()
{
	m_currentTool = VHandleTool::instance( m_part );
	m_canvas->viewport()->setCursor( QCursor( arrowCursor ) );
}


void
KarbonView::pathInsertKnots()
{
	VInsertKnotsDlg* dialog = new VInsertKnotsDlg();

	if( dialog->exec() )
		m_part->addCommand( new VInsertKnotsCmd( &m_part->document(), dialog->knots() ), true );

	delete( dialog );
}

void
KarbonView::pathPolygonize()
{
	VPolygonizeDlg* dialog = new VPolygonizeDlg();
	dialog->setFlatness( 5.0 );

	if( dialog->exec() )
		m_part->addCommand( new VPolygonizeCmd( &m_part->document(), dialog->flatness() ), true );

	delete( dialog );
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
	m_canvas->repaintAll();
}

void
KarbonView::zoomChanged()
{
	bool bOK;
	double zoomFactor = m_zoomAction->currentText().toDouble( &bOK ) / 100.0;
	setZoom( zoomFactor );
	// TODO : I guess we should define a document size member at this point...
	m_canvas->resizeContents( int( 800 * zoomFactor ), int( 600 * zoomFactor ) );
	m_canvas->repaintAll();
	m_canvas->setFocus();
}

void
KarbonView::solidFillClicked()
{
	if( shell()->rootView() == this )
	{
		VFillDlg* dialog = new VFillDlg( m_part );
		dialog->exec();
                delete dialog;
	}
}

void
KarbonView::strokeClicked()
{
	if( shell()->rootView() == this )
	{
		VStrokeDlg* dialog = new VStrokeDlg( m_part );
		dialog->exec();
                delete dialog;
	}
}

void
KarbonView::slotStrokeColorChanged( const QColor &c )
{
	kdDebug() << "In KarbonView::slotStrokeColorChanged" << endl;
	VColor color;
	float r = float( c.red() ) / 255.0, g = float( c.green() ) / 255.0, b = float( c.blue() ) / 255.0;

	color.setValues( &r, &g, &b, 0L );
	m_part->document().setDefaultStrokeColor( color );

	m_part->addCommand( new VStrokeCmd( &m_part->document(), color ), true );
}

void
KarbonView::slotFillColorChanged( const QColor &c )
{
	kdDebug() << "In KarbonView::slotFillColorChanged" << endl;
	VColor color;
	float r = float( c.red() ) / 255.0, g = float( c.green() ) / 255.0, b = float( c.blue() ) / 255.0;

	color.setValues( &r, &g, &b, 0L );
	m_part->document().setDefaultFillColor( color );

	m_part->addCommand( new VFillCmd( &m_part->document(), color ), true );
}

void
KarbonView::viewColorManager()
{
	VColorDlg dialog( this );
	dialog.show();
}

void
KarbonView::refreshView()
{
	m_canvas->repaintAll();
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
		i18n( "Bring to &Front" ), 0, QKeySequence("Shift+PgUp"), this,
		SLOT( selectionMoveToTop() ), actionCollection(), "object_move_totop" );
	new KAction(
		i18n( "&Raise" ), 0, QKeySequence("Ctrl+PgUp"), this,
		SLOT( selectionMoveUp() ), actionCollection(), "object_move_up" );
	new KAction(
		i18n( "&Lower" ), 0, QKeySequence("Ctrl+PgDown"), this,
		SLOT( selectionMoveDown() ), actionCollection(), "object_move_down" );
	new KAction(
		i18n( "Send to &Back" ), 0, QKeySequence("Shift+PgDown"), this,
		SLOT( selectionMoveToBottom() ), actionCollection(), "object_move_tobottom" );
	new KAction(
		i18n( "Group Objects" ), 0, QKeySequence("Shift+G"), this,
		SLOT( groupSelection() ), actionCollection(), "selection_group" );
	new KAction(
		i18n( "Ungroup Objects" ), 0, QKeySequence("Shift+U"), this,
		SLOT( ungroupSelection() ), actionCollection(), "selection_ungroup" );
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
		i18n( "&Ellipse" ), "14_ellipse", 0, this,
		SLOT( ellipseTool() ), actionCollection(), "tool_ellipse" );
	m_polygonToolAction = new KToggleAction(
		i18n( "&Polygon" ), "14_polygon", 0, this,
		SLOT( polygonTool() ), actionCollection(), "tool_polygon" );
	m_rectangleToolAction = new KToggleAction(
		i18n( "&Rectangle" ), "14_rectangle", 0, this,
		SLOT( rectangleTool() ), actionCollection(), "tool_rectangle" );
	m_roundRectToolAction = new KToggleAction(
		i18n( "&Round Rectangle" ), "14_roundrect", 0, this,
		SLOT( roundRectTool() ), actionCollection(), "tool_roundrect" );
	m_sinusToolAction = new KToggleAction(
		i18n( "S&inus" ), "14_sinus", 0, this,
		SLOT( sinusTool() ), actionCollection(), "tool_sinus" );
	m_selectToolAction = new KToggleAction(
		i18n( "&Select Objects" ), "select", 0, this,
		SLOT( selectTool() ), actionCollection(), "tool_select" );
	m_rotateToolAction = new KToggleAction(
		i18n( "&Rotate Objects" ), "14_rotate", 0, this,
		SLOT( rotateTool() ), actionCollection(), "tool_rotate" );
	m_scaleToolAction = new KToggleAction(
		i18n( "&Scale Objects" ), "14_scale", 0, this,
		SLOT( scaleTool() ), actionCollection(), "tool_scale" );
	m_shearToolAction = new KToggleAction(
		i18n( "&Shear Objects" ), "14_shear", 0, this,
		SLOT( shearTool() ), actionCollection(), "tool_shear" );
	m_spiralToolAction = new KToggleAction(
		i18n( "S&piral" ), "14_spiral", 0, this,
		SLOT( spiralTool() ), actionCollection(), "tool_spiral" );
	m_starToolAction = new KToggleAction(
		i18n( "S&tar" ), "14_star", 0, this,
		SLOT( starTool() ), actionCollection(), "tool_star" );
	m_textToolAction = new KToggleAction(
		i18n( "Text" ), "14_text", 0, this,
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

	// text ----->
	m_setFontFamily = new KFontAction(
		i18n( "Set Font Family" ), 0, actionCollection(), "setFontFamily" );
	m_setFontFamily->setCurrentItem( 0 );

	//connect( m_setFontFamily,
	//	SIGNAL( activated( const QString& ) ),
	//	SLOT( setFontFamily( const QString& ) ) );

	m_setFontSize = new KFontSizeAction(
		i18n( "Set Font Size" ), 0, actionCollection(), "setFontSize" );
	m_setFontSize->setCurrentItem( 0 );

	//connect( m_setFontSize,
	//	SIGNAL( activated( const QString& ) ),
	//	SLOT( setFontSize( const QString& ) ) );

	m_setFontItalic = new KToggleAction(
		i18n( "&Italic" ), "text_italic", 0, actionCollection(), "setFontItalic" );
	m_setFontBold = new KToggleAction(
		i18n( "&Bold" ), "text_bold", 0, actionCollection(), "setFontBold" );

	//m_setTextColor = new TKSelectColorAction(
	//	i18n("Set Text Color"), TKSelectColorAction::TextColor,
	//	actionCollection(), "setTextColor" );
	//connect( m_setTextColor, SIGNAL( activated() ), SLOT( setTextColor() ) );
	// text <-----

	// path ----->
	new KAction(
		i18n( "&Insert Knots" ), 0, 0, this,
		SLOT( pathInsertKnots() ), actionCollection(), "path_insert_knots" );
	new KAction(
		i18n( "&Polygonize" ), 0, 0, this,
		SLOT( pathPolygonize() ), actionCollection(), "path_polygonize" );
	// path <-----

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
	m_zoomAction->setEditable( true );
	m_zoomAction->setCurrentItem( 2 );

	new KAction(
		i18n( "&Color Manager" ), "colorman", 0, this,
		SLOT( viewColorManager() ), actionCollection(), "view_color_manager" );

	new KAction(
		i18n( "&Refresh" ), 0, QKeySequence("Ctrl+W"), this,
		SLOT( refreshView() ), actionCollection(), "view_refresh" );
	// view <-----

	// toolbox ---->
	m_toolbox = VToolContainer::instance( m_part, this );

	connect(
		m_toolbox, SIGNAL( selectToolActivated() ),
		this, SLOT( selectTool() ) );
	connect(
		m_toolbox, SIGNAL( scaleToolActivated() ),
		this, SLOT( scaleTool() ) );
	connect(
		m_toolbox, SIGNAL( rotateToolActivated() ),
		this, SLOT( rotateTool() ) );
	connect(
		m_toolbox, SIGNAL( shearToolActivated() ),
		this, SLOT( shearTool() ) );
	connect(
		m_toolbox, SIGNAL( ellipseToolActivated() ),
		this, SLOT( ellipseTool() ) );
	connect(
		m_toolbox, SIGNAL( rectangleToolActivated() ),
		this, SLOT( rectangleTool() ) );
	connect(
		m_toolbox, SIGNAL( roundRectToolActivated() ),
		this, SLOT( roundRectTool() ) );
	connect(
		m_toolbox, SIGNAL( polygonToolActivated() ),
		this, SLOT( polygonTool() ) );
	connect(
		m_toolbox, SIGNAL( starToolActivated() ),
		this, SLOT( starTool() ) );
	connect(
		m_toolbox, SIGNAL( sinusToolActivated() ),
		this, SLOT( sinusTool() ) );
	connect(
		m_toolbox, SIGNAL( spiralToolActivated() ),
		this, SLOT( spiralTool() ) );
	connect(
		m_toolbox, SIGNAL( textToolActivated() ),
		this, SLOT( textTool() ) );
	connect(
		m_toolbox, SIGNAL( solidFillActivated() ),
		this, SLOT( solidFillClicked() ) );
	connect(
		m_toolbox, SIGNAL( strokeActivated() ),
		this, SLOT( strokeClicked() ) );
	connect(
		m_toolbox, SIGNAL( strokeColorChanged( const QColor & ) ),
		this, SLOT( slotStrokeColorChanged( const QColor & ) ) );
	connect( m_toolbox, SIGNAL( fillColorChanged( const QColor & ) ),
		this, SLOT( slotFillColorChanged( const QColor & ) ) );

	shell()->moveDockWindow( m_toolbox, Qt::DockLeft );
	m_toolbox->show();

	m_configureAction = new KAction(
		i18n( "Configure Karbon..." ), "configure", 0, this,
		SLOT( configure() ), actionCollection(), "configure" );
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
	if( object == m_canvas->viewport() )
	{
		m_currentTool->m_part = m_part;
		return m_currentTool->eventFilter( this, event );
	}
	else
		return false;
}

void KarbonView::reorganizeGUI()
{
	if( statusBar())
	{
		if( m_part->showStatusBar() )
			statusBar()->show();
		else
			statusBar()->hide();
	}
}

void KarbonView::setNumberOfRecentFiles( int number )
{
	if( shell() ) // 0 when embedded into konq !
		shell()->setMaxRecentItems( number );
}

void KarbonView::configure()
{
	VConfigureDlg dialog( this );
	dialog.exec();
}

#include "karbon_view.moc"

