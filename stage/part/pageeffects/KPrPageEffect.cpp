/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageEffect.h"

#include <QPainter>
#include <QWidget>

#include "KPrPageEffectStrategy.h"
#include <KoGenStyle.h>
#include <KoXmlWriter.h>

KPrPageEffect::KPrPageEffect(int duration, const QString &id, KPrPageEffectStrategy *strategy)
    : m_duration(duration)
    , m_id(id)
    , m_strategy(strategy)
{
    Q_ASSERT(strategy);
}

KPrPageEffect::~KPrPageEffect() = default;

void KPrPageEffect::setup(const Data &data, QTimeLine &timeLine)
{
    timeLine.setDuration(m_duration);
    m_strategy->setup(data, timeLine);
    timeLine.setEasingCurve(QEasingCurve(QEasingCurve::Linear));
}

bool KPrPageEffect::useGraphicsView()
{
    return m_strategy->useGraphicsView();
}

bool KPrPageEffect::paint(QPainter &p, const Data &data)
{
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);

    bool finish = data.m_finished;

    if (currPos >= data.m_timeLine.endFrame()) {
        finish = true;
    }

    if (!finish) {
        m_strategy->paintStep(p, currPos, data);
    } else {
        p.drawPixmap(0, 0, data.m_newPage);
    }

    return !finish;
}

void KPrPageEffect::next(const Data &data)
{
    m_strategy->next(data);
}

void KPrPageEffect::finish(const Data &data)
{
    m_strategy->finish(data);
}

int KPrPageEffect::duration() const
{
    return m_duration;
}

const QString &KPrPageEffect::id() const
{
    return m_id;
}

int KPrPageEffect::subType() const
{
    return m_strategy->subType();
}

void KPrPageEffect::saveOdfSmilAttributes(KoXmlWriter &xmlWriter) const
{
    qreal seconds = m_duration / qreal(1000.0);
    xmlWriter.addAttribute("smil:dur", seconds);
    return m_strategy->saveOdfSmilAttributes(xmlWriter);
}

void KPrPageEffect::saveOdfSmilAttributes(KoGenStyle &style) const
{
    QString speed("slow");
    if (m_duration < 2500) {
        speed = "fast";
    } else if (m_duration < 7500) {
        speed = "medium";
    }
    style.addProperty("presentation:transition-speed", speed);
    return m_strategy->saveOdfSmilAttributes(style);
}

void KPrPageEffect::loadOdf(const KoXmlElement & /*element*/)
{
}
