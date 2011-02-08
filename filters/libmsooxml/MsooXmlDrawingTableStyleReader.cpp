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

MsooXmlDrawingTableStyleReader::MsooXmlDrawingTableStyleReader(KoOdfWriters* writers)
: MsooXmlCommonReader(writers)
, m_context(0)
, m_currentStyle(0)
, m_currentBorder()
, m_currentTableStyleProperties()
{
}

MsooXmlDrawingTableStyleReader::~MsooXmlDrawingTableStyleReader()
{
}

MsooXmlDrawingTableStyleContext::MsooXmlDrawingTableStyleContext(MsooXmlImport* _import, const QString& _path, const QString& _file, DrawingMLTheme* _themes, QMap< QString, DrawingTableStyle* >* _styleList, QMap< QString, QString > _colorMap)
: styleList(_styleList)
, import(_import)
, path(_path)
, file(_file)
, themes(_themes)
, colorMap(_colorMap)
{
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
        BREAK_IF_END_OF(CURRENT_EL)
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

    m_currentStyle = new DrawingTableStyle;

    QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(styleId)

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(band1H)
            ELSE_TRY_READ_IF(band1V)
            ELSE_TRY_READ_IF(band2H)
            ELSE_TRY_READ_IF(band2V)
//             ELSE_TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(firstCol)
            ELSE_TRY_READ_IF(firstRow)
            ELSE_TRY_READ_IF(lastCol)
            ELSE_TRY_READ_IF(lastRow)
            ELSE_TRY_READ_IF(neCell)
            ELSE_TRY_READ_IF(nwCell)
            ELSE_TRY_READ_IF(seCell)
            ELSE_TRY_READ_IF(swCell)
//             ELSE_TRY_READ_IF(tblBg)
            ELSE_TRY_READ_IF(wholeTbl)
//             ELSE_WRONG_FORMAT
        }
    }

    m_context->styleList->insert(styleId, m_currentStyle);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL band1H
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_band1H()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::Band1Horizontal, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL band1V
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_band1V()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::Band1Vertical, m_currentTableStyleProperties);

    READ_EPILOGUE

}

#undef CURRENT_EL
#define CURRENT_EL band2H
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_band2H()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::Band2Horizontal, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL band2V
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_band2V()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::Band2Horizontal, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL firstCol
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_firstCol()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::FirstCol, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL firstRow
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_firstRow()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::FirstRow, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lastCol
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_lastCol()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::LastCol, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lastRow
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_lastRow()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::LastRow, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL neCell
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_neCell()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::NeCell, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL nwCell
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_nwCell()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::NwCell, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL seCell
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_seCell()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::SeCell, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL swCell
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_swCell()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::SwCell, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL wholeTbl
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_wholeTbl()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = new TableStyleProperties;

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(tcStyle)
            ELSE_TRY_READ_IF(tcTxStyle)
            ELSE_WRONG_FORMAT
        }
    }

    m_currentStyle->addProperties(DrawingTableStyle::WholeTbl, m_currentTableStyleProperties);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcStyle
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcStyle()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
//             TRY_READ_IF(cell3D)
            /*ELSE_*/TRY_READ_IF(fill)
//             ELSE_TRY_READ_IF(fillRef)
            ELSE_TRY_READ_IF(tcBdr)
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

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcBdr
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcBdr()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(bottom)
//             ELSE_TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(insideH)
            ELSE_TRY_READ_IF(insideV)
            ELSE_TRY_READ_IF(left)
            ELSE_TRY_READ_IF(right)
            ELSE_TRY_READ_IF(tl2br)
            ELSE_TRY_READ_IF(top)
            ELSE_TRY_READ_IF(tr2bl)
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
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->bottom = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::BottomBorder;
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
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->top = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::TopBorder;
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
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->left = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::LeftBorder;
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
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->right = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::RightBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tl2br
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tl2br()
{
    READ_PROLOGUE

    while(!atEnd()) {
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->tl2br = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::Tl2brBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL)
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tr2bl
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tr2bl()
{
    READ_PROLOGUE

    while(!atEnd()) {
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->tr2bl = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::Tr2blBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL)
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL insideV
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_insideV()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->insideV = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::InsideVBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL insideH
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_insideH()
{
    READ_PROLOGUE

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(ln)) {
                TRY_READ(Table_ln)
                m_currentTableStyleProperties->insideH = m_currentBorder;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::InsideHBorder;
            }
//             ELSE_TRY_READ_IF(lnRef)
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

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
        BREAK_IF_END_OF(CURRENT_EL)
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

#undef CURRENT_EL
#define CURRENT_EL fill
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_fill()
{
    READ_PROLOGUE
    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
//             TRY_READ_IF(blipFill)
//             ELSE_TRY_READ_IF(grandFill)
//             ELSE_TRY_READ_IF(grpFill)
            /*else */if(QUALIFIED_NAME_IS(noFill)) {
                SKIP_EVERYTHING_AND_RETURN
            }
//             ELSE_TRY_READ_IF(pattFill)
            else if(QUALIFIED_NAME_IS(solidFill)) {
                TRY_READ(solidFill)
                m_currentTableStyleProperties->backgroundColor = m_currentColor;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::BackgroundColor;
            }
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#define blipFill_NS "a"

#include <MsooXmlCommonReaderImpl.h>

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "p" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h>
