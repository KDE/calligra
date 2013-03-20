#include "DocumentView.h"

#include <QGraphicsProxyWidget>
#include <QPluginLoader>
#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>
#include <QStyleOptionGraphicsItem>
#include <QThreadPool>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QDebug>

#include <kurl.h>
#include <kpluginfactory.h>

#include <KoStyleManager.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoCanvasBase.h>
#include <KoProgressProxy.h>

#include <KWPart.h>
#include <KWView.h>
#include <KWDocument.h>
#include <Words.h>
#include <KWCanvasItem.h>
#include <KWPage.h>
#include <KWPageManager.h>

/**************************************************************************
 * Manager
 */

class Manager : public QObject
{
public:
    QThreadPool *m_threadPool;
    QReadWriteLock m_threadLock;

    Manager() : QObject(), m_threadPool(new QThreadPool()) {
        initPlugins();
    }
    ~Manager() {
        QWriteLocker locker(&m_threadLock);
        delete m_threadPool;
    }

    KWPart* createPart() {
        //TODO reuse the KWPart-factory plugin rather then create manually

        KWPart *part = new KWPart(this);
        KWDocument *document = new KWDocument(part);
        document->setCheckAutoSaveFile(false);
        document->setAutoErrorHandlingEnabled(false);
        document->resourceManager()->setResource(KoText::StyleManager, QVariant::fromValue<KoStyleManager*>(new KoStyleManager(0)));
        part->setDocument(document);
        return part;
    }

private:
    QList<QObject*> m_plugins;

    void initPlugins() {
        qDebug() << "REGISTERED PLUGINS START";
        Q_FOREACH(QObject* o, QPluginLoader::staticInstances()) {
            qDebug() << ">>>>>>>>>> PLUGIN FACTORY" << o << o->metaObject()->className() << o->objectName();
            if (KPluginFactory *f = qobject_cast<KPluginFactory*>(o)) {
                QObject *plugin = f->create<QObject>(this);
                Q_ASSERT(plugin);
                if (plugin) {
                    m_plugins.append(plugin);
                    qDebug() << ">>>>>>>>>> PLUGIN INSTANCE" << plugin << plugin->metaObject()->className();
                }
            }

            /*
            if (KoShapeFactoryBase *shapeFactory = dynamic_cast<KoShapeFactoryBase*>(o)) {
                qDebug() << "Register ShapeFactory id=" << shapeFactory->id() << "name=" << shapeFactory->name();
                KoShapeRegistry::instance()->addFactory(shapeFactory); //TextShape_SHAPEID);
            }
            */
        }
        qDebug() << "REGISTERED PLUGINS END";
        Q_ASSERT_X(KoShapeRegistry::instance()->value(TextShape_SHAPEID), __FUNCTION__, "No TextShape-plugin loaded");
    }
};

Q_GLOBAL_STATIC(Manager, s_manager)

/**************************************************************************
 * Page
 */

class Page::Private
{
public:
    Document *m_doc;
    KWPage m_page;
    bool m_isDirty;
    Private(Document *doc, const KWPage &page) : m_doc(doc), m_page(page), m_isDirty(false) {}
};

Page::Page(Document *doc, const KWPage &page)
    : QObject(doc)
    , d(new Private(doc, page))
{
}

Page::~Page()
{
    delete d;
}

Document* Page::doc() const
{
    return d->m_doc;
}

const KWPage &Page::page() const
{
    return d->m_page;
}

QRectF Page::rect() const
{
    return d->m_page.rect();
}

int Page::pageNumber() const
{
    return d->m_page.pageNumber();
}

void Page::markDirty()
{
    d->m_isDirty = true;
}

void Page::maybeUpdateThumbnail()
{
    if (d->m_isDirty) {
        d->m_isDirty = false;
        forceUpdateThumbnail();
    }
}

void Page::forceUpdateThumbnail()
{
#if 0
    PageThread *thread = new PageThread(this);
    thread->setAutoDelete(true);
    connect(thread, SIGNAL(thumbnailFinished(QImage)), this, SIGNAL(thumbnailFinished(QImage)));
    s_manager()->m_threadPool->start(thread);
#else
    PageThread unthreaded(this);
    connect(&unthreaded, SIGNAL(thumbnailFinished(QImage)), this, SIGNAL(thumbnailFinished(QImage)));
    unthreaded.run();
#endif
}

/**************************************************************************
 * PageThread
 */

PageThread::PageThread(Page *page)
    : QObject()
    , QRunnable()
    , m_page(page)
{
}

void PageThread::run()
{
    struct PageThreadDebug {
        PageThreadDebug() { qDebug() << Q_FUNC_INFO << "START"; }
        ~PageThreadDebug() { qDebug() << Q_FUNC_INFO << "DONE"; }
    };
    PageThreadDebug debugHelper;

    QReadLocker locker(&s_manager()->m_threadLock);

    KWPage kwpage = m_page->page();
    KoPart *kopart = m_page->doc()->part();
    KWDocument *kwdoc = kopart ? dynamic_cast<KWDocument*>(kopart->document()) : 0;
    Q_ASSERT(kwdoc);
    if (!kwdoc)
        return;

    KWCanvasItem *canvasItem = static_cast<KWCanvasItem*>(kwdoc->documentPart()->canvasItem());
    Q_ASSERT(canvasItem);
    if (!canvasItem)
        return;

    KoShapeManager *shapeManager = canvasItem->shapeManager();

    const KWPage &page = kwpage;
    qDebug()<<"THREADDDDD PAGE="<<page.pageNumber();
    QRectF pageRect = page.rect();

    /*
    QRectF pRect(page.rect());
    KoPageLayout layout;
    layout.width = pRect.width();
    layout.height = pRect.height();

    KoPAUtil::setZoom(layout, thumbSize, zoomHandler);
    QRect pageRect = KoPAUtil::pageRect(layout, thumbSize, zoomHandler);
    */

    QSize pageSize = pageRect.size().toSize();
    QImage thumbnail(pageSize, QImage::Format_RGB32);
    thumbnail.fill(QColor(Qt::white).rgb());

    QImage img = page.thumbnail(pageSize, shapeManager);
    //QPainter imgPainter(&img);
    //imgPainter.setPen(QPen(Qt::black));
    //imgPainter.drawRect(QRect(0, 0, pageSize.width() - 1, pageSize.height() - 1));
    //imgPainter.end();

    QPainter painter(&thumbnail);
    painter.drawImage(QRectF(QPointF(0.0, 0.0), pageRect.size()), img);
    painter.end();

    Q_EMIT thumbnailFinished(thumbnail);
}

/**************************************************************************
 * Document
 */

class DocumentProgressProxy : public KoProgressProxy {
public:
    DocumentProgressProxy(Document *doc) : m_doc(doc) {}
    ~DocumentProgressProxy() {}
    int maximum() const { return 100; }
    void setValue(int value) { m_doc->emitProgressUpdated(value); }
    void setRange(int /*minimum*/, int /*maximum*/) {}
    void setFormat(const QString &/*format*/) {}
private:
    Document *m_doc;
};

class Document::Private
{
public:
    KoPart *m_part;
    QList<Page*> m_pages;
    DocumentProgressProxy *m_progressProxy;
    Private(Document *q) : m_part(0), m_progressProxy(new DocumentProgressProxy(q)) {}
};

Document::Document(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

Document::~Document()
{
    delete d->m_part;
    delete d->m_progressProxy;
    delete d;
}

KoPart* Document::part() const
{
    return d->m_part;
}

bool Document::openFile(const QString &file)
{
    qDeleteAll(d->m_pages);
    d->m_pages.clear();
    Q_EMIT pagesRemoved();

    delete d->m_part;
    d->m_part = s_manager()->createPart();
    if (!d->m_part)
        return false;

    d->m_progressProxy->setValue(0);
    d->m_part->document()->setProgressProxy(d->m_progressProxy);

    connect(d->m_part->document(), SIGNAL(pageSetupChanged()), this, SLOT(slotPageSetupChanged()));
    connect(d->m_part->document(), SIGNAL(layoutFinished()), this, SLOT(slotLayoutFinished()));

    KUrl url(file);
    bool ok = d->m_part->document()->openUrl(url);

    d->m_progressProxy->setValue(-1);

    return ok;
}

void Document::slotPageSetupChanged()
{
    qDebug() << Q_FUNC_INFO;

    KWDocument *kwdoc = dynamic_cast<KWDocument*>(d->m_part->document());
    Q_ASSERT(kwdoc);

#if 0
    while (!kwdoc->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();
        if (!QCoreApplication::hasPendingEvents())
            break;
    }
#endif

    KWPageManager *kwPageManager = kwdoc->pageManager();
    Q_ASSERT(kwPageManager);
    QList<Page*> pages;
    for(int i = d->m_pages.count() + 1; i <= kwPageManager->pageCount(); ++i) {
        qDebug() << "PAGE ADDED =>" << i;
        KWPage kwpage = kwPageManager->page(i);
        Q_ASSERT(kwpage.isValid());
        Page *page = new Page(this, kwpage);
        d->m_pages.append(page);
        pages.append(page);
    }
    if (!pages.isEmpty())
        Q_EMIT pagesAdded(pages);
}

void Document::slotLayoutFinished()
{
    qDebug() << "!!!!!!!!!!!!" << Q_FUNC_INFO;

#if 0
    static bool reentranceCheck = false;
    Q_ASSERT(!reentranceCheck);
    reentranceCheck = true;
    KWDocument *kwdoc = dynamic_cast<KWDocument*>(d->m_part->document());
    while (!kwdoc->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();
        if (!QCoreApplication::hasPendingEvents())
            break;
    }
    reentranceCheck = false;
#endif

    foreach(Page *page, d->m_pages)
        page-> markDirty();
    Q_EMIT layoutFinished();
}

/**************************************************************************
 * PageItem
 */

PageItem::PageItem(DocumentItem *view, Page *page)
    : QObject(view)
    , QGraphicsPixmapItem(view)
    , m_view(view)
    , m_page(page)
{
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    //setTransformationMode(Qt::SmoothTransformation);

    //QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    //effect->setBlurRadius(6);
    //pageItem->setGraphicsEffect(effect);

    connect(m_page, SIGNAL(thumbnailFinished(QImage)), this, SLOT(slotThumbnailFinished(QImage)));
}

PageItem::~PageItem()
{
}

QRectF PageItem::boundingRect() const
{
    QRectF r = QGraphicsPixmapItem::boundingRect();
    r.setSize(m_page->rect().size());
    return r;
}

void PageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    m_page->maybeUpdateThumbnail();

    QGraphicsPixmapItem::paint(painter, option, widget);

    painter->setPen(QPen(Qt::black));
    painter->drawRect(option->rect);
}

void PageItem::slotThumbnailFinished(const QImage &image)
{
    qDebug() << Q_FUNC_INFO << image.size();
    setPixmap(QPixmap::fromImage(image));
}

/**************************************************************************
 * DocumentItem
 */

DocumentItem::DocumentItem(QObject *parentObject, QGraphicsItem *parentItem)
    : QObject(parentObject)
    , QGraphicsRectItem(parentItem)
    , m_doc(new Document(this))
    , m_width(0.0)
    , m_height(0.0)
    , m_margin(5.0)
    , m_spacing(10.0)
{
    connect(m_doc, SIGNAL(pagesAdded(QList<Page*>)), this, SLOT(slotPagesAdded(QList<Page*>)));
    connect(m_doc, SIGNAL(pagesRemoved()), this, SLOT(slotPagesRemoved()));
    connect(m_doc, SIGNAL(layoutFinished()), this, SLOT(slotLayoutFinished()));
}

DocumentItem::~DocumentItem()
{
}

void DocumentItem::slotPagesAdded(QList<Page*> pages)
{
    qDebug() << Q_FUNC_INFO << pages;
    Q_FOREACH(Page *page, pages) {
        PageItem *pageItem = new PageItem(this, page);
        if (m_height > 0.0)
            m_height += m_spacing;
        pageItem->setPos(m_margin, m_margin + m_height);
        m_height += page->rect().height();
        m_width = qMax(m_width, page->rect().width());
    }
    Q_EMIT sizeChanged();
}

void DocumentItem::slotPagesRemoved()
{
    m_width = m_height = 0.0;
    qDeleteAll(childItems());
}

void DocumentItem::slotLayoutFinished()
{
    update();
}

QRectF DocumentItem::boundingRect() const
{
    qreal width = m_width + m_margin*2;
    qreal height = m_height + m_margin*2;
    qreal x = (width * scale() - width);
    qreal y = (height * scale() - height);
    return QRectF(QPointF(x, y), QSizeF(width, height));
}

bool DocumentItem::openFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
    bool ok = m_doc->openFile(file);
    return ok;
}

/**************************************************************************
 * DocumentView
 */

DocumentView::DocumentView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_doc(new DocumentItem(this, this))
{
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);
    //setSmooth(true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    connect(m_doc->document(), SIGNAL(progressUpdated(int)), this, SIGNAL(progressUpdated(int)));
    connect(m_doc, SIGNAL(sizeChanged()), this, SLOT(slotSizeChanged()));
}

DocumentView::~DocumentView()
{
}

QRectF DocumentView::boundingRect() const
{
//    QRectF r = QDeclarativeItem::boundingRect();
//    //QRectF r = m_doc->boundingRect();
//    return QRectF(0.0, 0.0, r.width(), r.height());

    return QDeclarativeItem::boundingRect();

//    QRectF r = m_doc->boundingRect();
//    r = mapRectFromItem(m_doc, r);
//    return r;
}

QPointF DocumentView::pos() const
{
    return m_doc->pos();
}

void DocumentView::setPos(const QPointF &position)
{
    m_doc->setPos(position.x(), position.y());
}

qreal DocumentView::zoom() const
{
    return m_doc->scale();
}

//bool DocumentView::isZoomToFit() const
//{
//    QSizeF size = m_doc->boundingRect().size();
//    return qFuzzyCompare(qMin(width() / size.width(), height() / size.height()), m_totalScaleFactor);
//}

void DocumentView::setZoom(qreal factor)
{
    m_doc->setScale(factor);
}

//void DocumentView::zoomToFit()
//{
//    QSizeF size = m_doc->boundingRect().size();
//    //qreal factor = qMin(width() / size.width(), height() / size.height());
//    qreal factor = width() / size.width();
//    zoomToCenter(factor);
//}

//void DocumentView::zoomToCenter(qreal factor)
//{
//    zoom(factor);
//    QRectF r = m_doc->mapRectToParent(m_doc->boundingRect());
//    QSizeF s = boundingRect().size();
//    qreal x = (s.width() - r.width()) / 2;
//    qreal y = (s.height() - r.height()) / 2;
//    m_doc->setPos(x, y);
//}

bool DocumentView::openFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
    bool ok = m_doc->openFile(file);
    return ok;
}

void DocumentView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qDebug() << Q_FUNC_INFO << newGeometry;

    //QRectF r(newGeometry);
    //r.moveTopLeft(QPointF(0.0, 0.0));
    //m_proxyWidget->setGeometry(r);
}

bool DocumentView::sceneEvent(QEvent *event)
{
#if 0
    switch (event->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd: {
            QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

            if (touchPoints.count() == 1 /* && m_borderMousePress == NoBorder */ ) {
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                qreal x = touchPoint0.pos().x() - touchPoint0.lastPos().x();
                qreal y = touchPoint0.pos().y() - touchPoint0.lastPos().y();
                QRectF r1 = boundingRect();
                QRectF r2 = m_doc->mapRectToParent(m_doc->boundingRect());
                if (r2.width() <= r1.width()) {
                    x = 0;
                } else if (x > 0) {
                    x = r2.left() >= r1.left() ? 0 : qMin(x, r1.left() - r2.left());
                } else {
                    x = r2.right() >= r1.right() ? qMin(x, r2.right() - r1.right()) : 0;
                }
                if (r2.height() <= r1.height()) {
                    y = 0;
                } else if (y > 0) {
                    y = r2.top() >= r1.top() ? 0 : qMin(y, r1.top() - r2.top());
                } else {
                    y = r2.bottom() >= r1.bottom() ? qMin(y, r2.bottom() - r1.bottom()) : 0;
                }
                m_doc->moveBy(x, y);
            } else if (touchPoints.count() == 2) {
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                qreal scaleFactor = QLineF(touchPoint0.pos(), touchPoint1.pos()).length() / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
                if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                    m_totalScaleFactor *= scaleFactor;
                    scaleFactor = 1;
                }

                QRectF r1 = boundingRect();
                QRectF r2 = m_doc->mapRectToParent(m_doc->boundingRect());
                m_doc->setScale(m_totalScaleFactor * scaleFactor);
                QRectF r3 = m_doc->mapRectToParent(m_doc->boundingRect());
                qreal x = (r2.width() - r3.width()) / 2;
                qreal y = (r2.height() - r3.height()) / 2;
                if (x > 0 && r1.width() < r3.width()) {
                    if (r3.left() + x > r1.left()) {
                        x = qMax<qreal>(0, r3.left() - r1.left());
                    } else if (r3.right() + x < r1.right()) {
                        x = qMax<qreal>(0, r1.right() - r3.right());
                    }
                }
                if (y > 0 && r1.height() < r3.height()) {
                    if (r3.top() + y > r1.top()) {
                        y = qMax<qreal>(0, r3.top() - r1.top());
                    } else if (r3.bottom() + y < r1.bottom()) {
                        y = qMax<qreal>(0, r1.bottom() - r3.bottom());
                    }
                }
                m_doc->moveBy(x, y);
            }
            return true;
        }
        case QEvent::GraphicsSceneMouseDoubleClick: {
            if (isZoomToFit())
                zoomToCenter(m_totalScaleFactor * 2.0);
            else
                zoomToFit();
            return true;
        }
        default: {
            break;
        }
    }
#endif
    return QDeclarativeItem::sceneEvent(event);
}

QVariant DocumentView::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QVariant result = QDeclarativeItem::itemChange(change, value);
    qDebug() << Q_FUNC_INFO << "change=" << change << "value=" << value << "result=" << result;
    return result;
}

void DocumentView::slotSizeChanged()
{
   QRectF r = m_doc->boundingRect();
   r = mapRectFromItem(m_doc, r);
   setImplicitWidth(r.width());
   setImplicitHeight(r.height());
}
