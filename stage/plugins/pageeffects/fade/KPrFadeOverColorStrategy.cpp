/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrFadeOverColorStrategy.h"
#include "KPrFadeEffectFactory.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPainter>
#include <QWidget>

#include <KoGenStyle.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

KPrFadeOverColorStrategy::KPrFadeOverColorStrategy()
    : KPrPageEffectStrategy(KPrFadeEffectFactory::FadeOverColor, "fade", "fadeOverColor", false, true)
    , m_fadeColor(Qt::black)
{
}

KPrFadeOverColorStrategy::~KPrFadeOverColorStrategy() = default;

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
    } else {
        qreal value = 1 - (data.m_timeLine.valueForTime(data.m_currentTime) * qreal(2.0));
        if (value >= 0) {
            data.m_oldPageItem->setOpacity(value);
        } else {
            data.m_oldPageItem->hide();
            data.m_newPageItem->setOpacity(-value);
        }
    }
}

void KPrFadeOverColorStrategy::finish(const KPrPageEffect::Data &data)
{
    data.m_graphicsView->hide();
}

void KPrFadeOverColorStrategy::saveOdfSmilAttributes(KoXmlWriter &xmlWriter) const
{
    KPrPageEffectStrategy::saveOdfSmilAttributes(xmlWriter);
    xmlWriter.addAttribute("smil:fadeColor", m_fadeColor.name());
}

void KPrFadeOverColorStrategy::saveOdfSmilAttributes(KoGenStyle &style) const
{
    KPrPageEffectStrategy::saveOdfSmilAttributes(style);
    style.addProperty("smil:fadeColor", m_fadeColor.name());
}

void KPrFadeOverColorStrategy::loadOdfSmilAttributes(const KoXmlElement &element)
{
    // use black as default
    m_fadeColor.setNamedColor(element.attributeNS(KoXmlNS::smil, "fadeColor", "#000000"));
}
