#include "DocumentView.h"

#include <QDebug>

DocumentView::DocumentView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
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

}

DocumentView::~DocumentView()
{
}

bool DocumentView::loadFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
}

void DocumentView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qDebug() << Q_FUNC_INFO << newGeometry;
}
