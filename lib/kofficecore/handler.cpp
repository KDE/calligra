#include "handler.h"
#include "koView.h"
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

class PartResizeHandlerPrivate {
public:
    PartResizeHandlerPrivate( const QWMatrix& matrix, KoDocumentChild* child,
			      KoDocumentChild::Gadget gadget, const QPoint& point ) :
	m_gadget(gadget), m_child(child), m_parentMatrix(matrix) {
	
	m_geometryStart = child->geometry();
	m_matrix = child->matrix() * matrix;
	m_invertParentMatrix = matrix.invert();
	
	bool ok = true;
	m_invert = m_matrix.invert( &ok );
	ASSERT( ok );
	m_mouseStart = m_invert.map( m_invertParentMatrix.map( point ) );
    }
    ~PartResizeHandlerPrivate() {}

    KoDocumentChild::Gadget m_gadget;
    QPoint m_mouseStart;
    QRect m_geometryStart;
    KoDocumentChild* m_child;
    QWMatrix m_invert;
    QWMatrix m_matrix;
    QWMatrix m_parentMatrix;
    QWMatrix m_invertParentMatrix;
};

PartResizeHandler::PartResizeHandler( QWidget* widget, const QWMatrix& matrix, KoDocumentChild* child,
				      KoDocumentChild::Gadget gadget, const QPoint& point )
    : EventHandler( widget )
{
    child->lock();
    d=new PartResizeHandlerPrivate(matrix, child, gadget, point);
}

PartResizeHandler::~PartResizeHandler()
{
    d->m_child->unlock();
    delete d;
    d=0L;
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
	QPoint p = d->m_invert.map( d->m_invertParentMatrix.map( e->pos() ) );
	QRegion rgn( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) );

	double x1_x, x1_y, x2_x, x2_y;
	d->m_matrix.map( double( p.x() ), 0.0, &x1_x, &x1_y );
	d->m_matrix.map( double( d->m_mouseStart.x() ), 0.0, &x2_x, &x2_y );		
	double y1_x, y1_y, y2_x, y2_y;
	d->m_matrix.map( 0.0, double( p.y() ), &y1_x, &y1_y );
	d->m_matrix.map( 0.0, double( d->m_mouseStart.y() ), &y2_x, &y2_y );		
		
	double dx = x2_x - x1_x;
	double dy = x2_y - x1_y;
	int x = int( sqrt( dx * dx + dy * dy ) * ( d->m_mouseStart.x() < p.x() ? 1.0 : -1.0 ) );
		
	dx = y2_x - y1_x;
	dy = y2_y - y1_y;
	int y = int( sqrt( dx * dx + dy * dy ) * ( d->m_mouseStart.y() < p.y() ? 1.0 : -1.0 ) );

	switch( d->m_gadget )
        {
	case KoDocumentChild::TopLeft:
	    {
		x = QMIN( d->m_geometryStart.width() - 1, x );
		y = QMIN( d->m_geometryStart.height() - 1, y );

		d->m_child->setGeometry( QRect( d->m_geometryStart.x() + x, d->m_geometryStart.y() + y,
					     d->m_geometryStart.width() - x, d->m_geometryStart.height() - y ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case KoDocumentChild::TopMid:
	    {
		y = QMIN( d->m_geometryStart.height() - 1, y );

		d->m_child->setGeometry( QRect( d->m_geometryStart.x(), d->m_geometryStart.y() + y,
					     d->m_geometryStart.width(), d->m_geometryStart.height() - y ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case KoDocumentChild::TopRight:
	    {
		x = QMAX( -d->m_geometryStart.width() + 1, x );
		y = QMIN( d->m_geometryStart.height() - 1, y );

		d->m_child->setGeometry( QRect( d->m_geometryStart.x(), d->m_geometryStart.y() + y,
					     d->m_geometryStart.width() + x, d->m_geometryStart.height() - y ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case KoDocumentChild::MidLeft:
	    {
		x = QMIN( d->m_geometryStart.width() - 1, x );
		
		d->m_child->setGeometry( QRect( d->m_geometryStart.x() + x, d->m_geometryStart.y(),
					     d->m_geometryStart.width() - x, d->m_geometryStart.height() ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case KoDocumentChild::MidRight:
	    {
		x = QMAX( -d->m_geometryStart.width() + 1, x );

		d->m_child->setGeometry( QRect( d->m_geometryStart.x(), d->m_geometryStart.y(),
					     d->m_geometryStart.width() + x, d->m_geometryStart.height() ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case KoDocumentChild::BottomLeft:
	    {
		x = QMIN( d->m_geometryStart.width() - 1, x );
		y = QMAX( -d->m_geometryStart.height() + 1, y );

		d->m_child->setGeometry( QRect( d->m_geometryStart.x() + x, d->m_geometryStart.y(),
					     d->m_geometryStart.width() - x, d->m_geometryStart.height() + y ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case KoDocumentChild::BottomMid:
	    {
		y = QMAX( -d->m_geometryStart.height() + 1, y );

		d->m_child->setGeometry( QRect( d->m_geometryStart.x(), d->m_geometryStart.y(),
					     d->m_geometryStart.width(), d->m_geometryStart.height() + y ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
	    }
	    break;
	case KoDocumentChild::BottomRight:
	    {
		x = QMAX( -d->m_geometryStart.width() + 1, x );
		y = QMAX( -d->m_geometryStart.height() + 1, y );

		d->m_child->setGeometry( QRect( d->m_geometryStart.x(), d->m_geometryStart.y(),
					     d->m_geometryStart.width() + x, d->m_geometryStart.height() + y ) );
		((QWidget*)target())->repaint( rgn.unite( d->m_child->frameRegion( d->m_parentMatrix, TRUE ) ) );
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

class PartMoveHandlerPrivate {
public:
    PartMoveHandlerPrivate( const QWMatrix& matrix, KoDocumentChild* child,
			    const QPoint& point) : m_dragChild(child),
						   m_parentMatrix(matrix) {					
	m_invertParentMatrix = matrix.invert();
	m_mouseDragStart = m_invertParentMatrix.map( point );
	m_geometryDragStart = m_dragChild->geometry();
	m_rotationDragStart = m_dragChild->rotationPoint();
    }
    ~PartMoveHandlerPrivate() {}

    KoDocumentChild* m_dragChild;
    QPoint m_mouseDragStart;
    QRect m_geometryDragStart;
    QPoint m_rotationDragStart;
    QWMatrix m_invertParentMatrix;
    QWMatrix m_parentMatrix;
};

PartMoveHandler::PartMoveHandler( QWidget* widget, const QWMatrix& matrix, KoDocumentChild* child,
				  const QPoint& point )
    : EventHandler( widget )
{
    child->lock();
    d=new PartMoveHandlerPrivate(matrix, child, point);
}

PartMoveHandler::~PartMoveHandler()
{
    d->m_dragChild->unlock();
    delete d;
    d=0L;
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
	
	QRegion bound = d->m_dragChild->frameRegion( d->m_parentMatrix, TRUE );
	QPoint pos = d->m_invertParentMatrix.map( e->pos() );
	d->m_dragChild->setGeometry( QRect( d->m_geometryDragStart.x() + pos.x() - d->m_mouseDragStart.x(),
					     d->m_geometryDragStart.y() + pos.y() - d->m_mouseDragStart.y(),
					     d->m_geometryDragStart.width(), d->m_geometryDragStart.height() ) );
	d->m_dragChild->setRotationPoint( QPoint( d->m_rotationDragStart.x() + pos.x() - d->m_mouseDragStart.x(),
					       d->m_rotationDragStart.y() + pos.y() - d->m_mouseDragStart.y() ) );
	((QWidget*)target())->repaint( bound.unite( d->m_dragChild->frameRegion( d->m_parentMatrix, TRUE ) ) );

	return TRUE;
    }

    return FALSE;
}

// -------------------------------------------------------

ContainerHandler::ContainerHandler( KoView* view, QWidget* widget )
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

	KoDocumentChild::Gadget gadget = KoDocumentChild::NoGadget;
	KoDocumentChild* child = m_view->selectedChild();
	if (child )
	    gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	if ( gadget == KoDocumentChild::NoGadget )
        {
	    child = m_view->activeChild();
	    if (child )
		gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	}
	
	if ( e->button() == LeftButton && gadget == KoDocumentChild::Move )
        {
	    (void)new PartMoveHandler( (QWidget*)target(), m_view->matrix(), child, e->pos() );
	    return TRUE;
	}
	else if ( e->button() == LeftButton && gadget != KoDocumentChild::NoGadget )
        {
	    (void)new PartResizeHandler( (QWidget*)target(), m_view->matrix(), child, gadget, e->pos() );
	    return TRUE;
	}

	return FALSE;
    }
    else if ( ev->type() == QEvent::MouseMove )
    {
	QMouseEvent* e = (QMouseEvent*)ev;

	KoDocumentChild* child = m_view->selectedChild();
	KoDocumentChild::Gadget gadget = KoDocumentChild::NoGadget;
	if ( child )
	    gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	if ( gadget == KoDocumentChild::NoGadget )
        {
	    child = m_view->activeChild();
	    if (child )
		gadget = child->gadgetHitTest( e->pos(), m_view->matrix() );
	}

	if ( gadget == KoDocumentChild::TopLeft || gadget == KoDocumentChild::BottomRight )
	    ((QWidget*)target())->setCursor( sizeFDiagCursor );
	else if ( gadget == KoDocumentChild::TopRight || gadget == KoDocumentChild::BottomLeft )
	    ((QWidget*)target())->setCursor( sizeBDiagCursor );	
	else if ( gadget == KoDocumentChild::TopMid || gadget == KoDocumentChild::BottomMid )
	    ((QWidget*)target())->setCursor( sizeVerCursor );
	else if ( gadget == KoDocumentChild::MidLeft || gadget == KoDocumentChild::MidRight )
	    ((QWidget*)target())->setCursor( sizeHorCursor );
	else if ( gadget == KoDocumentChild::Move )
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
