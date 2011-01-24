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

#ifndef MSOOXMLDOCUMENTTABLESTYLE_H
#define MSOOXMLDOCUMENTTABLESTYLE_H

#include "MsooXmlTableStyle.h"

#include <msooxml_export.h>

namespace MSOOXML {

class MSOOXML_EXPORT DocumentTableStyle : public TableStyle
{
public:
    DocumentTableStyle();
    virtual ~DocumentTableStyle();

    QString baseStyleName() const;
    void setBaseStyleName(QString base);

    TableStyleProperties* properties() const;
    /// the style takes ownership of the properties
    void setProperties(TableStyleProperties* properties);

private:
    QString m_baseStyleName;
    TableStyleProperties* m_properties;
};

class MSOOXML_EXPORT DocumentTableStyleInstanceProperties : public TableStyleInstanceProperties
{
public:
    DocumentTableStyleInstanceProperties(int rowCount, int columnCount);
    virtual ~DocumentTableStyleInstanceProperties();

    QMap<QString, TableStyleProperties*> styleList() const;
    void setStyleList(QMap<QString, TableStyleProperties*> styleList);

private:
    QMap<QString, TableStyleProperties*> m_styleList;
};

class MSOOXML_EXPORT DocumentTableStyleInstance : public TableStyleInstance
{
public:
    DocumentTableStyleInstance(DocumentTableStyle* style, DocumentTableStyleInstanceProperties properties);
    virtual ~DocumentTableStyleInstance();

    virtual KoCellStyle::Ptr style(int row, int column);
};

}

#endif
