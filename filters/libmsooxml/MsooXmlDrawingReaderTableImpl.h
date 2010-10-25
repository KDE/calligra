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

/**
* This file deals with the tables designed for the DrawingML namespace
* the table starts at tbl §21.1.3.13
*/

#include <MsooXmlDrawingTableStyleReader.h>

#undef CURRENT_EL
#define CURRENT_EL tbl
//! tbl (Table) §21.1.3.13
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tbl()
{
    READ_PROLOGUE
    MSOOXML::Utils::XmlWriteBuffer tableBuf;
    body = tableBuf.setWriter(body);

    m_currentTableName = QLatin1String("Table") + QString::number(m_currentTableNumber + 1);
    m_currentTableStyle = KoGenStyle(KoGenStyle::TableAutoStyle, "table");
    m_currentTableWidth = 0.0;
    m_currentTableRowNumber = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(tblPr)
            ELSE_TRY_READ_IF(tblGrid)
            ELSE_TRY_READ_IF(tr)
            ELSE_WRONG_FORMAT
        }
    }

    body = tableBuf.originalWriter();
//TODO this seems to not be needed anymore, it seems it's taken care up in the hierarchy.
//However, I'm not absolutely sure so I'll just comment it for now.
//     body->startElement("draw:frame");
//     body->addAttribute("svg:x", QString("%1pt").arg(EMU_TO_POINT(m_svgX)));
//     body->addAttribute("svg:y", QString("%1pt").arg(EMU_TO_POINT(m_svgY)));
//     body->addAttribute("svg:width", QString("%1pt").arg(EMU_TO_POINT(m_svgWidth)));
//     body->addAttribute("svg:height", QString("%1pt").arg(EMU_TO_POINT(m_svgHeight)));

    body->startElement("table:table");
    body->addAttribute("table:name", m_currentTableName);
    m_currentTableStyle.addProperty(
        "style:width", QString::number(m_currentTableWidth) + QLatin1String("cm"),
        KoGenStyle::TableType);
    //! @todo fix hardcoded table:align
    m_currentTableStyle.addProperty("table:align", "left");

    //! @todo fix hardcoded style:master-page-name
    m_currentTableStyle.addAttribute("style:master-page-name", "Standard");
    const QString tableStyleName(
        mainStyles->insert(
            m_currentTableStyle,
            m_currentTableName,
            KoGenStyles::DontAddNumberToName)
    );
    body->addAttribute("table:style-name", tableStyleName);

    foreach (const QString columnWidth, m_columnsWidth) {
        body->startElement("table:table-column");

        KoGenStyle columnStyle = KoGenStyle(KoGenStyle::TableColumnAutoStyle, "table-column");
        columnStyle.addProperty("style:column-width", columnWidth);

        const QString columnStyleName = mainStyles->insert(columnStyle, "col");

        body->addAttribute("table:style-name", columnStyleName);
        body->addAttribute("table:default-cell-style-name", m_defaultCellStyle);

        body->endElement(); // table:table-column
    }
    m_columnsWidth.clear();

    (void)tableBuf.releaseWriter();
    body->endElement(); // table:table

//     body->endElement(); // draw:frame

    m_currentTableNumber++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblPr
//! tblPr handler (Table Properties) §21.1.3.15
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tblPr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(bandCol)
    TRY_READ_ATTR(bandRow)
    TRY_READ_ATTR(firstCol)
    TRY_READ_ATTR(firstRow)
    TRY_READ_ATTR(lastCol)
    TRY_READ_ATTR(lastRow)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
//             TRY_READ_IF(blipFill)
//             ELSE_TRY_READ_IF(effectDrag)
//             ELSE_TRY_READ_IF(effectLst)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_TRY_READ_IF(gradFill)
//             ELSE_TRY_READ_IF(grpFill)
//             ELSE_TRY_READ_IF(noFill)
//             ELSE_TRY_READ_IF(pattFill)
//             ELSE_TRY_READ_IF(solidFill)
//             ELSE_TRY_READ_IF(tableStyle)
//             ELSE_TRY_READ_IF(tableStyle)
            /*ELSE_*/TRY_READ_IF(tableStyleId)
//             ELSE_WRONG_FORMAT
        }
    }

    // Needed temporarily as there are no lists necessarily
    if (m_context->tableStyleList) {
        MSOOXML::TableStyle tableStyle = m_context->tableStyleList->tableStyle(m_styleId);

        m_defaultCellStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::WholeTbl).saveStyle(*mainStyles);

        if(bandCol == "1") {
            m_oddColumnStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::Band1Horizontal).saveStyle(*mainStyles);
            m_evenColumnStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::Band2Horizontal).saveStyle(*mainStyles);
        }
        if(bandRow == "1") {
            m_oddRowStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::Band1Vertical).saveStyle(*mainStyles);
            m_evenRowStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::Band2Vertical).saveStyle(*mainStyles);
        }
        if(firstCol == "1") {
            m_firstColStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::FirstCol).saveStyle(*mainStyles);
        }
        if(firstRow == "1") {
            m_firstRowStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::FirstRow).saveStyle(*mainStyles);
        }
        if(lastCol == "1") {
            m_lastColStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::LastCol).saveStyle(*mainStyles);
        }
        if(firstCol == "1") {
            m_lastRowStyle = tableStyle.propertiesForType(MSOOXML::TableStyleProperties::LastRow).saveStyle(*mainStyles);
        }
    }
//     TRY_READ_ATTR(rtl)

    READ_EPILOGUE
}


#undef CURRENT_EL
#define CURRENT_EL tblGrid
//! tblGrid handler (Table Grid) §21.1.3.14
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tblGrid()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(gridCol)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gridCol
//! gridCol handler (Grid Column Definition) §21.1.3.2
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_gridCol()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(w)
    const QString widthCm(MSOOXML::Utils::EMU_to_ODF(w));

    m_columnsWidth.append(widthCm);
    m_currentTableWidth += widthCm.left(widthCm.length()-2).toFloat();

    while(!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
//         if(isStartElement()) {
//             TRY_READ_IF(extLst)
//         }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tr
//! tr handler (Table Row)§21.1.3.18
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tr()
{
    READ_PROLOGUE
    MSOOXML::Utils::XmlWriteBuffer rowBuf;
    body = rowBuf.setWriter(body);
    m_currentTableColumnNumber = 0;
    m_currentTableRowStyle = KoGenStyle(KoGenStyle::TableRowAutoStyle, "table-row");
    m_currentTableRowNumber = 0;

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(h)
    m_currentTableRowStyle.addProperty("style:row-height", MSOOXML::Utils::EMU_to_ODF(h), KoGenStyle::TableRowType);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(tc)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_WRONG_FORMAT
        }
    }

    body = rowBuf.originalWriter();
    body->startElement("table:table-row");

    //! @todo add style:keep-together property
    //! @todo add fo:keep-together
    const QString tableRowStyleName(
        mainStyles->insert(
            m_currentTableRowStyle,
            m_currentTableName + '.' + QString::number(m_currentTableRowNumber + 1),
            KoGenStyles::DontAddNumberToName)
    );
    body->addAttribute("table:style-name", tableRowStyleName);


    (void)rowBuf.releaseWriter();
    body->endElement(); // table:table-row

    m_currentTableRowNumber++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tc
//! tc handler (Table Cell) § 21.1.3.16
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tc()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(gridSpan)
    TRY_READ_ATTR_WITHOUT_NS(rowSpan)

    MSOOXML::Utils::XmlWriteBuffer cellBuf;
    body = cellBuf.setWriter(body);
    m_currentTableCellStyle = KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if(qualifiedName() == "a:txBody") {
                TRY_READ(DrawingML_txBody);
            }
//             ELSE_TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(tcPr)
//             ELSE_WRONG_FORMAT
        }
    }

    body = cellBuf.originalWriter();

    body->startElement("table:table-cell");

    if (!gridSpan.isEmpty()) {
        body->addAttribute("table:number-columns-spanned", gridSpan);
    }
    if (!rowSpan.isEmpty()) {
        body->addAttribute("table:number-rows-spanned", rowSpan);
    }

    bool lastColumn = false;

    READ_EPILOGUE_WITHOUT_RETURN

    readNext();
    if (QUALIFIED_NAME_IS(tr)) {
        lastColumn = true;
    }
    undoReadNext();

//     m_currentTableCellStyle.addProperty("fo:border-bottom", "0.5pt solid #000000");
// 
//     if (m_currentTableColumnNumber == 0) {
//         if (!m_borderStyles.key(LeftBorder).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:border-left", m_borderStyles.key(LeftBorder));
//         }
//         if (!m_borderPaddings.key(LeftBorder).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:padding-left", m_borderPaddings.key(LeftBorder));
//         }
//     }
//     else {
//         if (!m_borderStyles.key(InsideV).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:border-left", m_borderStyles.key(InsideV));
//         }
//         if (!m_borderPaddings.key(InsideV).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:padding-left", m_borderPaddings.key(InsideV));
//         }
//     }
//     if (lastColumn) {
//         if (!m_borderStyles.key(RightBorder).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:border-right", m_borderStyles.key(RightBorder));
//         }
//         if (!m_borderPaddings.key(RightBorder).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:padding-right", m_borderPaddings.key(RightBorder));
//         }
//     }
//     if (m_currentTableRowNumber == 0) {
//         if (!m_borderStyles.key(TopBorder).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:border-top", m_borderStyles.key(TopBorder));
//         }
//         if (!m_borderPaddings.key(TopBorder).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:padding-top", m_borderPaddings.key(TopBorder));
//         }
//     }
//     else {
//         if (!m_borderStyles.key(InsideH).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:border-top", m_borderStyles.key(InsideH));
//         }
//         if (!m_borderPaddings.key(InsideH).isEmpty()) {
//             m_currentTableCellStyle.addProperty("fo:padding-top", m_borderPaddings.key(InsideH));
//         }
//     }
// 
//     //! @todo real border style get from w:tblPr/w:tblStyle@w:val
//     //m_currentTableCellStyle.addProperty("fo:border", "0.5pt solid #000000");
// 
//     const QString tableCellStyleName(
//         mainStyles->insert(
//             m_currentTableCellStyle,
//             m_currentTableName + '.' + MSOOXML::Utils::columnName(m_currentTableColumnNumber)
//                 + QString::number(m_currentTableRowNumber + 1),
//             KoGenStyles::DontAddNumberToName)
//     );
//     body->addAttribute("table:style-name", tableCellStyleName);

    body->addAttribute("office:value-type", "string");

    (void)cellBuf.releaseWriter();
    body->endElement(); // table:table-cell

    m_currentTableColumnNumber++;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL tcPr
//! tcPr handler  (Table Cell Properties) §21.1.3.17
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tcPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
//            TRY_READ_IF(blipFill)
//            ELSE_TRY_READ_IF(cell3D)
//            ELSE_TRY_READ_IF(extLst)
//            ELSE_TRY_READ_IF(gradFill)
//            ELSE_TRY_READ_IF(grpFill)
//            ELSE_TRY_READ_IF(headers)
//            ELSE_TRY_READ_IF(lnB)
//            ELSE_TRY_READ_IF(lnBlToTr)
//            ELSE_TRY_READ_IF(lnL)
//            ELSE_TRY_READ_IF(lnR)
//            ELSE_TRY_READ_IF(lnTlToBr)
//            ELSE_TRY_READ_IF(noFill)
//            ELSE_TRY_READ_IF(pattFill)
//            ELSE_TRY_READ_IF(solidFill)
//             ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tableStyleId
//! tableStyleId (Table Style ID) §21.1.3.12
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tableStyleId()
{
    READ_PROLOGUE

    readNext();
    m_styleId = text().toString();
    readNext();

    READ_EPILOGUE
}
