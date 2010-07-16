/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>

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
#include "conversion.h"

#include <wv2/src/word97_generated.h>
#include <wv2/src/ms_odraw.h>

#include <kdebug.h>
#include <QList>
#include <QRectF>
#include <KoGenStyle.h>

#include "document.h"
#include "texthandler.h"

using Conversion::twipsToPt;

KWordTableHandler::KWordTableHandler(KoXmlWriter* bodyWriter, KoGenStyles* mainStyles) :
m_floatingTable(false)
{
    // This strange value (-2), is used to create a check that e.g.  a
    // table row is not written before a table:table is started.
    m_row = -2;
    m_column = -2;

    m_bodyWriter = bodyWriter; //for writing text content
    m_mainStyles = mainStyles; //for formatting styles
}

// Called by Document before invoking the table-row-functors
void KWordTableHandler::tableStart(KWord::Table* table)
{
    kDebug(30513);

    Q_ASSERT(table);
    Q_ASSERT(!table->name.isEmpty());

    KoXmlWriter*  writer = currentWriter();
    wvWare::SharedPtr<const wvWare::Word97::TAP> tap = table->tap;

    m_currentTable = table;
    m_cellOpen = false;

#if 0
    for (unsigned int i = 0; i < (unsigned int)table->m_cellEdges.size(); i++)
        kDebug(30513) << table->m_cellEdges[i];
#endif

    m_row = -1;
    m_currentY = 0;

    //check if the table is inside of an absolutely positioned frame
    if ( (tap->dxaAbs != 0 || tap->dyaAbs) )
    {
        KoGenStyle userStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        QString drawStyleName;
        int dxaAbs = 0;

        writer->startElement("text:p", false);
        writer->addAttribute("text:style-name", "Standard");

        //process wrapping information
        if (tap->textWrap) {
            //right aligned
            if (tap->dxaAbs == -8) {
                userStyle.addProperty("style:wrap", "left");
            }
            //left aligned
            else if (tap->dxaAbs == 0) {
                userStyle.addProperty("style:wrap", "right");
            } else {
                userStyle.addProperty("style:wrap", "parallel");
            }
           //ODF-1.2: specifies the number of paragraphs that can wrap around a
           //frame if wrap mode is in {left, right, parallel, dynamic} and
           //anchor type is in {char, paragraph}
            userStyle.addProperty("style:number-wrapped-paragraphs", "no-limit");
        } else {
            userStyle.addProperty("style:wrap", "none");
        }

        //margin information is related to wrapping of text around the table
        userStyle.addPropertyPt("fo:margin-left", twipsToPt(tap->dxaFromText));
        userStyle.addPropertyPt("fo:margin-right", twipsToPt(tap->dxaFromTextRight));
        userStyle.addPropertyPt("fo:margin-top", twipsToPt(tap->dyaFromText));
        userStyle.addPropertyPt("fo:margin-bottom", twipsToPt(tap->dyaFromTextBottom));

        //MS-DOC - sprmPDxaAbs - relative horizontal position to anchor
        // (-4) - center, (-8) - right, (-12) - inside, (-16) - outside
        if (tap->dxaAbs == -4) {
            userStyle.addProperty("style:horizontal-pos","center");
        }
        else if (tap->dxaAbs == -8)  {
            userStyle.addProperty("style:horizontal-pos","right");
            userStyle.addPropertyPt("fo:margin-right", 0);
        }
        else if (tap->dxaAbs == -12) {
            userStyle.addProperty("style:horizontal-pos","inside");
        }
        else if (tap->dxaAbs == -16) {
            userStyle.addProperty("style:horizontal-pos","outside");
        }
        else {
            dxaAbs = tap->dxaAbs;
            userStyle.addProperty("style:horizontal-pos","from-left");
            userStyle.addPropertyPt("fo:margin-left", 0);
        }

        int dyaAbs = 0;
        //MS-DOC - sprmPDyaAbs - relative vertical position to anchor
        // (-4) - top, (-8) - middle, (-12) - bottom, (-16) - inside,
        // (-20) - outside
        if (tap->dyaAbs == -4) {
            userStyle.addProperty("style:vertical-pos","top");
        }
        else if (tap->dyaAbs == -8) {
            userStyle.addProperty("style:vertical-pos","middle");
        }
        else if (tap->dyaAbs == -12) {
            userStyle.addProperty("style:vertical-pos","bottom");
        }
        else if (tap->dyaAbs == -16) {
            userStyle.addProperty("style:vertical-pos","inline");
        }
        else if (tap->dyaAbs == -20) {
            userStyle.addProperty("style:vertical-pos","inline");
        }
        else {
            dyaAbs = tap->dyaAbs;
            userStyle.addProperty("style:vertical-pos","from-top");
        }
        //MS-DOC - PositionCodeOperand - anchor vertical position
        // 0 - margin, 1 - page, 2 - paragraph
        if (tap->pcVert == 0) {
            userStyle.addProperty("style:vertical-rel","page-content");
        }
        else if (tap->pcVert == 1) {
            userStyle.addProperty("style:vertical-rel","page");
        }
        else if (tap->pcVert == 2) {
            userStyle.addProperty("style:vertical-rel","paragraph");
        }
        //MS-DOC - PositionCodeOperand - anchor horizontal position
        // 0 - current column, 1 - margin, 2 - page
        if (tap->pcHorz == 0) {
            userStyle.addProperty("style:horizontal-rel","paragraph");
        }
        else if (tap->pcHorz == 1) {
            userStyle.addProperty("style:horizontal-rel","page-content");
        }
        else if (tap->pcHorz == 2) {
            userStyle.addProperty("style:horizontal-rel","page");
        }

        drawStyleName = "fr";
        drawStyleName = m_mainStyles->insert(userStyle, drawStyleName);
        writer->startElement("draw:frame");
        writer->addAttribute("draw:style-name", drawStyleName.toUtf8());
        writer->addAttribute("text:anchor-type", "paragraph");

        writer->addAttributePt("svg:width", 
                               (double)(table->m_cellEdges[table->m_cellEdges.size() - 1] - 
                               table->m_cellEdges[0]) / 20.0);

        writer->addAttributePt("svg:x", (double)(dxaAbs + tap->rgdxaCenter[0])/20);
        writer->addAttributePt("svg:y", (double)dyaAbs/20);
        writer->startElement("draw:text-box");

        m_floatingTable = true;
    }

    //table style
    KoGenStyle tableStyle(KoGenStyle::TableAutoStyle, "table");

    //in case a header or footer is processed, save the style into styles.xml
    if (document()->writingHeader()) {
        tableStyle.setAutoStyleInStylesDotXml(true);
    }

    if (tap->fBiDi == 1) {
        tableStyle.addProperty("style:writing-mode", "rl-tb");
    } else {
        tableStyle.addProperty("style:writing-mode", "lr-tb");
    }

    //process horizontal align information
    QString align;
    if (m_floatingTable != true) {
        switch (tap->jc) {
        case wvWare::hAlignLeft:
            align = QString("left");
            break;
        case wvWare::hAlignCenter:
            align = QString("center");
            break;
        case wvWare::hAlignRight:
            align = QString("right");
            break;
        }
    } else {
        align = QString("margins");
    } 
    tableStyle.addProperty("table:align", align);
    tableStyle.addPropertyPt("style:width", (table->m_cellEdges[table->m_cellEdges.size()-1] - table->m_cellEdges[0]) / 20.0);
    tableStyle.addProperty("style:border-model", "collapsing");

    //process the margin information 
    if (m_floatingTable != true) {
        int margin = tap->rgdxaCenter[0];
        tableStyle.addPropertyPt("fo:margin-left", twipsToPt(margin));
    }

    //check if we need a master page name attribute.
    if (document()->writeMasterPageName() && !document()->writingHeader()) {
        tableStyle.addAttribute("style:master-page-name", document()->masterPageName());
        document()->set_writeMasterPageName(false);
    }

    //check if the break-before property is required
    if (document()->textHandler()->breakBeforePage() && !document()->writingHeader())
    {
        tableStyle.addProperty("fo:break-before", "page", KoGenStyle::TableType);
        document()->textHandler()->set_breakBeforePage(false);
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

void KWordTableHandler::tableEnd()
{
    kDebug(30513) ;
    m_currentTable = 0L; // we don't own it, Document does
    KoXmlWriter*  writer = currentWriter();

    writer->endElement();//table:table

    //check if the table is inside of an absolutely positioned frame
    if ( m_floatingTable == true )
    {
        writer->endElement(); //draw:text-box
        writer->endElement(); //draw:frame
        writer->endElement(); //close the <text:p>

        m_floatingTable = false;
    }
}

void KWordTableHandler::tableRowStart(wvWare::SharedPtr<const wvWare::Word97::TAP> tap)
{
    kDebug(30513) ;
    if (m_row == -2) {
        kWarning(30513) << "tableRowStart: tableStart not called previously!";
        return;
    }
    Q_ASSERT(m_currentTable);
    Q_ASSERT(!m_currentTable->name.isEmpty());
    m_row++;
    m_column = -1;
    m_tap = tap;
    KoXmlWriter*  writer = currentWriter();
    //kDebug(30513) << "tableRowStart row=" << m_row
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
        //kDebug(30513) << "default border" << brc.brcType << (brc.dptLineWidth / 8.0);
        m_borderStyle[i] = Conversion::setBorderAttributes(brc);
        m_margin[i] = QString::number(brc.dptSpace) + "pt";
    }
    // We ignore brc.dptSpace (spacing), brc.fShadow (shadow), and brc.fFrame (?)

    qreal rowHeightPt = Conversion::twipsToPt(qAbs(tap->dyaRowHeight));         // convert twips to Pts
    QString rowHeightString = QString::number(rowHeightPt).append("pt");        // make height string from number

    if (tap->dyaRowHeight > 0) {
        rowStyle.addProperty("style:min-row-height", rowHeightString);
    } else if (tap->dyaRowHeight < 0) {
        rowStyle.addProperty("style:row-height", rowHeightString);
    }

    if (tap->fCantSplit) {
        rowStyle.addProperty("style:keep-together", "always");
    }

    QString rowStyleName = m_mainStyles->insert(rowStyle, QLatin1String("row"));

    //start table row in content
    writer->startElement("table:table-row");
    writer->addAttribute("table:style-name", rowStyleName.toUtf8());
}

void KWordTableHandler::tableRowEnd()
{
    kDebug(30513);
    m_currentY += rowHeight();
    KoXmlWriter*  writer = currentWriter();
    //end table row in content
    writer->endElement();//table:table-row
}

KoXmlWriter * KWordTableHandler::currentWriter()
{
    if (document()->writingHeader() && document()->headerWriter() != NULL)
        return document()->headerWriter();
    else
        return m_bodyWriter;
}

static const wvWare::Word97::BRC& brcWinner(const wvWare::Word97::BRC& brc1, const wvWare::Word97::BRC& brc2)
{
    if (brc1.brcType == 0 || brc1.brcType >= 64)
        return brc2;
    else if (brc2.brcType == 0 || brc2.brcType >= 64)
        return brc1;
    else if (brc1.dptLineWidth >= brc2.dptLineWidth)
        return brc1;
    else
        return brc2;
}

void KWordTableHandler::tableCellStart()
{
    kDebug(30513) ;
    Q_ASSERT(m_tap);
    if (!m_tap)
        return;
    KoXmlWriter*  writer = currentWriter();

    //increment the column number so we know where we are
    m_column++;
    //get the number of cells in this row
    int nbCells = m_tap->itcMac;
    //make sure we haven't gotten more columns than possible
    //with the number of cells
    Q_ASSERT(m_column < nbCells);
    //if our column number is greater than or equal to number
    //of cells, just return
    if (m_column >= nbCells)
        return;

    // Get table cell descriptor
    //merging, alignment, ... information
    const wvWare::Word97::TC& tc = m_tap->rgtc[ m_column ];
    const wvWare::Word97::SHD& shd = m_tap->rgshd[ m_column ];

    //left boundary of current cell
    int left = m_tap->rgdxaCenter[ m_column ]; // in DXAs
    //right boundary of current cell
    int right = m_tap->rgdxaCenter[ m_column+1 ]; // in DXAs

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
        //kDebug(30513) <<"fVertRestart is set!";
        // This cell is the first one of a series of vertically merged cells ->
        // we want to find out its size.
        QList<KWord::Row>::Iterator it = m_currentTable->rows.begin() +  m_row + 1;
        for (; it != m_currentTable->rows.end(); ++it)  {
            // Find cell right below us in row (*it), if any
            KWord::TAPptr tapBelow = (*it).tap;
            const wvWare::Word97::TC* tcBelow = 0L;
            for (int c = 0; !tcBelow && c < tapBelow->itcMac ; ++c) {
                if (qAbs(tapBelow->rgdxaCenter[ c ] - left) <= 3
                        && qAbs(tapBelow->rgdxaCenter[ c + 1 ] - right) <= 3) {
                    tcBelow = &tapBelow->rgtc[ c ];
                    //kDebug(30513) <<"found cell below, at (Word) column" << c <<" fVertMerge:" << tcBelow->fVertMerge;
                }
            }
            if (tcBelow && tcBelow->fVertMerge && !tcBelow->fVertRestart)
                ++rowSpan;
            else
                break;
        }
        //kDebug(30513) <<"rowSpan=" << rowSpan;
    }

    // Check how many cells that means, according to our cell edge array.
    int leftCellNumber  = m_currentTable->columnNumber(left);
    int rightCellNumber = m_currentTable->columnNumber(right);

    // In cases where not all columns are present, ensure that the last
    // column spans the remainder of the table.
    // ### It would actually be more closer to the original if we created
    // an empty cell from m_column+1 to the last column. (table-6.doc)
    if (m_column == nbCells - 1)  {
        rightCellNumber = m_currentTable->m_cellEdges.size() - 1;
        right = m_currentTable->m_cellEdges[ rightCellNumber ];
    }

#if 0
    kDebug(30513) << "left edge = " << left << ", right edge = " << right;

    kDebug(30513) << "leftCellNumber = " << leftCellNumber
    << ", rightCellNumber = " << rightCellNumber;
#endif
    Q_ASSERT(rightCellNumber >= leftCellNumber);   // you'd better be...
    int colSpan = rightCellNumber - leftCellNumber; // the resulting number of merged cells horizontally

    // Put a filler in for cells that are part of a merged cell.
    //
    // The MSWord spec says they must be empty anyway (and we'll get a
    // warning if not).
    if (tc.fVertMerge && !tc.fVertRestart) {
        m_cellOpen = true;
        writer->startElement("table:covered-table-cell");
        
        m_colSpan = colSpan; // store colSpan so covered elements can be added on cell close
        return;
    }
    // We are now sure we have a real cell (and not a covered one)

    QRectF cellRect(left / 20.0,  // left
                    m_currentY, // top
                    (right - left) / 20.0,   // width
                    rowHeight());  // height
    // I can pass these sizes to ODF now...
#if 0
    kDebug(30513) << " tableCellStart row=" << m_row << " WordColumn="
    << m_column << " colSpan="
    << colSpan << " (from" << leftCellNumber
    << " to" << rightCellNumber << " for KWord) rowSpan="
    << rowSpan << " cellRect=" << cellRect;
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
#if 0
    kDebug(30513) << "CellBorders=" << m_row << m_column
    << "top" << tc.brcTop.brcType << tc.brcTop.dptLineWidth
    << "left" << tc.brcLeft.brcType << tc.brcLeft.dptLineWidth
    << "bottom" << tc.brcBottom.brcType << tc.brcBottom.dptLineWidth
    << "right" << tc.brcRight.brcType << tc.brcRight.dptLineWidth;
#endif

    const wvWare::Word97::BRC brcNone;
    const wvWare::Word97::BRC& brcTop = (m_row > 0) ?
                                        brcWinner(tc.brcTop, m_tap->rgbrcTable[4]) :
                                        ((tc.brcTop.brcType > 0 && tc.brcTop.brcType < 64) ? tc.brcTop : m_tap->rgbrcTable[0]);
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
        QString kba = Conversion::borderKOfficeAttributes(brcTop);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("koffice:specialborder-top",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcTop);
        if (!dba.isEmpty())
            cellStyle.addProperty("style:border-line-width-top", dba);
    }

    //left
    if (brcLeft.brcType > 0 && brcLeft.brcType < 64) {
        cellStyle.addProperty("fo:border-left", Conversion::setBorderAttributes(brcLeft));
        QString kba = Conversion::borderKOfficeAttributes(brcLeft);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("koffice:specialborder-left",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcLeft);
        if (!dba.isEmpty())
            cellStyle.addProperty("style:border-line-width-left", dba);
    }

    //bottom
    if (brcBottom.brcType != 0 && brcBottom.brcType < 64) {
        cellStyle.addProperty("fo:border-bottom", Conversion::setBorderAttributes(brcBottom));
        QString kba = Conversion::borderKOfficeAttributes(brcBottom);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("koffice:specialborder-bottom",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcBottom);
        if (!dba.isEmpty())
            cellStyle.addProperty("style:border-line-width-bottom", dba);
    }

    //right
    if (brcRight.brcType > 0 && brcRight.brcType < 64) {
        cellStyle.addProperty("fo:border-right", Conversion::setBorderAttributes(brcRight));
        QString kba = Conversion::borderKOfficeAttributes(brcRight);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("koffice:specialborder-right",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcRight);
        if (!dba.isEmpty())
            cellStyle.addProperty("style:border-line-width-right", dba);
    }

    //top left to bottom right
    if (brcTL2BR.brcType > 0 && brcTL2BR.brcType < 64) {
        cellStyle.addProperty("style:diagonal-tl-br", Conversion::setBorderAttributes(brcTL2BR));
        QString kba = Conversion::borderKOfficeAttributes(brcTL2BR);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("koffice:specialborder-tl-br",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcTL2BR);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:diagonal-tl-br-widths", dba);
        }
    }

    //top right to bottom left
    if (brcTR2BL.brcType > 0 && brcTR2BL.brcType < 64) {
        cellStyle.addProperty("style:diagonal-bl-tr", Conversion::setBorderAttributes(brcTR2BL));
        QString kba = Conversion::borderKOfficeAttributes(brcTR2BL);
        if (!kba.isEmpty()) {
            cellStyle.addProperty("koffice:specialborder-tr-bl",kba);
        }
        QString dba = Conversion::setDoubleBorderAttributes(brcTR2BL);
        if (!dba.isEmpty()) {
            cellStyle.addProperty("style:diagonal-bl-tr-widths", dba);
        }
    }

    //check if we have to ignore the shading information   
    if (!shd.shdAutoOrNill) {
        QString color = QString('#');
        //ipatPct5 to ipatPct90
        if (shd.ipat >= 0x02 && shd.ipat <= 0x0d) {
            //get the color from the shading pattern
            uint grayColor = Conversion::shadingPatternToColor(shd.ipat);
            color.append(QString::number(grayColor | 0xff000000, 16).right(6).toUpper());
            cellStyle.addProperty("fo:background-color", color);
        }
        //ipatSolid
        else if (shd.ipat == 0x01) {
            color.append(QString::number(shd.cvBack | 0xff000000, 16).right(6).toUpper());
            cellStyle.addProperty("fo:background-color", color);
        }
        else {
            kDebug(30513) << "Warning: Unsupported shading pattern";
        }
    }

    //text direction
    //if(tc.fVertical) {
    //    cellStyle.addProperty("style:direction", "ttb");
    //}

    //process vertical alignment information 
    QString align;
    switch (tc.vertAlign) {
    case wvWare::vAlignTop:
        align = QString("top");
        break;
    case wvWare::vAlignMiddle:
        align = QString("middle");
        break;
    case wvWare::vAlignBottom:
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

    QString cellStyleName = m_mainStyles->insert(cellStyle, QLatin1String("cell"));

    //emit sigTableCellStart( m_row, leftCellNumber, rowSpan, colSpan, cellRect, m_currentTable->name, brcTop, brcBottom, brcLeft, brcRight, m_tap->rgshd[ m_column ] );

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
        // If we don't set it to colSpan, we still need to (re)set it
        // to a known value.
        m_colSpan = 1;
    }
}

void KWordTableHandler::tableCellEnd()
{
    kDebug(30513);

    // Text lists aren't closed explicitly so we have to close them
    // when something happens like a new paragraph or, in this case,
    // the table cell ends.
    if (document()->textHandler()->listIsOpen())
        document()->textHandler()->closeList();
    KoXmlWriter*  writer = currentWriter();

    // End table cell in content, but only if we actually opened a cell.
    if (m_cellOpen) {
        QList<const char*> openTags = writer->tagHierarchy();
        for (int i = 0; i < openTags.size(); ++i)
            kDebug(30513) << openTags[i];

        writer->endElement();//table:table-cell
        m_cellOpen = false;
    } else
        kDebug(30513) << "Didn't close the cell because !m_cellOpen!!";

    // If this cell covers other cells (i.e. is merged), then create
    // as many table:covered-table-cell tags as there are covered
    // columns.
    for (int i = 1; i < m_colSpan; i++) {
        writer->startElement("table:covered-table-cell");
        writer->endElement();
    }
    m_colSpan = 1;
}


// Add cell edge into the cache of cell edges for a given table.
// Might as well keep it sorted here
void KWord::Table::cacheCellEdge(int cellEdge)
{
    kDebug(30513) ;
    uint size = m_cellEdges.size();
    // Do we already know about this edge?
    for (unsigned int i = 0; i < size; i++) {
        if (m_cellEdges[i] == cellEdge)  {
            kDebug(30513) << cellEdge << " -> found";
            return;
        }
        //insert it in the right place if necessary
        if (m_cellEdges[i] > cellEdge) {
            m_cellEdges.insert(i, cellEdge);
            kDebug(30513) << cellEdge << " -> added. Size=" << size + 1;
            return;
        }
    }
    //add it at the end if this edge is larger than all the rest
    m_cellEdges.append(cellEdge);
    kDebug(30513) << cellEdge << " -> added. Size=" << size + 1;
}

// Lookup a cell edge from the cache of cell edges
// And return the column number
int KWord::Table::columnNumber(int cellEdge) const
{
    kDebug(30513) ;
    for (unsigned int i = 0; i < (unsigned int)m_cellEdges.size(); i++) {
        if (m_cellEdges[i] == cellEdge)
            return i;
    }
    // This can't happen, if cacheCellEdge has been properly called
    kWarning(30513) << "Column not found for cellEdge x=" << cellEdge << " - BUG.";
    return 0;
}

double KWordTableHandler::rowHeight() const
{
    kDebug(30513) ;
    return qMax(m_tap->dyaRowHeight / 20.0, 20.0);
}

#include "tablehandler.moc"
