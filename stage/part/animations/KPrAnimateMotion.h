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

#ifndef KPRANIMATEMOTION_H
#define KPRANIMATEMOTION_H

#include "KPrAnimationBase.h"

#include <QPainterPath>
#include "stage_export.h"

class KoPathShape;

class STAGE_EXPORT KPrAnimateMotion : public KPrAnimationBase
{
public:
    explicit KPrAnimateMotion(KPrShapeAnimation *shapeAnimation);
    ~KPrAnimateMotion() override;

    bool loadOdf( const KoXmlElement &element, KoShapeLoadingContext &context ) override;
    bool saveOdf(KoPASavingContext & paContext) const override;
    void init(KPrAnimationCache *animationCache, int step) override;
    QPainterPath pathOutline() const;
    KoPathShape *path() const;
    KoPathShape *getPath(qreal zoom, const QSizeF &pageSize, bool absolutePosition = true) const;

    QSizeF currentPageSize() const;
    qreal currentZoom() const;

protected:
    void next(int currentTime) override;

private:
    KoPathShape *m_motionPath;
    mutable qreal m_currentZoom;
    mutable QSizeF m_currentPageSize;
    mutable QPointF m_currentPosition;
};

#endif // KPRANIMATEMOTION_H

