#include "CTTextDocumentCanvas.h"
#include "CTCanvasController.h"

#include <KoDocument.h>
#include <KoPart.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KWDocument.h>
#include <KMimeType>
#include <KMimeTypeTrader>
#include <KDE/KDebug>
#include <KActionCollection>
#include <QGraphicsWidget>

CTTextDocumentCanvas::CTTextDocumentCanvas()
    : m_canvasBase(0)
    , m_canvasController(0)
{
}

bool CTTextDocumentCanvas::openFile(const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoPart *part = KMimeTypeTrader::createInstanceFromQuery<KoPart>(mimetype,
                      QLatin1String("CalligraPart"), 0, QString(), QVariantList(), &error);

    if (!part) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    KoDocument * document = static_cast<KWDocument*> (part->document());
    document->openUrl (KUrl (uri));

    m_canvasBase = dynamic_cast<KoCanvasBase*> (part->canvasItem());
    createAndSetCanvasControllerOn(m_canvasBase);

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    return true;
}

void CTTextDocumentCanvas::setSource(const QString& source)
{
    m_source = source;
    openFile(source);
    emit sourceChanged();
}

QString CTTextDocumentCanvas::source() const
{
    return m_source;
}

CTTextDocumentCanvas::~CTTextDocumentCanvas()
{
}

void CTTextDocumentCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void CTTextDocumentCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CTCanvasController *controller = new CTCanvasController(new KActionCollection(this));
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

#include "CTTextDocumentCanvas.moc"
