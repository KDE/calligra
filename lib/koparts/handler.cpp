#include "handler.h"
#include "container.h"
#include <math.h>
#include <qwmatrix.h>

EventHandler::EventHandler( QObject* target )
{
    m_target = target;

    m_target->installEventFilter( this );
}

EventHandler::~EventHandler()
{
}

QObject* EventHandler::target()
{
    return m_target;
}

// ------------------------------------------------------

PartResizeHandler::PartResizeHandler( QWidget* widget, const QWMatrix& matrix, PartChild* child,
				      PartChild::Gadget gadget, const QPoint& point )
    : EventHandler( widget )
{
    child->lock();

    m_gadget = gadget;
    m_child = child;
    m_geometryStart = child->geometry();
    m_matrix = child->matrix() * matrix;
    m_parentMatrix = matrix;
    m_invertParentMatrix = matrix.invert();

    bool ok = TRUE;
    m_invert = m_matrix.invert( &ok );
    ASSERT( ok );

    m_mouseStart = m_invert.map( m_invertParentMatrix.map( point ) );
}

PartResizeHandler::~PartResizeHandler()
{
    m_child->unlock();
}

bool PartResizeHandler::eventFilter( QObject*, QEvent* ev )
{
    if ( ev->type() == QEvent::MouseButtonRelease )
    {
	delete this;
	return TRUE;
    }
    else if ( ev->type() == QEvent::MouseMove )
    {
	QMouseEvent* e = (QMouseEvent*)ev;
	QPoint p = m_invert.map( m_invertParentMatrix.map( e->pos() ) );
	QRegion rgn( m_child->frameRegion( m_parentMatrix, TRUE ) );

	double x1_x, x1_y, x2_x, x2_y;
	m_matrix.map( double( p.x() ), 0.0, &x1_x, &x1_y );
	m_matrix.map( double( m_mouseStart.x() ), 0.0, &x2_x, &x2_y );		
	double y1_x, y1_y, y2_x, y2_y;
	m_matrix.map( 0.0, double( p.y() ), &y1_x, &y1_y );
	m_matrix.map( 0.0, double( m_mouseStart.y() ), &y2_x, &y2_y );		
		
	double dx = x2_x - x1_x;
	double dy = x2_y - x1_y;
	int x = int( sqrt( dx * dx + dy * dy ) * ( m_mouseStart.x() < p.x() ? 1.0 : -1.0 ) );
		
	dx = y2_x - y1_x;
	dy = y2_y - y1_y;
	int y = int( sqrt( dx * dx + dy * dy ) * ( m_mouseStart.y() < p.y() ? 1.0 : -1.0 ) );

	switch( m_gadget )
        {
	case PartChild::TopLeft:
	    {
		x = QMIN( m_geometryStart.width() - 1, x );
		y = QMIN( m_geometryStart.height() - 1, y );

		m_child->setGeometry( QRect( m_geometryStart.x() + x, m_geometryStart.y() + y,
					     m_geometryStart.width() - x, m_geometryStart.height() - y ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case PartChild::TopMid:
	    {
		y = QMIN( m_geometryStart.height() - 1, y );

		m_child->setGeometry( QRect( m_geometryStart.x(), m_geometryStart.y() + y,
					     m_geometryStart.width(), m_geometryStart.height() - y ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case PartChild::TopRight:
	    {
		x = QMAX( -m_geometryStart.width() + 1, x );
		y = QMIN( m_geometryStart.height() - 1, y );

		m_child->setGeometry( QRect( m_geometryStart.x(), m_geometryStart.y() + y,
					     m_geometryStart.width() + x, m_geometryStart.height() - y ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case PartChild::MidLeft:
	    {
		x = QMIN( m_geometryStart.width() - 1, x );
		
		m_child->setGeometry( QRect( m_geometryStart.x() + x, m_geometryStart.y(),
					     m_geometryStart.width() - x, m_geometryStart.height() ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case PartChild::MidRight:
	    {
		x = QMAX( -m_geometryStart.width() + 1, x );

		m_child->setGeometry( QRect( m_geometryStart.x(), m_geometryStart.y(),
					     m_geometryStart.width() + x, m_geometryStart.height() ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case PartChild::BottomLeft:
	    {
		x = QMIN( m_geometryStart.width() - 1, x );
		y = QMAX( -m_geometryStart.height() + 1, y );

		m_child->setGeometry( QRect( m_geometryStart.x() + x, m_geometryStart.y(),
					     m_geometryStart.width() - x, m_geometryStart.height() + y ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case PartChild::BottomMid:
	    {
		y = QMAX( -m_geometryStart.height() + 1, y );

		m_child->setGeometry( QRect( m_geometryStart.x(), m_geometryStart.y(),
					     m_geometryStart.width(), m_geometryStart.height() + y ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case PartChild::BottomRight:
	    {
		x = QMAX( -m_geometryStart.width() + 1, x );
		y = QMAX( -m_geometryStart.height() + 1, y );

		m_child->setGeometry( QRect( m_geometryStart.x(), m_geometryStart.y(),
					     m_geometryStart.width() + x, m_geometryStart.height() + y ) );
		((QWidget*)target())->repaint( rgn.unite( m_child->frameRegion( m_parentMatrix, TRUE ) ) );
	    }
	    break;
	default:
	    ASSERT( 0 );
	}
	return TRUE;
    }

    return FALSE;
}

// --------------------------------------------------------------

PartMoveHandler::PartMoveHandler( QWidget* widget, const QWMatrix& matrix, PartChild* child,
				  const QPoint& point )
    : EventHandler( widget )
{
    child->lock();
	
    m_dragChild = child;
    m_parentMatrix = matrix;
    m_invertParentMatrix = matrix.invert();
    m_mouseDragStart = m_invertParentMatrix.map( point );
    m_geometryDragStart = m_dragChild->geometry();
    m_rotationDragStart = m_dragChild->rotationPoint();
}

PartMoveHandler::~PartMoveHandler()
{
    m_dragChild->unlock();
}

bool PartMoveHandler::eventFilter( QObject*, QEvent* ev )
{
    if ( ev->type() == QEvent::MouseButtonRelease )
    {
	delete this;
	return TRUE;
    }
    else if ( ev->type() == QEvent::MouseMove )
    {
	QMouseEvent* e = (QMouseEvent*)ev;
	
	QRegion bound = m_dragChild->frameRegion( m_parentMatrix, TRUE );
	QPoint pos = m_invertParentMatrix.map( e->pos() );
	m_dragChild->setGeometry( QRect( m_geometryDragStart.x() + pos.x() - m_mouseDragStart.x(),
					     m_geometryDragStart.y() + pos.y() - m_mouseDragStart.y(),
					     m_geometryDragStart.width(), m_geometryDragStart.height() ) );
	m_dragChild->setRotationPoint( QPoint( m_rotationDragStart.x() + pos.x() - m_mouseDragStart.x(),
					       m_rotationDragStart.y() + pos.y() - m_mouseDragStart.y() ) );
	((QWidget*)target())->repaint( bound.unite( m_dragChild->frameRegion( m_parentMatrix, TRUE ) ) );

	return TRUE;
    }

    return FALSE;
}

// -------------------------------------------------------

ContainerHandler::ContainerHandler( ContainerView* view, QWidget* widget )
    : EventHandler( widget )
{
    m_view = view;
}

ContainerHandler::~ContainerHandler()
{
}

bool ContainerHandler::eventFilter( QObject*, QEvent* ev )
{
    if ( ev->type() == QEvent::MouseButtonPress )
    {
	QMouseEvent* e = (QMouseEvent*)ev;

	PartChild::Gadget gadget = PartChild::NoGadget;
	PartChild* child = m_view->selectedChild();
	if (child )
	    gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	if ( gadget == PartChild::NoGadget )
        {
	    child = m_view->activeChild();
	    if (child )
		gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	}
	
	if ( e->button() == LeftButton && gadget == PartChild::Move )
        {
	    (void)new PartMoveHandler( (QWidget*)target(), m_view->matrix(), child, e->pos() );
	    return TRUE;
	}
	else if ( e->button() == LeftButton && gadget != PartChild::NoGadget )
        {
	    (void)new PartResizeHandler( (QWidget*)target(), m_view->matrix(), child, gadget, e->pos() );
	    return TRUE;
	}

	return FALSE;
    }
    else if ( ev->type() == QEvent::MouseMove )
    {
	QMouseEvent* e = (QMouseEvent*)ev;

	PartChild* child = m_view->selectedChild();
	PartChild::Gadget gadget = PartChild::NoGadget;
	if ( child )
	    gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	if ( gadget == PartChild::NoGadget )
        {
	    child = m_view->activeChild();
	    if (child )
		gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	}

	if ( gadget == PartChild::TopLeft || gadget == PartChild::BottomRight )
	    ((QWidget*)target())->setCursor( sizeFDiagCursor );
	else if ( gadget == PartChild::TopRight || gadget == PartChild::BottomLeft )
	    ((QWidget*)target())->setCursor( sizeBDiagCursor );	
	else if ( gadget == PartChild::TopMid || gadget == PartChild::BottomMid )
	    ((QWidget*)target())->setCursor( sizeVerCursor );
	else if ( gadget == PartChild::MidLeft || gadget == PartChild::MidRight )
	    ((QWidget*)target())->setCursor( sizeHorCursor );
	else if ( gadget == PartChild::Move )
	    ((QWidget*)target())->setCursor( pointingHandCursor );
	else
        {
	    ((QWidget*)target())->setCursor( arrowCursor );
	    return FALSE;
	}
	
	return TRUE;
    }

    return FALSE;
}

#include "handler.moc"
