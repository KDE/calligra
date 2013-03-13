#include "CQTextDocumentCanvas.h"
#include "CQCanvasController.h"
#include "CQTextDocumentModel.h"

#include <KoDocument.h>
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWCanvasItem.h>
#include <KMimeType>
#include <KMimeTypeTrader>
#include <KDE/KDebug>
#include <KActionCollection>
#include <QGraphicsWidget>
#include <QTextDocument>

CQTextDocumentCanvas::CQTextDocumentCanvas()
    : m_canvasBase(0)
    , m_canvasController(0)
    , m_zoomController(0)
    , m_zoomMode(ZOOM_CONSTANT)
    , m_findText(new KoFindText(this))
    , m_documentModel(0)
{
    connect (m_findText, SIGNAL(updateCanvas()), SLOT(updateCanvas()));
    connect (m_findText, SIGNAL(matchFound(KoFindMatch)), SLOT(findMatchFound(KoFindMatch)));
    connect (m_findText, SIGNAL(noMatchFound()), SLOT(findNoMatchFound()));
}

bool CQTextDocumentCanvas::openFile(const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoPart *part = KMimeTypeTrader::createInstanceFromQuery<KoPart>(mimetype,
                      QLatin1String("Calligra/Part"), 0, QString(), QVariantList(), &error);

    if (!part) {
        kDebug() << "Doc can't be openend because" << error;
        return false;
    }

    KoDocument * document = static_cast<KWDocument*> (part->document());
    document->openUrl (KUrl (uri));

    m_canvasBase = dynamic_cast<KoCanvasBase*> (part->canvasItem());
    createAndSetCanvasControllerOn(m_canvasBase);
    createAndSetZoomController(m_canvasBase);
    updateZoomControllerAccordingToDocument(document);
    updateControllerWithZoomMode();

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    KWCanvasItem *kwCanvasItem = dynamic_cast<KWCanvasItem*>(m_canvasBase);
    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(kwCanvasItem ->shapeManager()->shapes(), texts);
    m_findText->setDocuments(texts);

    KWDocument *kwDocument = static_cast<KWDocument*>(document);
    m_documentModel = new CQTextDocumentModel(this, kwDocument, kwCanvasItem->shapeManager());
    emit documentModelChanged();

    return true;
}

void CQTextDocumentCanvas::setSource(const QString& source)
{
    m_source = source;
    openFile(source);
    emit sourceChanged();
}

QString CQTextDocumentCanvas::source() const
{
    return m_source;
}

CQTextDocumentCanvas::~CQTextDocumentCanvas()
{
}

void CQTextDocumentCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
            updateControllerWithZoomMode();
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void CQTextDocumentCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    m_canvasController = controller;
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQTextDocumentCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    m_zoomController = new KoZoomController(m_canvasController,
                                                            zoomHandler,
                                                            new KActionCollection(this));
}

void CQTextDocumentCanvas::setZoomMode(ZoomMode zoomMode)
{
    m_zoomMode = zoomMode;
    updateControllerWithZoomMode();
    emit zoomModeChanged();
}

CQTextDocumentCanvas::ZoomMode CQTextDocumentCanvas::zoomMode() const
{
    return m_zoomMode;
}

void CQTextDocumentCanvas::updateZoomControllerAccordingToDocument(const KoDocument* document)
{
    const KWDocument *kwDoc = static_cast<const KWDocument*>(document);
    m_zoomController->setPageSize (kwDoc->pageManager()->begin().rect().size());
}

void CQTextDocumentCanvas::updateControllerWithZoomMode()
{
    KoZoomMode::Mode zoomMode;
    switch (m_zoomMode) {
        case ZOOM_CONSTANT: zoomMode = KoZoomMode::ZOOM_CONSTANT; break;
        case ZOOM_PAGE: zoomMode = KoZoomMode::ZOOM_PAGE; break;
        case ZOOM_WIDTH: zoomMode = KoZoomMode::ZOOM_WIDTH; break;
    }
    m_zoomController->setZoom(zoomMode, 1.0);
}

QString CQTextDocumentCanvas::searchTerm() const
{
    return m_searchTerm;
}

void CQTextDocumentCanvas::setSearchTerm(const QString& term)
{
    m_searchTerm = term;
    if (!term.isEmpty()) {
        m_findText->find(term);
    }
    emit searchTermChanged();
}

void CQTextDocumentCanvas::findMatchFound(const KoFindMatch &match)
{
    QTextCursor cursor = match.location().value<QTextCursor>();
    m_canvasBase->canvasItem()->update();

    m_canvasBase->resourceManager()->setResource (KoText::CurrentTextAnchor, cursor.anchor());
    m_canvasBase->resourceManager()->setResource (KoText::CurrentTextPosition, cursor.position());
}

void CQTextDocumentCanvas::findNoMatchFound()
{
    kDebug() << "Match for " << m_searchTerm << " not found";
}

void CQTextDocumentCanvas::updateCanvas()
{
    KWCanvasItem* kwCanvasItem = dynamic_cast<KWCanvasItem*> (m_canvasBase);
    kwCanvasItem->update();
}

void CQTextDocumentCanvas::findNext()
{
    m_findText->findNext();
}

void CQTextDocumentCanvas::findPrevious()
{
    m_findText->findPrevious();
}

QObject* CQTextDocumentCanvas::documentModel() const
{
    return m_documentModel;
}

#include "CQTextDocumentCanvas.moc"