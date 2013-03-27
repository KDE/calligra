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

Page* PageItem::page() const
{
    return m_page;
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
    , m_doc(new Document())
    , m_width(0.0)
    , m_height(0.0)
    , m_margin(5.0)
    , m_spacing(10.0)
{
    connect(m_doc, SIGNAL(layoutFinished()), this, SLOT(slotLayoutFinished()));
}

DocumentItem::~DocumentItem()
{
    delete m_doc;
}

void DocumentItem::slotLayoutFinished()
{
    qDebug() << Q_FUNC_INFO;

    QList<Page*> pages = m_doc->pages();
    QList<QGraphicsItem *> pageItems = childItems();

    if (pageItems.count() > pages.count()) {
        for(int i = pageItems.count() - 1; i >= pages.count(); --i) {
            delete pageItems.takeLast();
        }
    }

    m_width = m_height = 0.0;
    for(int i = 0; i < pageItems.count(); ++i) {
        PageItem *pageItem = static_cast<PageItem*>(pageItems[i]);
        Page *page = pages[i];
        Q_ASSERT_X(pageItem->page() == page, __FUNCTION__, qPrintable(QString("hmmmm, pages out of sync :/ %1").arg(i)));
        Q_ASSERT_X(pageItem->page()->rect() != page->rect(), __FUNCTION__, qPrintable(QString("hmmmm, page boundaries out of sync :/ %1").arg(i)));
        m_height += page->rect().height();
        m_width = qMax(m_width, page->rect().width());
    }

    for(int i = pageItems.count(); i < pages.count(); ++i) {
        Page *page = pages[i];
        PageItem *pageItem = new PageItem(this, page);
        if (m_height > 0.0)
            m_height += m_spacing;
        pageItem->setPos(m_margin, m_margin + m_height);
        m_height += page->rect().height();
        m_width = qMax(m_width, page->rect().width());
    }

    Q_EMIT sizeChanged();

    //update();
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
