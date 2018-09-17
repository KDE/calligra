/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "Layout.h"

#include <QEvent>
#include <QPainter>
#include <QCoreApplication>

#include <KoShape.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>

#include <Utils.h>

struct Layout::Private : public KoShape {
    Private() : eventSent(false) {}
    Layout* self;
    QList<KoShape*> shapes;
    QString id;
    bool eventSent;
    void removeDependees();
    void triggerRelayout();
protected:
    virtual void shapeChanged(ChangeType type, KoShape * shape);
private:
    // Fake
    virtual void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &) {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
        qFatal("Shouldn't be called");
    }
    virtual bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context) {
        Q_UNUSED(element);
        Q_UNUSED(context);
        qFatal("Shouldn't be called");
        return false;
    }
    virtual void saveOdf(KoShapeSavingContext & context) const {
        Q_UNUSED(context);
        qFatal("Shouldn't be called");
    }

};

static int event_type_delayed_relayout = QEvent::registerEventType();

void Layout::Private::removeDependees()
{
    foreach(KoShape * shape, shapes) {
        shape->removeDependee(this);
    }
}

void Layout::Private::shapeChanged(ChangeType type, KoShape * shape)
{
    switch(type) {
    case PositionChanged:
    case RotationChanged:
    case ScaleChanged:
    case ShearChanged:
    case SizeChanged:
    case GenericMatrixChange:
        self->shapeGeometryChanged(shape);
        triggerRelayout();
        break;
    default:
        break;
    }
}

void Layout::Private::triggerRelayout()
{
    if(!eventSent) {
        QCoreApplication::postEvent(self, new QEvent(QEvent::Type(event_type_delayed_relayout)));
        eventSent = true;
    }
}


Layout::Layout(const QString& _id) : d(new Private)
{
    d->self = this;
    d->id = _id;
}

Layout::~Layout()
{
    d->removeDependees();
    delete d;
}

const QString& Layout::id() const
{
    return d->id;
}

void Layout::replaceLayout(Layout* layout)
{
    layout->d->removeDependees(); // Avoid both layout to fight for the shapes position
    addShapes(layout->d->shapes);
    d->triggerRelayout();
}

void Layout::addShapes(QList<KoShape*> _shapes)
{
    foreach(KoShape * shape, _shapes) {
        Q_ASSERT(!d->shapes.contains(shape));
        d->shapes.push_back(shape);
    }
    shapesAdded(_shapes);
    foreach(KoShape * shape, _shapes) {
        shape->addDependee(d);
    }
    d->triggerRelayout();
}

void Layout::addShape(KoShape* _shape)
{
    Q_ASSERT(!d->shapes.contains(_shape));
    d->shapes.push_back(_shape);
    shapeAdded(_shape);
    _shape->addDependee(d);
    d->triggerRelayout();
}

void Layout::removeShape(KoShape* _shape)
{
    _shape->removeDependee(d);
    d->shapes.removeAll(_shape);
    shapeRemoved(_shape);
    d->triggerRelayout();
}

void Layout::shapesAdded(QList<KoShape*> _shapes)
{
    foreach(KoShape * shape, _shapes) {
        shapeAdded(shape);
    }
}


QRectF Layout::boundingBox() const
{
    QRectF b;
    Utils::containerBoundRec(shapes(), b);
    return b;
}

const QList<KoShape*>& Layout::shapes() const
{
    return d->shapes;
}

bool Layout::event(QEvent * e)
{
    if(e->type() == event_type_delayed_relayout) {
        Q_ASSERT(d->eventSent);
        e->accept();
        relayout();
        d->eventSent = false;
        return true;
    } else {
        return QObject::event(e);
    }
}
