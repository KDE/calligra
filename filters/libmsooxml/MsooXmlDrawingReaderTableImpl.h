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

/**
 * This file deals with the tables designed for the DrawingML namespace
 * the table starts at tbl §21.1.3.13
 */

#include "MsooXmlTableStyle.h"
using namespace MSOOXML;

#undef CURRENT_EL
#define CURRENT_EL tbl
//! tbl (Table) §21.1.3.13
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tbl()
{
    m_table = KoTable::create();

    READ_PROLOGUE

    bool tableSetByMe = false;
    if (!m_insideTable) {
        // Handling nested tables
        tableSetByMe = true;
        m_insideTable = true;
    }

    m_tableStyle = 0;

    if (!d->tableStyleList) {
        d->tableStyleList = new QMap<QString, MSOOXML::DrawingTableStyle*>;

        QString tableStylesFile;
        QString tableStylesPath;
        MSOOXML::Utils::splitPathAndFile(m_context->tableStylesFilePath, &tableStylesPath, &tableStylesFile);

        MSOOXML::MsooXmlDrawingTableStyleReader tableStyleReader(this);
        MSOOXML::MsooXmlDrawingTableStyleContext tableStyleReaderContext(m_context->import, tableStylesPath,
                                                                        tableStylesFile, &m_context->slideMasterProperties->theme,
                                                                        d->tableStyleList, m_context->colorMap);
        m_context->import->loadAndParseDocument(&tableStyleReader, m_context->tableStylesFilePath, &tableStyleReaderContext);
    }

    m_table->setName(QLatin1String("Table") + QString::number(m_currentTableNumber + 1));
    m_currentTableRowNumber = 0;
    m_currentTableColumnNumber = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tblPr)
            ELSE_TRY_READ_IF(tblGrid)
            ELSE_TRY_READ_IF(tr)
            SKIP_UNKNOWN
//             ELSE_WRONG_FORMAT
        }
    }

    defineStyles();

    m_table->saveOdf(*body, *mainStyles);

    m_currentTableNumber++;

    if (tableSetByMe) {
        m_insideTable = false;
    }

    READ_EPILOGUE
}

void MSOOXML_CURRENT_CLASS::defineStyles()
{
    const int rowCount = m_table->rowCount();
    const int columnCount = m_table->columnCount();
    QPair<int, int> spans;

    MSOOXML::DrawingTableStyleConverterProperties converterProperties;
    converterProperties.setRowCount(rowCount);
    converterProperties.setColumnCount(columnCount);
    converterProperties.setRoles(m_activeRoles);
    converterProperties.setLocalStyles(m_localTableStyles);
    // TODO: converterProperties.setLocalDefaulCelltStyle()
    MSOOXML::DrawingTableStyleConverter styleConverter(converterProperties, m_tableStyle);
    for(int row = 0; row < rowCount; ++row ) {
        for(int column = 0; column < columnCount; ++column ) {
            spans.first = m_table->cellAt(row, column)->rowSpan();
            spans.second = m_table->cellAt(row, column)->columnSpan();
            KoCellStyle::Ptr style = styleConverter.style(row, column, spans);
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
    TRY_READ_ATTR_WITHOUT_NS(bandCol)
    if(MSOOXML::Utils::convertBooleanAttr(bandCol)) {
        m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::ColumnBanded;
    }
    TRY_READ_ATTR_WITHOUT_NS(bandRow)
    if(MSOOXML::Utils::convertBooleanAttr(bandRow)) {
        m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::RowBanded;
    }
    TRY_READ_ATTR_WITHOUT_NS(firstCol)
    if(MSOOXML::Utils::convertBooleanAttr(firstCol)) {
        m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::FirstCol;
    }
    TRY_READ_ATTR_WITHOUT_NS(firstRow)
    if(MSOOXML::Utils::convertBooleanAttr(firstRow)) {
        m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::FirstRow;
    }
    TRY_READ_ATTR_WITHOUT_NS(lastCol)
    if(MSOOXML::Utils::convertBooleanAttr(lastCol)) {
        m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::FirstCol;
    }
    TRY_READ_ATTR_WITHOUT_NS(lastRow)
    if(MSOOXML::Utils::convertBooleanAttr(lastCol)) {
        m_activeRoles |= MSOOXML::DrawingTableStyleConverterProperties::LastCol;
    }
//     TRY_READ_ATTR_WITHOUT_NS(rtl)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
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
            SKIP_UNKNOWN
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
        BREAK_IF_END_OF(CURRENT_EL)
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
        BREAK_IF_END_OF(CURRENT_EL)
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
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tc)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_WRONG_FORMAT
            SKIP_UNKNOWN
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

    TRY_READ_ATTR_WITHOUT_NS(vMerge)
    TRY_READ_ATTR_WITHOUT_NS(hMerge)

    if (vMerge == "1" || hMerge == "1") {
        cell->setCovered(true);
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == "a:txBody") {
                QBuffer buffer;

                KoXmlWriter* oldBody = body;
                KoXmlWriter newBody(&buffer, oldBody->indentLevel()+1);
                body = &newBody;

                TRY_READ_IN_CONTEXT(DrawingML_txBody);

                KoRawCellChild* textChild = new KoRawCellChild(buffer.data());
                cell->appendChild(textChild);

                body = oldBody;
            }
            // ELSE_TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(tcPr)
            SKIP_UNKNOWN
            // ELSE_WRONG_FORMAT
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
    QString id = text().toString();

    QString predefinedTable = getPresetTable(id);
    if (!predefinedTable.isEmpty()) {
        predefinedTable.prepend("<a:tblStyleLst xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">");
        predefinedTable.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
        predefinedTable.append("</a:tblStyleLst>");
        QString tableStylesFile;
        QString tableStylesPath;
        QBuffer tempDevice;
        tempDevice.setData(predefinedTable.toLatin1());
        tempDevice.open(QIODevice::ReadOnly);
        MSOOXML::Utils::splitPathAndFile(m_context->tableStylesFilePath, &tableStylesPath, &tableStylesFile);
        MSOOXML::MsooXmlDrawingTableStyleReader tableStyleReader(this);
        MSOOXML::MsooXmlDrawingTableStyleContext tableStyleReaderContext(m_context->import, tableStylesPath,
                                                                         tableStylesFile, &m_context->slideMasterProperties->theme,
                                                                         d->tableStyleList, m_context->colorMap);
        m_context->import->loadAndParseFromDevice(&tableStyleReader, &tempDevice, &tableStyleReaderContext);
    }

    m_tableStyle = d->tableStyleList->value(text().toString());
    readNext();

    READ_EPILOGUE
}

// Local styles

#undef CURRENT_EL
#define CURRENT_EL tcPr
//! tcPR (Table Cell Properties) §21.1.3.17
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tcPr()
{
    READ_PROLOGUE

    m_currentLocalStyleProperties = new MSOOXML::TableStyleProperties();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
//             TRY_READ_IF(blipFill)
//             ELSE_TRY_READ_IF(cell3D)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_TRY_READ_IF(gradFill)
//             ELSE_TRY_READ_IF(grpFill)
//             ELSE_TRY_READ_IF(lnBlToTr)
//             ELSE_TRY_READ_IF(lnTlToBr)
//             ELSE_TRY_READ_IF(pattFill)
            if (QUALIFIED_NAME_IS(lnL)) {
                TRY_READ(Table_lnL)
                m_currentLocalStyleProperties->left = m_currentBorder;
                m_currentLocalStyleProperties->setProperties |= TableStyleProperties::LeftBorder;
            }
            else if (QUALIFIED_NAME_IS(lnR)) {
                TRY_READ(Table_lnR)
                m_currentLocalStyleProperties->right = m_currentBorder;
                m_currentLocalStyleProperties->setProperties |= TableStyleProperties::RightBorder;
            }
            else if (QUALIFIED_NAME_IS(lnT)) {
                TRY_READ(Table_lnT)
                m_currentLocalStyleProperties->top = m_currentBorder;
                m_currentLocalStyleProperties->setProperties |= TableStyleProperties::TopBorder;
            }
            else if (QUALIFIED_NAME_IS(lnB)) {
                TRY_READ(Table_lnB)
                m_currentLocalStyleProperties->bottom = m_currentBorder;
                m_currentLocalStyleProperties->setProperties |= TableStyleProperties::BottomBorder;
            }
            else if (QUALIFIED_NAME_IS(solidFill)) {
                TRY_READ(solidFill)
                m_currentLocalStyleProperties->backgroundColor = m_currentColor;
                m_currentLocalStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BackgroundColor;
                if (m_currentAlpha > 0) {
                    m_currentLocalStyleProperties->backgroundOpacity = m_currentAlpha;
                    m_currentLocalStyleProperties->setProperties |= MSOOXML::TableStyleProperties::BackgroundOpacity;
                }
            }
            SKIP_UNKNOWN // Added to make sure that solidfill eg inside 'lnT' does not mess with the color
        }
    }

    m_localTableStyles.setLocalStyle(m_currentLocalStyleProperties, m_currentTableRowNumber, m_currentTableColumnNumber);

    READ_EPILOGUE
}
