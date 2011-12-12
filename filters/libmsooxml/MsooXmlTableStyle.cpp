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

void TableStyleConverter::applyStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    if(!styleProperties) {
        return;
    }

    if (styleProperties->target == TableStyleProperties::TableCell) {
        applyCellBordersStyle(styleProperties, style);
    } else {
        applyTableBordersStyle(styleProperties, style, row, column);
    }

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

void TableStyleConverter::applyTableBordersStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    const int lastRow = m_row - 1;
    const int lastColumn = m_column - 1;

    //Borders, are a bit tricky too; we have to take into account whether the cell
    //has borders facing other cells or facing the border of the table.

    TableStyleProperties::Properties setProperties = styleProperties->setProperties;

    if (setProperties & TableStyleProperties::TopBorder) {
        if (row == 0) {
            KoBorder::BorderData* topData;
            topData = &styleProperties->top;
            style->borders()->setTopBorderData(*topData);

//             if (!styleProperties->bordersToEdgesOnly) {
//                 style->borders()->setTopBorderData(*topData);
//             }
        }
//         else {
//             topData = &styleProperties->insideH;
//             if (topData->innerPen.widthF() == 0) {
//                 topData = &styleProperties->top;
//             }
//         }
    }

    if (setProperties & TableStyleProperties::BottomBorder) {
        if (row == lastRow) {
            KoBorder::BorderData* bottomData;
            bottomData = &styleProperties->bottom;
            style->borders()->setBottomBorderData(*bottomData);

//             if (!styleProperties->bordersToEdgesOnly) {
//                 style->borders()->setBottomBorderData(*bottomData);
//             }
        }
//         else {
//             bottomData = &styleProperties->insideH;
//             if (bottomData->innerPen.widthF() == 0) {
//                 bottomData = &styleProperties->bottom;
//             }
//         }
    }

    if (setProperties & TableStyleProperties::LeftBorder) {
        if (column == 0) {
            KoBorder::BorderData* leftData;
            leftData = &styleProperties->left;
            style->borders()->setLeftBorderData(*leftData);

//             if (!styleProperties->bordersToEdgesOnly) {
//                 style->borders()->setLeftBorderData(*leftData);
//             }
        }
//         else {
//             leftData = &styleProperties->insideV;
//             if (leftData->innerPen.widthF() == 0) {
//                 leftData = &styleProperties->left;
//             }
//         }
    }

    if (setProperties & TableStyleProperties::RightBorder) {
        if (column == lastColumn) {
            KoBorder::BorderData* rightData;
            rightData = &styleProperties->right;
            style->borders()->setRightBorderData(*rightData);

//             if (!styleProperties->bordersToEdgesOnly) {
//                 style->borders()->setRightBorderData(*rightData);
//             }
        }
//         else {
//             rightData = &styleProperties->insideV;
//             if (rightData->innerPen.widthF() == 0) {
//                 rightData = &styleProperties->right;
//             }
//         }
    }

    if (setProperties & TableStyleProperties::InsideVBorder) {
        KoBorder::BorderData* insideVData;
        insideVData = &styleProperties->insideV;
        if (column == lastColumn) {
            style->borders()->setLeftBorderData(*insideVData);
        }
        else if (column == 0) {
            style->borders()->setRightBorderData(*insideVData);
        }
        else {
            style->borders()->setLeftBorderData(*insideVData);
            style->borders()->setRightBorderData(*insideVData);
        }
    }

    if (setProperties & TableStyleProperties::InsideHBorder) {
        KoBorder::BorderData* insideHData;
        insideHData = &styleProperties->insideH;
        if (row == lastRow) {
            style->borders()->setTopBorderData(*insideHData);
        }
        else if (row == 0) {
            style->borders()->setBottomBorderData(*insideHData);
        }
        else {
            style->borders()->setTopBorderData(*insideHData);
            style->borders()->setBottomBorderData(*insideHData);
        }
    }

    if (setProperties & TableStyleProperties::Tl2brBorder) {
        style->borders()->setTlbrBorderData(styleProperties->tl2br);
    }
    if (setProperties & TableStyleProperties::Tr2blBorder) {
        style->borders()->setTrblBorderData(styleProperties->tr2bl);
    }
}

void TableStyleConverter::applyCellBordersStyle(TableStyleProperties* props, KoCellStyle::Ptr& style)
{
    TableStyleProperties::Properties setProperties = props->setProperties;

    if (setProperties & TableStyleProperties::TopBorder) {
        style->borders()->setTopBorderData(props->top);
    }

    if (setProperties & TableStyleProperties::BottomBorder) {
        style->borders()->setBottomBorderData(props->bottom);
    }

    if (setProperties & TableStyleProperties::LeftBorder) {
        style->borders()->setLeftBorderData(props->left);
    }

    if (setProperties & TableStyleProperties::RightBorder) {
        style->borders()->setRightBorderData(props->right);
    }

    if (setProperties & TableStyleProperties::Tl2brBorder) {
        style->borders()->setTlbrBorderData(props->tl2br);
    }
    if (setProperties & TableStyleProperties::Tr2blBorder) {
        style->borders()->setTrblBorderData(props->tr2bl);
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
