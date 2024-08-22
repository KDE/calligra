/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoRow.h"

#include <KoXmlWriter.h>

#include <QMap>
#include <QString>

namespace
{
const QMap<KoRow::Visibility, QString> visibilityMap{{KoRow::Collapse, "collapse"}, {KoRow::Filter, "filter"}, {KoRow::Visible, "visible"}};
}

KoRow::KoRow()
    : m_defaultCellStyle(nullptr)
    , m_style(nullptr)
    , m_visibility(Visible)
{
}

KoRow::~KoRow() = default;

void KoRow::setStyle(KoRowStyle::Ptr style)
{
    m_style = style;
}

KoRowStyle::Ptr KoRow::style()
{
    return m_style;
}

KoCellStyle::Ptr KoRow::defualtCellStyle() const
{
    return m_defaultCellStyle;
}

void KoRow::setDefaultCellStyle(KoCellStyle::Ptr defaultStyle)
{
    m_defaultCellStyle = defaultStyle;
}

void KoRow::setVisibility(KoRow::Visibility visibility)
{
    m_visibility = visibility;
}

KoRow::Visibility KoRow::visibility()
{
    return m_visibility;
}

void KoRow::saveOdf(KoXmlWriter &writer, KoGenStyles &styles)
{
    writer.startElement("table:table-row");
    if (m_style) {
        writer.addAttribute("table:style-name", m_style->saveOdf(styles));
    }
    if (m_defaultCellStyle) {
        writer.addAttribute("table:default-cell-style-name", m_defaultCellStyle->saveOdf(styles));
    }
    writer.addAttribute("table:visibility", visibilityMap.value(m_visibility));
}

void KoRow::finishSaveOdf(KoXmlWriter &writer, KoGenStyles &styles)
{
    Q_UNUSED(styles)
    writer.endElement(); // table:row
}
