#include <kis_tool_sioxselection.h>

#include <qpainter.h>

#include <kis_debug.h>
#include <klocale.h>

#include <KoCanvasController.h>
#include <KoPointerEvent.h>

#include <kis_canvas2.h>
#include <kis_cursor.h>
#include <kis_view2.h>


KisToolSioxSelection::KisToolSioxSelection(KoCanvasBase * canvas)
    : KisTool(canvas, KisCursor::arrowCursor()), m_canvas( dynamic_cast<KisCanvas2*>(canvas) )
{
    Q_ASSERT(m_canvas);
    setObjectName("tool_sioxselection");

}

KisToolSioxSelection::~KisToolSioxSelection()
{
}

void KisToolSioxSelection::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    // Add code here to initialize your tool when it got activated
    KisTool::activate(toolActivation, shapes);
}

void KisToolSioxSelection::deactivate()
{
    // Add code here to initialize your tool when it got deactivated
    KisTool::deactivate();
}

void KisToolSioxSelection::mousePressEvent(KoPointerEvent *event)
{
    event->ignore();
}


void KisToolSioxSelection::mouseMoveEvent(KoPointerEvent *event)
{
    event->ignore();
}

void KisToolSioxSelection::mouseReleaseEvent(KoPointerEvent *event)
{
    event->ignore();
}

void KisToolSioxSelection::paint(QPainter& gc, const KoViewConverter &converter)
{
    Q_UNUSED(gc);
    Q_UNUSED(converter);
}

// Uncomment if you have a configuration widget
// QWidget* KisToolSioxSelection::createOptionWidget()
// {
//     return 0;
// }
//
// QWidget* KisToolSioxSelection::optionWidget()
// {
//         return 0;
// }

#include "kis_tool_sioxselection.moc"
