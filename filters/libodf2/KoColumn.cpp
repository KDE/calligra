/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoColumn.h"
#include "KoCellStyle.h"
#include "KoColumnStyle.h"

#include <KoXmlWriter.h>

#include <QString>

namespace
{
const QMap<KoColumn::Visibility, QString> visibilityMap{{KoColumn::Collapse, "collapse"}, {KoColumn::Filter, "filter"}, {KoColumn::Visible, "visible"}};
}

KoColumn::KoColumn()
    : m_defaultCellStyle(nullptr)
    , m_style(nullptr)
    , m_visibility(Visible)
{
}

KoColumn::~KoColumn() = default;

void KoColumn::setStyle(KoColumnStyle::Ptr style)
{
    m_style = style;
}

KoColumnStyle::Ptr KoColumn::style()
{
    return m_style;
}

KoCellStyle::Ptr KoColumn::defualtCellStyle() const
{
    return m_defaultCellStyle;
}

void KoColumn::setDefaultCellStyle(KoCellStyle::Ptr defaultStyle)
{
    m_defaultCellStyle = defaultStyle;
}

void KoColumn::setVisibility(KoColumn::Visibility visibility)
{
    m_visibility = visibility;
}

KoColumn::Visibility KoColumn::visibility()
{
    return m_visibility;
}

void KoColumn::saveOdf(KoXmlWriter &writer, KoGenStyles &styles)
{
    writer.startElement("table:table-column");
    if (m_style) {
        writer.addAttribute("table:style-name", m_style->saveOdf(styles));
    }
    if (m_defaultCellStyle) {
        writer.addAttribute("table:default-cell-style-name", m_defaultCellStyle->saveOdf(styles));
    }
    writer.addAttribute("table:visibility", visibilityMap.value(m_visibility));
    writer.endElement(); // table:column
}
