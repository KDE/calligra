/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qbuttongroup.h>
#include <qdragobject.h>
#include <qiconset.h>
#include <qpainter.h>

#include <kaction.h>
#include <kcolordrag.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <koMainWindow.h>
#include <kstatusbar.h>
#include <kstdaction.h>

// tools:
#include "vellipsetool.h"
#include "vgradienttool.h"
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
#include "vdeletecmd.h"
#include "vfillcmd.h"
#include "vflattencmd.h"
#include "vgroupcmd.h"
#include "vinsertknotscmd.h"
#include "vroundcornerscmd.h"
#include "vstrokecmd.h"
//#include "vtextcmd.h"
#include "vwhirlpinchcmd.h"

// dialogs:
#include "vcolordlg.h"
#include "vconfiguredlg.h"
#include "vfilldlg.h"
#include "vflattendlg.h"
#include "vinsertknotsdlg.h"
#include "vroundcornersdlg.h"
#include "vstrokedlg.h"
#include "vwhirlpinchdlg.h"

#include "karbon_factory.h"
#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_view_iface.h"
#include "vgroup.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vqpainter.h"
//#include "vtext.h"
#include "vselection.h"
#include "vtoolcontainer.h"
#include "vstrokefillpreview.h"

#include <tkfloatspinboxaction.h>

#include <kdebug.h>

// TODO: only for testing:
#include "vwhirlpinch.h"


KarbonView::KarbonView( KarbonPart* part, QWidget* parent, const char* name )
	: KoView( part, parent, name ), m_part( part )
{
	setInstance( KarbonFactory::instance() );
	setAcceptDrops( true );

	setXMLFile( QString::fromLatin1( "karbon.rc" ) );
	initActions();
	m_dcop = 0;
	dcopObject(); // build it


	// dialogs:
	m_insertKnotsDlg = new VInsertKnotsDlg();

	m_flattenDlg = new VFlattenDlg();
	m_flattenDlg->setFlatness( 5.0 );

	m_roundCornersDlg = new VRoundCornersDlg();
	m_roundCornersDlg->setRadius( 10.0 );

	m_whirlPinchDlg = new VWhirlPinchDlg();
	m_whirlPinchDlg->setAngle( 20.0 );
	m_whirlPinchDlg->setPinch( 0.0 );
	m_whirlPinchDlg->setRadius( 100.0 );

	
	// widgets:
	m_canvas = new VCanvas( this, part );
	m_canvas->viewport()->installEventFilter( this );
	m_canvas->setGeometry( 0, 0, width(), height() );

	// tools:
	m_ellipseTool = new VEllipseTool( this );
	m_gradTool = new VGradientTool( this );
	m_polygonTool = new VPolygonTool( this );
	m_rectangleTool = new VRectangleTool( this );
	m_rotateTool = new VRotateTool( this );
	m_roundRectTool = new VRoundRectTool( this );
	m_scaleTool = new VScaleTool( this );
	m_selectTool = new VSelectTool( this );
	m_shearTool = new VShearTool( this );
	m_sinusTool = new VSinusTool( this );
	m_spiralTool = new VSpiralTool( this );
	m_starTool = new VStarTool( this );


	// set up factory
	m_painterFactory = new VPainterFactory;
	m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );
	m_painterFactory->setEditPainter( canvasWidget()->viewport(), width(), height() );

	// set up status bar message
	m_status = new KStatusBarLabel( QString::null, 0, statusBar() );
	m_status->setAlignment( AlignLeft | AlignVCenter );
	m_status->setMinimumWidth( 300 );
	addStatusBarItem( m_status, 0 );

	//Create Color Manager
	m_ColorManager = new VColorDlg( m_part, this );

	if( shell() )
	{
		setNumberOfRecentFiles( m_part->maxRecentFiles() );
	}

	// initial tool is select-tool:
	m_currentTool = m_selectTool;
	selectTool();
	reorganizeGUI();
}

KarbonView::~KarbonView()
{
	// dialogs:
	delete( m_insertKnotsDlg );
	delete( m_flattenDlg );
	delete( m_roundCornersDlg );
	delete( m_whirlPinchDlg );

	// tools:
	delete( m_ellipseTool );
	delete( m_gradTool );
	delete( m_polygonTool );
	delete( m_rectangleTool );
	delete( m_rotateTool );
	delete( m_roundRectTool );
	delete( m_scaleTool );
	delete( m_selectTool );
	delete( m_shearTool );
	delete( m_sinusTool );
	delete( m_spiralTool );
	delete( m_starTool );

	// widgets:
	//delete m_toolbox;
//	delete m_status;
	delete m_painterFactory;
//	delete m_canvas;
//	m_canvas = 0L;

	//delete m_ColorManager;
	delete m_dcop;
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
KarbonView::dragEnterEvent( QDragEnterEvent *event )
{
	event->accept( KColorDrag::canDecode( event ));
}

void
KarbonView::dropEvent ( QDropEvent *e )
{
	//Accepts QColor - from Color Manager's KColorPatch
	QColor color;
	VColor realcolor;

	if ( KColorDrag::decode( e, color) )
	{
		float r = color.red() / 255.0, g = color.green() / 255.0, b = color.blue() / 255.0;
		realcolor.setValues( &r, &g, &b, 0L );
		if( m_part )
			m_part->addCommand( new VFillCmd( &m_part->document(), realcolor ), true );
		selectionChanged();
	}
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
		//if ( i.current()->visible() )
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
	VObjectListIterator itr( m_part->document().selection()->objects() );
	VObjectList selection;
	for ( ; itr.current() ; ++itr )
	{
		VObject *temp = itr.current()->clone();
		temp->transform( QWMatrix().translate( VGlobal::copyOffset, VGlobal::copyOffset ) );
		selection.append( temp );
	}
	m_part->document().deselect();
	// calc new selection
	VObjectListIterator itr2( selection );
	for ( ; itr2.current() ; ++itr2 )
	{
		m_part->insertObject( itr2.current() );
		m_part->document().select( *( itr2.current() ) );
	}
	m_part->repaintAllViews();
}

void
KarbonView::editSelectAll()
{
	m_part->document().select();

	if( m_part->document().selection()->objects().count() > 0 )
		m_part->repaintAllViews();
}

void
KarbonView::editDeselectAll()
{
	if( m_part->document().selection()->objects().count() > 0 )
	{
		m_part->document().deselect();
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
/*
	if( m_part->document().selection()->objects().count() == 1 )
	{
		VGroup *grp = dynamic_cast<VGroup *>( m_part->document().selection().objects().getFirst() );
		if( grp )
		{
			grp->ungroup();
			delete grp;
			m_part->document().deselect();
		}
	}
*/
}

// TODO: remove this one someday:
void
KarbonView::dummyForTesting()
{
kdDebug() << "KarbonView::dummyForTesting()" << endl;

	const KoRect& rect = m_part->document().selection()->boundingBox();

	VWhirlPinch op(
		rect.center(),
		90.0,
		0.5,
		200.0 );

	VObjectListIterator itr( m_part->document().selection()->objects() );
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
	m_currentTool->deactivate();
	m_currentTool = m_ellipseTool;
	m_currentTool->activate();
}

void
KarbonView::polygonTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == m_polygonTool )
			m_polygonTool->showDialog();
		else
		{
			m_currentTool->deactivate();
			m_currentTool = m_polygonTool;
			m_currentTool->activate();
		}
	}
}

void
KarbonView::rectangleTool()
{
	m_currentTool->deactivate();
	m_currentTool = m_rectangleTool;
	m_currentTool->activate();
}

void
KarbonView::roundRectTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == m_roundRectTool )
			m_roundRectTool->showDialog();
		else
		{
			m_currentTool->deactivate();
			m_currentTool = m_roundRectTool;
			m_currentTool->activate();
		}
	}
}

void
KarbonView::selectTool()
{
	m_currentTool->deactivate();
	m_currentTool = m_selectTool;
	m_currentTool->activate();
	m_selectToolAction->setChecked( true );
}

void
KarbonView::rotateTool()
{
	m_currentTool->deactivate();
	m_currentTool = m_rotateTool;
	m_currentTool->activate();
	m_rotateToolAction->setChecked( true );
}

void
KarbonView::scaleTool()
{
	kdDebug() << "KarbonView::scaleTool()" << endl;
	m_currentTool->deactivate();
	m_currentTool = m_scaleTool;
	m_currentTool->activate();
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
	m_currentTool->deactivate();
	m_currentTool = m_shearTool;
	m_currentTool->activate();
	m_shearToolAction->setChecked( true );
}

void
KarbonView::sinusTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == m_sinusTool )
			m_sinusTool->showDialog();
		else
		{
			m_currentTool->deactivate();
			m_currentTool = m_sinusTool;
			m_currentTool->activate();
		}
	}
}

void
KarbonView::spiralTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == m_spiralTool )
			m_spiralTool->showDialog();
		else
		{
			m_currentTool->deactivate();
			m_currentTool = m_spiralTool;
			m_currentTool->activate();
		}
	}
}

void
KarbonView::starTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == m_starTool )
			m_starTool->showDialog();
		else
		{
			m_currentTool->deactivate();
			m_currentTool = m_starTool;
			m_currentTool->activate();
		}
	}
}

void
KarbonView::gradTool()
{
	if( shell()->rootView() == this )
	{
		if( m_currentTool == m_gradTool )
			m_gradTool->showDialog();
		else
		{
			m_currentTool->deactivate();
			m_currentTool = m_gradTool;
			m_currentTool->activate();
		}
	}
}


void
KarbonView::pathInsertKnots()
{
	if( m_insertKnotsDlg->exec() )
	{
		m_part->addCommand( new VInsertKnotsCmd(
			&m_part->document(), m_insertKnotsDlg->knots() ), true );
	}
}

void
KarbonView::pathFlatten()
{
	if( m_flattenDlg->exec() )
	{
		m_part->addCommand( new VFlattenCmd(
			&m_part->document(), m_flattenDlg->flatness() ), true );
	}
}

void
KarbonView::pathRoundCorners()
{
	if( m_roundCornersDlg->exec() )
	{
		m_part->addCommand( new VRoundCornersCmd(
			&m_part->document(), m_roundCornersDlg->radius() ), true );
	}
}

void
KarbonView::pathWhirlPinch()
{
	if( m_whirlPinchDlg->exec() )
	{
		m_part->addCommand( new VWhirlPinchCmd(
			&m_part->document(),
			m_whirlPinchDlg->angle(),
			m_whirlPinchDlg->pinch(),
			m_whirlPinchDlg->radius() ), true );
	}
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
		m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );
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
		connect(dialog, SIGNAL( fillChanged() ), this, SLOT( selectionChanged() ) );
		dialog->exec();
		delete dialog;
		disconnect(dialog, SIGNAL( fillChanged() ), this, SLOT( selectionChanged() ) );
	}
}

void
KarbonView::strokeClicked()
{
	if( shell()->rootView() == this )
	{
		VStrokeDlg* dialog = new VStrokeDlg( m_part );
		connect(dialog, SIGNAL( strokeChanged() ), this, SLOT( selectionChanged() ) );
		dialog->exec();
		delete dialog;
		disconnect(dialog, SIGNAL( strokeChanged() ), this, SLOT( selectionChanged() ) );
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

	m_toolbox->strokeFillPreview()->update( m_part->document().defaultStroke(), m_part->document().defaultFill() );
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

	m_toolbox->strokeFillPreview()->update( m_part->document().defaultStroke(), m_part->document().defaultFill() );
}

void
KarbonView::setLineWidth()
{
    setLineWidth( m_setLineWidth->value() );
}

//necessary for dcop call !
void
KarbonView::setLineWidth( double val)
{
	// TODO : maybe this can become a command
	VObjectListIterator itr( m_part->document().selection()->objects() );
	for ( ; itr.current() ; ++itr )
	{
		VStroke stroke( *( itr.current()->stroke() ) );
		stroke.setParent( itr.current() );
		stroke.setLineWidth( val );
		itr.current()->setStroke( stroke );
	}

	m_part->repaintAllViews();
}



void
KarbonView::viewColorManager()
{
	m_ColorManager->show();
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
		i18n( "&Group Objects" ), 0, QKeySequence("Ctrl+G"), this,
		SLOT( groupSelection() ), actionCollection(), "selection_group" );
	new KAction(
		i18n( "&Ungroup Objects" ), 0, QKeySequence("Ctrl+U"), this,
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
	m_gradToolAction = new KToggleAction(
		i18n( "G&radient" ), "14_star", 0, this,
		SLOT( gradTool() ), actionCollection(), "tool_grad" );
	/*m_textToolAction = new KToggleAction(
		i18n( "Text" ), "14_text", 0, this,
		SLOT( textTool() ), actionCollection(), "tool_text" );*/

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
	m_gradToolAction->setExclusiveGroup( "Tools" );
	//m_textToolAction->setExclusiveGroup( "Tools" );
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
		i18n( "&Flatten" ), 0, 0, this,
		SLOT( pathFlatten() ), actionCollection(), "path_flatten" );
	new KAction(
		i18n( "&Round Corners" ), 0, 0, this,
		SLOT( pathRoundCorners() ), actionCollection(), "path_round_corners" );
	new KAction(
		i18n( "&Whirl/Pinch" ), 0, 0, this,
		SLOT( pathWhirlPinch() ), actionCollection(), "path_whirlpinch" );
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

	// line width

	m_setLineWidth = new TKUFloatSpinBoxAction( i18n("Set Line Width"), "linewidth", 0, actionCollection(), "setLineWidth" );
	m_setLineWidth->setIconMode(TK::IconOnly);
	m_setLineWidth->setDecimals(1);
	m_setLineWidth->setMinValue(0.0);
	m_setLineWidth->setLineStep(0.5);
	connect( m_setLineWidth, SIGNAL(activated()), this, SLOT(setLineWidth()) );
	//connect( m_pDoc, SIGNAL(unitsChanged(int)), m_setLineWidth, SLOT(setUnit(int)) );

	// toolbox ---->
	m_toolbox = VToolContainer::instance( m_part, this );
	m_toolbox->strokeFillPreview()->update( part()->document().defaultStroke(), part()->document().defaultFill() );

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
		m_toolbox, SIGNAL( gradToolActivated() ),
		this, SLOT( gradTool() ) );
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
		return m_currentTool->eventFilter( event );
	else
		return false;
}

void
KarbonView::reorganizeGUI()
{
	if( statusBar() )
	{
		if( m_part->showStatusBar() )
			statusBar()->show();
		else
			statusBar()->hide();
	}
}

void
KarbonView::setNumberOfRecentFiles( int number )
{
	if( shell() ) // 0 when embedded into konq !
		shell()->setMaxRecentItems( number );
}

void
KarbonView::configure()
{
	VConfigureDlg dialog( this );
	dialog.exec();
}

void
KarbonView::selectionChanged()
{
	if( part()->document().selection()->objects().count() > 0)
	{
		m_toolbox->strokeFillPreview()->update( *part()->document().selection()->objects().getFirst()->stroke(),
												*part()->document().selection()->objects().getFirst()->fill() );
		m_setLineWidth->setValue( part()->document().selection()->objects().getFirst()->stroke()->lineWidth() );
	}
}

#include "karbon_view.moc"

