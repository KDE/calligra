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
, m_properties()
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

DocumentTableStyleInstanceProperties::DocumentTableStyleInstanceProperties()
: TableStyleConverterProperties()
{
}

DocumentTableStyleInstanceProperties::~DocumentTableStyleInstanceProperties()
{
}

void DocumentTableStyleInstanceProperties::setStyleList(QMap<QString, TableStyleProperties*> styleList)
{
    m_styleList = styleList;
}

QMap<QString, TableStyleProperties*> DocumentTableStyleInstanceProperties::styleList() const
{
    return m_styleList;
}

DocumentTableStyleInstance::DocumentTableStyleInstance(DocumentTableStyleInstanceProperties properties, DocumentTableStyle* style)
: TableStyleConverter(properties.rowCount(), properties.columnCount() )
{
}

DocumentTableStyleInstance::~DocumentTableStyleInstance()
{
}

KoCellStyle::Ptr DocumentTableStyleInstance::style(int row, int column)
{
    KoCellStyle::Ptr style;

    return style;
}
