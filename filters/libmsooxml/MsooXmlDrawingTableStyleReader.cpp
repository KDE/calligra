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

#include "MsooXmlDrawingTableStyleReader.h"

#include <KoGenStyles.h>
#include <KoGenStyle.h>
#include <KoOdfGraphicStyles.h>

#define MSOOXML_CURRENT_NS "a"
#define MSOOXML_CURRENT_CLASS MsooXmlDrawingTableStyleReader

#include <MsooXmlReader_p.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlImport.h>
#include <MsooXmlUnits.h>

#include <KoXmlWriter.h>

#include <QString>

#define MSOOXMLDRAWINGTABLESTYLEREADER_CPP

using namespace MSOOXML;

TableStyleInstanceProperties::TableStyleInstanceProperties(int rowCount, int columnCount)
: m_rowCount(rowCount)
, m_columnCount(columnCount)
, m_rowBandSize(1)
, m_columnBandSize(1)
, m_role(TableStyle::WholeTbl)
{
}

TableStyleInstanceProperties::~TableStyleInstanceProperties()
{
}

TableStyleInstanceProperties& TableStyleInstanceProperties::columnBandSize(int size)
{
    Q_ASSERT(size >= 0);
    m_columnBandSize = size;

    return *this;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::roles(TableStyleInstanceProperties::Roles roles)
{
    m_role = roles;

    return *this;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::rowBandSize(int size)
{
    m_rowBandSize = size;

    return *this;
}

TableStyleInstance::TableStyleInstance(TableStyle* style, TableStyleInstanceProperties properties)
: m_style(style)
, m_properties(properties)
{
    Q_ASSERT(m_style);
}

TableStyleInstance::~TableStyleInstance()
{
}

KoCellStyle::Ptr TableStyleInstance::style(int row, int column)
{
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_properties.m_rowCount);
    Q_ASSERT(column >= 0);
    Q_ASSERT(column < m_properties.m_columnCount);

    //TODO can we magically improve the creation of the styles?
    //For now I'll take the naive approach and say no. There are
    //way, way too many things to take into account so, reusing
    //the styles doesn't seem feasible.

    KoCellStyle::Ptr cellStyle = KoCellStyle::create();

    //This is a somehow tangled process.
    //First we have to apply the properties in the following order:
    //* Whole table
    //* Banded columns, even column banding
    //* Banded rows, even row banding
    //* First row, last row
    //* First column, last column
    //* Top left, top right, bottom left, bottom right
    //
    //Note that Subsequent properties that apply override previous ones.
    //
    //Note the standard doesn't say what happens to colliding properties
    //in the same hierarchy level (say, it's the first column and last column,
    //or it's first row and last row at once, I assume left to right priority
    //on the elements in the previous list.)
    //
    //See MSOOXML Table Styles §17.7.6 for details

     const TableStyleInstanceProperties::Roles& role = m_properties.m_role;
     const int& lastRow = m_properties.m_rowCount - 1 ;
     const int& lastColumn = m_properties.m_columnCount - 1 ;

    applyStyle(TableStyle::WholeTbl, cellStyle, row, column);

    if(role & TableStyleInstanceProperties::ColumnBanded) {
        //Is the column in the even band?
        if( (column % (m_properties.m_columnBandSize * 2)) > m_properties.m_columnBandSize) {
            applyStyle(TableStyle::Band1Vertical, cellStyle, row, column);
        }
        else {
            applyStyle(TableStyle::Band2Vertical, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::RowBanded) {
        //Is the row in the even band?
        if( (row % (m_properties.m_rowBandSize * 2)) > m_properties.m_columnBandSize) {
            applyStyle(TableStyle::Band1Horizontal, cellStyle, row, column);
        }
        else {
            applyStyle(TableStyle::Band2Horizontal, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::FirstRow) {
        if(row == 0) {
            applyStyle(TableStyle::FirstRow, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::LastRow) {
        if(row == lastRow) {
            applyStyle(TableStyle::FirstRow, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::FirstCol) {
        if(column == 0) {
            applyStyle(TableStyle::FirstCol, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::LastCol) {
        if(column == lastColumn) {
            applyStyle(TableStyle::LastCol, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::NeCell) {
        if(row == 0 && column == 0) {
            applyStyle(TableStyle::NwCell, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::NwCell) {
        if(row == 0 && column == lastColumn) {
            applyStyle(TableStyle::NeCell, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::SeCell) {
        if(row == lastRow && column == 0) {
            applyStyle(TableStyle::SwCell, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::SwCell) {
        if(row == lastRow && column == lastColumn) {
            applyStyle(TableStyle::SeCell, cellStyle, row, column);
        }
    }

    return cellStyle;
}

void TableStyleInstance::applyStyle(TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column)
{
    if(!m_style->properties(type)) {
        return;
    }

    //TODO apply other properties

    applyBordersStyle(type, style, row, column);
}

void TableStyleInstance::applyBordersStyle(TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column)
{
    //Borders, are a bit tricky too; we have to take into account whether the cell 
    //has borders facing other cells or facing the border of the table.

    KoBorder::BorderData* topData;
    if(row == 0) {
        topData = &m_style->properties(type)->top;
    }
    else {
        topData = &m_style->properties(type)->insideH;
    }
    style->borders()->setTopBorderColor(topData->color);
    style->borders()->setTopBorderSpacing(topData->spacing);
    style->borders()->setTopBorderStyle(topData->style);
    style->borders()->setTopBorderWidth(topData->width);

    KoBorder::BorderData* bottomData;
    if(row == m_properties.m_rowCount - 1) {
        bottomData = &m_style->properties(type)->bottom;
    }
    else {
        bottomData = &m_style->properties(type)->insideH;
    }
    style->borders()->setBottomBorderColor(bottomData->color);
    style->borders()->setBottomBorderSpacing(bottomData->spacing);
    style->borders()->setBottomBorderStyle(bottomData->style);
    style->borders()->setBottomBorderWidth(bottomData->width);

    KoBorder::BorderData* leftData;
    if(column == 0) {
        leftData = &m_style->properties(type)->left;
    }
    else {
        leftData = &m_style->properties(type)->insideV;
    }
    style->borders()->setLeftBorderColor(leftData->color);
    style->borders()->setLeftBorderSpacing(leftData->spacing);
    style->borders()->setLeftBorderStyle(leftData->style);
    style->borders()->setLeftBorderWidth(leftData->width);

    KoBorder::BorderData* rightData;
    if(column == m_properties.m_columnCount - 1) {
        rightData = &m_style->properties(type)->right;
    }
    else {
        rightData = &m_style->properties(type)->insideV;
    }
    style->borders()->setRightBorderColor(rightData->color);
    style->borders()->setRightBorderSpacing(rightData->spacing);
    style->borders()->setRightBorderStyle(rightData->style);
    style->borders()->setRightBorderWidth(rightData->width);
}

TableStyle::TableStyle()
{
}

TableStyle::~TableStyle()
{
}

void TableStyle::setId(const QString& id)
{
    m_id = id;
}

QString TableStyle::id() const
{
    return m_id;
}

void TableStyle::addProperties(TableStyle::Type type, TableStyleProperties* properties)
{
    m_properties.insert(type, properties);
}

TableStyleProperties* TableStyle::properties(TableStyle::Type type) const
{
    return m_properties.value(type);
}

TableStyleList::TableStyleList()
{
}

TableStyleList::~TableStyleList()
{
}

TableStyle TableStyleList::tableStyle(const QString& id) const
{
    return m_styles.value(id);
}

void TableStyleList::insertStyle(QString id, TableStyle style)
{
    m_styles.insert(id, style);
}

MsooXmlDrawingTableStyleReader::MsooXmlDrawingTableStyleReader(KoOdfWriters* writers)
: MsooXmlCommonReader(writers)
{
}

MsooXmlDrawingTableStyleReader::~MsooXmlDrawingTableStyleReader()
{
}

MsooXmlDrawingTableStyleContext::MsooXmlDrawingTableStyleContext(MsooXmlImport* _import, const QString& _path, const QString& _file, DrawingMLTheme* _themes, TableStyleList* _styleList)
{
    import = _import;
    path = _path;
    file = _file;
    themes = _themes;
    styleList = _styleList;
}

MsooXmlDrawingTableStyleContext::~MsooXmlDrawingTableStyleContext()
{
}

KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read(MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<MsooXmlDrawingTableStyleContext*>(context);
    Q_ASSERT(m_context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    KoFilter::ConversionStatus result = read_tblStyleLst();
    Q_ASSERT(result == KoFilter::OK);

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL tblStyleLst
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tblStyleLst()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            TRY_READ_IF(tblStyle)
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblStyle
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tblStyle()
{
    READ_PROLOGUE

    QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(styleId)

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
//             TRY_READ_IF(band1H)
//             ELSE_TRY_READ_IF(band1V)
//             ELSE_TRY_READ_IF(band2H)
//             ELSE_TRY_READ_IF(band2V)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_TRY_READ_IF(firstCol)
//             ELSE_TRY_READ_IF(firstRow)
//             ELSE_TRY_READ_IF(lastCol)
//             ELSE_TRY_READ_IF(lastRow)
//             ELSE_TRY_READ_IF(neCell)
//             ELSE_TRY_READ_IF(swCell)
//             ELSE_TRY_READ_IF(seCell)
//             ELSE_TRY_READ_IF(swCell)
//             ELSE_TRY_READ_IF(tblBg)
            /*ELSE_*/TRY_READ_IF(wholeTbl)
//             ELSE_WRONG_FORMAT
        }
    }

    m_context->styleList->insertStyle(styleId, m_currentStyle);
    m_currentStyle = TableStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wholeTbl
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_wholeTbl()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcStyle
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcStyle()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
//             TRY_READ_IF(cell3D)
//             ELSE_TRY_READ_IF(fill)
//             ELSE_TRY_READ_IF(fillRef)
            /*ELSE_*/TRY_READ_IF(tcBdr)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcTxStyle
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcTxStyle()
{
    READ_PROLOGUE
    SKIP_EVERYTHING_AND_RETURN
}

#undef CURRENT_EL
#define CURRENT_EL tcBdr
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcBdr()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            TRY_READ_IF(bottom)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_TRY_READ_IF(insideH)
//             ELSE_TRY_READ_IF(insideV)
            ELSE_TRY_READ_IF(left)
            ELSE_TRY_READ_IF(right)
//             ELSE_TRY_READ_IF(tl2br)
            ELSE_TRY_READ_IF(top)
//             ELSE_TRY_READ_IF(tr2bl)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bottom
KoFilter::ConversionStatus MSOOXML::MsooXmlDrawingTableStyleReader::read_bottom()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties.bottom = m_currentBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL top
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_top()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties.top = m_currentBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL left
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_left()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties.left = m_currentBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL right
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_right()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties.right = m_currentBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

// #undef CURRENT_EL
// #define CURRENT_EL tl2br
// KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tl2br()
// {
//     READ_PROLOGUE
// 
//     while(!atEnd()) {
//         if(isStartElement()) {
//             if(QUALIFIED_NAME_IS(ln)) {
//                 TRY_READ(Table_ln)
//                 m_currentTableStyleProperties.tl2br = m_currentBorder;
//             }
// //             ELSE_TRY_READ_IF(lnRef)
// //             ELSE_WRONG_FORMAT
//         }
//         BREAK_IF_END_OF(CURRENT_EL);
//     }
// 
//     READ_EPILOGUE
// }
// 
// #undef CURRENT_EL
// #define CURRENT_EL tr2bl
// KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tr2bl()
// {
//     READ_PROLOGUE
// 
//     while(!atEnd()) {
//         if(isStartElement()) {
//             if(QUALIFIED_NAME_IS(ln)) {
//                 TRY_READ(Table_ln)
//                 m_currentTableStyleProperties.tr2bl = m_currentBorder;
//             }
// //             ELSE_TRY_READ_IF(lnRef)
// //             ELSE_WRONG_FORMAT
//         }
//         BREAK_IF_END_OF(CURRENT_EL);
//     }
// 
//     READ_EPILOGUE
// }

#undef CURRENT_EL
#define CURRENT_EL ln
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_Table_ln()
{
    READ_PROLOGUE2(Table_ln)

    QXmlStreamAttributes attrs = attributes();

    m_currentBorder = KoBorder::BorderData();

    //compound line type
    TRY_READ_ATTR_WITHOUT_NS(cmpd)
    //double lines
    if( cmpd.isEmpty() || cmpd == "sng" ) {
        m_currentBorder.style = KoBorder::BorderSolid;
    }
    //single line
    else if (cmpd == "dbl") {
        m_currentBorder.style = KoBorder::BorderDouble;
    }
    //thick thin double lines
    else if (cmpd == "thickThin") {
        //FIXME it seem we don't support this properly. Use solid for now.
        m_currentBorder.style = KoBorder::BorderDouble;
    }
    //thin thick double lines
    else if (cmpd == "thinThick") {
        //FIXME it doesn't seem we support this properly.
        m_currentBorder.style = KoBorder::BorderDouble;
    }
    //thin thick thin triple lines
    else if (cmpd == "tri") {
        //NOTE: There is not triple in ODF
        m_currentBorder.style = KoBorder::BorderSolid;
    }

    TRY_READ_ATTR_WITHOUT_NS(w) //width
    m_currentBorder.width = EMU_TO_POINT(w.toDouble());

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(solidFill)) {
                TRY_READ(solidFill);
                m_currentBorder.style = KoBorder::BorderSolid;
                m_currentBorder.color = m_currentColor;
            }
            else if (QUALIFIED_NAME_IS(prstDash)) {
                attrs = attributes();
                //TODO find out how other colors are handled
                m_currentBorder.color = Qt::black;
                TRY_READ_ATTR_WITHOUT_NS(val)
                //TODO support other dash types. Make it its own function.
                if (val == "dash") {
                    m_currentBorder.style = KoBorder::BorderDashed;
                }
                else if(val == "dashDot") {
                    m_currentBorder.style = KoBorder::BorderDashDotPattern;
                }
                else if(val == "dot") {
                    m_currentBorder.style = KoBorder::BorderDotted;
                }
            }
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#define blipFill_NS "a"
#define SETUP_PARA_STYLE_IN_READ_P

#include <MsooXmlCommonReaderImpl.h>

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "p" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h>
