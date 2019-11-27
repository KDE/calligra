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

#include <MsooXmlTableStyle.h>
#include <KoTblStyle.h>

/**
 * The idea behind these classes is the following:
 * > A document has a list of table styles identifiable by ID.
 * > A table style has a number of properties to be used if the
 * table that references the style toggles them on.
 * > Those are stored on a table style properties.
 *
 * > Now the way a style for a cell is composed can be quite complex
 * depending on a lot of things. Mainly:
 *  > The properties toggled and their precedence,
 *    the rule of thumb for the precedence is that it's higher
 *    the more more specific it is.
 *  > The position in which the cell is. The styles have a
 *    particularly tricky property: borders. The styles can
 *    specify (in the same style) the style for a border
 *    depending whether is in the outside of the table or
 *    if it's an inside border. That's why the size of the
 *    table is needed.
 *
 * Also, we might need to apply local styles (styles to one cell,)
 * or default styles for all the cells in a table. Defined in the very
 * table.
 *
 * For these reasons we don't apply styles directly but we use a style
 * converter for a specific table with a specific togglers for styles,
 * specific local styles or specific default styles and size. This converter
 * will give a KoCellStyle back.
 */

namespace MSOOXML
{

/// Reading and storage

class KOMSOOXML_EXPORT DrawingTableStyle : public TableStyle
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

    DrawingTableStyle();
    ~DrawingTableStyle() override;

    //the style takes ownership of the properties
    void addProperties(Type type, TableStyleProperties* properties);
    TableStyleProperties* properties(Type type) const;

    //Style of the whole table, not cell styles
    KoTblStyle::Ptr mainStyle;

private:
    QMap<Type, TableStyleProperties*> m_properties;
    //TODO: handle the table background stored in the element TblBg
};

class KOMSOOXML_EXPORT DrawingTableStyleConverterProperties : public TableStyleConverterProperties
{
public:
    DrawingTableStyleConverterProperties();
    ~DrawingTableStyleConverterProperties() override;

    enum Role {
        FirstRow = 1,
        FirstCol = 2,
        LastCol = 4,
        LastRow = 8,
        NeCell = 16,
        NwCell = 32,
        SeCell = 64,
        SwCell = 128,
        RowBanded = 256,
        ColumnBanded = 512,
        WholeTbl = 1024
    };
    Q_DECLARE_FLAGS(Roles, Role)

    void setRoles(Roles roles);
    Roles roles() const;

private:
    Roles m_role;
};

class KOMSOOXML_EXPORT DrawingTableStyleConverter : public TableStyleConverter
{
public:
    explicit DrawingTableStyleConverter(DrawingTableStyleConverterProperties const& properties, DrawingTableStyle* style =0);
    ~DrawingTableStyleConverter() override;

    KoCellStyle::Ptr style(int row, int column, const QPair<int, int> &spans) override;

private:
    void applyStyle(MSOOXML::DrawingTableStyle::Type type, KoCellStyle::Ptr& style, int row, int column, const QPair<int, int> &spans);

    DrawingTableStyle * const m_style;
    DrawingTableStyleConverterProperties const& m_properties;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(MSOOXML::DrawingTableStyleConverterProperties::Roles)

#endif
