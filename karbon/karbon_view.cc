/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qiconset.h>
#include <qpainter.h>

#include <kaction.h>
#include <klocale.h>
#include <kstdaction.h>

#include "karbon_factory.h"
#include "karbon_part.h"
#include "karbon_view.h"
#include "vctool_rectangle.h"

#include <kdebug.h>

VTool* KarbonView::s_currentTool = 0L;

KarbonView::KarbonView( KarbonPart* part, QWidget* parent, const char* name )
	: KoView( part, parent, name ), m_part( part )
{
	if ( s_currentTool == 0L )
		s_currentTool = VCToolRectangle::instance( part );

	setInstance( KarbonFactory::instance() );
	setXMLFile( QString::fromLatin1( "karbon.rc" ) );

	initActions();

	m_canvas = new VCanvas( this, part );
	m_canvas->installEventFilter( this );
	m_canvas->setGeometry( 0, 0, width(), height() );
}

KarbonView::~KarbonView()
{
	delete m_canvas;
	m_canvas = 0L;
}

void
KarbonView::updateReadWrite( bool /*rw*/ )
{
}

void
KarbonView::resizeEvent( QResizeEvent* /*event*/ ) {
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
}

void
KarbonView::initActions()
{
	// edit
	KStdAction::cut( this, SLOT( editCut() ), actionCollection(),
		"edit_cut" );
	KStdAction::copy( this, SLOT( editCopy() ), actionCollection(),
		"edit_copy");
	KStdAction::paste( this, SLOT( editPaste() ), actionCollection(),
		"edit_paste" );
  	KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(),
		"edit_selectall" );

	// object
	new KAction( i18n("&Ellipse"), 0, actionCollection(),
		"insert_ellipse" );
	new KAction( i18n("&Polygon"), 0, actionCollection(),
		"insert_polygon" );
	new KAction( i18n("&Rectangle"), 0, actionCollection(),
		"insert_rectangle" );
	new KAction( i18n("S&inus"), 0, actionCollection(),
		"insert_sinus" );
	new KAction( i18n("&Spiral"), 0, actionCollection(),
		"insert_spiral" );
	new KAction( i18n("S&tar"), 0, actionCollection(),
		"insert_star" );

	// view
	m_zoomAction = new KSelectAction( i18n("&Zoom"), 0, actionCollection(),
		"view_zoom" );
	QStringList stl;
	stl << i18n("25%") << i18n("50%") << i18n("100%");
	m_zoomAction->setItems(stl);
	m_zoomAction->setCurrentItem(2);
	m_zoomAction->setEditable(true);
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
	if ( object == m_canvas )
		return s_currentTool->eventFilter( event );
	else
		return false;
}

#include "karbon_view.moc"
