/* This file is part of the KDE project
 *
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KPrFadeOverColorStrategy.h"
#include "KPrFadeEffectFactory.h"

#include <QWidget>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>

#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyle.h>
#include <kdebug.h>

KPrFadeOverColorStrategy::KPrFadeOverColorStrategy()
: KPrPageEffectStrategy(KPrFadeEffectFactory::FadeOverColor, "fade", "fadeOverColor", false, true)
, m_fadeColor(Qt::black)
{
}

KPrFadeOverColorStrategy::~KPrFadeOverColorStrategy()
{
}

void KPrFadeOverColorStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, 1000); // TODO might not be needed
    data.m_graphicsView->setBackgroundBrush(m_fadeColor);
    data.m_oldPageItem->setZValue(1);
    data.m_newPageItem->setZValue(2);
    data.m_newPageItem->setOpacity(0);
    data.m_oldPageItem->show();
    data.m_newPageItem->show();
}

void KPrFadeOverColorStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    Q_UNUSED(p);
    Q_UNUSED(currPos);
    Q_UNUSED(data);
}

void KPrFadeOverColorStrategy::next(const KPrPageEffect::Data &data)
{
    int frame = data.m_timeLine.frameForTime(data.m_currentTime);
    if (frame >= data.m_timeLine.endFrame()) {
        finish(data);
    }
    else {
        qreal value = 1 - (data.m_timeLine.valueForTime(data.m_currentTime) * qreal(2.0));
        if (value >= 0) {
            data.m_oldPageItem->setOpacity(value);
        }
        else {
            data.m_oldPageItem->hide();
            data.m_newPageItem->setOpacity(-value);
        }
    }
}

void KPrFadeOverColorStrategy::finish(const KPrPageEffect::Data &data)
{
    data.m_graphicsView->hide();
}

void KPrFadeOverColorStrategy::saveOdfSmilAttributes(KoXmlWriter & xmlWriter) const
{
    KPrPageEffectStrategy::saveOdfSmilAttributes(xmlWriter);
    xmlWriter.addAttribute("smil:fadeColor", m_fadeColor.name());
}

void KPrFadeOverColorStrategy::saveOdfSmilAttributes(KoGenStyle & style) const
{
    KPrPageEffectStrategy::saveOdfSmilAttributes(style);
    style.addProperty("smil:fadeColor", m_fadeColor.name());
}

void KPrFadeOverColorStrategy::loadOdfSmilAttributes(const KoXmlElement & element)
{
    // use black as default
    m_fadeColor.setNamedColor(element.attributeNS(KoXmlNS::smil, "fadeColor", "#000000"));
}
