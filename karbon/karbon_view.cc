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

#include <qbuttongroup.h>
#include <qdockarea.h>
#include <qdragobject.h>
#include <qiconset.h>
#include <qpainter.h>
#include <qtoolbutton.h>

#include <kaction.h>
#include <kcolordrag.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <koMainWindow.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kiconloader.h>

// tools:
#include "vellipsetool.h"
#include "vgradienttool.h"
#include "vpolygontool.h"
#include "vrectangletool.h"
#include "vrotatetool.h"
#include "vroundrecttool.h"
#include "vselection.h"
#include "vselectnodestool.h"
#include "vselecttool.h"
#include "vsheartool.h"
#include "vsinustool.h"
#include "vspiraltool.h"
#include "vstartool.h"
#include "vpolylinetool.h"

// commands:
#include "vdeletecmd.h"
#include "vfillcmd.h"
#include "vflattencmd.h"
#include "vgroupcmd.h"
#include "vungroupcmd.h"
#include "vzordercmd.h"
#include "vinsertknotscmd.h"
#include "vroundcornerscmd.h"
#include "vstrokecmd.h"
//#include "vtextcmd.h"
#include "vwhirlpinchcmd.h"

// dialogs:
#include "vconfiguredlg.h"
#include "vfilldlg.h"
#include "vflattendlg.h"
#include "vinsertknotsdlg.h"
#include "vroundcornersdlg.h"
#include "vstrokedlg.h"
#include "vtransformdlg.h"
#include "vwhirlpinchdlg.h"

//dockers
#include "vcolordocker.h"
#include "vtooldocker.h"

#include "karbon_factory.h"
#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_view_iface.h"
#include "vgroup.h"
#include "vpainterfactory.h"
#include "vqpainter.h"
//#include "vtext.h"
#include "vstrokefillpreview.h"
#include "vstatebutton.h"
#include <kdeversion.h>

#include <tkfloatspinboxaction.h>

#include <kdebug.h>


// TODO: only for testing
#include "vsegment.h"
#include "vpath.h"


KarbonView::KarbonView( KarbonPart* part, QWidget* parent, const char* name )
	: KoView( part, parent, name ), m_part( part )
{
	setInstance( KarbonFactory::instance() );
	setAcceptDrops( true );

	if( !part->isReadWrite() )
		setXMLFile( QString::fromLatin1( "karbon_readonly.rc" ) );
	else
		setXMLFile( QString::fromLatin1( "karbon.rc" ) );

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

	// tools:
	m_ellipseTool = new VEllipseTool( this );
	m_gradTool = new VGradientTool( this );
	m_polygonTool = new VPolygonTool( this );
	m_rectangleTool = new VRectangleTool( this );
	m_rotateTool = new VRotateTool( this );
	m_roundRectTool = new VRoundRectTool( this );
	m_selectTool = new VSelectTool( this );
	m_selectNodesTool = new VSelectNodesTool( this );
	m_shearTool = new VShearTool( this );
	m_sinusTool = new VSinusTool( this );
	m_spiralTool = new VSpiralTool( this );
	m_starTool = new VStarTool( this );
	m_polylineTool = new VPolylineTool( this );

	// set up status bar message
	m_status = new KStatusBarLabel( QString::null, 0, statusBar() );
	m_status->setAlignment( AlignLeft | AlignVCenter );
	m_status->setMinimumWidth( 300 );
	addStatusBarItem( m_status, 0 );

	initActions();

	m_strokeFillPreview = 0L;
	m_ColorManager = 0L;
	VToolDocker *_toolContainer = m_part->toolContainer();
	if( shell() )
	{
		if( !_toolContainer )
		{
			_toolContainer = new VToolDocker( m_part, this );
			mainWindow()->addDockWindow( _toolContainer, DockLeft );
			m_part->setToolContainer( _toolContainer );
		}
		connect( _toolContainer, SIGNAL( selectToolActivated() ),		this, SLOT( selectTool() ) );
		connect( _toolContainer, SIGNAL( selectNodesToolActivated() ),	this, SLOT( selectNodesTool() ) );
		connect( _toolContainer, SIGNAL( rotateToolActivated() ),		this, SLOT( rotateTool() ) );
		connect( _toolContainer, SIGNAL( shearToolActivated() ),		this, SLOT( shearTool() ) );
		connect( _toolContainer, SIGNAL( rectangleToolActivated() ),	this, SLOT( rectangleTool() ) );
		connect( _toolContainer, SIGNAL( roundRectToolActivated() ),	this, SLOT( roundRectTool() ) );
		connect( _toolContainer, SIGNAL( ellipseToolActivated() ),		this, SLOT( ellipseTool() ) );
   		connect( _toolContainer, SIGNAL( polygonToolActivated() ),		this, SLOT( polygonTool() ) );
		connect( _toolContainer, SIGNAL( starToolActivated() ),			this, SLOT( starTool() ) );
		connect( _toolContainer, SIGNAL( sinusToolActivated() ),		this, SLOT( sinusTool() ) );
		connect( _toolContainer, SIGNAL( spiralToolActivated() ),		this, SLOT( spiralTool() ) );
		connect( _toolContainer, SIGNAL( gradToolActivated() ),			this, SLOT( gradTool() ) );
		connect( _toolContainer, SIGNAL( polylineToolActivated() ),		this, SLOT( polylineTool() ) );
		m_part->toolContainer()->show();
		m_strokeFillPreview = _toolContainer->strokeFillPreview();
		connect( m_strokeFillPreview, SIGNAL( strokeChanged( const VStroke & ) ), this, SLOT( selectionChanged() ) );
		connect( m_strokeFillPreview, SIGNAL( fillChanged( const VFill & ) ), this, SLOT( selectionChanged() ) );

		//Create Dockers
		m_ColorManager = new VColorDocker( m_part, this );
		connect( m_strokeFillPreview, SIGNAL( strokeSelected() ), m_ColorManager, SLOT( setStrokeDocker() ) );
		connect( m_strokeFillPreview, SIGNAL( fillSelected( ) ), m_ColorManager, SLOT( setFillDocker() ) );
	}

	m_TransformDlg = new VTransformDlg( m_part, this );

	setNumberOfRecentFiles( m_part->maxRecentFiles() );

	// initial tool is select-tool:
	m_currentTool = m_selectTool;
	reorganizeGUI();

	// widgets:
	m_canvas = new VCanvas( this, part );
	m_canvas->viewport()->installEventFilter( this );
	m_canvas->setGeometry( 0, 0, width(), height() );

	// set up factory
	m_painterFactory = new VPainterFactory;
	m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );
	m_painterFactory->setEditPainter( canvasWidget()->viewport(), width(), height() );

	selectTool();
	zoomChanged();
	if( shell() ) selectionChanged();
}

KarbonView::~KarbonView()
{
	// dialogs:
	delete( m_insertKnotsDlg );
	delete( m_flattenDlg );
	delete( m_roundCornersDlg );
	delete( m_whirlPinchDlg );
	delete( m_ColorManager );
	delete( m_TransformDlg );

	// tools:
	delete( m_ellipseTool );
	delete( m_gradTool );
	delete( m_polygonTool );
	delete( m_rectangleTool );
	delete( m_rotateTool );
	delete( m_roundRectTool );
	delete( m_selectTool );
	delete( m_shearTool );
	delete( m_sinusTool );
	delete( m_spiralTool );
	delete( m_starTool );
	delete( m_polylineTool );

	// widgets:
	delete( m_status );
	delete( m_painterFactory );
	delete( m_canvas );
	delete( m_dcop );
}

DCOPObject* KarbonView::dcopObject()
{
	if( !m_dcop )
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
	kdDebug() << "Moving to : " << m_canvas->contentsHeight() - height() << endl;
	m_canvas->setContentsPos( 0, m_canvas->contentsHeight() - height() );
	reorganizeGUI();
}

void
KarbonView::dragEnterEvent( QDragEnterEvent *event )
{
	event->accept( KColorDrag::canDecode( event ) );
}

void
KarbonView::dropEvent ( QDropEvent *e )
{
	//Accepts QColor - from Color Manager's KColorPatch
	QColor color;
	VColor realcolor;

	if( KColorDrag::decode( e, color) )
	{
		float r = color.red() / 255.0;
		float g = color.green() / 255.0;
		float b = color.blue() / 255.0;

		realcolor.set( r, g, b );

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
	KoRect rect( 0, 0, width(), height() );
	for( ; i.current(); ++i )
		//if( i.current()->visible() )
			i.current()->draw( &p, &rect );

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
	for( ; itr.current() ; ++itr )
	{
		VObject *temp = itr.current()->clone();
		temp->transform( QWMatrix().translate( VGlobal::copyOffset, VGlobal::copyOffset ) );
		selection.append( temp );
	}
	m_part->document().selection()->clear();
	// Calc new selection
	VObjectListIterator itr2( selection );
	for( ; itr2.current() ; ++itr2 )
	{
		m_part->insertObject( itr2.current() );
		m_part->document().selection()->append( itr2.current() );
	}
	m_part->repaintAllViews();
}

void
KarbonView::editSelectAll()
{
	m_part->document().selection()->append();

	if( m_part->document().selection()->objects().count() > 0 )
		m_part->repaintAllViews();
	selectionChanged();
}

void
KarbonView::editDeselectAll()
{
	if( m_part->document().selection()->objects().count() > 0 )
	{
		m_part->document().selection()->clear();
		m_part->repaintAllViews();
	}
	selectionChanged();
}

void
KarbonView::editDeleteSelection()
{
kdDebug() << "*********" << endl;
	m_part->addCommand(
		new VDeleteCmd( &m_part->document() ),
		true );
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
KarbonView::selectionBringToFront()
{
	m_part->addCommand(
		new VZOrderCmd( &m_part->document(), VZOrderCmd::bringToFront ), true );
}

void
KarbonView::selectionMoveUp()
{
	m_part->addCommand(
		new VZOrderCmd( &m_part->document(), VZOrderCmd::up ), true );
}

void
KarbonView::selectionMoveDown()
{
	m_part->addCommand(
		new VZOrderCmd( &m_part->document(), VZOrderCmd::down ), true );
}

void
KarbonView::selectionSendToBack()
{
	m_part->addCommand(
		new VZOrderCmd( &m_part->document(), VZOrderCmd::sendToBack ), true );
}

void
KarbonView::groupSelection()
{
	m_part->addCommand( new VGroupCmd( &m_part->document() ), true );
	selectionChanged();
}

void
KarbonView::ungroupSelection()
{
	m_part->addCommand( new VUnGroupCmd( &m_part->document() ), true );
	selectionChanged();
}

// TODO: remove this one someday:
void
KarbonView::dummyForTesting()
{
kdDebug() << "KarbonView::dummyForTesting()" << endl;

	VPath s( 0L );
	s.moveTo( KoPoint(100,100) );
	s.lineTo( KoPoint(100,300) );
	s.lineTo( KoPoint(400,300) );
	s.lineTo( KoPoint(400,100) );
	s.close();
kdDebug() << "***" << s.counterClockwise() << endl;

	VPath t( 0L );
	t.moveTo( KoPoint(100,100) );
	t.lineTo( KoPoint(100,300) );
	t.lineTo( KoPoint(400,300) );
	t.lineTo( KoPoint(400,100) );
	t.close();

	t.revert();

kdDebug() << "***" << t.counterClockwise() << endl;

//	m_part->document().append( p );

//	m_part->repaintAllViews();
}

void
KarbonView::objectTrafoTranslate()
{
	if( m_TransformDlg->isVisible() == false )
	{
		mainWindow()->addDockWindow( m_TransformDlg, DockRight );
		m_TransformDlg->setTab( Translate );
		m_TransformDlg->show();
	}
}

void
KarbonView::objectTrafoScale()
{
	if( m_TransformDlg->isVisible() == false )
	{
		mainWindow()->addDockWindow( m_TransformDlg, DockRight );
		m_TransformDlg->setTab( Scale );
		m_TransformDlg->show();
	}
}

void
KarbonView::objectTrafoRotate()
{
	if( m_TransformDlg->isVisible() == false )
	{
		mainWindow()->addDockWindow( m_TransformDlg, DockRight );
		m_TransformDlg->setTab( Rotate );
		m_TransformDlg->show();
	}
}

void
KarbonView::objectTrafoShear()
{
	if( m_TransformDlg->isVisible() == false )
	{
		mainWindow()->addDockWindow( m_TransformDlg, DockRight );
		m_TransformDlg->setTab( Shear );
		m_TransformDlg->show();
	}
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
	if( m_currentTool == m_polygonTool )
		m_polygonTool->showDialog();
	else
	{
		m_currentTool->deactivate();
		m_currentTool = m_polygonTool;
		m_currentTool->activate();
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
	if( m_currentTool == m_roundRectTool )
		m_roundRectTool->showDialog();
	else
	{
		m_currentTool->deactivate();
		m_currentTool = m_roundRectTool;
		m_currentTool->activate();
	}
}

void
KarbonView::selectTool()
{
	m_currentTool->deactivate();
	m_currentTool = m_selectTool;
	m_currentTool->activate();
	m_canvas->repaintAll();
}

void
KarbonView::selectNodesTool()
{
	m_currentTool->deactivate();
	m_currentTool = m_selectNodesTool;
	m_currentTool->activate();
	m_canvas->repaintAll();
}

void
KarbonView::rotateTool()
{
	m_currentTool->deactivate();
	m_currentTool = m_rotateTool;
	m_currentTool->activate();
	m_canvas->repaintAll();
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
	m_currentTool->deactivate();
	m_currentTool = m_shearTool;
	m_currentTool->activate();
	m_canvas->repaintAll();
}

void
KarbonView::sinusTool()
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

void
KarbonView::spiralTool()
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

void
KarbonView::starTool()
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

void
KarbonView::gradTool()
{
	if( m_currentTool == m_gradTool )
		m_gradTool->showDocker();
	else
	{
		m_currentTool->deactivate();
		m_currentTool = m_gradTool;
		m_currentTool->activate();
	}
}

void
KarbonView::polylineTool()
{
	if( m_currentTool == m_polylineTool )
	{
		// Ends the current polyline and prepare for the next.
		m_currentTool->deactivate();
		m_currentTool->activate();
	}
	else
	{
		m_currentTool->deactivate();
		m_currentTool = m_polylineTool;
		m_currentTool->activate();
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
	canvasWidget()->pixmap()->fill();
	if( m_viewAction->currentItem() == 1 )
		m_painterFactory->setWireframePainter( canvasWidget()->pixmap(), width(), height() );
	else
		m_painterFactory->setPainter( canvasWidget()->pixmap(), width(), height() );
	m_canvas->repaintAll();
}

void
KarbonView::zoomChanged()
{
	bool bOK;
	double zoomFactor = m_zoomAction->currentText().toDouble( &bOK ) / 100.0;
	// above 2000% probably doesnt make sense... (Rob)
	if( zoomFactor > 20 )
	{
		zoomFactor = 20;
		m_zoomAction->changeItem( m_zoomAction->currentItem(), " 2000%" );
	}
	setZoom( zoomFactor );
	// TODO : I guess we should define a document size member at this point...
	//kdDebug() << "m_part->pageLayout().ptWidth :" << m_part->pageLayout().ptWidth << endl;
	//kdDebug() << "m_part->pageLayout().ptHeight :" << m_part->pageLayout().ptHeight << endl;
	m_canvas->resizeContents( int( m_part->pageLayout().ptWidth * zoomFactor ),
								int( m_part->pageLayout().ptHeight * zoomFactor ) );
	m_canvas->repaintAll();
	m_canvas->setFocus();
}

void
KarbonView::solidFillClicked()
{
	if( shell() && shell()->rootView() == this )
	{
		VFillDlg* dialog = new VFillDlg( m_part );
		connect(dialog, SIGNAL( fillChanged( const VFill & ) ), this, SLOT( selectionChanged() ) );
		dialog->exec();
		delete dialog;
		disconnect(dialog, SIGNAL( fillChanged( const VFill & ) ), this, SLOT( selectionChanged() ) );
	}
}

void
KarbonView::strokeClicked()
{
	if( shell() && shell()->rootView() == this )
	{
		VStrokeDlg* dialog = new VStrokeDlg( m_part );
		connect(dialog, SIGNAL( strokeChanged( const VStroke & ) ), this, SLOT( selectionChanged() ) );
		dialog->exec();
		delete dialog;
		disconnect(dialog, SIGNAL( strokeChanged( const VStroke & ) ), this, SLOT( selectionChanged() ) );
	}
}

void
KarbonView::slotStrokeChanged( const VStroke &c )
{
	m_part->document().selection()->setStroke( c );

	m_part->addCommand( new VStrokeCmd( &m_part->document(), &c ), true );

	m_strokeFillPreview->update( *( m_part->document().selection()->stroke() ),
								 *( m_part->document().selection()->fill() ) );
}

void
KarbonView::slotFillChanged( const VFill &f )
{
	m_part->document().selection()->setFill( f );

	m_part->addCommand( new VFillCmd( &m_part->document(), f ), true );

	m_strokeFillPreview->update( *( m_part->document().selection()->stroke() ),
								 *( m_part->document().selection()->fill() ) );
}

void
KarbonView::slotJoinStyleClicked()
{
	VObjectListIterator itr( m_part->document().selection()->objects() );
	for( ; itr.current() ; ++itr )
	{
		VStroke stroke( *( itr.current()->stroke() ) );
		stroke.setParent( itr.current() );
		stroke.setLineJoin( (VStroke::VLineJoin)m_joinStyle->getState() );
		itr.current()->setStroke( stroke );
	}

	m_part->repaintAllViews();
}

void
KarbonView::slotCapStyleClicked()
{
	VObjectListIterator itr( m_part->document().selection()->objects() );
	for( ; itr.current() ; ++itr )
	{
		VStroke stroke( *( itr.current()->stroke() ) );
		stroke.setParent( itr.current() );
		stroke.setLineCap( (VStroke::VLineCap)m_capStyle->getState() );
		itr.current()->setStroke( stroke );
	}

	m_part->repaintAllViews();
}

void
KarbonView::setLineWidth()
{
    setLineWidth( m_setLineWidth->value() );
	selectionChanged();
}

//necessary for dcop call !
void
KarbonView::setLineWidth( double val)
{
	m_part->addCommand( new VStrokeLineWidthCmd( &m_part->document(), val ), true );
	return;
}

void
KarbonView::viewColorManager()
{
	if( m_ColorManager->isVisible() == false )
	{
		mainWindow()->addDockWindow( m_ColorManager, DockRight );
		m_ColorManager->show();
	}
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
		SLOT( selectionBringToFront() ), actionCollection(), "object_move_totop" );
	new KAction(
		i18n( "&Raise" ), 0, QKeySequence("Ctrl+PgUp"), this,
		SLOT( selectionMoveUp() ), actionCollection(), "object_move_up" );
	new KAction(
		i18n( "&Lower" ), 0, QKeySequence("Ctrl+PgDown"), this,
		SLOT( selectionMoveDown() ), actionCollection(), "object_move_down" );
	new KAction(
		i18n( "Send to &Back" ), 0, QKeySequence("Shift+PgDown"), this,
		SLOT( selectionSendToBack() ), actionCollection(), "object_move_tobottom" );
	m_groupObjects = new KAction(
		i18n( "&Group Objects" ), "14_group", QKeySequence("Ctrl+G"), this,
		SLOT( groupSelection() ), actionCollection(), "selection_group" );
	m_ungroupObjects = new KAction(
		i18n( "&Ungroup Objects" ), "14_ungroup", QKeySequence("Ctrl+U"), this,
		SLOT( ungroupSelection() ), actionCollection(), "selection_ungroup" );
	new KAction(
		i18n( "&Translate" ), "14_translate", 0, this,
		SLOT( objectTrafoTranslate() ), actionCollection(), "object_trafo_translate" );
	new KAction(
		i18n( "&Rotate" ), "14_rotate", 0, this,
		SLOT( objectTrafoRotate() ), actionCollection(), "object_trafo_rotate" );
	new KAction(
		i18n( "S&hear" ), "14_shear", 0, this,
		SLOT( objectTrafoShear() ), actionCollection(), "object_trafo_shear" );
	// object <-----

	// text ----->
	// Disable the text, hopefully it will make a comback soon (Rob)
	/*m_setFontFamily = new KFontAction(
		i18n( "Set Font Family" ), 0, actionCollection(), "setFontFamily" );
	m_setFontFamily->setCurrentItem( 0 );*/

	//connect( m_setFontFamily,
	//	SIGNAL( activated( const QString& ) ),
	//	SLOT( setFontFamily( const QString& ) ) );

	/*m_setFontSize = new KFontSizeAction(
		i18n( "Set Font Size" ), 0, actionCollection(), "setFontSize" );
	m_setFontSize->setCurrentItem( 0 );*/

	//connect( m_setFontSize,
	//	SIGNAL( activated( const QString& ) ),
	//	SLOT( setFontSize( const QString& ) ) );

	/*m_setFontItalic = new KToggleAction(
		i18n( "&Italic" ), "text_italic", 0, actionCollection(), "setFontItalic" );
	m_setFontBold = new KToggleAction(
		i18n( "&Bold" ), "text_bold", 0, actionCollection(), "setFontBold" );*/

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
		<< i18n( "   25%" )
		<< i18n( "   50%" )
		<< i18n( "  100%" )
		<< i18n( "  200%" )
		<< i18n( "  300%" )
		<< i18n( "  400%" )
		<< i18n( "  800%" );

	m_zoomAction->setItems( stl );
	m_zoomAction->setEditable( true );
	m_zoomAction->setCurrentItem( 2 );

	new KAction(
		i18n( "&Color Manager" ), "colorman", 0, this,
		SLOT( viewColorManager() ), actionCollection(), "view_color_manager" );

	// view <-----

	// line width

	m_setLineWidth = new TKUFloatSpinBoxAction( i18n("Set Line Width"), "linewidth", 0, actionCollection(), "setLineWidth" );
	m_setLineWidth->setIconMode( TK::IconOnly );
	m_setLineWidth->setDecimals( 1 );
	m_setLineWidth->setMinValue( 0.0 );
	m_setLineWidth->setLineStep( 0.5 );
	connect( m_setLineWidth, SIGNAL( activated() ), this, SLOT( setLineWidth() ) );
	//connect( m_pDoc, SIGNAL( unitsChanged(int) ), m_setLineWidth, SLOT( setUnit(int) ) );

	// set up join style widget
	m_joinStyle = new VStateButton( this );
	m_joinStyle->addState( new QPixmap( DesktopIcon( "join_bevel" ) ) );
    m_joinStyle->addState( new QPixmap( DesktopIcon( "join_miter" ) ) );
    m_joinStyle->addState( new QPixmap( DesktopIcon( "join_round" ) ) );
    m_joinStyle->setState( 0 );
#if KDE_VERSION >= 305
	new KWidgetAction( m_joinStyle, i18n("Set Join Style"), 0, this, SLOT( slotJoinStyleClicked() ), actionCollection(), "setJoinStyle" );
#endif
	connect( m_joinStyle, SIGNAL( clicked() ), this, SLOT( slotJoinStyleClicked() ) );

	// set up cap style widget
	m_capStyle = new VStateButton( this );
	m_capStyle->addState( new QPixmap( DesktopIcon( "cap_butt" ) ) );
    m_capStyle->addState( new QPixmap( DesktopIcon( "cap_square" ) ) );
    m_capStyle->addState( new QPixmap( DesktopIcon( "cap_round" ) ) );
    m_capStyle->setState( 0 );
#if KDE_VERSION >= 305
	new KWidgetAction( m_capStyle, i18n("Set Cap Style"), 0, this, SLOT( slotCapStyleClicked() ), actionCollection(), "setCapStyle" );
#endif
	connect( m_capStyle, SIGNAL( clicked() ), this, SLOT( slotCapStyleClicked() ) );

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
	int count = part()->document().selection()->objects().count();
	if( count > 0)
	{
		VGroup *group = dynamic_cast<VGroup *>( part()->document().selection()->objects().getFirst() );
		m_groupObjects->setEnabled( !group && ( count > 1 ) );
		m_ungroupObjects->setEnabled( group && ( count == 1 )  );
		m_strokeFillPreview->update( *part()->document().selection()->objects().getFirst()->stroke(),
												*part()->document().selection()->objects().getFirst()->fill() );
		part()->document().selection()->setStroke( *( part()->document().selection()->objects().getFirst()->stroke() ) );
		part()->document().selection()->setFill( *( part()->document().selection()->objects().getFirst()->fill() ) );
		m_setLineWidth->setEnabled( true );
		m_setLineWidth->setValue( part()->document().selection()->objects().getFirst()->stroke()->lineWidth() );
		if( m_ColorManager->isStrokeDocker() )
		{
			VColor *c = new VColor ( m_part->document().selection()->objects().getFirst()->stroke()->color() );
			m_ColorManager->setColor( c );
		}
		else
		{
			VColor *c = new VColor ( m_part->document().selection()->objects().getFirst()->fill()->color() );
			m_ColorManager->setColor( c );
		}
	}
	else
	{
		m_strokeFillPreview->update( *( part()->document().selection()->stroke() ),
									 *( part()->document().selection()->fill() ) );
		m_setLineWidth->setEnabled( false );
		m_groupObjects->setEnabled( false );
		m_ungroupObjects->setEnabled( false );
	}
}

void KarbonView::setUnit(KoUnit::Unit /*_unit*/)
{
    m_ellipseTool->refreshUnit();
    m_rectangleTool->refreshUnit();
    m_sinusTool->refreshUnit();
    m_starTool->refreshUnit();
    m_roundRectTool->refreshUnit();
}

#include "karbon_view.moc"
