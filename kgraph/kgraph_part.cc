#include <qpainter.h>

//#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>

#include <kgraph_view.h>
#include <kgraph_shell.h>
#include <kgraph_factory.h>
#include <kgraph_part.h>


KGraphPart::KGraphPart(QObject *parent, const char *name, bool singleViewMode)
    : KoDocument(parent, name, singleViewMode) {

    //kDebugInfo(31000, "PART: vor setInstance()");
    //setInstance(KGraphFactory::global());
    //kDebugInfo(31000, "PART: nach setInstance()");
    //setXMLFile("kgraph.rc");
    //kDebugInfo(31000, "PART: nach setXMLFile()");

    KStdAction::cut(this, SLOT( a_editcut() ), actionCollection(), "editcut" );
    kDebugInfo(31000, "PART: cut action");
}

bool KGraphPart::initDoc() {
    // If nothing is loaded, do initialize here
    return true;
}

QCString KGraphPart::mimeType() const {
    return "application/x-kgraph";
}

KoView *KGraphPart::createView(QWidget *parent, const char *name) {

    KGraphView *view = new KGraphView(this, parent, name);
    addView(view);
    return view;
}

KoMainWindow *KGraphPart::createShell() {

    KoMainWindow *shell = new KGraphShell;
    shell->setRootDocument(this);
    shell->show();
    return shell;
}

void KGraphPart::paintContent(QPainter &painter, const QRect &rect, bool /*transparent*/) {

    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
    int left=rect.left()/20;
    int right=rect.right()/20+1;
    int top=rect.top()/20;
    int bottom=rect.bottom()/20+1;

    for(int x=left; x<right; ++x)
        painter.drawLine(x*20, top*20, x*20, bottom*20);
    for(int y=left; y<right; ++y)
        painter.drawLine(left*20, y*20, right*20, y*20);
}

void KGraphPart::a_editcut() {
    kDebugInfo(31000, "KGraphPart: a_editcut called");
}
#include <kgraph_part.moc>
