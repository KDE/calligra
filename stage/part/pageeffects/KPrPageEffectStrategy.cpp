/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageEffectStrategy.h"

#include <QWidget>

#include <KoGenStyle.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

KPrPageEffectStrategy::KPrPageEffectStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse, bool graphicsView)
    : m_subType(subType)
    , m_smilData(smilType, smilSubType, reverse)
    , m_graphicsView(graphicsView)
{
}

KPrPageEffectStrategy::~KPrPageEffectStrategy() = default;

int KPrPageEffectStrategy::subType() const
{
    return m_subType;
}

void KPrPageEffectStrategy::finish(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

void KPrPageEffectStrategy::saveOdfSmilAttributes(KoXmlWriter &xmlWriter) const
{
    xmlWriter.addAttribute("smil:type", m_smilData.type);
    xmlWriter.addAttribute("smil:subtype", m_smilData.subType);
    if (m_smilData.reverse) {
        xmlWriter.addAttribute("smil:direction", "reverse");
    }
}

void KPrPageEffectStrategy::saveOdfSmilAttributes(KoGenStyle &style) const
{
    style.addProperty("smil:type", m_smilData.type);
    style.addProperty("smil:subtype", m_smilData.subType);
    if (m_smilData.reverse) {
        style.addProperty("smil:direction", "reverse");
    }
}

void KPrPageEffectStrategy::loadOdfSmilAttributes(const KoXmlElement &element)
{
    Q_UNUSED(element);
}

const QString &KPrPageEffectStrategy::smilType() const
{
    return m_smilData.type;
}

const QString &KPrPageEffectStrategy::smilSubType() const
{
    return m_smilData.subType;
}

bool KPrPageEffectStrategy::reverse() const
{
    return m_smilData.reverse;
}

bool KPrPageEffectStrategy::useGraphicsView() const
{
    return m_graphicsView;
}
