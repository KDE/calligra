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
#include "MsooXmlTheme.h"


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

#include <QString>

using namespace MSOOXML;

MsooXmlDrawingTableStyleReader::MsooXmlDrawingTableStyleReader(KoOdfWriters* writers)
: MsooXmlCommonReader(writers)
, m_context(0)
, m_currentStyle(0)
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

    return result;
}

#undef CURRENT_EL
#define CURRENT_EL tblStyleLst
/*
 Parent elements:
 - [done] root

 Child elements:
 - [done] tblStyle (Table Style) §20.1.4.2.26

*/
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tblStyleLst()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tblStyle)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblStyle
/*
 Parent elements:
 - [done] tblStyleLst (§20.1.4.2.27)

 Child elements:
 - [done] band1H (Band 1 Horizontal) §20.1.4.2.1
 - [done] band1V (Band 1 Vertical) §20.1.4.2.2
 - [done] band2H (Band 2 Horizontal) §20.1.4.2.3
 - [done] band2V (Band 2 Vertical) §20.1.4.2.4
 - extLst (Extension List) §20.1.2.2.15
 - [done] firstCol (First Column) §20.1.4.2.11
 - [done] firstRow (First Row) §20.1.4.2.12
 - [done] lastCol (Last Column) §20.1.4.2.16
 - [done] lastRow (Last Row) §20.1.4.2.17
 - [done] neCell (Northeast Cell) §20.1.4.2.20
 - [done] nwCell (Northwest Cell) §20.1.4.2.21
 - [done] seCell (Southeast Cell) §20.1.4.2.23
 - [done] swCell (Southwest Cell) §20.1.4.2.24
 - [done] tblBg (Table Background) §20.1.4.2.25
 - [done] wholeTbl (Whole Table) §20.1.4.2.34
*/
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
            ELSE_TRY_READ_IF(tblBg)
            ELSE_TRY_READ_IF(wholeTbl)
            SKIP_UNKNOWN
//             ELSE_WRONG_FORMAT
        }
    }

    m_context->styleList->insert(styleId, m_currentStyle);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblBg
/*
 Parent elements:
 - tableStyle (§21.1.3.11);
 - [done] tblStyle (§20.1.4.2.26)

 Child elements:
 - effect (Effect) §20.1.4.2.7
 - effectRef (Effect Reference) §20.1.4.2.8
 - [done] fill (Fill) §20.1.4.2.9
 - [done] fillRef (Fill Reference) §20.1.4.2.10

*/
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tblBg()
{
    READ_PROLOGUE

    m_currentTableStyleProperties = m_currentStyle->properties(DrawingTableStyle::WholeTbl);
    if (m_currentTableStyleProperties == 0) {
        m_currentTableStyleProperties = new TableStyleProperties;
    }

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
            TRY_READ_IF(fill)
            else if (name() == "fillRef") {
                // NOTE: This is a heavy simplification for the moment
                // In reality we should use graphic properties in the cell-style
                // but it is not supported atm.
                TRY_READ(fillRef)
                if (m_currentColor.isValid()) {
                    m_currentTableStyleProperties->backgroundColor = m_currentColor;
                    m_currentTableStyleProperties->setProperties |= TableStyleProperties::BackgroundColor;
                }
            }
            SKIP_UNKNOWN
        }
    }

    // What should happen if tblBg defines a picture, is it meant for call cells separately or one picture
    // divided between the cells?
    m_currentStyle->addProperties(DrawingTableStyle::WholeTbl, m_currentTableStyleProperties);

    popCurrentDrawStyle();

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

    m_currentTableStyleProperties = m_currentStyle->properties(DrawingTableStyle::WholeTbl);
    if (m_currentTableStyleProperties == 0) {
        m_currentTableStyleProperties = new TableStyleProperties;
    }

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
/*
 Parent elements:
 - [done] band1H (§20.1.4.2.1);
 - [done] band1V (§20.1.4.2.2);
 - [done] band2H (§20.1.4.2.3);
 - [done] band2V (§20.1.4.2.4);
 - [done] firstCol (§20.1.4.2.11);
 - [done] firstRow (§20.1.4.2.12);
 - [done] lastCol (§20.1.4.2.16);
 - [done] lastRow (§20.1.4.2.17);
 - [done] neCell (§20.1.4.2.20);
 - [done] nwCell (§20.1.4.2.21);
 - [done] seCell (§20.1.4.2.23);
 - [done] swCell (§20.1.4.2.24);
 - [done] wholeTbl (§20.1.4.2.34)

 Child elements:
 - cell3D (Cell 3-D) §21.1.3.1
 - [done] fill (Fill) §20.1.4.2.9
 - [done] fillRef (Fill Reference) §20.1.4.2.10
 - [done] tcBdr (Table Cell Borders) §20.1.4.2.28

*/
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcStyle()
{
    READ_PROLOGUE

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if(isStartElement()) {
//             TRY_READ_IF(cell3D)
            TRY_READ_IF(fill)
            else if (name() == "fillRef") {
                // NOTE: This is a heavy simplification for the moment
                // In reality we should use graphic properties in the cell-style
                // but it is not supported atm.
                TRY_READ(fillRef)
                if (m_currentColor.isValid()) {
                    m_currentTableStyleProperties->backgroundColor = m_currentColor;
                    m_currentTableStyleProperties->setProperties |= TableStyleProperties::BackgroundColor;
                }
            }
            ELSE_TRY_READ_IF(tcBdr)
            SKIP_UNKNOWN
//             ELSE_WRONG_FORMAT
        }
    }

    popCurrentDrawStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tcTxStyle
/*
 Parent elements:
 - [done] band1H (§20.1.4.2.1);
 - [done] band1V (§20.1.4.2.2);
 - [done] band2H (§20.1.4.2.3);
 - [done] band2V (§20.1.4.2.4);
 - [done] firstCol (§20.1.4.2.11);
 - [done] firstRow (§20.1.4.2.12);
 - [done] lastCol (§20.1.4.2.16);
 - [done] lastRow (§20.1.4.2.17);
 - [done] neCell (§20.1.4.2.20);
 - [done] nwCell (§20.1.4.2.21);
 - [done] seCell (§20.1.4.2.23);
 - [done] swCell (§20.1.4.2.24);
 - [done] wholeTbl (§20.1.4.2.34)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - font (Font) §20.1.4.2.13
 - [done] fontRef (Font Reference) §20.1.4.1.17
 - hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33

*/
KoFilter::ConversionStatus MsooXmlDrawingTableStyleReader::read_tcTxStyle()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    m_currentColor = QColor();
    m_referredFontName.clear();
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(scrgbClr)
            //TODO hslClr hue, saturation, luminecence color
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(fontRef)
            SKIP_UNKNOWN
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(b)
    TRY_READ_ATTR_WITHOUT_NS(i)
    if (b == "on") {
        m_currentTextStyle.addProperty("fo:font-weight", "bold");
    }
    if (i == "on") {
        m_currentTextStyle.addProperty("fo:font-style", "italic");
    }
    if (m_currentColor.isValid()) {
        m_currentTextStyle.addProperty("fo:color", m_currentColor.name());
        m_currentColor = QColor();
    }
    if (!m_referredFontName.isEmpty()) {
        m_currentTextStyle.addProperty("fo:font-family", m_referredFontName);
    }

    m_currentTableStyleProperties->textStyle = m_currentTextStyle;

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
            SKIP_UNKNOWN
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
#define CURRENT_EL fill
/*
 Parent elements:
 - tblBg (§20.1.4.2.25);
 - [done] tcStyle (§20.1.4.2.29)

 Child elements:
 - blipFill (Picture Fill) §20.1.8.14
 - gradFill (Gradient Fill) §20.1.8.33
 - grpFill (Group Fill) §20.1.8.35
 - [done] noFill (No Fill) §20.1.8.44
 - pattFill (Pattern Fill) §20.1.8.47
 - [done] solidFill (Solid Fill) §20.1.8.54

*/
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
            if (QUALIFIED_NAME_IS(noFill)) {
                SKIP_EVERYTHING_AND_RETURN
            }
//             ELSE_TRY_READ_IF(pattFill)
            else if(QUALIFIED_NAME_IS(solidFill)) {
                TRY_READ(solidFill)
                m_currentTableStyleProperties->backgroundColor = m_currentColor;
                m_currentTableStyleProperties->setProperties |= TableStyleProperties::BackgroundColor;
                if (m_currentAlpha > 0) {
                    m_currentTableStyleProperties->backgroundOpacity = m_currentAlpha;
                    m_currentTableStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BackgroundOpacity;
                }
            }
            SKIP_UNKNOWN
//             ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#include "MsooXmlDrawingMLSharedImpl.h"

