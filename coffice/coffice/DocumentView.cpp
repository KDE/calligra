#include "DocumentView.h"

#include <QGraphicsProxyWidget>
#include <QPluginLoader>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>
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
    Private(Document *doc, const KWPage &page) : m_doc(doc), m_page(page) {}
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

void Page::updateThumbnail()
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
    QPainter imgPainter(&img);
    imgPainter.setPen(QPen(Qt::black));
    imgPainter.drawRect(QRect(0, 0, pageSize.width() - 1, pageSize.height() - 1));
    imgPainter.end();

    QPainter painter(&thumbnail);
    painter.drawImage(QRectF(QPointF(0.0, 0.0), pageRect.size()), img);
    painter.end();

    Q_EMIT thumbnailFinished(thumbnail);
}

/**************************************************************************
 * Document
 */

class Document::Private
{
public:
    KoPart *m_part;
    QList<Page*> m_pages;
    Private() : m_part(0) {}
};

Document::Document(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

Document::~Document()
{
    delete d->m_part;
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

    if (!d->m_part) {
        d->m_part = s_manager()->createPart();
        if (!d->m_part)
            return false;

        connect(d->m_part->document(), SIGNAL(pageSetupChanged()), this, SLOT(slotPageSetupChanged()));
        connect(d->m_part->document(), SIGNAL(layoutFinished()), this, SLOT(slotLayoutFinished()));
    }

    KUrl url(file);
    bool ok = d->m_part->document()->openUrl(url);
    if (!ok)
        return false;

    return true;
}

void Document::slotPageSetupChanged()
{
    qDebug() << "!!!!!!!!!!!!" << Q_FUNC_INFO;

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
        page-> updateThumbnail();
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
    setTransformationMode(Qt::SmoothTransformation);

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
    //if (pixmap().isNull()) {
    //}
    //qDebug() << Q_FUNC_INFO << m_page->pageNumber();
    QGraphicsPixmapItem::paint(painter, option, widget);

    //painter->setPen(QPen(Qt::black));
    //painter->drawRect(option->rect);
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
    , m_margin(10.0)
{
    connect(m_doc, SIGNAL(pagesAdded(QList<Page*>)), this, SLOT(slotPagesAdded(QList<Page*>)));
    connect(m_doc, SIGNAL(pagesRemoved()), this, SLOT(slotPagesRemoved()));
}

DocumentItem::~DocumentItem()
{
}

void DocumentItem::slotPagesAdded(QList<Page*> pages)
{
    qDebug() << Q_FUNC_INFO << pages;
    Q_FOREACH(Page *page, pages) {
        PageItem *pageItem = new PageItem(this, page);
        m_height += m_margin;
        pageItem->setPos(m_margin, m_height);
        m_height += page->rect().height();
        m_width = qMax(m_width, page->rect().width());
    }
    Q_EMIT sizeChanged();
}

void DocumentItem::slotPagesRemoved()
{
    qDeleteAll(childItems());
    m_width = m_height = 0.0;
}

QRectF DocumentItem::boundingRect() const
{
    return QRectF(0.0, 0.0, m_width + m_margin*2, m_height + m_margin*2);
}

bool DocumentItem::openFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
    bool ok = m_doc->openFile(file);

    /*
    setImplicitWidth(maxWidth + margin * 2);
    setImplicitHeight(y);
    */

    return ok;
}

/**************************************************************************
 * DocumentView
 */

DocumentView::DocumentView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_doc(new DocumentItem(this, this))
{
    //setFlag(QGraphicsItem::ItemClipsToShape, true);
    //setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    connect(m_doc, SIGNAL(sizeChanged()), this, SLOT(slotSizeChanged()));
}

DocumentView::~DocumentView()
{
}

bool DocumentView::openFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
    return m_doc->openFile(file);
}

bool DocumentView::openFileWithDialog()
{
    QFileDialog *dlg = new QFileDialog();
    dlg->setAcceptMode(QFileDialog::AcceptOpen);
    dlg->setFileMode(QFileDialog::ExistingFile);
    dlg->setOption(QFileDialog::ReadOnly);
    dlg->setDirectory(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
    QStringList filters;
    filters << "OpenDocument Text Files (*.odt)"
            << "Any files (*)";
    dlg->setNameFilters(filters);
    bool ok = false;
    if (dlg->exec() && dlg && !dlg->selectedFiles().isEmpty()) {
        QString file = dlg->selectedFiles().first();
        if (!file.isEmpty())
            ok = openFile(file);
    }
    delete dlg;
    return ok;
}

void DocumentView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qDebug() << Q_FUNC_INFO << newGeometry;

    //QRectF r(newGeometry);
    //r.moveTopLeft(QPointF(0.0, 0.0));
    //m_proxyWidget->setGeometry(r);
}

void DocumentView::slotSizeChanged()
{
    QRectF r = m_doc->boundingRect();
    setImplicitWidth(r.width());
    setImplicitHeight(r.height());
}
