/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <qiconset.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>

#include "karbon_view.h"
#include "karbon_factory.h"
#include "karbon_part.h"

#include <kdebug.h>

KarbonView::KarbonView( KarbonPart* part, QWidget* parent, const char* name )
	: KoView( part, parent, name ), m_part( part )
{
	setInstance( KarbonFactory::instance() );
	setXMLFile( QString::fromLatin1("karbon.rc") );

	initActions();

	m_canvas = new VCanvas( this, part );
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
	KStdAction::cut( this,
		SLOT( editCut() ), actionCollection(), "edit_cut" );
	KStdAction::copy( this,
		SLOT( editCopy() ), actionCollection(), "edit_copy");
	KStdAction::paste( this,
		SLOT( editPaste() ), actionCollection(), "edit_paste" );
 	KStdAction::selectAll( this,
		SLOT( editSelectAll() ), actionCollection(), "edit_selectall" );

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

#include "karbon_view.moc"
