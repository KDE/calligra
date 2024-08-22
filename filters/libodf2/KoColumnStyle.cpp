/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoColumnStyle.h"
#include <KoGenStyle.h>

namespace
{
class BreakStyleMap : public QMap<KoColumnStyle::BreakType, QString>
{
public:
    BreakStyleMap()
    {
        insert(KoColumnStyle::NoBreak, QString());
        insert(KoColumnStyle::AutoBreak, "auto");
        insert(KoColumnStyle::ColumnBreak, "column");
        insert(KoColumnStyle::PageBreak, "page");
    }
} breakStyleMap;

const QString prefix = "col";
const char familyName[] = "table-column";
}

KOSTYLE_DECLARE_SHARED_POINTER_IMPL(KoColumnStyle)

KoColumnStyle::KoColumnStyle()
    : KoStyle()
    , m_breakAfter(NoBreak)
    , m_breakBefore(NoBreak)
    , m_width(15)
    , m_widthType(ExactWidth)
{
}

KoColumnStyle::~KoColumnStyle() = default;

qreal KoColumnStyle::width() const
{
    return m_width;
}

void KoColumnStyle::setWidth(qreal width)
{
    m_width = width;
}

KoColumnStyle::BreakType KoColumnStyle::breakAfter() const
{
    return m_breakAfter;
}

void KoColumnStyle::setBreakAfter(KoColumnStyle::BreakType breakAfter)
{
    m_breakAfter = breakAfter;
}

KoColumnStyle::BreakType KoColumnStyle::breakBefore() const
{
    return m_breakBefore;
}

void KoColumnStyle::setBreakBefore(KoColumnStyle::BreakType breakBefore)
{
    m_breakBefore = breakBefore;
}

void KoColumnStyle::setWidthType(KoColumnStyle::WidthType type)
{
    m_widthType = type;
}

KoColumnStyle::WidthType KoColumnStyle::widthType() const
{
    return m_widthType;
}

QString KoColumnStyle::defaultPrefix() const
{
    return prefix;
}

KoGenStyle::Type KoColumnStyle::styleType() const
{
    return KoGenStyle::TableColumnStyle;
}

KoGenStyle::Type KoColumnStyle::automaticstyleType() const
{
    return KoGenStyle::TableColumnAutoStyle;
}

const char *KoColumnStyle::styleFamilyName() const
{
    return familyName;
}

void KoColumnStyle::prepareStyle(KoGenStyle &style) const
{
    if (m_breakAfter != NoBreak) {
        style.addProperty("fo:break-after", breakStyleMap.value(m_breakAfter));
    }
    if (m_breakBefore != NoBreak) {
        style.addProperty("fo:break-before", breakStyleMap.value(m_breakBefore));
    }

    switch (m_widthType) {
    case MinimumWidth:
        style.addPropertyPt("style:min-column-width", m_width);
        break;
    case ExactWidth:
        style.addPropertyPt("style:column-width", m_width);
        break;
    case OptimalWidth:
        style.addProperty("style:use-optimal-column-width", "true");
        break;
    }
}
