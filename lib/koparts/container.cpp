#include "container.h"
#include "frame.h"
#include "handler.h"
#include "toolbox.h"
#include "shell.h"

#include <qpainter.h>
#include <qwhatsthis.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qregion.h>

ContainerPart::ContainerPart( QObject* parent, const char* name )
    : Part( parent, name )
{
    m_children.setAutoDelete( TRUE );
}

ContainerPart::~ContainerPart()
{
}

void ContainerPart::insertChild( PartChild* child )
{
    m_children.append( child );

    connect( child, SIGNAL( changed( PartChild* ) ), this, SIGNAL( childChanged( PartChild* ) ) );
}

QList<PartChild>& ContainerPart::children()
{
    return m_children;
}

void ContainerPart::paintEverything( QPainter& painter, const QRect& rect, bool transparent, View* view )
{
    paintContent( painter, rect, transparent );
    paintChildren( painter, rect, view );
}

void ContainerPart::paintChildren( QPainter& painter, const QRect& /*rect*/, View* view )
{
    QListIterator<PartChild> it( m_children );
    for( ; it.current(); ++it )
    {
	// #### todo: paint only if child is visible inside rect
	painter.save();
	paintChild( it.current(), painter, view );
	painter.restore();
    }
}

void ContainerPart::paintChild( PartChild* child, QPainter& painter, View* view )
{
    Shell* shell = view->shell();

    QRegion rgn = painter.clipRegion();
	
    child->transform( painter );
    child->part()->paintEverything( painter, child->contentRect(), child->isTransparent(), view );

    if ( shell )
    {
	painter.scale( 1.0 / child->xScaling(), 1.0 / child->yScaling() );

	int w = int( (double)child->contentRect().width() * child->xScaling() );
	int h = int( (double)child->contentRect().height() * child->yScaling() );
	if ( ( shell->selectedPart() == child->part() &&
	       shell->selectedView() == view ) ||
	     ( shell->activePart() == child->part() &&
	       shell->activeView() == view ) )
        {
	    painter.setClipRegion( rgn );

	    painter.setPen( black );
	    painter.fillRect( -5, -5, w + 10, 5, white );
	    painter.fillRect( -5, h, w + 10, 5, white );
	    painter.fillRect( -5, -5, 5, h + 10, white );
	    painter.fillRect( w, -5, 5, h + 10, white );
	    painter.fillRect( -5, -5, w + 10, 5, BDiagPattern );
	    painter.fillRect( -5, h, w + 10, 5, BDiagPattern );		
	    painter.fillRect( -5, -5, 5, h + 10, BDiagPattern );
	    painter.fillRect( w, -5, 5, h + 10, BDiagPattern );
	
	    if ( shell->selectedPart() == child->part() &&
		 shell->selectedView() == view )
	    {
		QColor color;
		if ( view->part() == this )
		    color = black;
		else
		    color = gray;
		painter.fillRect( -5, -5, 5, 5, color );
		painter.fillRect( -5, h, 5, 5, color );
		painter.fillRect( w, h, 5, 5, color );
		painter.fillRect( w, -5, 5, 5, color );
		painter.fillRect( w / 2 - 3, -5, 5, 5, color );
		painter.fillRect( w / 2 - 3, h, 5, 5, color );
		painter.fillRect( -5, h / 2 - 3, 5, 5, color );
		painter.fillRect( w, h / 2 - 3, 5, 5, color );
	    }
	}
    }
}

Part* ContainerPart::hitTest( const QPoint& p, const QWMatrix& matrix )
{
    QListIterator<PartChild> it( m_children );
    for( ; it.current(); ++it )
    {
	Part* part = it.current()->hitTest( p, matrix );
	if ( part )
	    return part;
    }

    return this;
}

PartChild* ContainerPart::child( Part* part )
{
    QListIterator<PartChild> it( m_children );
    for( ; it.current(); ++it )
    {
	if ( it.current()->part() == part )
	    return it.current();
    }

    return 0;
}

// ------------------------------------------------------------

ContainerView::ContainerView( Part* part, QWidget* parent, const char* name )
    : View( part, parent, name )
{
    m_children.setAutoDelete( TRUE );

    setMouseTracking( TRUE );

    connect( part, SIGNAL( childChanged( PartChild* ) ), this, SLOT( slotChildChanged( PartChild* ) ) );
}

ContainerView::~ContainerView()
{
}

QWMatrix ContainerView::matrix() const
{
    QWMatrix m;
    m.scale( xScaling(), yScaling() );
    return m;
}

void ContainerView::setScaling( double x, double y )
{
    View::setScaling( x, y );

    QListIterator<ViewChild> it( m_children );
    for( ; it.current(); ++it )
    {
	QRect geom = it.current()->partChild()->geometry();
	
	it.current()->frame()->view()->setScaling( x * it.current()->partChild()->yScaling(),
						   y * it.current()->partChild()->xScaling() );
	it.current()->frame()->setGeometry( geom.x() * x, geom.y() * y,
					    geom.width() * x, geom.height() * y );
    }
}

Part* ContainerView::hitTest( const QPoint& pos )
{
    if ( selectedChild() && selectedChild()->frameRegion( matrix() ).contains( pos ) )
	return 0;
    if ( activeChild() && activeChild()->frameRegion( matrix() ).contains( pos ) )
	return 0;
	
    ContainerPart* my = (ContainerPart*)part();
    return my->Part::hitTest( QPoint( pos.x() / xScaling(), pos.y() / yScaling() ) );
}

void ContainerView::viewSelectEvent( ViewSelectEvent* ev )
{
    if ( ev->part() != part() )
    {
	PartChild* ch = ((ContainerPart*)part())->child( ev->part() );
	if ( ch && ev->selected() )
        {
	    emit regionInvalidated( ch->frameRegion( matrix() ), TRUE );
	    emit childSelected( ch );
	}
	else if ( ch )
        {
	    qDebug("Invalidating unselected frame");
	    emit regionInvalidated( ch->frameRegion( matrix() ), TRUE );
	    emit childUnselected( ch );
	}
	else
	    emit invalidated();
    }
    else
	View::viewSelectEvent( ev );
}

void ContainerView::viewActivateEvent( ViewActivateEvent* ev )
{
    if ( ev->part() != part() )
    {
	PartChild* ch = ((ContainerPart*)part())->child( ev->part() );
	if ( ch && ev->activated() )
        {
	    if ( ch->isRectangle() && !ch->isTransparent() )
            {
		Frame* frame = new Frame( canvas() );
		View* view = ch->part()->createView( frame );
		view->setScaling( xScaling() * ch->yScaling(), yScaling() * ch->xScaling() );
		QRect geom = ch->geometry();
		frame->setGeometry( geom.x() * xScaling() + canvasXOffset(), geom.y() * yScaling() + canvasYOffset(),
				    geom.width() * xScaling(), geom.height() * yScaling() );
		frame->setView( view );
		frame->show();
		frame->raise();
		m_children.append( new ViewChild( ch, frame ) );
		connect( view, SIGNAL( activated( bool ) ), this, SLOT( slotChildActivated( bool ) ) );
		shell()->setActiveView( view );
	    }
	    else
	    {
		emit regionInvalidated( ch->frameRegion( matrix() ), TRUE );
		emit childActivated( ch );
	    }
	}
	else if ( ch )
        {
	    emit regionInvalidated( ch->frameRegion( matrix() ), TRUE );
	    emit childDeactivated( ch );
	}
	else
	    emit invalidated();
    }
    else
	View::viewActivateEvent( ev );
}

ViewChild* ContainerView::child( Part* part )
{
    if ( !part )
	return 0;

    QListIterator<ViewChild> it( m_children );
    for( ; it.current(); ++it )
    {
	if ( it.current()->partChild()->part() == part )
	    return it.current();
    }

    return 0;
}

ViewChild* ContainerView::child( View* view )
{
    if ( !view )
	return 0;

    QListIterator<ViewChild> it( m_children );
    for( ; it.current(); ++it )
    {
	if ( it.current()->frame()->view() == view )
	    return it.current();
    }

    return 0;
}

PartChild* ContainerView::selectedChild()
{
    return ((ContainerPart*)part())->child( shell()->selectedPart() );
}

PartChild* ContainerView::activeChild()
{
    return ((ContainerPart*)part())->child( shell()->activePart() );
}

void ContainerView::slotChildChanged( PartChild* ch )
{
    QRegion rgn( ch->oldPointArray( matrix() ) );
    emit regionInvalidated( ch->frameRegion( matrix(), TRUE ).unite( rgn ), TRUE );
}

bool ContainerView::hasPartInWindow( Part* part )
{
    return child( part ) != 0;
}

void ContainerView::slotChildActivated( bool a )
{
    // Only interested in deactivate events
    if ( a )
	return;

    ViewChild* ch = child( (View*)sender() );
    if ( !ch )
	return;

    View* view = ch->frame()->view();

    // Is the new active view a child of this one ?
    // In this case we may not delete!
    View* n = shell()->activeView();
    QObject* o = n;
    while( o )
	if ( o == view )
	    return;
	else
	    o = o->parent();

    m_children.remove( ch );

    // #### HACK
    // We want to delete as many views as possible and this
    // trick is used to go upwards in the view-tree.
    emit activated( FALSE );
}

void ContainerView::paintEverything( QPainter& painter, const QRect& rect, bool transparent )
{
    part()->paintEverything( painter, rect, transparent, this );
}

#include "container.moc"
