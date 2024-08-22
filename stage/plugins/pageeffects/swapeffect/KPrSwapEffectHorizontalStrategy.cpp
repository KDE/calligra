/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSwapEffectHorizontalStrategy.h"
#include "KPrSwapEffectFactory.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPainter>
#include <QWidget>

KPrSwapEffectHorizontalStrategy::KPrSwapEffectHorizontalStrategy()
    : KPrPageEffectStrategy(KPrSwapEffectFactory::Horizontal, "swapEffect", "horizontal", false, true)
{
}

KPrSwapEffectHorizontalStrategy::~KPrSwapEffectHorizontalStrategy() = default;

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
    } else {
        qreal scaleMini = 0.3;
        int degreeMax = 25;

        qreal factor = (qreal)(data.m_timeLine.endFrame() - frame) / (data.m_timeLine.endFrame());
        qreal scaleFactor;
        qreal degree;
        if (factor < 0.5) {
            data.m_oldPageItem->setZValue(1);
            data.m_newPageItem->setZValue(2);
        } else {
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
        m_transform.translate(size.width() / 2, size.height() / 2).scale(scaleFactor, scaleFactor).translate(-size.width() / 2, -size.height() / 2);

        degree = (1 - factor) * degreeMax;
        m_transform.translate(size.width() / 2, size.height() / 2).rotate(degree, Qt::YAxis).translate(-size.width() / 2, -size.height() / 2);

        // Translate
        if (factor > 0.5) {
            m_transform.translate((1 - factor) * 2 * dxMax, 0);
        } else {
            m_transform.translate(factor * 2 * dxMax, 0);
        }
        data.m_oldPageItem->setTransform(m_transform);

        // Item 2
        size = data.m_newPageItem->pixmap().size();
        m_transform.reset();
        scaleFactor = scaleMini + (1 - factor) * (1 - scaleMini);
        m_transform.translate(size.width() / 2, size.height() / 2).scale(scaleFactor, scaleFactor).translate(-size.width() / 2, -size.height() / 2);

        degree = factor * degreeMax;
        m_transform.translate(size.width() / 2, size.height() / 2).rotate(-degree, Qt::YAxis).translate(-size.width() / 2, -size.height() / 2);

        if (factor > 0.5) {
            m_transform.translate(-(1 - factor) * 2 * dxMax, 0);
        } else {
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
