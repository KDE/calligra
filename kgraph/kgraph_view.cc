#include <qpainter.h>

//#include <kaction.h>
//#include <kstdaction.h>
#include <klocale.h>
//#include <kdebug.h>

#include <kgraph_global.h>
#include <kgraph_part.h>
#include <kgraph_view.h>


KGraphView::KGraphView(KGraphPart *part, QWidget *parent, const char *name)
    : KoView(part, parent, name) {
    
    setInstance(KGraphFactory::global());
    setXMLFile("kgraph.rc");
}

void KGraphView::paintEvent(QPaintEvent *ev) {

    QPainter painter;
    painter.begin(this);

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument()->paintEverything(painter, ev->rect(), false, this);

    painter.end();
}

void KGraphView::updateReadWrite(bool /*readwrite*/) {
}
#include <kgraph_view.moc>
