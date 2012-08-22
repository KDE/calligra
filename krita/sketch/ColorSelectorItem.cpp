/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Dan Leinir Turthra Jensen <admin@leinir.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "ColorSelectorItem.h"
#include <plugins/extensions/dockers/advancedcolorselector/kis_color_selector_component.h>
#include <plugins/extensions/dockers/advancedcolorselector/kis_color_selector_ring.h>
#include <plugins/extensions/dockers/advancedcolorselector/kis_color_selector_triangle.h>
#include <plugins/extensions/dockers/advancedcolorselector/kis_color_selector_simple.h>
#include <plugins/extensions/dockers/advancedcolorselector/kis_color_selector_wheel.h>
#include <kis_canvas2.h>
#include <kis_view2.h>
#include <kis_canvas_resource_provider.h>
#include <KoCanvasResourceManager.h>
#include <QPainter>

class ColorSelectorItem::Private
{
public:
    Private()
        : selector(new KisColorSelector)
        , view(0)
        , lastColorRole(KisColorSelectorBase::Foreground)
    {
        ring = new KisColorSelectorRing(selector);
        triangle = new KisColorSelectorTriangle(selector);
        slider = new KisColorSelectorSimple(selector);
        square = new KisColorSelectorSimple(selector);
        wheel = new KisColorSelectorWheel(selector);
        main = triangle;
        sub = ring;
    }

    KisColorSelector* selector;

    KisColorSelectorRing* ring;
    KisColorSelectorTriangle* triangle;
    KisColorSelectorSimple* slider;
    KisColorSelectorSimple* square;
    KisColorSelectorWheel* wheel;

    KisColorSelectorComponent* main;
    KisColorSelectorComponent* sub;

    KisView2* view;
    KisColorSelectorBase::ColorRole lastColorRole;
};

ColorSelectorItem::ColorSelectorItem(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , d(new Private)
{
    setFlag( QGraphicsItem::ItemHasNoContents, false );
}

ColorSelectorItem::~ColorSelectorItem()
{
    delete d;
}

void ColorSelectorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QRectF bounds = boundingRect();
    if(d->selector->configuration().subType==KisColorSelector::Ring) {
        d->ring->setGeometry(bounds.x(),bounds.y(),bounds.width(), bounds.height());
        if(d->selector->configuration().mainType==KisColorSelector::Triangle) {
            d->triangle->setGeometry(bounds.width()/2 - d->ring->innerRadius(),
                                     bounds.height()/2 - d->ring->innerRadius(),
                                     d->ring->innerRadius()*2,
                                     d->ring->innerRadius()*2);
        }
        else {
            int size = d->ring->innerRadius()*2/sqrt(2.);
            d->square->setGeometry(bounds.width()/2 - size/2,
                                   bounds.height()/2 - size/2,
                                   size,
                                   size);
        }
    }
    else {
        // type wheel and square
        if(d->selector->configuration().mainType==KisColorSelector::Wheel) {
            d->main->setGeometry(bounds.x(), bounds.y() + height()*0.1, bounds.width(), bounds.height()*0.9);
            d->sub->setGeometry( bounds.x(), bounds.y(),                bounds.width(), bounds.height()*0.1);
        }
        else {
            if(bounds.height()>bounds.width()) {
                d->main->setGeometry(bounds.x(), bounds.y() + bounds.height()*0.1, bounds.width(), bounds.height()*0.9);
                d->sub->setGeometry( bounds.x(), bounds.y(),                       bounds.width(), bounds.height()*0.1);
            }
            else {
                d->main->setGeometry(bounds.x(), bounds.y() + bounds.height()*0.1, bounds.width(), bounds.height()*0.9);
                d->sub->setGeometry( bounds.x(), bounds.y(),                       bounds.width(), bounds.height()*0.1);
            }
        }
    }
    if(d->view) {
        if (d->lastColorRole == KisColorSelectorBase::Foreground) {
            d->selector->setColor(d->view->resourceProvider()->resourceManager()->foregroundColor().toQColor());
        } else {
            d->selector->setColor(d->view->resourceProvider()->resourceManager()->backgroundColor().toQColor());
        }
    }

    d->main->paintEvent(painter);
    d->sub->paintEvent(painter);
}

bool ColorSelectorItem::sceneEvent(QEvent* event)
{
    return QDeclarativeItem::sceneEvent(event);
}

bool ColorSelectorItem::event(QEvent* event)
{
    return QDeclarativeItem::event(event);
}

void ColorSelectorItem::inputMethodEvent(QInputMethodEvent* event)
{
    QDeclarativeItem::inputMethodEvent(event);
}

QObject* ColorSelectorItem::view() const
{
    return d->view;
}

void ColorSelectorItem::setView(QObject* newView)
{
    d->view = qobject_cast<KisView2*>( newView );
    emit viewChanged();
}

#include "ColorSelectorItem.moc"
