#include <koRuler.h>
#include <qpainter.h>
#include <kdebug.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vcanvas.h"

VCanvas::VCanvas( KarbonView* view, KarbonPart* part )
    : QScrollView( view, "VCanvas",
	Qt::WNorthWestGravity | Qt::WResizeNoErase | Qt::WRepaintNoErase),
	m_part(part), m_view(view), m_vRuler(0L), m_hRuler(0L)
{
    viewport()->setFocusPolicy(QWidget::StrongFocus);
    viewport()->setMouseTracking(true);
    setMouseTracking(true);
    setFocus();
    viewport()->setBackgroundMode(QWidget::NoBackground);
}

void
VCanvas::paintEvent( QPaintEvent* event )
{
    QPainter painter;
    painter.begin( this );

    // Let the document do the drawing
    m_part->paintEverything( painter, event->rect(), false, m_view );

    painter.end();
}

void
VCanvas::resizeEvent( QResizeEvent* /*event*/ )
{
}

#include <vcanvas.moc>
