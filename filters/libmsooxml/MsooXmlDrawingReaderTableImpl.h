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
    MSOOXML::Utils::XmlWriteBuffer tableBuf;
    body = tableBuf.setWriter(body);

    //const QXmlStreamAttributes attrs(attributes());
    //TRY_READ_ATTR(bandDol)
    //TRY_READ_ATTR(bandRow)
    //TRY_READ_ATTR(firstCol)
    //TRY_READ_ATTR(firstRow)
    //TRY_READ_ATTR(lastCol)
    //TRY_READ_ATTR(lastRow)
    //TRY_READ_ATTR(rtl)

    m_currentTableName = QLatin1String("Table") + QString::number(m_currentTableNumber + 1);
    m_currentTableStyle = KoGenStyle(KoGenStyle::TableAutoStyle, "table");
    m_currentTableWidth = 0.0;
    m_currentTableRowNumber = 0;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            TRY_READ_IF(tblPr)
            ELSE_TRY_READ_IF(tblGrid)
            ELSE_TRY_READ_IF(tr)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }

    body = tableBuf.originalWriter();

    body->startElement("draw:frame");
    body->addAttribute("svg:x", QString("%1pt").arg(EMU_TO_POINT(m_svgX)));
    body->addAttribute("svg:y", QString("%1pt").arg(EMU_TO_POINT(m_svgY)));
    body->addAttribute("svg:width", QString("%1pt").arg(EMU_TO_POINT(m_svgWidth)));
    body->addAttribute("svg:height", QString("%1pt").arg(EMU_TO_POINT(m_svgHeight)));

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
    uint column = 0;
    foreach (const ColumnStyleInfo& columnStyle, columnStyles) {
        body->startElement("table:table-column");
        const QString columnStyleName(
            mainStyles->insert(
                *columnStyle.style,
                m_currentTableName + '.' + MSOOXML::Utils::columnName(column),
                KoGenStyles::DontAddNumberToName)
        );
        body->addAttribute("table:style-name", columnStyleName);
        if (columnStyle.count > 1) {
            body->addAttribute("table:number-columns-repeated", columnStyle.count);
        }
        body->endElement(); // table:table-column
        column += columnStyle.count;
    }
    clearColumnStyles();

    (void)tableBuf.releaseWriter();
    body->endElement(); // table:table

    body->endElement(); // draw:frame

    m_currentTableNumber++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblPr
//! tblPr handler (Table Properties) §21.1.3.15
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tblPr()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
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
        BREAK_IF_END_OF(CURRENT_EL);
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
        if (isStartElement()) {
            TRY_READ_IF(gridCol)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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

    KoGenStyle *columnStyle = new KoGenStyle(KoGenStyle::TableColumnAutoStyle, "table-column");
    if (!widthCm.isEmpty()) {
        columnStyle->addProperty("style:column-width", widthCm, KoGenStyle::TableColumnType);
        m_currentTableWidth += widthCm.left(widthCm.length()-2).toFloat();
    }
    // only add the style if it different than the previous; else just increate the counter
    if (columnStyles.isEmpty() || !(*columnStyles.last().style == *columnStyle)) {
        columnStyles.append(ColumnStyleInfo(columnStyle));
    }
    columnStyles.last().count++;

    while(!atEnd()) {
        readNext();
//         if(isStartElement()) {
//             TRY_READ_IF(extLst)
//         }
        BREAK_IF_END_OF(CURRENT_EL);
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
        if (isStartElement()) {
            TRY_READ_IF(tc)
//             ELSE_TRY_READ_IF(extLst)
//             ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
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
    MSOOXML::Utils::XmlWriteBuffer cellBuf;
    body = cellBuf.setWriter(body);
    m_currentTableCellStyle = KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if(qualifiedName() == "a:txBody") {
                TRY_READ(DrawingML_txBody);
            }
//             ELSE_TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(tcPr)
//             ELSE_WRONG_FORMAT
        }

        BREAK_IF_END_OF(CURRENT_EL);
    }

    body = cellBuf.originalWriter();
    body->startElement("table:table-cell");

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
        BREAK_IF_END_OF(CURRENT_EL);
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
    //text().toString();
    readNext();

    READ_EPILOGUE
}

#undef CURRENT_EL
void MSOOXML_CURRENT_CLASS::clearColumnStyles() {
    foreach (const ColumnStyleInfo& info, columnStyles) {
        delete info.style;
    }
    columnStyles.clear();
}
