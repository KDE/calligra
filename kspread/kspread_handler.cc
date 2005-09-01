#include "kspread_handler.h"
#include "kspread_view.h"

#include <qpainter.h>
#define KS_MIN_RECTSIZE 3

KSpreadInsertHandler::KSpreadInsertHandler( KSpreadView* view, QWidget* widget, const KoDocumentEntry& entry, bool ischart )
    : EventHandler( widget ), m_entry( entry )
{
    m_isChart = ischart;
    m_view = view;
    m_started = false;
    m_clicked = false;

    widget->setCursor( crossCursor );
}

KSpreadInsertHandler::~KSpreadInsertHandler()
{
    ((QWidget*)target())->setCursor( arrowCursor );
    m_view->resetInsertHandle();
}

bool KSpreadInsertHandler::eventFilter( QObject*, QEvent* ev )
{
    if ( ev->type() == QEvent::MouseButtonPress )
    {
	QMouseEvent* e = (QMouseEvent*)ev;
	
	m_geometryStart = e->pos();
	m_geometryEnd = e->pos();
	m_started = false;
	m_clicked = true;
	
	return true;
    }
    else if ( ev->type() == QEvent::MouseMove )
    {
	if ( !m_clicked )
	    return true;
	
	QMouseEvent* e = (QMouseEvent*)ev;
	
	QPainter painter;
	painter.begin( (QWidget*)target() );
	painter.setRasterOp( NotROP );

	QPen pen;
	pen.setStyle( DashLine );
	painter.setPen( pen );

	if ( m_started )
        {
	    int x = m_geometryStart.x();
	    int y = m_geometryStart.y();
	    if ( x > m_geometryEnd.x() )
		x = m_geometryEnd.x();
	    if ( y > m_geometryEnd.y() )
		y = m_geometryEnd.y();
	    int w = m_geometryEnd.x() - m_geometryStart.x();
	    if ( w < 0 ) w *= -1;
	    int h = m_geometryEnd.y() - m_geometryStart.y();
	    if ( h < 0 ) h *= -1;

	    if ( w < KS_MIN_RECTSIZE ) w = KS_MIN_RECTSIZE;
	    if ( h < KS_MIN_RECTSIZE ) h = KS_MIN_RECTSIZE;

	    painter.drawRect( x, y, w, h );
	}
	else
	    m_started = true;

	m_geometryEnd = e->pos();

	int x = m_geometryStart.x();
	int y = m_geometryStart.y();
	if ( x > m_geometryEnd.x() )
	    x = m_geometryEnd.x();
	if ( y > m_geometryEnd.y() )
	    y = m_geometryEnd.y();
	int w = m_geometryEnd.x() - m_geometryStart.x();
	if ( w < 0 ) w *= -1;
	int h = m_geometryEnd.y() - m_geometryStart.y();
	if ( h < 0 ) h *= -1;

	if ( w < KS_MIN_RECTSIZE ) w = KS_MIN_RECTSIZE;
	if ( h < KS_MIN_RECTSIZE ) h = KS_MIN_RECTSIZE;

	painter.drawRect( x, y, w, h );
	painter.end();

	return true;
    }
    else if ( ev->type() == QEvent::MouseButtonRelease )
    {
	QMouseEvent* e = (QMouseEvent*)ev;
	
	if ( !m_started )
        {
	    delete this;
	    return true;
	}
	
	m_geometryEnd = e->pos();

	int x = m_geometryStart.x();
	int y = m_geometryStart.y();
	if ( x > m_geometryEnd.x() )
	    x = m_geometryEnd.x();
	if ( y > m_geometryEnd.y() )
	    y = m_geometryEnd.y();
	int w = m_geometryEnd.x() - m_geometryStart.x();
	if ( w < 0 ) w *= -1;
	int h = m_geometryEnd.y() - m_geometryStart.y();
	if ( h < 0 ) h *= -1;

	if ( w < KS_MIN_RECTSIZE ) w = KS_MIN_RECTSIZE;
	if ( h < KS_MIN_RECTSIZE ) h = KS_MIN_RECTSIZE;
        
	QPainter painter;
	painter.begin( (QWidget*)target() );

	QPen pen;
	pen.setStyle( DashLine );
	painter.setPen( pen );

	painter.setRasterOp( NotROP );
	painter.drawRect( x, y, w, h );
	painter.end();

	if ( m_isChart )
	    m_view->insertChart( QRect( x, y, w, h  ), m_entry );
	else
	    m_view->insertChild( QRect( x, y, w, h  ), m_entry );

	delete this;
	return true;
    }
    else if ( ev->type() == QEvent::KeyPress )
    {
	QKeyEvent* e = (QKeyEvent*)ev;
	if ( e->key() != Key_Escape )
	    return false;
	
	delete this;
	return true;
    }

    return false;
}

#include "kspread_handler.moc"
