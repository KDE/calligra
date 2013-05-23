/*
 *
 */

#include "CQLinkArea.h"

#include <QDebug>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QPainter>

struct LinkLayerLink
{
    QRectF linkRect;
    QUrl linkTarget;
};

class CQLinkArea::Private
{
public:
    Private()
        : clickInProgress(false)
        , wiggleFactor(4)
    {}
    QVariantList links;
    QList<LinkLayerLink> realLinks;

    bool clickInProgress;
    QPointF clickLocation;
    int wiggleFactor;
};

CQLinkArea::CQLinkArea(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , d(new Private)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    setAcceptTouchEvents(true);
}

CQLinkArea::~CQLinkArea()
{
    delete d;
}

void CQLinkArea::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    painter->save();
    QColor thing(Qt::black);
    thing.setAlpha(100);
    painter->setPen(thing);
    foreach(const LinkLayerLink& link, d->realLinks) {
        QRectF inverted(link.linkRect.y(), link.linkRect.x(), link.linkRect.height(), link.linkRect.width());
        painter->drawRect(inverted);
    }
    painter->restore();
}

QVariantList CQLinkArea::links() const
{
    return d->links;
}

void CQLinkArea::setLinks(const QVariantList& newLinks)
{
    d->links = newLinks;
    // run through the new data and cache a data list with the information
    // so we don't have to interpret the QObjects all the time
    d->realLinks.clear();
    foreach(const QVariant& var, newLinks)
    {
        QObject* obj = var.value<QObject*>();
        if(!obj)
            continue;
        LinkLayerLink link;
        link.linkRect = obj->property("linkRect").toRectF().adjusted(-d->wiggleFactor, -d->wiggleFactor, d->wiggleFactor, d->wiggleFactor);
        link.linkTarget = obj->property("linkTarget").toUrl();
        d->realLinks.append(link);
    }
    emit linksChanged();
}

void CQLinkArea::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    d->clickInProgress = true;
    d->clickLocation = event->pos();
}

void CQLinkArea::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    d->clickInProgress = false;
    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(), QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if(!rect.contains(event->pos().toPoint())) {
        return;
    }
    QUrl url;
    QPointF inverted(event->pos().y(), event->pos().x());
    foreach(const LinkLayerLink& link, d->realLinks) {
        if(link.linkRect.contains(inverted)) {
            url = link.linkTarget;
            break;
        }
    }
    if(url.isEmpty()) {
        emit clicked();
    }
    else {
        emit linkClicked(url);
    }
}

void CQLinkArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    emit doubleClicked();
}
