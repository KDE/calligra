/* This file is part of the KDE project
 * Copyright (C) 2010 Carlos Licea <carlos@kdab.com>
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

#ifndef MSOOXMLDRAWINGTABLESTYLE_H
#define MSOOXMLDRAWINGTABLESTYLE_H

#include "msooxml_export.h"

#include <KoCellStyle.h>

#include <QFlags>

namespace MSOOXML
{

struct MSOOXML_EXPORT TableStyleProperties
{
    KoBorder::BorderData bottom;
    KoBorder::BorderData insideH;
    KoBorder::BorderData insideV;
    KoBorder::BorderData left;
    KoBorder::BorderData right;
    KoBorder::BorderData tl2br;
    KoBorder::BorderData top;
    KoBorder::BorderData tr2bl;

    QColor backgroundColor;
};

class MSOOXML_EXPORT TableStyle
{
public:
    enum Type {
        NoType,
        FirstRow,
        FirstCol,
        LastCol,
        LastRow,
        NeCell,
        NwCell,
        SeCell,
        SwCell,
        Band1Horizontal,
        Band2Horizontal,
        Band1Vertical,
        Band2Vertical,
        WholeTbl
    };

    TableStyle();
    ~TableStyle();

    void setId(const QString& id);
    QString id() const;

    void addProperties(Type type, TableStyleProperties* properties);
    TableStyleProperties* properties(Type type) const;

private:
    QString m_id;
    QMap<Type, TableStyleProperties*> m_properties;
    //TODO handle the table background stored in the element TblBg
};

class MSOOXML_EXPORT TableStyleList
{
public:
    TableStyleList();
    ~TableStyleList();

    TableStyle tableStyle(const QString& id) const;
    void insertStyle(QString id, MSOOXML::TableStyle style);

private:
    QMap<QString, TableStyle> m_styles;
};

class MSOOXML_EXPORT TableStyleInstanceProperties
{
    friend class TableStyleInstance;
public:
    TableStyleInstanceProperties(int rowCount, int columnCount);
    ~TableStyleInstanceProperties();

    TableStyleInstanceProperties& rowBandSize(int size);
    TableStyleInstanceProperties& columnBandSize(int size);

    enum Role {
        FirstRow,
        FirstCol,
        LastCol,
        LastRow,
        NeCell,
        NwCell,
        SeCell,
        SwCell,
        RowBanded,
        ColumnBanded,
        WholeTbl
    };
    Q_DECLARE_FLAGS(Roles, Role)

    TableStyleInstanceProperties& roles(Roles roles);

private:
    int m_rowCount;
    int m_columnCount;
    int m_rowBandSize;
    int m_columnBandSize;
    Roles m_role;
};

class MSOOXML_EXPORT TableStyleInstance
{
public:
    TableStyleInstance(TableStyle* style, TableStyleInstanceProperties properties);
    ~TableStyleInstance();

    KoCellStyle::Ptr style(int row, int column);

private:
    void applyStyle(MSOOXML::TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column);
    void applyBordersStyle(MSOOXML::TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column);
    void applyBackground(MSOOXML::TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column);

    TableStyle* m_style;
    TableStyleInstanceProperties m_properties;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(MSOOXML::TableStyleInstanceProperties::Roles)

#endif
