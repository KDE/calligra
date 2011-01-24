/* This file is part of the KDE project
 * Copyright (C) 2010-2011 Carlos Licea <carlos@kdab.com>
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

#include "MsooXmlDocumentTableStyle.h"

using namespace MSOOXML;

DocumentTableStyle::DocumentTableStyle()
: TableStyle()
, m_baseStyleName()
, m_properties(0)
{
}

DocumentTableStyle::~DocumentTableStyle()
{
    delete m_properties;
}

QString DocumentTableStyle::baseStyleName() const
{
    return m_baseStyleName;
}

void DocumentTableStyle::setBaseStyleName(QString base)
{
    m_baseStyleName = base;
}

TableStyleProperties* DocumentTableStyle::properties() const
{
    return m_properties;
}

void DocumentTableStyle::setProperties(TableStyleProperties* properties)
{
    m_properties = properties;
}

DocumentTableStyleConverterProperties::DocumentTableStyleConverterProperties()
: TableStyleConverterProperties()
{
}

DocumentTableStyleConverterProperties::~DocumentTableStyleConverterProperties()
{
}

void DocumentTableStyleConverterProperties::setStyleList(QMap< QString, DocumentTableStyle* > styleList)
{
    m_styleList = styleList;
}

QMap< QString, DocumentTableStyle* > DocumentTableStyleConverterProperties::styleList() const
{
    return m_styleList;
}

DocumentTableStyleConverter::DocumentTableStyleConverter(DocumentTableStyleConverterProperties properties, DocumentTableStyle* style)
: TableStyleConverter(properties.rowCount(), properties.columnCount() )
, m_properties(properties)
, m_style(style)
{
}

DocumentTableStyleConverter::~DocumentTableStyleConverter()
{
}

void DocumentTableStyleConverter::applyBasedStylesProperties(DocumentTableStyle* style, KoCellStyle::Ptr& odfStyle, int row, int column)
{
    if(!style) {
        return;
    }

    if(!style->baseStyleName().isEmpty()) {
        DocumentTableStyle* baseStyle = m_properties.styleList().value(style->baseStyleName());
        applyBasedStylesProperties(baseStyle, odfStyle, row, column);
    }

    applyStyle(style->properties(), odfStyle, row, column);
}

KoCellStyle::Ptr DocumentTableStyleConverter::style(int row, int column)
{
    KoCellStyle::Ptr odfStyle = KoCellStyle::create();

    applyBasedStylesProperties(m_style, odfStyle, row, column);

    applyStyle(m_properties.localDefaultCellStyle(), odfStyle, row, column);

    TableStyleProperties* localStyle = m_properties.localStyles().localStyle(row, column);
    if(localStyle) {
        TableStyleConverter::applyStyle(localStyle, odfStyle, row, column);
    }

    return odfStyle;
}
