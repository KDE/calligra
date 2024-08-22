/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006-2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoSelection.h"
#include "KoPointerEvent.h"
#include "KoSelection_p.h"
#include "KoShapeContainer.h"
#include "KoShapeGroup.h"
#include "KoShapePaintingContext.h"

#include <QPainter>
#include <QTimer>

QRectF KoSelectionPrivate::sizeRect()
{
    bool first = true;
    QRectF bb;

    QTransform invSelectionTransform = q->absoluteTransformation(nullptr).inverted();

    QRectF bound;

    if (!selectedShapes.isEmpty()) {
        QList<KoShape *>::const_iterator it = selectedShapes.constBegin();
        for (; it != selectedShapes.constEnd(); ++it) {
            if (dynamic_cast<KoShapeGroup *>(*it))
                continue;

            const QTransform shapeTransform = (*it)->absoluteTransformation(nullptr);
            const QRectF shapeRect(QRectF(QPointF(), (*it)->size()));

            if (first) {
                bb = (shapeTransform * invSelectionTransform).mapRect(shapeRect);
                bound = shapeTransform.mapRect(shapeRect);
                first = false;
            } else {
                bb = bb.united((shapeTransform * invSelectionTransform).mapRect(shapeRect));
                bound = bound.united(shapeTransform.mapRect(shapeRect));
            }
        }
    }

    globalBound = bound;
    return bb;
}

void KoSelectionPrivate::requestSelectionChangedEvent()
{
    if (eventTriggered)
        return;
    eventTriggered = true;
    QTimer::singleShot(0, q, [this]() {
        selectionChangedEvent();
    });
}

void KoSelectionPrivate::selectionChangedEvent()
{
    eventTriggered = false;
    Q_EMIT q->selectionChanged();
}

void KoSelectionPrivate::selectGroupChildren(KoShapeGroup *group)
{
    if (!group)
        return;

    foreach (KoShape *shape, group->shapes()) {
        if (selectedShapes.contains(shape))
            continue;
        selectedShapes << shape;

        KoShapeGroup *childGroup = dynamic_cast<KoShapeGroup *>(shape);
        if (childGroup)
            selectGroupChildren(childGroup);
    }
}

void KoSelectionPrivate::deselectGroupChildren(KoShapeGroup *group)
{
    if (!group)
        return;

    foreach (KoShape *shape, group->shapes()) {
        if (selectedShapes.contains(shape))
            selectedShapes.removeAll(shape);

        KoShapeGroup *childGroup = dynamic_cast<KoShapeGroup *>(shape);
        if (childGroup)
            deselectGroupChildren(childGroup);
    }
}

////////////

KoSelection::KoSelection()
    : KoShape(*(new KoSelectionPrivate(this)))
{
}

KoSelection::~KoSelection() = default;

void KoSelection::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
    Q_UNUSED(paintcontext);
}

void KoSelection::select(KoShape *shape, bool recursive)
{
    Q_D(KoSelection);
    Q_ASSERT(shape != this);
    Q_ASSERT(shape);
    if (!shape->isSelectable() || !shape->isVisible(true))
        return;

    // save old number of selected shapes
    int oldSelectionCount = d->selectedShapes.count();

    if (!d->selectedShapes.contains(shape))
        d->selectedShapes << shape;

    // automatically recursively select all child shapes downwards in the hierarchy
    KoShapeGroup *group = dynamic_cast<KoShapeGroup *>(shape);
    if (group)
        d->selectGroupChildren(group);

    if (recursive) {
        // recursively select all parents and their children upwards the hierarchy
        KoShapeContainer *parent = shape->parent();
        while (parent) {
            KoShapeGroup *parentGroup = dynamic_cast<KoShapeGroup *>(parent);
            if (!parentGroup)
                break;
            if (!d->selectedShapes.contains(parentGroup)) {
                d->selectedShapes << parentGroup;
                d->selectGroupChildren(parentGroup);
            }
            parent = parentGroup->parent();
        }
    }

    if (d->selectedShapes.count() == 1) {
        setTransformation(shape->absoluteTransformation(nullptr));
        updateSizeAndPosition();
    } else {
        // reset global bound if there were no shapes selected before
        if (!oldSelectionCount)
            d->globalBound = QRectF();

        setTransformation(QTransform());
        // we are resetting the transformation here anyway,
        // so we can just add the newly selected shapes to the bounding box
        // in document coordinates and then use that size and position
        int newSelectionCount = d->selectedShapes.count();
        for (int i = oldSelectionCount; i < newSelectionCount; ++i) {
            KoShape *shape = d->selectedShapes[i];

            // don't add the rect of the group rect, as it can be invalid
            if (dynamic_cast<KoShapeGroup *>(shape)) {
                continue;
            }
            const QTransform shapeTransform = shape->absoluteTransformation(nullptr);
            const QRectF shapeRect(QRectF(QPointF(), shape->size()));

            d->globalBound = d->globalBound.united(shapeTransform.mapRect(shapeRect));
        }
        setSize(d->globalBound.size());
        setPosition(d->globalBound.topLeft());
    }

    d->requestSelectionChangedEvent();
}

void KoSelection::deselect(KoShape *shape, bool recursive)
{
    Q_D(KoSelection);
    if (!d->selectedShapes.contains(shape))
        return;

    d->selectedShapes.removeAll(shape);

    KoShapeGroup *group = dynamic_cast<KoShapeGroup *>(shape);
    if (recursive) {
        // recursively find the top group upwards int the hierarchy
        KoShapeGroup *parentGroup = dynamic_cast<KoShapeGroup *>(shape->parent());
        while (parentGroup) {
            group = parentGroup;
            parentGroup = dynamic_cast<KoShapeGroup *>(parentGroup->parent());
        }
    }
    if (group)
        d->deselectGroupChildren(group);

    if (count() == 1)
        setTransformation(firstSelectedShape()->absoluteTransformation(nullptr));

    updateSizeAndPosition();

    d->requestSelectionChangedEvent();
}

void KoSelection::deselectAll()
{
    Q_D(KoSelection);
    // reset the transformation matrix of the selection
    setTransformation(QTransform());

    if (d->selectedShapes.isEmpty())
        return;
    d->selectedShapes.clear();
    d->requestSelectionChangedEvent();
}

int KoSelection::count() const
{
    Q_D(const KoSelection);
    int count = 0;
    foreach (KoShape *shape, d->selectedShapes)
        if (dynamic_cast<KoShapeGroup *>(shape) == nullptr)
            ++count;
    return count;
}

bool KoSelection::hitTest(const QPointF &position) const
{
    Q_D(const KoSelection);
    if (count() > 1) {
        QRectF bb(boundingRect());
        return bb.contains(position);
    } else if (count() == 1) {
        return (*d->selectedShapes.begin())->hitTest(position);
    } else { // count == 0
        return false;
    }
}
void KoSelection::updateSizeAndPosition()
{
    Q_D(KoSelection);
    QRectF bb = d->sizeRect();
    QTransform matrix = absoluteTransformation(nullptr);
    setSize(bb.size());
    QPointF p = matrix.map(bb.topLeft() + matrix.inverted().map(position()));
    setPosition(p);
}

QRectF KoSelection::boundingRect() const
{
    return absoluteTransformation(nullptr).mapRect(QRectF(QPointF(), size()));
}

const QList<KoShape *> KoSelection::selectedShapes(KoFlake::SelectionType strip) const
{
    Q_D(const KoSelection);
    QList<KoShape *> answer;
    // strip the child objects when there is also a parent included.
    bool doStripping = strip == KoFlake::StrippedSelection;
    foreach (KoShape *shape, d->selectedShapes) {
        KoShapeContainer *container = shape->parent();
        if (strip != KoFlake::TopLevelSelection && dynamic_cast<KoShapeGroup *>(shape))
            // since a KoShapeGroup
            // guarantees all its children are selected at the same time as itself
            // is selected we will only return its children.
            continue;
        bool add = true;
        while (doStripping && add && container) {
            if (dynamic_cast<KoShapeGroup *>(container) == nullptr && d->selectedShapes.contains(container))
                add = false;
            container = container->parent();
        }
        if (strip == KoFlake::TopLevelSelection && container && d->selectedShapes.contains(container))
            add = false;
        if (add)
            answer << shape;
    }
    return answer;
}

bool KoSelection::isSelected(const KoShape *shape) const
{
    Q_D(const KoSelection);
    if (shape == this)
        return true;

    foreach (KoShape *s, d->selectedShapes) {
        if (s == shape)
            return true;
    }

    return false;
}

KoShape *KoSelection::firstSelectedShape(KoFlake::SelectionType strip) const
{
    QList<KoShape *> set = selectedShapes(strip);
    if (set.isEmpty())
        return nullptr;
    return *(set.begin());
}

void KoSelection::setActiveLayer(KoShapeLayer *layer)
{
    Q_D(KoSelection);
    d->activeLayer = layer;
    Q_EMIT currentLayerChanged(layer);
}

KoShapeLayer *KoSelection::activeLayer() const
{
    Q_D(const KoSelection);
    return d->activeLayer;
}

void KoSelection::saveOdf(KoShapeSavingContext &) const
{
}

bool KoSelection::loadOdf(const KoXmlElement &, KoShapeLoadingContext &)
{
    return true;
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoSelection.cpp"
