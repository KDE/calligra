/*
 *
 */

#include "LinkArea.h"

using namespace Calligra::Components;

class LinkArea::Private
{
public:
    Private()
        : document{ nullptr }
        , clickInProgress(false)
        , wiggleFactor(4)
    { }

    Calligra::Components::Document* document;

    bool clickInProgress;
    QPointF clickLocation;
    int wiggleFactor;
};

LinkArea::LinkArea(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
}

LinkArea::~LinkArea()
{
    delete d;
}

Calligra::Components::Document* LinkArea::document() const
{
    return d->document;
}

void LinkArea::setDocument(Calligra::Components::Document* newDocument)
{
    if( newDocument != d->document )
    {
        d->document = newDocument;
        emit documentChanged();
    }
}

void LinkArea::mousePressEvent(QMouseEvent* event)
{
    d->clickInProgress = true;
    d->clickLocation = event->pos();
}

void LinkArea::mouseReleaseEvent(QMouseEvent* event)
{
    d->clickInProgress = false;
    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(), QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if(!rect.contains(event->pos())) {
        return;
    }

    QUrl url;
    if( d->document )
        url = d->document->urlAtPoint( event->pos() );

    if(url.isEmpty()) {
        emit clicked();
    }
    else {
        emit linkClicked(url);
    }
    event->accept();
}

void LinkArea::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked();
}
