#include "example_view.h"
#include "example_global.h"
#include "example_part.h"

#include <qpainter.h>
#include <qaction.h>

ExampleView::ExampleView( ExamplePart* part, QWidget* parent, const char* name )
    : ContainerView( part, parent, name )
{
    m_cut = new QAction( tr("&Cut"), ExampleBarIcon("editcut"), 0, this, SLOT( cut() ),
			 actionCollection(), "cut");
}

void ExampleView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    part()->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}

void ExampleView::cut()
{
    qDebug("CUT called");
}

#include "example_view.moc"
