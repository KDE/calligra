#include "kspread_handler.h"
#include "kspread_view.h"

#include <qpainter.h>

KSpreadInsertHandler::KSpreadInsertHandler( KSpreadView* view, QWidget* widget, const KoDocumentEntry& entry, bool ischart )
    : EventHandler( widget ), m_entry( entry )
{
    m_isChart = ischart;
    m_view = view;
    m_started = FALSE;
    m_clicked = FALSE;

    widget->setCursor( crossCursor );
}

KSpreadInsertHandler::~KSpreadInsertHandler()
{
    ((QWidget*)target())->setCursor( arrowCursor );
}

bool KSpreadInsertHandler::eventFilter( QObject*, QEvent* ev )
{
    if ( ev->type() == QEvent::MouseButtonPress )
    {
	qDebug("PRESS");
	QMouseEvent* e = (QMouseEvent*)ev;
	
	m_geometryStart = e->pos();
	m_geometryEnd = e->pos();
	m_started = FALSE;
	m_clicked = TRUE;
	
	return TRUE;
    }
    else if ( ev->type() == QEvent::MouseMove )
    {
	qDebug("MOVE");
	if ( !m_clicked )
	    return TRUE;
	
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
	
	    painter.drawRect( x, y, w, h );
	}
	else
	    m_started = TRUE;

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

	painter.drawRect( x, y, w, h );
	painter.end();

	return TRUE;
    }
    else if ( ev->type() == QEvent::MouseButtonRelease )
    {
	qDebug("RELEASE");
	QMouseEvent* e = (QMouseEvent*)ev;
	
	if ( !m_started )
        {
	    delete this;
	    return TRUE;
	}
	
	qDebug("RELEASE 2");
		
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
	return TRUE;
    }
    else if ( ev->type() == QEvent::KeyPress )
    {
	QKeyEvent* e = (QKeyEvent*)ev;
	if ( e->key() != Key_Escape )
	    return FALSE;
	
	delete this;
	return TRUE;
    }

    return FALSE;
}

#include "kspread_handler.moc"
