/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoShapeLayer.h"
#include "KoShapeLoadingContext.h"
#include "KoShapeSavingContext.h"
#include "SimpleShapeContainerModel.h"
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QRectF>

#include <algorithm>

KoShapeLayer::KoShapeLayer()
    : KoShapeContainer(new SimpleShapeContainerModel())
{
    setSelectable(false);
}

KoShapeLayer::KoShapeLayer(KoShapeContainerModel *model)
    : KoShapeContainer(model)
{
    setSelectable(false);
}

bool KoShapeLayer::hitTest(const QPointF &position) const
{
    Q_UNUSED(position);
    return false;
}

QRectF KoShapeLayer::boundingRect() const
{
    QRectF bb;

    foreach (KoShape *shape, shapes()) {
        if (bb.isEmpty())
            bb = shape->boundingRect();
        else
            bb = bb.united(shape->boundingRect());
    }

    return bb;
}

void KoShapeLayer::saveOdf(KoShapeSavingContext &context) const
{
    QList<KoShape *> shapes = this->shapes();
    std::sort(shapes.begin(), shapes.end(), KoShape::compareShapeZIndex);

    foreach (KoShape *shape, shapes) {
        shape->saveOdf(context);
    }
}

bool KoShapeLayer::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // set layer name
    setName(element.attributeNS(KoXmlNS::draw, "name"));
    // layer locking
    setGeometryProtected(element.attributeNS(KoXmlNS::draw, "protected", "false") == "true");
    // layer visibility
    setVisible(element.attributeNS(KoXmlNS::draw, "display", "false") != "none");

    // add layer by name into shape context
    context.addLayer(this, name());

    return true;
}

void KoShapeLayer::paintComponent(QPainter &, const KoViewConverter &, KoShapePaintingContext &)
{
}
