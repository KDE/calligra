/* This file is part of the KDE project
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrSwapEffectHorizontalStrategy.h"
#include "KPrSwapEffectFactory.h"

#include <QWidget>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>

KPrSwapEffectHorizontalStrategy::KPrSwapEffectHorizontalStrategy()
: KPrPageEffectStrategy(KPrSwapEffectFactory::Horizontal, "swapEffect", "horizontal", false, true)
{
}

KPrSwapEffectHorizontalStrategy::~KPrSwapEffectHorizontalStrategy()
{
}

void KPrSwapEffectHorizontalStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, 500);
    data.m_oldPageItem->show();
    data.m_newPageItem->show();
}

void KPrSwapEffectHorizontalStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    Q_UNUSED(p);
    Q_UNUSED(currPos);
    Q_UNUSED(data);
}

void KPrSwapEffectHorizontalStrategy::next(const KPrPageEffect::Data &data)
{
    int frame = data.m_timeLine.frameForTime(data.m_currentTime);
    if (frame >= data.m_timeLine.endFrame()) {
        finish(data);
    }
    else {
        qreal scaleMini= 0.3;
        int degreeMax = 25;

        qreal factor = (qreal)(data.m_timeLine.endFrame() - frame) / (data.m_timeLine.endFrame());
        qreal scaleFactor;
        qreal degree;
        if (factor<0.5) {
            data.m_oldPageItem->setZValue(1);
            data.m_newPageItem->setZValue(2);
        }
        else{
            data.m_oldPageItem->setZValue(2);
            data.m_newPageItem->setZValue(1);
        }

        data.m_oldPageItem->show();
        data.m_newPageItem->show();

        // Item 1
        QSize size = data.m_oldPageItem->pixmap().size();
        int dxMax = size.width() / 1.8;
        m_transform.reset();
        scaleFactor = 1 - (1 - factor) * (1 - scaleMini);
        m_transform.translate(size.width() / 2, size.height() / 2).scale(scaleFactor, scaleFactor)
                   .translate(-size.width() / 2, -size.height() / 2);

        degree = (1 - factor) * degreeMax;
        m_transform.translate(size.width() / 2, size.height() / 2).rotate(degree, Qt::YAxis)
                   .translate(-size.width() / 2, -size.height() / 2);

        // Translate
        if (factor > 0.5) {
            m_transform.translate((1 - factor) * 2 * dxMax, 0);
        }
        else {
            m_transform.translate(factor * 2 * dxMax, 0);
        }
        data.m_oldPageItem->setTransform(m_transform);

        // Item 2
        size = data.m_newPageItem->pixmap().size();
        m_transform.reset();
        scaleFactor = scaleMini+(1 - factor) * (1 - scaleMini);
        m_transform.translate(size.width() / 2, size.height() / 2).scale(scaleFactor, scaleFactor)
                   .translate(-size.width() / 2, -size.height() / 2);

        degree = factor * degreeMax;
        m_transform.translate(size.width() / 2, size.height() / 2).rotate(-degree, Qt::YAxis)
                   .translate(-size.width() / 2,-size.height() / 2);

        if (factor > 0.5) {
            m_transform.translate(-(1 - factor) * 2 * dxMax, 0);
        }
        else {
            m_transform.translate(-factor * 2 * dxMax, 0);
        }

        data.m_newPageItem->setTransform(m_transform);
    }
}

void KPrSwapEffectHorizontalStrategy::finish(const KPrPageEffect::Data &data)
{
    data.m_graphicsView->hide();
    data.m_oldPageItem->hide();
    data.m_newPageItem->hide();
}
