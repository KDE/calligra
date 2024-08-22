/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSpaceRotationFromBottomStrategy.h"
#include "KPrSpaceRotationEffectFactory.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPainter>
#include <QWidget>

KPrSpaceRotationFromBottomStrategy::KPrSpaceRotationFromBottomStrategy()
    : KPrPageEffectStrategy(KPrSpaceRotationEffectFactory::FromBottom, "spaceRotation", "bottomToTop", false, true)
{
}

KPrSpaceRotationFromBottomStrategy::~KPrSpaceRotationFromBottomStrategy() = default;

void KPrSpaceRotationFromBottomStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, 180);
    data.m_oldPageItem->show();
}

void KPrSpaceRotationFromBottomStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    Q_UNUSED(p);
    Q_UNUSED(currPos);
    Q_UNUSED(data);
}

void KPrSpaceRotationFromBottomStrategy::next(const KPrPageEffect::Data &data)
{
    int frame = data.m_timeLine.frameForTime(data.m_currentTime);
    if (frame >= data.m_timeLine.endFrame()) {
        finish(data);
    } else {
        data.m_oldPageItem->hide();
        data.m_newPageItem->hide();
        if (frame <= 90) {
            data.m_oldPageItem->show();
        } else {
            data.m_newPageItem->show();
        }

        m_transform.reset();
        int degree = (frame <= 90) ? frame : frame + 180;
        int h = data.m_widget->size().height() / 2;
        int w = data.m_widget->size().width() / 2;
        m_transform.translate(w, h).rotate(degree, Qt::XAxis).translate(-w, -h);
        if (frame <= 90) {
            data.m_oldPageItem->setTransform(m_transform);
        } else {
            data.m_newPageItem->setTransform(m_transform);
        }
    }
}

void KPrSpaceRotationFromBottomStrategy::finish(const KPrPageEffect::Data &data)
{
    data.m_graphicsView->hide();
    data.m_oldPageItem->hide();
    data.m_newPageItem->hide();
}
