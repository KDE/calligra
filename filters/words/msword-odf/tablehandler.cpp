/* This file is part of the Calligra project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "tablehandler.h"

#include <wv2/src/word97_generated.h>
#include "texthandler.h"
#include "document.h"
#include "conversion.h"
#include "msdoc.h"
#include "MsDocDebug.h"

#include <QList>
#include <QRectF>
#include <KoGenStyle.h>


//#define DEBUG_TABLEHANDLER

using Conversion::twipsToPt;

WordsTableHandler::WordsTableHandler(KoXmlWriter* bodyWriter, KoGenStyles* mainStyles)
{
    // This strange value (-2), is used to create a check that e.g.  a
    // table row is not written before a table:table is started.
    m_row = -2;
    m_column = -2;

    m_bodyWriter = bodyWriter; //for writing text content
    m_mainStyles = mainStyles; //for formatting styles
}

KoXmlWriter * WordsTableHandler::currentWriter() const
{
    return document()->textHandler()->currentWriter();
}

// Called by Document before invoking the table-row-functors
void WordsTableHandler::tableStart(Words::Table* table)
{
    debugMsDoc;

    Q_ASSERT(table);
    Q_ASSERT(!table->name.isEmpty());

    wvWare::SharedPtr<const wvWare::Word97::TAP> tap = table->tap;
    KoXmlWriter* writer = currentWriter();

    m_currentTable = table;
    m_cellOpen = false;
    m_row = -1;
    m_currentY = 0;

#ifdef DEBUG_TABLEHANDLER
    for (unsigned int i = 0; i < (unsigned int)table->m_cellEdges.size(); i++) {
        debugMsDoc << table->m_cellEdges[i];
    }
#endif

    if (m_currentTable->floating) {
        const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
        KoGenStyle style(KoGenStyle::GraphicAutoStyle, "graphic");
        if (document()->writingHeader()) {
            style.setAutoStyleInStylesDotXml(true);
        }

        //style:wrap
        if (tap->textWrap) {
            if (tap->dxaAbs == -8) {
                style.addProperty("style:wrap", "left", gt);
            }
            else if (tap->dxaAbs == 0) {
                style.addProperty("style:wrap", "right", gt);
            } else {
                style.addProperty("style:wrap", "parallel", gt);
            }
            //ODF-1.2: Specifies the number of paragraphs that can wrap around
            //a frame if wrap mode is in {left, right, parallel, dynamic} and
            //anchor type is in {char, paragraph}.
            style.addProperty("style:number-wrapped-paragraphs", "no-limit", gt);
        } else {
            style.addProperty("style:wrap", "none", gt);
        }
        //fo:margin
        style.addPropertyPt("fo:margin-left", twipsToPt(tap->dxaFromText), gt);
        style.addPropertyPt("fo:margin-right", twipsToPt(tap->dxaFromTextRight), gt);
        style.addPropertyPt("fo:margin-top", twipsToPt(tap->dyaFromText), gt);
        style.addPropertyPt("fo:margin-bottom", twipsToPt(tap->dyaFromTextBottom), gt);

        int dxaAbs = 0;
        int dyaAbs = 0;

        //style:horizontal-pos - horizontal position of the anchor
        QString pos = Conversion::getHorizontalPos(tap->dxaAbs);
        style.addProperty("style:horizontal-pos", pos, gt);
        if (pos == "from-left") {
            dxaAbs = tap->dxaAbs;
        }
        //style:vertical-pos - vertical position of the anchor
        pos = Conversion::getVerticalPos(tap->dyaAbs);
        style.addProperty("style:vertical-pos", pos, gt);
        if (pos == "from-top") {
            dyaAbs = tap->dyaAbs;
        }
        //style:vertical-rel - relative vertical position of the anchor
        pos = Conversion::getVerticalRel(tap->pcVert);
	if (!pos.isEmpty()) {
            style.addProperty("style:vertical-rel", pos, gt);
        }
        //style:horizontal-rel - relative horizontal position of the anchor
        pos = Conversion::getHorizontalRel(tap->pcHorz);
        if (!pos.isEmpty()) {
            style.addProperty("style:horizontal-rel", pos, gt);
        }
        //draw:auto-grow-height
        style.addProperty("draw:auto-grow-height", "true", gt);

        const QString drawStyleName = m_mainStyles->insert(style);

        writer->startElement("draw:frame");
        writer->addAttribute("draw:style-name", drawStyleName.toUtf8());
        writer->addAttribute("text:anchor-type", "paragraph");

        int width = table->m_cellEdges[table->m_cellEdges.size() - 1] - table->m_cellEdges[0];
        writer->addAttributePt("svg:width", twipsToPt(width));

        if (style.property("style:horizontal-pos", gt) == "from-left") {
            writer->addAttributePt("svg:x", twipsToPt(dxaAbs + tap->rgdxaCenter[0]));
        }

        writer->addAttributePt("svg:y", twipsToPt(dyaAbs));
        writer->startElement("draw:text-box");
    } //absolutely positioned table

    KoGenStyle tableStyle(KoGenStyle::TableAutoStyle, "table");
    if (document()->writingHeader()) {
        tableStyle.setAutoStyleInStylesDotXml(true);
    }

    //TODO: process the border color information <table:border-color>

    if (tap->fBiDi == 1) {
        tableStyle.addProperty("style:writing-mode", "rl-tb");
    } else {
        tableStyle.addProperty("style:writing-mode", "lr-tb");
    }

    //process horizontal align information
    QString align;
    if (m_currentTable->floating) {
        align = QString("margins");
    } else {
        switch (tap->jc) {
        case hAlignLeft:
            align = QString("left");
            break;
        case hAlignCenter:
            align = QString("center");
            break;
        case hAlignRight:
            align = QString("right");
            break;
        }
    }
    tableStyle.addProperty("table:align", align);

    int width = table->m_cellEdges[table->m_cellEdges.size() - 1] - table->m_cellEdges[0];
    tableStyle.addPropertyPt("style:width", twipsToPt(width));
    tableStyle.addProperty("table:border-model", "collapsing");

    //process the margin information
    if (!m_currentTable->floating) {
        tableStyle.addPropertyPt("fo:margin-left", twipsToPt(tap->rgdxaCenter[0]));
    }

    //check if we need a master page name attribute.
    if (document()->writeMasterPageName() && !document()->writingHeader()) {
        tableStyle.addAttribute("style:master-page-name", document()->masterPageName());
        document()->set_writeMasterPageName(false);
    }

    QString tableStyleName = m_mainStyles->insert(tableStyle, QLatin1String("Table"), KoGenStyles::AllowDuplicates);

    //start table in content
    writer->startElement("table:table");
    writer->addAttribute("table:style-name", tableStyleName);

    // Write the table:table-column descriptions.
    for (int r = 0; r < table->m_cellEdges.size() - 1; r++) {
        KoGenStyle tableColumnStyle(KoGenStyle::TableColumnAutoStyle, "table-column");

        //in case a header or footer is processed, save the style into styles.xml
        if (document()->writingHeader()) {
            tableColumnStyle.setAutoStyleInStylesDotXml(true);
        }

        tableColumnStyle.addPropertyPt("style:column-width",
                                       (table->m_cellEdges[r+1] - table->m_cellEdges[r]) / 20.0);

        QString tableColumnStyleName;
        if (r >= 26) {
            tableColumnStyleName = m_mainStyles->insert(tableColumnStyle, tableStyleName + ".A" + QChar('A' + r - 26), KoGenStyles::DontAddNumberToName);
        } else {
            tableColumnStyleName = m_mainStyles->insert(tableColumnStyle, tableStyleName + '.' + QChar('A' + r), KoGenStyles::DontAddNumberToName);
        }

        writer->startElement("table:table-column");
        writer->addAttribute("table:style-name", tableColumnStyleName);
        writer->endElement();
    }
}

void WordsTableHandler::tableEnd()
{
    debugMsDoc ;

    KoXmlWriter*  writer = currentWriter();
    writer->endElement(); //table:table

    //check if the table is inside of an absolutely positioned frame
    if (m_currentTable->floating) {
        writer->endElement(); //draw:text-box
        writer->endElement(); //draw:frame
    }

    m_currentTable = 0L; // we don't own it, Document does
}

void WordsTableHandler::tableRowStart(wvWare::SharedPtr<const wvWare::Word97::TAP> tap)
{
    debugMsDoc ;
    if (m_row == -2) {
        warnMsDoc << "tableRowStart: tableStart not called previously!";
        return;
    }
    Q_ASSERT(m_currentTable);
    Q_ASSERT(!m_currentTable->name.isEmpty());
    m_row++;
    m_column = -1;
    m_tap = tap;
    KoXmlWriter*  writer = currentWriter();
    //debugMsDoc << "tableRowStart row=" << m_row
    //            << ", number of cells: " << tap->itcMac;

    KoGenStyle rowStyle(KoGenStyle::TableRowAutoStyle, "table-row");

    //in case a header or footer is processed, save the style into styles.xml
    if (document()->writingHeader()) {
        rowStyle.setAutoStyleInStylesDotXml(true);
    }

    // The 6 BRC objects are for top, left, bottom, right,
    // insidehorizontal, insidevertical (default values).
    for (int i = 0; i < 6; i++) {
        const wvWare::Word97::BRC& brc = tap->rgbrcTable[i];
        //debugMsDoc << "default border" << brc.brcType << (brc.dptLineWidth / 8.0);
        m_borderStyle[i] = Conversion::setBorderAttributes(brc);
        m_margin[i] = QString::number(brc.dptSpace) + "pt";
    }
    // We ignore brc.dptSpace (spacing), brc.fShadow (shadow), and brc.fFrame (?)
    QString rowHeightString = QString::number(twipsToPt(qAbs(tap->dyaRowHeight)), 'f').append("pt");

    if (tap->dyaRowHeight > 0) {
        rowStyle.addProperty("style:min-row-height", rowHeightString);
    } else if (tap->dyaRowHeight < 0) {
        rowStyle.addProperty("style:row-height", rowHeightString);
    }

    if (tap->fCantSplit) {
        rowStyle.addProperty("fo:keep-together", "always");
    }

    QString rowStyleName = m_mainStyles->insert(rowStyle, QLatin1String("row"));

    //start table row in content
    writer->startElement("table:table-row");
    writer->addAttribute("table:style-name", rowStyleName.toUtf8());
}

void WordsTableHandler::tableRowEnd()
{
    debugMsDoc;
    m_currentY += rowHeight();
    KoXmlWriter*  writer = currentWriter();
    //end table row in content
    writer->endElement();//table:table-row
}

static const wvWare::Word97::BRC& brcWinner(const wvWare::Word97::BRC& brc1, const wvWare::Word97::BRC& brc2)
{
    if (brc1.brcType == 0 || brc1.brcType >= 64) {
        return brc2;
    }
    else if (brc2.brcType == 0 || brc2.brcType >= 64) {
        return brc1;
    }
    else if (brc1.dptLineWidth >= brc2.dptLineWidth) {
        return brc1;
    } else {
        return brc2;
    }
}

void WordsTableHandler::tableCellStart()
{
    debugMsDoc ;

    if (!m_tap) {
        return;
    }
    KoXmlWriter*  writer = currentWriter();

    //increment the column number so we know where we are
    m_column++;
    //get the number of cells in this row
    int nbCells = m_tap->itcMac;
    //make sure we didn't get more columns than possible number of cells
    Q_ASSERT(m_column < nbCells);
    //if our column number is greater than or equal to number of cells
    if (m_column >= nbCells) {
        return;
    }
    // Get table cell descriptor
    //merging, alignment, ... information
    const wvWare::Word97::TC& tc = m_tap->rgtc[ m_column ];

    //left boundary of current cell
    int leftEdgePos = m_tap->rgdxaCenter[ m_column ]; // in DXAs
    //right boundary of current cell
    int rightEdgePos = m_tap->rgdxaCenter[ m_column+1 ]; // in DXAs

    // Check for merged cells
    // ## We can ignore that one. Our cell-edge magic is much more flexible.
#if 0
    int colSize = 1;
    if (tc.fFirstMerged) {
        // This cell is the first one of a series of merged cells ->
        // we want to find out its size.
        int i = m_column + 1;
        while (i < nbCells && m_tap->rgtc[ i ].fMerged && !m_tap->rgtc[i].fFirstMerged) {
            ++colSize;
            ++i;
        }
    }
#endif
    int rowSpan = 1;
    //if this is the first of some vertically merged cells...
    if (tc.fVertRestart) {
//         debugMsDoc <<"fVertRestart is set!";
        // This cell is the first one of a series of vertically merged cells ->
        // we want to find out its size.
        QList<Words::Row>::ConstIterator it = m_currentTable->rows.constBegin() +  m_row + 1;
        for (; it != m_currentTable->rows.constEnd(); ++it)  {
            // Find cell right below us in row (*it), if any
            Words::TAPptr tapBelow = (*it).tap;
            const wvWare::Word97::TC* tcBelow = 0L;
            for (int c = 0; !tcBelow && c < tapBelow->itcMac ; ++c) {
                if (qAbs(tapBelow->rgdxaCenter[ c ] - leftEdgePos) <= 3
                        && qAbs(tapBelow->rgdxaCenter[ c + 1 ] - rightEdgePos) <= 3) {
                    tcBelow = &tapBelow->rgtc[ c ];
//                     debugMsDoc <<"found cell below, at (Word) column" << c
//                                   <<" fVertMerge:" << tcBelow->fVertMerge;
                }
            }
            if (tcBelow && tcBelow->fVertMerge && !tcBelow->fVertRestart) {
                ++rowSpan;
            } else {
                break;
            }
        }
        //debugMsDoc <<"rowSpan=" << rowSpan;
    }

    // Check how many cells that means, according to our cell edge array.
    int leftCellNumber  = m_currentTable->columnNumber(leftEdgePos);
    int rightCellNumber = m_currentTable->columnNumber(rightEdgePos);

    // In cases where not all columns are present, ensure that the last
    // column spans the remainder of the table.
    // ### It would actually be more closer to the original if we created
    // an empty cell from m_column+1 to the last column. (table-6.doc)
    if (m_column == nbCells - 1)  {
        rightCellNumber = m_currentTable->m_cellEdges.size() - 1;
        rightEdgePos = m_currentTable->m_cellEdges[ rightCellNumber ];
    }

#ifdef DEBUG_TABLEHANDLER
    debugMsDoc << "left edge = " << leftEdgePos << ", right edge = " << rightEdgePos;
    debugMsDoc << "leftCellNumber = " << leftCellNumber << ", rightCellNumber = " << rightCellNumber;
#endif

    //NOTE: The cacheCellEdge f. took care of unsorted tap->rgdxaCenter values.
    //The following assert is not up2date.
//     Q_ASSERT(rightCellNumber >= leftCellNumber);

    // the resulting number of merged cells horizontally
    int colSpan = rightCellNumber - leftCellNumber;

    // Put a filler in for cells that are part of a merged cell.  According to
    // the [MS-DOC] spec. those must be empty (we'll get a warning if not).
    //
    if (tc.fVertMerge && !tc.fVertRestart) {
        m_cellOpen = true;
        writer->startElement("table:covered-table-cell");
        // store colSpan so covered elements can be added on cell close
        m_colSpan = colSpan;
        return;
    }
    // We are now sure we have a real cell (and not a covered one)
    QRectF cellRect(leftEdgePos / 20.0,  // left
                    m_currentY, // top
                    (rightEdgePos - leftEdgePos) / 20.0,   // width
                    rowHeight());  // height
    // I can pass these sizes to ODF now...
#ifdef DEBUG_TABLEHANDLER
    debugMsDoc << " tableCellStart row=" << m_row << ", column=" << m_column <<
                     " colSpan=" << colSpan <<
                     " (from" << leftCellNumber << " to" << rightCellNumber << " for Words)" <<
                     " rowSpan=" << rowSpan <<
                     " cellRect=" << cellRect;
#endif

    // Sort out the borders.
    //
    // From experimenting with Word the following can be said about
    // horizontal borders:
    //
    //  - They always use the collapsing border model (.doc
    //    additionally has the notion of table wide borders)
    //  - The default borders are merged into the odt output by
    //   "winning" over the cell borders due to wider lines
    //  - Word also defines table-wide borders (even between cell
    //    minimum values)
    //  - If the default is thicker or same width then it wins. At the
    //    top or bottom of table the cell always wins
    //
    // The following can be said about vertical borders:
    //  - The cell to the left of the border always defines the value.
    //  - Well then a winner with the table wide definitions is also found.
    //
#ifdef DEBUG_TABLEHANDLER
    debugMsDoc << "CellBorders=" << m_row << m_column
                  << "top" << tc.brcTop.brcType << tc.brcTop.dptLineWidth
                  << "left" << tc.brcLeft.brcType << tc.brcLeft.dptLineWidth
                  << "bottom" << tc.brcBottom.brcType << tc.brcBottom.dptLineWidth
                  << "right" << tc.brcRight.brcType << tc.brcRight.dptLineWidth;
#endif

    const wvWare::Word97::BRC brcNone;
    const wvWare::Word97::BRC& brcTop = (m_row > 0) ?
                                        brcWinner(tc.brcTop, m_tap->rgbrcTable[4]) :
                                        ((tc.brcTop.brcType > 0 && tc.brcTop.brcType < 64) ? tc.brcTop :
                                        m_tap->rgbrcTable[0]);
    const wvWare::Word97::BRC& brcBottom = (m_row < m_currentTable->rows.size() - 1) ?
                                           brcWinner(tc.brcBottom, m_tap->rgbrcTable[4]) :
                                           brcWinner(tc.brcBottom, m_tap->rgbrcTable[2]);
    const wvWare::Word97::BRC& brcLeft = (m_column > 0) ?
                                         brcWinner(tc.brcLeft, m_tap->rgbrcTable[5]) :
                                         brcWinner(tc.brcLeft, m_tap->rgbrcTable[1]);
    const wvWare::Word97::BRC& brcRight = (m_column < nbCells - 1) ?
                                          brcWinner(tc.brcRight, m_tap->rgbrcTable[5]) :
                                          brcWinner(tc.brcRight, m_tap->rgbrcTable[3]);

    const wvWare::Word97::BRC& brcTL2BR = tc.brcTL2BR;
    const wvWare::Word97::BRC& brcTR2BL = tc.brcTR2BL;

    KoGenStyle cellStyle(KoGenStyle::TableCellAutoStyle, "table-cell");

    //in case a header or footer is processed, save the style into styles.xml
    if (document()->writingHeader()) {
        cellStyle.setAutoStyleInStylesDotXml(true);
    }

    //set borders for the four edges of the cell
    if (brcTop.brcType > 0 && brcTop.brcType < 64) {
        cellStyle.addProperty("fo:border-top", Conversion::setBorderAttributes(brcTop));
        QString kba = Conversion::borderCalligraAttributes(brcTop);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("calligra:specialborder-top",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcTop);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:border-line-width-top", dba);
        }
    }

    //left
    if (brcLeft.brcType > 0 && brcLeft.brcType < 64) {
        cellStyle.addProperty("fo:border-left", Conversion::setBorderAttributes(brcLeft));
        QString kba = Conversion::borderCalligraAttributes(brcLeft);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("calligra:specialborder-left",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcLeft);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:border-line-width-left", dba);
        }
    }

    //bottom
    if (brcBottom.brcType != 0 && brcBottom.brcType < 64) {
        cellStyle.addProperty("fo:border-bottom", Conversion::setBorderAttributes(brcBottom));
        QString kba = Conversion::borderCalligraAttributes(brcBottom);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("calligra:specialborder-bottom",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcBottom);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:border-line-width-bottom", dba);
        }
    }

    //right
    if (brcRight.brcType > 0 && brcRight.brcType < 64) {
        cellStyle.addProperty("fo:border-right", Conversion::setBorderAttributes(brcRight));
        QString kba = Conversion::borderCalligraAttributes(brcRight);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("calligra:specialborder-right",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcRight);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:border-line-width-right", dba);
        }
    }

    //top left to bottom right
    if (brcTL2BR.brcType > 0 && brcTL2BR.brcType < 64) {
        cellStyle.addProperty("style:diagonal-tl-br", Conversion::setBorderAttributes(brcTL2BR));
        QString kba = Conversion::borderCalligraAttributes(brcTL2BR);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("calligra:specialborder-tl-br",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcTL2BR);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:diagonal-tl-br-widths", dba);
        }
    }

    //top right to bottom left
    if (brcTR2BL.brcType > 0 && brcTR2BL.brcType < 64) {
        cellStyle.addProperty("style:diagonal-bl-tr", Conversion::setBorderAttributes(brcTR2BL));
        QString kba = Conversion::borderCalligraAttributes(brcTR2BL);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("calligra:specialborder-tr-bl",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcTR2BL);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:diagonal-bl-tr-widths", dba);
        }
    }

    //text direction
    //if(tc.fVertical) {
    //    cellStyle.addProperty("style:direction", "ttb");
    //}

    //process vertical alignment information
    QString align;
    switch (tc.vertAlign) {
    case vAlignTop:
        align = QString("top");
        break;
    case vAlignMiddle:
        align = QString("middle");
        break;
    case vAlignBottom:
        align = QString("bottom");
        break;
    }
    cellStyle.addProperty("style:vertical-align", align);

    //process cell padding information
    qreal padVert = twipsToPt(m_tap->padVert);
    qreal padHorz = twipsToPt(m_tap->padHorz);
    cellStyle.addPropertyPt("fo:padding-top", padVert);
    cellStyle.addPropertyPt("fo:padding-bottom", padVert);
    cellStyle.addPropertyPt("fo:padding-left", padHorz);
    cellStyle.addPropertyPt("fo:padding-right", padHorz);

    QString cellStyleName = m_mainStyles->insert(cellStyle, "cell");

//     emit sigTableCellStart( m_row, leftCellNumber, rowSpan, colSpan, cellRect, m_currentTable->name,
//                             brcTop, brcBottom, brcLeft, brcRight, m_tap->rgshd[ m_column ] );

    // Start a table cell in the content.
    writer->startElement("table:table-cell");
    m_cellOpen = true;
    writer->addAttribute("table:style-name", cellStyleName.toUtf8());

    if (rowSpan > 1) {
        writer->addAttribute("table:number-rows-spanned", rowSpan);
    }
    if (colSpan > 1) {
        writer->addAttribute("table:number-columns-spanned", colSpan);
        m_colSpan = colSpan;
    } else {
        // If not set to colSpan, we need to (re)set it to a known value.
        m_colSpan = 1;
    }
    m_cellStyleName = cellStyleName;
}

void WordsTableHandler::tableCellEnd()
{
    debugMsDoc;

    if (!m_cellOpen) {
        debugMsDoc << "BUG: !m_cellOpen";
        return;
    }

    // Text lists aren't closed explicitly so we have to close them
    // when something happens like a new paragraph or, in this case,
    // the table cell ends.
    if (document()->textHandler()->listIsOpen()) {
        document()->textHandler()->closeList();
    }
    KoXmlWriter*  writer = currentWriter();


    QList<const char*> openTags = writer->tagHierarchy();
    for (int i = 0; i < openTags.size(); ++i) {
        debugMsDoc << openTags[i];
    }
    writer->endElement();//table:table-cell
    m_cellOpen = false;

    // If this cell covers other cells (i.e. is merged), then create as many
    // table:covered-table-cell tags as there are covered columns.
    for (int i = 1; i < m_colSpan; i++) {
        writer->startElement("table:covered-table-cell");
        writer->endElement();
    }
    m_colSpan = 1;

    //Leaving out the table:style-name attribute and creation of the
    //corresponding style for covered table cells in the tableCellStart f.
    if (!m_tap || m_cellStyleName.isEmpty()) {
        return;
    }

    //process shading information
    const wvWare::Word97::SHD& shd = m_tap->rgshd[ m_column ];
    QString color = Conversion::shdToColorStr(shd,
                                              document()->textHandler()->paragraphBgColor(),
                                              document()->textHandler()->paragraphBaseFontColorBkp());

    if (!color.isNull()) {
        KoGenStyle* cellStyle = m_mainStyles->styleForModification(m_cellStyleName, "table-cell");
        Q_ASSERT(cellStyle);
        if (cellStyle) {
            cellStyle->addProperty("fo:background-color", color, KoGenStyle::TableCellType);
        }
        m_cellStyleName.clear();

        //add the current background-color to stack
//         document()->pushBgColor(color);
    }
}

Words::Table::Table()
: floating(false)
{
}

void Words::Table::cacheCellEdge(int cellEdge)
{
    debugMsDoc ;
    uint size = m_cellEdges.size();
    // Do we already know about this edge?
    for (unsigned int i = 0; i < size; i++) {
        if (m_cellEdges[i] == cellEdge)  {
            debugMsDoc << cellEdge << " -> found";
            return;
        }
        //insert it in the right place if necessary
        if (m_cellEdges[i] > cellEdge) {
            m_cellEdges.insert(i, cellEdge);
            debugMsDoc << cellEdge << " -> added. Size=" << size + 1;
            return;
        }
    }
    //add it at the end if this edge is larger than all the rest
    m_cellEdges.append(cellEdge);
    debugMsDoc << cellEdge << " -> added. Size=" << size + 1;
}

int Words::Table::columnNumber(int cellEdge) const
{
    debugMsDoc ;
    for (unsigned int i = 0; i < (unsigned int)m_cellEdges.size(); i++) {
        if (m_cellEdges[i] == cellEdge) {
            return i;
        }
    }
    // This can't happen, if cacheCellEdge has been properly called
    warnMsDoc << "Column not found for cellEdge x=" << cellEdge << " - BUG.";
    return 0;
}

double WordsTableHandler::rowHeight() const
{
    debugMsDoc ;
    return qMax(m_tap->dyaRowHeight / 20.0, 20.0);
}
