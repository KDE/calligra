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
#include <KoPASavingContext.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoPathShape.h>
#include <KoPathShapeLoader.h>


KPrAnimateMotion::KPrAnimateMotion(KPrShapeAnimation *shapeAnimation)
: KPrAnimationBase(shapeAnimation)
, m_motionPath(new KoPathShape())
, m_currentZoom(1.0)
, m_currentPageSize(QSizeF(1.0, 1.0))
, m_currentPosition(QPointF(0.0, 0.0))
{
}

KPrAnimateMotion::~KPrAnimateMotion()
{
    delete m_motionPath;
}

bool KPrAnimateMotion::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    //<anim:animateMotion smil:dur="3s" smil:fill="hold" smil:targetElement="id1" svg:path="m0.0 0.0 1.0 0.5-1 0.5 1-1"/>
    QString path = element.attributeNS(KoXmlNS::svg, "path");
    if (!path.isEmpty()) {
        KoPathShapeLoader loader(m_motionPath);
        loader.parseSvg(path, true);
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

    KoPathShape *path = getPath(1.0, QSizeF(1, 1), false);
    writer.addAttribute("svg:path", path->toString());
    writer.endElement();
    return true;
}


void KPrAnimateMotion::init(KPrAnimationCache *animationCache, int step)
{
    QPainterPath path = m_motionPath->outline();
    if (m_fill == FillHold) {
        KoShape *shape = m_shapeAnimation->shape();
        m_animationCache = animationCache;
        QSizeF pageSize = m_animationCache->pageSize();
        QPointF endPoint = path.pointAtPercent(1);
        animationCache->init(step + 1, shape, m_shapeAnimation->textBlockUserData(), "transform", QTransform().translate(endPoint.x() * pageSize.width() * animationCache->zoom(), endPoint.y() * pageSize.height() * animationCache->zoom()));
    }
}

QPainterPath KPrAnimateMotion::pathOutline() const
{
    QPainterPath path = m_motionPath->outline();
    return path;
}

KoPathShape *KPrAnimateMotion::path() const
{
    return m_motionPath;
}

KoPathShape *KPrAnimateMotion::getPath(qreal zoom, const QSizeF &pageSize, bool absolutePosition) const
{
    QPointF point = m_motionPath->position();
    qreal xCorrection = pageSize.width() * zoom / (m_currentZoom * m_currentPageSize.width());
    qreal yCorrection = pageSize.height() * zoom / (m_currentZoom * m_currentPageSize.height());

    QPointF offset((point.x() - m_currentPosition.x()) * xCorrection,
                   (point.y() - m_currentPosition.y()) * yCorrection);

    if (absolutePosition) {
            m_motionPath->setPosition(QPointF(m_shapeAnimation->shape()->position().x() + m_shapeAnimation->shape()->size().width() / 2 + offset.x(),
                                          m_shapeAnimation->shape()->position().y() + m_shapeAnimation->shape()->size().height() / 2 + offset.y()));
            m_currentPosition = QPointF(m_shapeAnimation->shape()->position().x() + m_shapeAnimation->shape()->size().width() / 2,
                                        m_shapeAnimation->shape()->position().y() + m_shapeAnimation->shape()->size().height() / 2);
    }
    else {
        m_motionPath->setPosition(QPointF(offset.x(), offset.y()));
        m_currentPosition = QPointF(0, 0);
    }

    m_motionPath->setSize(QSizeF(m_motionPath->size().width() * xCorrection, m_motionPath->size().height() * yCorrection));
    m_currentZoom = zoom;
    m_currentPageSize = pageSize;
    return m_motionPath;
}

QSizeF KPrAnimateMotion::currentPageSize() const
{
    return m_currentPageSize;
}

qreal KPrAnimateMotion::currentZoom() const
{
    return m_currentZoom;
}

void KPrAnimateMotion::next(int currentTime)
{
    Q_ASSERT(m_animationCache);
    QPainterPath path = getPath(1.0, QSizeF(1, 1), false)->outline();
    QSizeF pageSize = m_animationCache->pageSize();
    QPointF point = path.pointAtPercent(qreal(currentTime)/qreal(animationDuration()));
    QPointF offset(point.x() * pageSize.width(), point.y() * pageSize.height());

    KoShape *shape = m_shapeAnimation->shape();
    QTransform transform;
    transform.translate(offset.x() * m_animationCache->zoom(), offset.y() * m_animationCache->zoom());
    m_animationCache->update(shape, m_shapeAnimation->textBlockUserData(), "transform", transform);
}
