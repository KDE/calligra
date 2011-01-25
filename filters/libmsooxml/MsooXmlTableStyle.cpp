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

    applyBordersStyle(styleProperties, style, row, column);
    applyBackground(styleProperties, style, row, column);
}

void TableStyleConverter::applyBackground(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if(styleProperties->setProperties & TableStyleProperties::BackgroundColor) {
        style->setBackgroundColor(styleProperties->backgroundColor);
    }
}

void TableStyleConverter::applyBordersStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    const int lastRow = m_row - 1;
    const int lastColumn = m_column - 1;

    //Borders, are a bit tricky too; we have to take into account whether the cell 
    //has borders facing other cells or facing the border of the table.

    TableStyleProperties::Properties setProperties = styleProperties->setProperties;

    if(setProperties & TableStyleProperties::TopBorder) {
        KoBorder::BorderData* topData;
        if(row == 0) {
            topData = &styleProperties->top;
        }
        else {
            topData = &styleProperties->insideH;
        }
        style->borders()->setTopBorderColor(topData->color);
        style->borders()->setTopBorderSpacing(topData->spacing);
        style->borders()->setTopBorderStyle(topData->style);
        style->borders()->setTopBorderWidth(topData->width);
    }

    if(setProperties & TableStyleProperties::BottomBorder) {
        KoBorder::BorderData* bottomData;
        if(row == lastRow) {
            bottomData = &styleProperties->bottom;
        }
        else {
            bottomData = &styleProperties->insideH;
        }
        style->borders()->setBottomBorderColor(bottomData->color);
        style->borders()->setBottomBorderSpacing(bottomData->spacing);
        style->borders()->setBottomBorderStyle(bottomData->style);
        style->borders()->setBottomBorderWidth(bottomData->width);
    }

    if(setProperties & TableStyleProperties::LeftBorder) {
        KoBorder::BorderData* leftData;
        if(column == 0) {
            leftData = &styleProperties->left;
        }
        else {
            leftData = &styleProperties->insideV;
        }
        style->borders()->setLeftBorderColor(leftData->color);
        style->borders()->setLeftBorderSpacing(leftData->spacing);
        style->borders()->setLeftBorderStyle(leftData->style);
        style->borders()->setLeftBorderWidth(leftData->width);
    }

    if(setProperties & TableStyleProperties::RightBorder) {
        KoBorder::BorderData* rightData;
        if(column == lastColumn) {
            rightData = &styleProperties->right;
        }
        else {
            rightData = &styleProperties->insideV;
        }
        style->borders()->setRightBorderColor(rightData->color);
        style->borders()->setRightBorderSpacing(rightData->spacing);
        style->borders()->setRightBorderStyle(rightData->style);
        style->borders()->setRightBorderWidth(rightData->width);
    }
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
