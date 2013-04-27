#include "DocumentView.h"
#include "AppManager.h"
#include "Document.h"

#include <QPainter>
#include <QGraphicsProxyWidget>
#include <QPluginLoader>
#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>
#include <QStyleOptionGraphicsItem>
#include <QThreadPool>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QQueue>
#include <QDebug>

#include <kurl.h>

/**************************************************************************
 * PageItem
 */

PageItem::PageItem(DocumentItem *view, const QSharedPointer<Page> &page)
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

    connect(m_page.data(), SIGNAL(thumbnailFinished(QImage)), this, SLOT(slotThumbnailFinished(QImage)));
}

PageItem::~PageItem()
{
}

QSharedPointer<Page> PageItem::page() const
{
    return m_page;
}

QRectF PageItem::boundingRect() const
{
    QRectF r = QGraphicsPixmapItem::boundingRect();
    QRectF p = m_page->rect();
    r.setWidth(p.width());
    r.setHeight(p.height());
    return r;
}

void PageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (m_page->isDirty()) {
        m_page->setDirty(false);
        m_page->doc()->updatePage(m_page);
    }

    if (m_view->pageColor().isValid()) {
        painter->fillRect(option->rect, QBrush(m_view->pageColor()));
    }

    QGraphicsPixmapItem::paint(painter, option, widget);

    //painter->setPen(QPen(QColor("#ffffff")));
    //painter->drawRect(option->rect);
}

void PageItem::slotThumbnailFinished(const QImage &image)
{
    qDebug() << Q_FUNC_INFO << image.size();

    QPixmap pixmap = QPixmap::fromImage(image);

    QSize size = m_page->rect().size().toSize();
    if (image.size() != size) // for the case the page was rescaled in between
        pixmap = pixmap.scaled(size);

    setPixmap(pixmap);
}

/**************************************************************************
 * DocumentItem
 */

DocumentItem::DocumentItem(QObject *parentObject, QGraphicsItem *parentItem)
    : QObject(parentObject)
    , QGraphicsRectItem(parentItem)
    , m_doc(new Document())
    , m_originalWidth(0.0)
    , m_originalHeight(0.0)
    , m_width(0.0)
    , m_height(0.0)
    , m_margin(5.0)
    , m_spacing(5.0)
    , m_zoom(1.0)
    , m_zoomTemp(1.0)
    , m_zoomTempBegin(0)
{
    connect(m_doc, SIGNAL(layoutFinished()), this, SLOT(slotLayoutFinished()));
}

DocumentItem::~DocumentItem()
{
    delete m_doc;
}

void DocumentItem::setZoomBegin()
{
    qDebug() << Q_FUNC_INFO << m_zoomTempBegin+1;
    ++m_zoomTempBegin;
}

void DocumentItem::setZoomEnd()
{
    qDebug() << Q_FUNC_INFO << m_zoomTempBegin-1;
    if (m_zoomTempBegin > 0 && --m_zoomTempBegin == 0)
        setZoom(m_zoomTemp);
}

qreal DocumentItem::zoom() const
{
    return (m_zoomTempBegin > 0) ? m_zoomTemp : m_zoom;
}

void DocumentItem::setZoom(qreal factor)
{
    if (m_zoomTempBegin > 0) {
        m_zoomTemp = factor;
        setScale(m_zoomTemp / m_zoom);
        return;
    }

    //prepareGeometryChange();

    Q_FOREACH(QGraphicsItem *item, childItems()) {
        PageItem *pageItem = static_cast<PageItem*>(item);
        QSharedPointer<Page> page = pageItem->page();
        //pageItem->prepareGeometryChange();
        QRectF r = page->originalRect();
        r.setWidth(r.width() * factor);
        r.setHeight(r.height() * factor);
        page->setRect(r);
        QPixmap image = pageItem->pixmap();
        if (!image.isNull()) {
            pageItem->setPixmap(image.scaled(r.size().toSize()));
        }
    }

    m_zoom = factor;
    m_zoomTemp = 1.0;
    setScale(1.0);

    //m_doc->update(QRectF());

    updateSize();
    Q_EMIT sizeChanged();

    //this->update();
}

void DocumentItem::updateSize()
{
    m_width = m_height = 0.0;
    QList<QGraphicsItem *> pageItems = childItems();
    for(int i = 0; i < pageItems.count(); ++i) {
        PageItem *pageItem = static_cast<PageItem*>(pageItems[i]);
        QSharedPointer<Page> page = pageItem->page();
        if (m_height > 0.0)
            m_height += m_spacing * m_zoom;
        qreal margin = m_margin * m_zoom;
        pageItem->setPos(margin, (margin + m_height));
        //pageItem->itemChange()
        m_height += page->rect().height();
        m_width = qMax(m_width, page->rect().width());
    }
    if (m_originalWidth == 0.0 && m_originalHeight == 0.0) {
        m_originalWidth = m_width;
        m_originalHeight = m_height;
    }
}

void DocumentItem::slotLayoutFinished()
{
    qDebug() << Q_FUNC_INFO;
\
    prepareGeometryChange();

    QList< QSharedPointer<Page> > pages = m_doc->pages();
    QList<QGraphicsItem *> pageItems = childItems();

    // Remove all PageItem's that are to much
    if (pageItems.count() > pages.count()) {
        for(int i = pageItems.count() - 1; i >= pages.count(); --i) {
            delete pageItems.takeLast();
        }
    }

    // Remove all PageItem's that are invalid meanwhile
    for(int i = 0; i < pageItems.count(); ++i) {
        PageItem *pageItem = static_cast<PageItem*>(pageItems[i]);
        const QSharedPointer<Page> &page = pages[i];
        if (!pageItem->page() || pageItem->page().data() != pages[i].data() || pageItem->page()->rect() != page->rect()) {
            for(int k = pageItems.count() - 1; k >= i; --k) {
                delete pageItems.takeLast();
            }
            break;
        }
    }

    // Determinate the properly new height/width values
    updateSize();

    // Create new PageItem's
    for(int i = pageItems.count(); i < pages.count(); ++i) {
        const QSharedPointer<Page> &page = pages[i];
        QRectF r = page->originalRect();
        r.setWidth(r.width() * m_zoom);
        r.setHeight(r.height() * m_zoom);
        page->setRect(r);
        PageItem *pageItem = new PageItem(this, page);
        if (m_height > 0.0)
            m_height += m_spacing * m_zoom;
        qreal margin = m_margin * m_zoom;
        pageItem->setPos(margin, (margin + m_height));
        m_height += page->rect().height();
        m_width = qMax(m_width, page->rect().width());
    }

    Q_EMIT sizeChanged();
}

QRectF DocumentItem::boundingRect() const
{
    qreal width = m_width + m_margin*2;
    qreal height = m_height + m_margin*2;
    //qreal x = (width * scale() - width);
    //qreal y = (height * scale() - height);
//    qreal x = (width * m_zoom - width);
//    qreal y = (height * m_zoom - height);
    qreal x = 0;
    qreal y = 0;
//    width += (width * m_zoom);
//    height += (height * m_zoom);
    return QRectF(QPointF(x, y), QSizeF(width, height));
}

bool DocumentItem::openFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
    qDeleteAll(childItems());
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

    connect(m_doc->document(), SIGNAL(openFileFailed(QString,QString)), this, SIGNAL(openFileFailed(QString,QString)));
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

qreal DocumentView::pageMargin() const
{
    return m_doc->pageMargin();
}

void DocumentView::setPageMargin(qreal margin)
{
    m_doc->setPageMargin(margin);
    Q_EMIT pageMarginChanged();
}

qreal DocumentView::pageSpacing() const
{
    return m_doc->pageSpacing();
}

void DocumentView::setPageSpacing(qreal spacing)
{
    m_doc->setPageSpacing(spacing);
    Q_EMIT pageSpacingChanged();
}

QColor DocumentView::pageColor() const
{
    return m_doc->brush().color();
}

void DocumentView::setPageColor(const QColor &color)
{
    m_doc->setPageColor(color);
    Q_EMIT pageColorChanged();
}

QPointF DocumentView::pos() const
{
    return m_doc->pos();
}

void DocumentView::setPos(const QPointF &position)
{
    m_doc->setPos(position.x(), position.y());
}

void DocumentView::setZoomBegin()
{
    m_doc->setZoomBegin();
}

void DocumentView::setZoomEnd()
{
    m_doc->setZoomEnd();
}

qreal DocumentView::zoom() const
{
    return m_doc->zoom();
}

void DocumentView::setZoom(qreal factor)
{
    if (qFuzzyCompare(m_doc->zoom(), factor))
        return;
    m_doc->setZoom(factor);
    slotSizeChanged();
    Q_EMIT zoomChanged();
}

//bool DocumentView::isZoomToFit() const
//{
//    QSizeF size = m_doc->boundingRect().size();
//    return qFuzzyCompare(qMin(width() / size.width(), height() / size.height()), m_totalScaleFactor);
//}

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

QString DocumentView::file() const
{
    return m_doc->document()->file();
}

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
