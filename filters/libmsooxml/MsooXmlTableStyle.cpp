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

#include "MsooXmlTableStyle.h"

using namespace MSOOXML;

TableStyleConverterProperties::TableStyleConverterProperties()
: m_rowCount(1)
, m_columnCount(1)
, m_rowBandSize(1)
, m_columnBandSize(1)
, m_localStyles()
, m_localDefaultCellStyle(0)
{
}

TableStyleConverterProperties::~TableStyleConverterProperties()
{
}

void TableStyleConverterProperties::setRowCount(int rowCount)
{
    m_rowCount = rowCount;
}

int TableStyleConverterProperties::rowCount() const
{
    return m_rowCount;
}


void TableStyleConverterProperties::setColumnCount(int columnCount)
{
    m_columnCount = columnCount;
}

int TableStyleConverterProperties::columnCount() const
{
    return m_columnCount;
}

void TableStyleConverterProperties::setColumnBandSize(int size)
{
    Q_ASSERT(size >= 0);
    m_columnBandSize = size;
}

int TableStyleConverterProperties::columnBandSize() const
{
    return m_columnBandSize;
}

void TableStyleConverterProperties::setRowBandSize(int size)
{
    m_rowBandSize = size;
}

int TableStyleConverterProperties::rowBandSize() const
{
    return m_rowBandSize;
}

void TableStyleConverterProperties::setLocalStyles(const MSOOXML::LocalTableStyles& localStyles)
{
    m_localStyles = localStyles;
}

LocalTableStyles TableStyleConverterProperties::localStyles() const
{
    return m_localStyles;
}

void TableStyleConverterProperties::setLocalDefaulCelltStyle(TableStyleProperties* properties)
{
    m_localDefaultCellStyle = properties;
}

TableStyleProperties* TableStyleConverterProperties::localDefaultCellStyle() const
{
    return m_localDefaultCellStyle;
}

TableStyleConverter::TableStyleConverter(int row, int column)
: m_row(row)
, m_column(column)
{
}

TableStyleConverter::~TableStyleConverter()
{
}
// TODO: Table-level exception properties, tblCellSpacing
//
// ECMA-376:
//
// The appearance of a table cell border in the document shall be determined by
// the following settings:
//
// * If the tblCellSpacing element value (§17.4.45;§17.4.44;§17.4.46) applied
// to the cell is non-zero, then the cell border shall always be displayed
//
// * Otherwise, the display of the border is subject to the conflict resolution
// algorithm defined by the tcBorders element (§17.4.67) and the tblBorders
// element (§17.4.40;§17.4.39)
//
//
// 17.4.39 tblBorders (Table Borders)
//
// If the cell spacing is zero, then there is a conflict [Example: Between the
// left border of all cells in the first column and the left border of the
// table. end example], which shall be resolved as follows:
//
// * If there is a cell border, then the cell border shall be displayed
//
// * If there is no cell border but there is a table-level exception border on
// this table row, then the table-level exception border shall be displayed
//
// * If there is no cell or table-level exception border, then the table border
// shall be displayed
//
//
// 17.4.67 tcBorders (Table Cell Borders)
//
// If the cell spacing is zero, then there can be a conflict between two
// adjacent cell borders [Example: Between the left border of all cells in the
// second column and the right border of all cells in the first column of the
// table. end example], which shall be resolved as follows:
//
// * If either conflicting table cell border is nil or none (no border), then
// the opposing border shall be displayed.
//
// * If a cell border conflicts with a table border, the cell border always
// wins.
//
// * Each border shall then be assigned a weight using the formula described in
// the spec, and the border value using this calculation shall be displayed
// over the alternative border:
//
void TableStyleConverter::applyStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style,
                                     int row, int column, const QPair<int, int> &spans)
{
    if(!styleProperties) {
        return;
    }

    switch (styleProperties->target) {
    case TableStyleProperties::TableRow:
        applyRowLevelBordersStyle(styleProperties, style, row, column, spans);
        break;
    case TableStyleProperties::TableColumn:
        applyColumnLevelBordersStyle(styleProperties, style, row, column, spans);
        break;
    case TableStyleProperties::TableCell:
        applyCellLevelBordersStyle(styleProperties, style);
        break;
    default:
        applyTableLevelBordersStyle(styleProperties, style, row, column, spans);
        break;
    }

    //TODO: A similar logic to borders should be used for all other properties!

    applyBackground(styleProperties, style, row, column);

    if (styleProperties->setProperties & TableStyleProperties::VerticalAlign) {
        style->setVerticalAlign(styleProperties->verticalAlign);
    }

    if (styleProperties->setProperties & TableStyleProperties::GlyphOrientation) {
        style->setGlyphOrientation(styleProperties->glyphOrientation);
    }

    if (!styleProperties->textStyle.isEmpty() || !styleProperties->textStyle.parentName().isEmpty()) {
        style->setTextStyle(styleProperties->textStyle);
    }

    if (!styleProperties->paragraphStyle.isEmpty() || !styleProperties->paragraphStyle.parentName().isEmpty()) {
        style->setParagraphStyle(styleProperties->paragraphStyle);
    }

    TableStyleProperties::Properties setProperties = styleProperties->setProperties;

    if (setProperties & TableStyleProperties::TopMargin) {
        style->setTopPadding(styleProperties->topMargin);
    }
    if (setProperties & TableStyleProperties::BottomMargin) {
        style->setBottomPadding(styleProperties->bottomMargin);
    }
    if (setProperties & TableStyleProperties::LeftMargin) {
        style->setLeftPadding(styleProperties->leftMargin);
    }
    if (setProperties & TableStyleProperties::RightMargin) {
        style->setRightPadding(styleProperties->rightMargin);
    }
}

void TableStyleConverter::applyBackground(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if (styleProperties->setProperties & TableStyleProperties::BackgroundColor) {
        style->setBackgroundColor(styleProperties->backgroundColor);
    }
    if (styleProperties->setProperties & TableStyleProperties::BackgroundOpacity) {
        style->setBackgroundOpacity(styleProperties->backgroundOpacity);
    }
}

void TableStyleConverter::applyTableLevelBordersStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style,
                                                      int row, int column, const QPair<int, int> &spans)
{
    const int lastRow = m_row;
    const int lastColumn = m_column;

    TableStyleProperties::Properties setProperties = styleProperties->setProperties;

    if (setProperties & TableStyleProperties::TopBorder) {
        if (row == 0) {
            KoBorder::BorderData* topData;
            topData = &styleProperties->top;
            style->borders()->setBorderData(KoBorder::TopBorder, *topData);
        }
    }

    if (setProperties & TableStyleProperties::BottomBorder) {
        if ((row + spans.first) == lastRow) {
            KoBorder::BorderData* bottomData;
            bottomData = &styleProperties->bottom;
            style->borders()->setBorderData(KoBorder::BottomBorder, *bottomData);
        }
    }

    if (setProperties & TableStyleProperties::LeftBorder) {
        if (column == 0) {
            KoBorder::BorderData* leftData;
            leftData = &styleProperties->left;
            style->borders()->setBorderData(KoBorder::LeftBorder, *leftData);
        }
    }

    if (setProperties & TableStyleProperties::RightBorder) {
        if ((column + spans.second) == lastColumn) {
            KoBorder::BorderData* rightData;
            rightData = &styleProperties->right;
            style->borders()->setBorderData(KoBorder::RightBorder, *rightData);
        }
    }

    if (setProperties & TableStyleProperties::InsideVBorder) {
        KoBorder::BorderData* insideVData;
        insideVData = &styleProperties->insideV;
        if (column != 0) {
            style->borders()->setBorderData(KoBorder::LeftBorder, *insideVData);
        }
        if ((column + spans.second) != lastColumn) {
            style->borders()->setBorderData(KoBorder::RightBorder, *insideVData);
        }
    }

    if (setProperties & TableStyleProperties::InsideHBorder) {
        KoBorder::BorderData* insideHData;
        insideHData = &styleProperties->insideH;
        if (row != 0) {
            style->borders()->setBorderData(KoBorder::TopBorder, *insideHData);
        }
        if ((row + spans.first) != lastRow) {
            style->borders()->setBorderData(KoBorder::BottomBorder, *insideHData);
        }
    }

    if (setProperties & TableStyleProperties::Tl2brBorder) {
        style->borders()->setBorderData(KoBorder::TlbrBorder, styleProperties->tl2br);
    }
    if (setProperties & TableStyleProperties::Tr2blBorder) {
        style->borders()->setBorderData(KoBorder::BltrBorder, styleProperties->tr2bl);
    }
}

void TableStyleConverter::reapplyTableLevelBordersStyle(TableStyleProperties* properties,
                                                        TableStyleProperties* localProperties,
                                                        TableStyleProperties* exceptionProperties,
                                                        KoCellStyle::Ptr& style,
                                                        int row, int column, const QPair<int, int> &spans)
{
    const int lastRow = m_row;
    const int lastColumn = m_column;

    TableStyleProperties::Properties setProperties;
    if (properties) {
        setProperties = properties->setProperties;
    }
    TableStyleProperties::Properties setLocalProperties;
    if (localProperties) {
        setLocalProperties = localProperties->setProperties;
    }
    TableStyleProperties::Properties setExceptionProperties;
    if (exceptionProperties) {
        setExceptionProperties = exceptionProperties->setProperties;
    }
    KoBorder::BorderData data;

    //TopBorder
    if (row == 0) {
        data = style->borders()->borderData(KoBorder::TopBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::TopBorder) {
                style->borders()->setBorderData(KoBorder::TopBorder, properties->top);
            }
            if (setLocalProperties & TableStyleProperties::TopBorder) {
                style->borders()->setBorderData(KoBorder::TopBorder, localProperties->top);
            }
            if (setExceptionProperties & TableStyleProperties::TopBorder) {
                style->borders()->setBorderData(KoBorder::TopBorder, exceptionProperties->top);
            }
        }
    }

    //BottomBorder
    if ((row + spans.first) == lastRow) {
        data = style->borders()->borderData(KoBorder::BottomBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::BottomBorder) {
                style->borders()->setBorderData(KoBorder::BottomBorder, properties->bottom);
            }
            if (setLocalProperties & TableStyleProperties::BottomBorder) {
                style->borders()->setBorderData(KoBorder::BottomBorder, localProperties->bottom);
            }
            if (setExceptionProperties & TableStyleProperties::BottomBorder) {
                style->borders()->setBorderData(KoBorder::BottomBorder, exceptionProperties->bottom);
            }
        }
    }

    //LeftBorder
    if (column == 0) {
        data = style->borders()->borderData(KoBorder::LeftBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::LeftBorder) {
                style->borders()->setBorderData(KoBorder::LeftBorder, properties->left);
            }
            if (setLocalProperties & TableStyleProperties::LeftBorder) {
                style->borders()->setBorderData(KoBorder::LeftBorder, localProperties->left);
            }
            if (setExceptionProperties & TableStyleProperties::LeftBorder) {
                style->borders()->setBorderData(KoBorder::LeftBorder, exceptionProperties->left);
            }
        }
    }

    //RightBorder
    if ((column + spans.second) == lastColumn) {
        data = style->borders()->borderData(KoBorder::RightBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::RightBorder) {
                style->borders()->setBorderData(KoBorder::RightBorder, properties->right);
            }
            if (setLocalProperties & TableStyleProperties::RightBorder) {
                style->borders()->setBorderData(KoBorder::RightBorder, localProperties->right);
            }
            if (setExceptionProperties & TableStyleProperties::RightBorder) {
                style->borders()->setBorderData(KoBorder::RightBorder, exceptionProperties->right);
            }
        }
    }

    //InsideVBorder
    if (column != 0) {
        data = style->borders()->borderData(KoBorder::LeftBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::InsideVBorder) {
                style->borders()->setBorderData(KoBorder::LeftBorder, properties->insideV);
            }
            if (setLocalProperties & TableStyleProperties::InsideVBorder) {
                style->borders()->setBorderData(KoBorder::LeftBorder, localProperties->insideV);
            }
            if (setExceptionProperties & TableStyleProperties::InsideVBorder) {
                style->borders()->setBorderData(KoBorder::LeftBorder, exceptionProperties->insideV);
            }
        }
    }

    if ((column + spans.second) != lastColumn) {
        data = style->borders()->borderData(KoBorder::RightBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::InsideVBorder) {
                style->borders()->setBorderData(KoBorder::RightBorder, properties->insideV);
            }
            if (setLocalProperties & TableStyleProperties::InsideVBorder) {
                style->borders()->setBorderData(KoBorder::RightBorder, localProperties->insideV);
            }
            if (setExceptionProperties & TableStyleProperties::InsideVBorder) {
                style->borders()->setBorderData(KoBorder::RightBorder, exceptionProperties->insideV);
            }
        }
    }

    //InsideHBorder
    if (row != 0) {
        data = style->borders()->borderData(KoBorder::TopBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::InsideHBorder) {
                style->borders()->setBorderData(KoBorder::TopBorder, properties->insideH);
            }
            if (setLocalProperties & TableStyleProperties::InsideHBorder) {
                style->borders()->setBorderData(KoBorder::TopBorder, localProperties->insideH);
            }
            if (setExceptionProperties & TableStyleProperties::InsideHBorder) {
                style->borders()->setBorderData(KoBorder::TopBorder, exceptionProperties->insideH);
            }
        }
    }

    if ((row + spans.first) != lastRow) {
        data = style->borders()->borderData(KoBorder::BottomBorder);
        //cell-level border set to "None"
        if ((data.outerPen.widthF() == 0) && (data.style == KoBorder::BorderSolid)) {
            if (setProperties & TableStyleProperties::InsideHBorder) {
                style->borders()->setBorderData(KoBorder::BottomBorder, properties->insideH);
            }
            if (setLocalProperties & TableStyleProperties::InsideHBorder) {
                style->borders()->setBorderData(KoBorder::BottomBorder, localProperties->insideH);
            }
            if (setExceptionProperties & TableStyleProperties::InsideHBorder) {
                style->borders()->setBorderData(KoBorder::BottomBorder, exceptionProperties->insideH);
            }
        }
    }

    //TODO: Tl2brBorder, Tr2blBorder
}

void TableStyleConverter::applyRowLevelBordersStyle(TableStyleProperties* props, KoCellStyle::Ptr& style,
                                                    int row, int column, const QPair<int, int> &spans)
{
    const int lastColumn = m_column;
    const int lastRow = m_row;

    TableStyleProperties::Properties setProperties = props->setProperties;

    if (setProperties & TableStyleProperties::TopBorder) {
        style->borders()->setBorderData(KoBorder::TopBorder, props->top);
    }

    if (setProperties & TableStyleProperties::BottomBorder) {
        style->borders()->setBorderData(KoBorder::BottomBorder, props->bottom);
    }

    if (setProperties & TableStyleProperties::LeftBorder) {
        if (column == 0) {
            style->borders()->setBorderData(KoBorder::LeftBorder, props->left);
        }
    }

    if (setProperties & TableStyleProperties::RightBorder) {
        if ((column + spans.second) == lastColumn) {
            style->borders()->setBorderData(KoBorder::RightBorder, props->right);
        }
    }

    if (setProperties & TableStyleProperties::InsideHBorder) {
        KoBorder::BorderData* insideHData;
        insideHData = &props->insideH;
        if (row != 0) {
            style->borders()->setBorderData(KoBorder::TopBorder, *insideHData);
        }
        if ((row + spans.first) != lastRow) {
            style->borders()->setBorderData(KoBorder::BottomBorder, *insideHData);
        }
    }

    if (setProperties & TableStyleProperties::InsideVBorder) {
        KoBorder::BorderData* insideVData;
        insideVData = &props->insideV;
        if (column != 0) {
            style->borders()->setBorderData(KoBorder::LeftBorder, *insideVData);
        }
        if ((column + spans.second) != lastColumn) {
            style->borders()->setBorderData(KoBorder::RightBorder, *insideVData);
        }
    }

    if (setProperties & TableStyleProperties::Tl2brBorder) {
        style->borders()->setBorderData(KoBorder::TlbrBorder, props->tl2br);
    }
    if (setProperties & TableStyleProperties::Tr2blBorder) {
        style->borders()->setBorderData(KoBorder::BltrBorder, props->tr2bl);
    }
}

void TableStyleConverter::applyColumnLevelBordersStyle(TableStyleProperties* props, KoCellStyle::Ptr& style,
                                                       int row, int column, const QPair<int, int> &spans)
{
    const int lastColumn = m_column;
    const int lastRow = m_row;

    TableStyleProperties::Properties setProperties = props->setProperties;

    if (setProperties & TableStyleProperties::TopBorder) {
        if (row == 0) {
            style->borders()->setBorderData(KoBorder::TopBorder, props->top);
        }
    }

    if (setProperties & TableStyleProperties::BottomBorder) {
        if ((row + spans.first) == lastRow) {
            style->borders()->setBorderData(KoBorder::BottomBorder, props->bottom);
        }
    }

    if (setProperties & TableStyleProperties::LeftBorder) {
        style->borders()->setBorderData(KoBorder::LeftBorder, props->left);
    }

    if (setProperties & TableStyleProperties::RightBorder) {
        style->borders()->setBorderData(KoBorder::RightBorder, props->right);
    }

    if (setProperties & TableStyleProperties::InsideHBorder) {
        KoBorder::BorderData* insideHData;
        insideHData = &props->insideH;
        if (row != 0) {
            style->borders()->setBorderData(KoBorder::TopBorder, *insideHData);
        }
        if ((row + spans.first) != lastRow) {
            style->borders()->setBorderData(KoBorder::BottomBorder, *insideHData);
        }
    }

    if (setProperties & TableStyleProperties::InsideVBorder) {
        KoBorder::BorderData* insideVData;
        insideVData = &props->insideV;
        if (column != 0) {
            style->borders()->setBorderData(KoBorder::LeftBorder, *insideVData);
        }
        if ((column + spans.second) != lastColumn) {
            style->borders()->setBorderData(KoBorder::RightBorder, *insideVData);
        }
    }

    if (setProperties & TableStyleProperties::Tl2brBorder) {
        style->borders()->setBorderData(KoBorder::TlbrBorder, props->tl2br);
    }
    if (setProperties & TableStyleProperties::Tr2blBorder) {
        style->borders()->setBorderData(KoBorder::BltrBorder, props->tr2bl);
    }
}

void TableStyleConverter::applyCellLevelBordersStyle(TableStyleProperties* props, KoCellStyle::Ptr& style)
{
    //NOTE: Let's keep the local variables until it's unstable.

    TableStyleProperties::Properties setProperties = props->setProperties;

    if (setProperties & TableStyleProperties::TopBorder) {
        KoBorder::BorderData* data = &props->top;
        style->borders()->setBorderData(KoBorder::TopBorder, *data);
    }

    if (setProperties & TableStyleProperties::BottomBorder) {
        KoBorder::BorderData* data = &props->bottom;
        style->borders()->setBorderData(KoBorder::BottomBorder, *data);
    }

    if (setProperties & TableStyleProperties::LeftBorder) {
        KoBorder::BorderData* data = &props->left;
        style->borders()->setBorderData(KoBorder::LeftBorder,*data);
    }

    if (setProperties & TableStyleProperties::RightBorder) {
        KoBorder::BorderData* data = &props->right;
        style->borders()->setBorderData(KoBorder::RightBorder, *data);
    }

    if (setProperties & TableStyleProperties::Tl2brBorder) {
        KoBorder::BorderData* data = &props->tl2br;
        style->borders()->setBorderData(KoBorder::TlbrBorder, *data);
    }
    if (setProperties & TableStyleProperties::Tr2blBorder) {
        KoBorder::BorderData* data = &props->tr2bl;
        style->borders()->setBorderData(KoBorder::BltrBorder,*data);
    }
    //TODO: process InsideHBorder, InsideVBorder
}

TableStyle::TableStyle()
: m_id()
{
}

TableStyle::~TableStyle()
{
//     qDeleteAll(m_properties.values());
}

void TableStyle::setId(const QString& id)
{
    m_id = id;
}

QString TableStyle::id() const
{
    return m_id;
}

LocalTableStyles::LocalTableStyles()
{
}

LocalTableStyles::~LocalTableStyles()
{
}

TableStyleProperties* LocalTableStyles::localStyle(int row, int column)
{
    const QPair<int,int> key(row,column);
    return m_properties.value(key);
}

void LocalTableStyles::setLocalStyle(TableStyleProperties* properties, int row, int column)
{
    const QPair<int,int> key(row,column);
    m_properties.insert(key, properties);
}
