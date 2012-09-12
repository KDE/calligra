/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrAnimateMotion.h"

#include "KPrAnimationCache.h"
#include "KPrShapeAnimation.h"

#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoPathShape.h>
#include <KoPathShapeLoader.h>

KPrAnimateMotion::KPrAnimateMotion(KPrShapeAnimation *shapeAnimation)
: KPrAnimationBase(shapeAnimation)
{
}

KPrAnimateMotion::~KPrAnimateMotion()
{
}

bool KPrAnimateMotion::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    //<anim:animateMotion smil:dur="3s" smil:fill="hold" smil:targetElement="id1" svg:path="m0.0 0.0 1.0 0.5-1 0.5 1-1"/>
    QString path = element.attributeNS(KoXmlNS::svg, "path");
    if (!path.isEmpty()) {
        KoPathShape pathShape;
        KoPathShapeLoader loader(&pathShape);
        loader.parseSvg(path, true);
        m_path = pathShape.outline();
        qDebug() << Q_FUNC_INFO << m_path << path;
    }
    KPrAnimationBase::loadOdf(element, context);

    return true;
}

bool KPrAnimateMotion::saveOdf(KoPASavingContext & paContext) const
{
    Q_UNUSED(paContext);
    KoXmlWriter &writer = paContext.xmlWriter();
    writer.startElement("anim:animateMotion");
    saveAttribute(paContext);

    KoPathShape *path = KoPathShape::createShapeFromPainterPath(m_path);
    // KoPathShape::createShapeFromPainterPath normalizes the path. Therefore we need to transform the string by the offset of the path
    QPointF offset = path->position();
    QTransform transform;
    transform.translate(offset.x(), offset.y());
    writer.addAttribute("svg:path", path->toString(transform));
    delete path;
    writer.endElement();
    return true;
}


void KPrAnimateMotion::init(KPrAnimationCache *animationCache, int step)
{
    if (m_fill == FillHold) {
        KoShape *shape = m_shapeAnimation->shape();
        m_animationCache = animationCache;
        QSizeF pageSize = m_animationCache->pageSize();
        QPointF endPoint = m_path.pointAtPercent(1);
        animationCache->init(step + 1, shape, m_shapeAnimation->textBlockData(), "transform", QTransform().translate(endPoint.x() * pageSize.width() * animationCache->zoom(), endPoint.y() * pageSize.height() * animationCache->zoom()));
    }
}

void KPrAnimateMotion::next(int currentTime)
{
    Q_ASSERT(m_animationCache);
    QSizeF pageSize = m_animationCache->pageSize();
    QPointF point = m_path.pointAtPercent(qreal(currentTime)/qreal(animationDuration()));
    QPointF offset(point.x() * pageSize.width(), point.y() * pageSize.height());
    qDebug() << Q_FUNC_INFO << currentTime << point << offset;

    KoShape *shape = m_shapeAnimation->shape();
    QTransform transform;
    transform.translate(offset.x() * m_animationCache->zoom(), offset.y() * m_animationCache->zoom());
    m_animationCache->update(shape, m_shapeAnimation->textBlockData(), "transform", transform);
}
