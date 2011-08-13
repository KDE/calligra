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

#include "KPrSpaceRotationFromBottomStrategy.h"
#include "KPrSpaceRotationEffectFactory.h"

#include <QWidget>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>

KPrSpaceRotationFromBottomStrategy::KPrSpaceRotationFromBottomStrategy()
: KPrPageEffectStrategy(KPrSpaceRotationEffectFactory::FromBottom, "spaceRotation", "bottomToTop", false, true)
{
}

KPrSpaceRotationFromBottomStrategy::~KPrSpaceRotationFromBottomStrategy()
{
}

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
    }
    else {
        data.m_oldPageItem->hide();
        data.m_newPageItem->hide();
        if (frame <= 90) {
            data.m_oldPageItem->show();
        }
        else {
            data.m_newPageItem->show();
        }

        m_transform.reset();
        int degree = (frame <= 90) ? frame : frame + 180;
        int h = data.m_widget->size().height() / 2;
        int w = data.m_widget->size().width() / 2;
        m_transform.translate(w, h).rotate(degree, Qt::XAxis).translate(-w, -h);
        if (frame<=90) {
            data.m_oldPageItem->setTransform(m_transform);
        }
        else {
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

