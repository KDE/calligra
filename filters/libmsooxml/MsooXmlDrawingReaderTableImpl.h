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

#undef CURRENT_EL
#define CURRENT_EL tbl
//! tbl (Table) §21.1.3.13
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tbl()
{
    READ_PROLOGUE

    m_table = new KoTable;

    m_table->setName(QLatin1String("Table") + QString::number(m_currentTableNumber + 1));
    m_currentTableRowNumber = 0;
    m_currentTableColumnNumber = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(tblPr)
            ELSE_TRY_READ_IF(tblGrid)
            ELSE_TRY_READ_IF(tr)
//             ELSE_WRONG_FORMAT
        }
    }

    defineStyles();

    m_table->saveOdf(*body, *mainStyles);

    delete m_table;

    m_currentTableNumber++;

    READ_EPILOGUE
}

void MSOOXML_CURRENT_CLASS::defineStyles()
{
    const int rowCount = m_table->rowCount();
    const int columnCount = m_table->columnCount();

    MSOOXML::TableStyleInstanceProperties styleProperties(rowCount, columnCount);
    styleProperties.roles(m_activeRoles);

    MSOOXML::TableStyleInstance styleInstance(&m_tableStyle, styleProperties);

    for(int row = 0; row < rowCount; ++row ) {
        for(int column = 0; column < columnCount; ++column ) {
            KoCellStyle::Ptr style = styleInstance.style(row, column);
            m_table->cellAt(row, column)->setStyle(style);
        }
    }
}

#undef CURRENT_EL
#define CURRENT_EL tblPr
//! tblPr handler (Table Properties) §21.1.3.15
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tblPr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(bandCol)
    if(MSOOXML::Utils::convertBooleanAttr(bandCol)) {
        m_activeRoles |= MSOOXML::TableStyleInstanceProperties::ColumnBanded;
    }
    TRY_READ_ATTR(bandRow)
    if(MSOOXML::Utils::convertBooleanAttr(bandRow)) {
        m_activeRoles |= MSOOXML::TableStyleInstanceProperties::RowBanded;
    }
    TRY_READ_ATTR(firstCol)
    if(MSOOXML::Utils::convertBooleanAttr(firstCol)) {
        m_activeRoles |= MSOOXML::TableStyleInstanceProperties::FirstCol;
    }
    TRY_READ_ATTR(firstRow)
    if(MSOOXML::Utils::convertBooleanAttr(firstRow)) {
        m_activeRoles |= MSOOXML::TableStyleInstanceProperties::FirstRow;
    }
    TRY_READ_ATTR(lastCol)
    if(MSOOXML::Utils::convertBooleanAttr(lastCol)) {
        m_activeRoles |= MSOOXML::TableStyleInstanceProperties::FirstCol;
    }
    TRY_READ_ATTR(lastRow)
    if(MSOOXML::Utils::convertBooleanAttr(lastCol)) {
        m_activeRoles |= MSOOXML::TableStyleInstanceProperties::LastCol;
    }
//     TRY_READ_ATTR(rtl)

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
            /*ELSE_*/TRY_READ_IF(tableStyleId)
//             ELSE_WRONG_FORMAT
        }
    }

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

    const qreal columnWidth = EMU_TO_POINT(w.toFloat());

    KoColumn* column = m_table->columnAt(m_currentTableColumnNumber++);
    KoColumnStyle::Ptr style = KoColumnStyle::create();
    style->setWidth(columnWidth);
    column->setStyle(style);

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

    m_currentTableColumnNumber = 0;

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(h)
    KoRow* row = m_table->rowAt(m_currentTableRowNumber);
    KoRowStyle::Ptr style = KoRowStyle::create();
    style->setHeight(EMU_TO_POINT(h.toFloat()));
    style->setHeightType(KoRowStyle::MinimumHeight);
    row->setStyle(style);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(tc)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_WRONG_FORMAT
        }
    }

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

    KoCell* cell = m_table->cellAt(m_currentTableRowNumber, m_currentTableColumnNumber);

    if (!gridSpan.isEmpty()) {
        cell->setColumnSpan(gridSpan.toInt());
    }
    if (!rowSpan.isEmpty()) {
        cell->setRowSpan(rowSpan.toInt());
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if(qualifiedName() == "a:txBody") {
                QBuffer* buffer = new QBuffer;
                KoXmlWriter* oldBody = body;
                body = new KoXmlWriter(buffer, oldBody->indentLevel()+1);

                TRY_READ(DrawingML_txBody);

                KoRawCellChild* textChild = new KoRawCellChild(buffer);
                cell->appendChild(textChild);
                delete body;
                body = oldBody;
            }
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_TRY_READ_IF(tcPr)
//             ELSE_WRONG_FORMAT
        }
    }

    m_currentTableColumnNumber++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tableStyleId
//! tableStyleId (Table Style ID) §21.1.3.12
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tableStyleId()
{
    READ_PROLOGUE

    readNext();
    m_tableStyle = m_context->tableStyleList->tableStyle(text().toString());
    readNext();
 
    READ_EPILOGUE
}
