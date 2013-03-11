#include "DocumentView.h"

#include <QDebug>

#include <kurl.h>
#include <KWPart.h>
#include <KWDocument.h>

DocumentView::DocumentView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_document(0)
{
    //setFlag(QGraphicsItem::ItemIsMovable, true);
    //setFlag(QGraphicsItem::ItemIsSelectable, true);
    //setFlag(QGraphicsItem::ItemIsFocusable, true);
    //setFlag(QGraphicsItem::ItemClipsToShape, true);
    //setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    //setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
    //setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    //setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    //setFlag(QGraphicsItem::ItemAcceptsInputMethod, true);
    //setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    KWPart *part = new KWPart(this);
    m_document = new KWDocument(part);
    Q_ASSERT(part->document());
}

DocumentView::~DocumentView()
{
}

bool DocumentView::loadFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
    KUrl url(file);
    return m_document->openUrl(url);
}

void DocumentView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qDebug() << Q_FUNC_INFO << newGeometry;
}
