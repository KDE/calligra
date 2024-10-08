/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoRowStyle.h"
#include <QActionEvent>

namespace
{
class BreakStyleMap : public QMap<KoRowStyle::BreakType, QString>
{
public:
    BreakStyleMap()
    {
        insert(KoRowStyle::NoBreak, QString());
        insert(KoRowStyle::AutoBreak, "auto");
        insert(KoRowStyle::ColumnBreak, "column");
        insert(KoRowStyle::PageBreak, "page");
    }
} breakStyleMap;

class KeepTogetherMap : public QMap<KoRowStyle::KeepTogetherType, QString>
{
public:
    KeepTogetherMap()
    {
        insert(KoRowStyle::DontKeepTogether, QString());
        insert(KoRowStyle::AutoKeepTogether, "auto");
        insert(KoRowStyle::AlwaysKeeptogether, "always");
    }
} keepTogetherMap;

const QString prefix = "row";
const char familyName[] = "table-row";
}

KOSTYLE_DECLARE_SHARED_POINTER_IMPL(KoRowStyle)

KoRowStyle::KoRowStyle()
    : KoStyle()
    , m_backgroundColor()
    , m_height(10)
    , m_heightType(ExactHeight)
    , m_breakAfter(NoBreak)
    , m_breakBefore(NoBreak)
    , m_keepTogether(DontKeepTogether)
{
}

KoRowStyle::~KoRowStyle() = default;

void KoRowStyle::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
}

QColor KoRowStyle::backgroundColor() const
{
    return m_backgroundColor;
}

void KoRowStyle::setBreakAfter(KoRowStyle::BreakType breakAfter)
{
    m_breakAfter = breakAfter;
}

KoRowStyle::BreakType KoRowStyle::breakAfter() const
{
    return m_breakAfter;
}

void KoRowStyle::setBreakBefore(KoRowStyle::BreakType breakBefore)
{
    m_breakBefore = breakBefore;
}

KoRowStyle::BreakType KoRowStyle::breakBefore() const
{
    return m_breakBefore;
}

void KoRowStyle::setHeightType(KoRowStyle::HeightType type)
{
    m_heightType = type;
}

void KoRowStyle::setHeight(qreal height)
{
    m_height = height;
}

qreal KoRowStyle::height() const
{
    return m_height;
}

void KoRowStyle::setKeepTogether(KoRowStyle::KeepTogetherType keepTogether)
{
    m_keepTogether = keepTogether;
}

KoRowStyle::KeepTogetherType KoRowStyle::keepTogether() const
{
    return m_keepTogether;
}

KoGenStyle::Type KoRowStyle::automaticstyleType() const
{
    return KoGenStyle::TableRowAutoStyle;
}

KoGenStyle::Type KoRowStyle::styleType() const
{
    return KoGenStyle::TableRowStyle;
}

QString KoRowStyle::defaultPrefix() const
{
    return prefix;
}

const char *KoRowStyle::styleFamilyName() const
{
    return familyName;
}

void KoRowStyle::prepareStyle(KoGenStyle &style) const
{
    if (m_breakAfter != NoBreak) {
        style.addProperty("fo:break-after", breakStyleMap.value(m_breakAfter));
    }
    if (m_breakBefore != NoBreak) {
        style.addProperty("fo:break-before", breakStyleMap.value(m_breakBefore));
    }

    if (m_keepTogether != DontKeepTogether) {
        style.addProperty("fo:keep-together", keepTogetherMap.value(m_keepTogether));
    }

    switch (m_heightType) {
    case MinimumHeight:
        style.addPropertyPt("style:min-row-height", m_height);
        break;
    case ExactHeight:
        style.addPropertyPt("style:row-height", m_height);
        break;
    case OptimalHeight:
        style.addProperty("style:use-optimal-row-height", "true");
        break;
    }
}
