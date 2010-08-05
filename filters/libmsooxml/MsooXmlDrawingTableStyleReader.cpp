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
#include <MsooXmlReader_p.h>

#define MSOOXML_CURRENT_NS "a"
#define MSOOXML_CURRENT_CLASS MsooXmlDrawingTableStyleReader

using namespace MSOOXML;

QPen Border::pen() const
{
    return m_pen;
}

void Border::setPen(const QPen& pen)
{
    m_pen = pen;
}

void TableStyleProperties::addBorder(Border border, BorderSide side)
{
    m_borders.insert(side, border);
}

Border TableStyleProperties::borderForSide(BorderSide side) const
{
    return m_borders.value(side);
}

QString TableStyle::stringFromType(TableStyle::Type type)
{
    switch(type) {
        case FirstRow:
            return "firstRow";
            break;
        case LastCol:
            return "lastCol";
            break;
        case LastRow:
            return "lastRow";
            break;
        case NeCell:
            return "neCell";
            break;
        case NwCell:
            return "nwCell";
            break;
        case SeCell:
            return "seCell";
            break;
        case SwCell:
            return "swCell";
            break;
        case TblBg:
            return "tblBg";
            break;
        case WholeTbl:
            return "wholeTbl";
            break;
    }
}

TableStyle::Type TableStyle::typeFromString(const QString& string)
{
    if(string == "firstRow") {
        return FirstRow;
    }
    else if(string == "lastCol") {
        return LastCol;
    }
    else if(string == "lastRow") {
        return LastRow;
    }
    else if(string == "neCell") {
        return NeCell;
    }
    else if(string == "nwCell") {
        return NwCell;
    }
    else if(string == "seCell") {
        return SeCell;
    }
    else if(string == "swCell") {
        return SwCell;
    }
    else if(string == "tblBg") {
        return TblBg;
    }
    else if(string == "wholeTbl") {
        return WholeTbl;
    }
}

void TableStyle::setId(const QString& id)
{
    m_id = id;
}


QString TableStyle::id() const
{
    return m_id;
}

TableStyleProperties TableStyle::propertiesForType(TableStyle::Type type)
{
    if(!m_properties.contains(type)) {
        if(!m_properties.contains(TableStyle::WholeTbl)) {
            //Return at least something
            return TableStyle();
        }

        //Asume WholeTbl
        return m_properties.value(TableStyle::WholeTbl);
    }

    return m_properties.value(type);
}

void TableStyle::addProperties(TableStyleProperties properties)
{
    m_properties.insert(properties.type, properties);
}

const MSOOXML::TableStyle& TableStyleList::tableStyle(const QString& id)
{
    return m_styles.value(id);
}

void TableStyleList::insertStyle(QString id, TableStyle style)
{
    m_styles.insert(id, style);
}

MsooXmlDrawingTableStyleReader::MsooXmlDrawingTableStyleReader(KoOdfWriters* writers)
: MsooXmlReader(writers)
{
}

MsooXmlDrawingTableStyleReader::~MsooXmlDrawingTableStyleReader()
{
}

KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read(MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<MsooXmlDrawingTablesStyleContext*>(context);
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
        if(isStartElement()) {
            TRY_READ_IF(tblStyle)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblStyle
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tblStyle()
{
    READ_PROLOGUE

    while(!atEnd()) {
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
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    QXmlAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(styleId)
    m_context->styleList.insertStyle(styleId, m_currentStyle);
    m_currentStyle = TableStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wholeTbl
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_wholeTbl()
{
    READ_PROLOGUE

    m_currentStylePropertiesType = TableStyle::WholeTbl;

    while(!atEnd()) {
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcStyle
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcStyle()
{
    READ_PROLOGUE

    while(!atEnd()) {
        if(isStartElement()) {
//             TRY_READ_IF(cell3D)
//             ELSE_TRY_READ_IF(fill)
//             ELSE_TRY_READ_IF(fillRef)
            /*ELSE_*/TRY_READ_IF(tcBrd)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undefine CURRENT_EL
#define CURRENT_EL tcTxStyle
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcTxStyle()
{
}

#undefine CURRENT_EL
#define CURRENT_EL tcBrd
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcBrd()
{
    READ_PROLOGUE

    while(!atEnd()) {
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
            m_currentStyleProperties.addBorder(m_currentBorder);
            m_currentBorder = Border();
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    READ_EPILOGUE
}

#undefine CURRENT_EL
#define CURRENT_EL bottom
KoFilter::ConversionStatus MSOOXML::MsooXmlDrawingTableStyleReader::read_bottom()
{
    READ_PROLOGUE

    m_currentBorderSide = TableStyleProperties::Bottom;

    while(!atEnd()) {
        if(isStartElement()) {
            TRY_READ_IF(ln)
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    m_currentBorder.setPen(m_currentPen);

    READ_EPILOGUE
}

#undefine CURRENT_EL
#define CURRENT_EL top
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_top()
{
    READ_PROLOGUE

    m_currentBorderSide = TableStyleProperties::Top;

    while(!atEnd()) {
        if(isStartElement()) {
            TRY_READ_IF(ln)
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    m_currentBorder.setPen(m_currentPen);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL left
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_left()
{
    READ_PROLOGUE

    m_currentBorderSide = TableStyleProperties::Left;

    while(!atEnd()) {
        if(isStartElement()) {
            TRY_READ_IF(ln)
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    m_currentBorder.setPen(m_currentPen);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL right
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_right()
{
    READ_PROLOGUE

    m_currentBorderSide = TableStyleProperties::Right;

    while(!atEnd()) {
        if(isStartElement()) {
            TRY_READ_IF(ln)
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    m_currentBorder.setPen(m_currentPen);

    READ_EPILOGUE
}

#include <MsooXmlCommonReaderDrawingMLImpl.h>
