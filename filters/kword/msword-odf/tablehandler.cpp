/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>

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

#include <wv2/word97_generated.h>

#include <kdebug.h>
#include <q3tl.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QRectF>

#include <KoGenStyle.h>

KWordTableHandler::KWordTableHandler(KoXmlWriter* bodyWriter, KoGenStyles* mainStyles)
{
    m_row = -2;
    m_column = -2;

    m_bodyWriter = bodyWriter; //for writing text content
    m_mainStyles = mainStyles; //for formatting styles
}

// Called by Document before invoking the table-row-functors
void KWordTableHandler::tableStart(KWord::Table* table)
{
    kDebug(30513);
    Q_ASSERT( table );
    Q_ASSERT( !table->name.isEmpty() );
    m_currentTable = table;
    m_cellOpen = false;

#if 1
    for (unsigned int i = 0; i < (unsigned int)table->m_cellEdges.size(); i++)
        kDebug(30513) << table->m_cellEdges[i];
#endif

    m_row = -1;
    m_currentY = 0;

    //start table in content
    m_bodyWriter->startElement("table:table");
    m_bodyWriter->startElement("table:table-column");
    kDebug(30513) << "max columns in this table: " << table->m_cellEdges.size()-1;
    m_bodyWriter->addAttribute("table:number-columns-repeated", table->m_cellEdges.size()-1);
    m_bodyWriter->endElement();
}

void KWordTableHandler::tableEnd()
{
    kDebug(30513) ;
    m_currentTable = 0L; // we don't own it, Document does
    //end table in content
    m_bodyWriter->endElement();//table:table
}

void KWordTableHandler::tableRowStart( wvWare::SharedPtr<const wvWare::Word97::TAP> tap )
{
    kDebug(30513) ;
    if ( m_row == -2 )
    {
        kWarning(30513) << "tableRowStart: tableStart not called previously!";
        return;
    }
    Q_ASSERT( m_currentTable );
    Q_ASSERT( !m_currentTable->name.isEmpty() );
    m_row++;
    m_column = -1;
    m_tap = tap;
    kDebug(30513) << "tableRowStart row=" << m_row << ", number of cells: " << tap->itcMac;

    KoGenStyle rowStyle(KoGenStyle::StyleAutoTableRow, "table-row");
    QString rowStyleName = m_mainStyles->lookup(rowStyle, QString("row"));

    //TODO figure out what the six different BRC objects are for,
    // instead of just using one of them to set the whole border
    //for(int i = 0; i < 6; i++) {
    //    const wvWare::Word97::BRC& brc = tap->rgbrcTable[i];
    //    kDebug(30513) << brc.toString().c_str();
    //}
    //get row border style, used in tableCellStart()
    const wvWare::Word97::BRC& brc = tap->rgbrcTable[0];
    m_borderStyle = Conversion::setBorderAttributes(brc);

    //start table row in content
    m_bodyWriter->startElement("table:table-row");
    m_bodyWriter->addAttribute("table:style-name", rowStyleName.toUtf8());
}

void KWordTableHandler::tableRowEnd()
{
    kDebug(30513);
    m_currentY += rowHeight();
    //end table row in content
    m_bodyWriter->endElement();//table:table-row
}

void KWordTableHandler::tableCellStart()
{
    kDebug(30513) ;
    Q_ASSERT( m_tap );
    if ( !m_tap )
        return;
    //increment the column number so we know where we are
    m_column++;
    //get the number of cells in this row
    int nbCells = m_tap->itcMac;
    //make sure we haven't gotten more columns than possible
    //with the number of cells
    Q_ASSERT( m_column < nbCells );
    //if our column number is greater than or equal to number
    //of cells, just return
    if ( m_column >= nbCells )
        return;

    // Get table cell descriptor
    //merging, alignment, ... information
    const wvWare::Word97::TC& tc = m_tap->rgtc[ m_column ];

    //left boundary of current cell
    int left = m_tap->rgdxaCenter[ m_column ]; // in DXAs
    //right boundary of current cell
    int right = m_tap->rgdxaCenter[ m_column+1 ]; // in DXAs

    // Check for merged cells
    // ## We can ignore that one. Our cell-edge magic is much more flexible.
#if 0
    int colSize = 1;
    if ( tc.fFirstMerged )
    {
        // This cell is the first one of a series of merged cells ->
        // we want to find out its size.
        int i = m_column + 1;
        while ( i < nbCells && m_tap->rgtc[ i ].fMerged && !m_tap->rgtc[i].fFirstMerged ) {
            ++colSize;
            ++i;
        }
    }
#endif
    int rowSpan = 1;
    //if this is the first of some vertically merged cells...
    if ( tc.fVertRestart )
    {
        //kDebug(30513) <<"fVertRestart is set!";
        // This cell is the first one of a series of vertically merged cells ->
        // we want to find out its size.
        Q3ValueList<KWord::Row>::Iterator it = m_currentTable->rows.at( m_row + 1 );
        for( ; it != m_currentTable->rows.end(); ++it )  {
            // Find cell right below us in row (*it), if any
            KWord::TAPptr tapBelow = (*it).tap;
            const wvWare::Word97::TC* tcBelow = 0L;
            for ( int c = 0; !tcBelow && c < tapBelow->itcMac ; ++c )
            {
                 if ( qAbs( tapBelow->rgdxaCenter[ c ] - left ) <= 3
                      && qAbs( tapBelow->rgdxaCenter[ c + 1 ] - right ) <= 3 ) {
                     tcBelow = &tapBelow->rgtc[ c ];
                     //kDebug(30513) <<"found cell below, at (Word) column" << c <<" fVertMerge:" << tcBelow->fVertMerge;
                 }
            }
            if ( tcBelow && tcBelow->fVertMerge && !tcBelow->fVertRestart )
                ++rowSpan;
            else
                break;
        }
        kDebug(30513) <<"rowSpan=" << rowSpan;
    }
    // Put a filler in for cells that are part of a merged cell
    // The MSWord spec says they must be empty anyway (and we'll get a warning if not).
    if ( tc.fVertMerge && !tc.fVertRestart ) {
        m_bodyWriter->startElement("table:covered-table-cell");
        m_cellOpen = true;
        return;
    }

    // Check how many cells that mean, according to our cell edge array
    int leftCellNumber = m_currentTable->columnNumber( left );
    int rightCellNumber = m_currentTable->columnNumber( right );

    // In cases where not all columns are present, ensure that the last
    // column spans the remainder of the table.
    // ### It would actually be more closer to the original if we created
    // an empty cell from m_column+1 to the last column. (table-6.doc)
    if ( m_column == nbCells - 1 )  {
        rightCellNumber = m_currentTable->m_cellEdges.size() - 1;
        right = m_currentTable->m_cellEdges[ rightCellNumber ];
    }

    kDebug(30513) << "left edge = " << left << ", right edge = " << right;

    kDebug(30513) << "leftCellNumber = " << leftCellNumber << ", rightCellNumber = "
        << rightCellNumber;
    Q_ASSERT( rightCellNumber >= leftCellNumber ); // you'd better be...
    int colSpan = rightCellNumber - leftCellNumber; // the resulting number of merged cells horizontally

    QRectF cellRect( left / 20.0, // left
                     m_currentY, // top
                     ( right - left ) / 20.0, // width
                     rowHeight() ); // height
    //I can pass these sizes to ODF now...

    kDebug(30513) <<" tableCellStart row=" << m_row <<" WordColumn=" << m_column <<" colSpan=" << colSpan <<" (from" << leftCellNumber <<" to" << rightCellNumber <<" for KWord) rowSpan=" << rowSpan <<" cellRect=" << cellRect;

    // Sort out the borders.
    // It seems we get this on the cells that are adjacent
    // to one has a border, as if it means "whatever the adjacent cell on this border
    // specifies". (cf table-22.doc)
    // We need to set the adjacent cell's border instead, in that case.
    // ### No idea how to do it properly for top/bottom though. The cell above might not have the same size...
    const wvWare::Word97::BRC& brcTop = tc.brcTop;
    const wvWare::Word97::BRC& brcBottom = tc.brcBottom;
    const wvWare::Word97::BRC& brcLeft =
     ( tc.brcLeft.ico == 255 && tc.brcLeft.dptLineWidth == 255 && m_column > 0 ) ?
        m_tap->rgtc[ m_column - 1 ].brcRight
        : tc.brcLeft;
    const wvWare::Word97::BRC& brcRight =
      ( tc.brcRight.ico == 255 && tc.brcRight.dptLineWidth == 255 && m_column < nbCells - 1 ) ?
        m_tap->rgtc[ m_column + 1 ].brcLeft
        : tc.brcRight;

    KoGenStyle cellStyle(KoGenStyle::StyleAutoTableCell, "table-cell");
    //set borders for the four edges of the cell
    //m_borderStyle is the row border style, set in tableRowStart
    if(brcTop.brcType == 0) {
        cellStyle.addProperty("fo:border-top", m_borderStyle);
    }
    //no border
    else if(brcTop.brcType == 255) {
        cellStyle.addProperty("fo:border-top", "thin none #000000");
    }
    //we have a border style defined here
    else {
        cellStyle.addProperty("fo:border-top", Conversion::setBorderAttributes(brcTop));
    }
    //bottom
    if(brcBottom.brcType == 0) {
        cellStyle.addProperty("fo:border-bottom", m_borderStyle);
    }
    else if(brcBottom.brcType == 255) {
        cellStyle.addProperty("fo:border-bottom", "thin none #000000");
    }
    else {
        cellStyle.addProperty("fo:border-bottom", Conversion::setBorderAttributes(brcBottom));
    }
    //left
    if(brcLeft.brcType == 0) {
        cellStyle.addProperty("fo:border-left", m_borderStyle);
    }
    else if(brcLeft.brcType == 255) {
        cellStyle.addProperty("fo:border-left", "thin none #000000");
    }
    else {
        cellStyle.addProperty("fo:border-left", Conversion::setBorderAttributes(brcLeft));
    }
    //right
    if(brcRight.brcType == 0) {
        cellStyle.addProperty("fo:border-right", m_borderStyle);
    }
    else if(brcRight.brcType == 255) {
        cellStyle.addProperty("fo:border-right", "thin none #000000");
    }
    else {
        cellStyle.addProperty("fo:border-right", Conversion::setBorderAttributes(brcRight));
    }

    //text direction
    //if(tc.fVertical) {
    //    cellStyle.addProperty("style:direction", "ttb");
    //}
    //vertical alignment
    if(tc.vertAlign == 0) {
        cellStyle.addProperty("style:vertical-align", "top");
    }
    else if(tc.vertAlign == 1) {
        cellStyle.addProperty("style:vertical-align", "middle");
    }
    else if(tc.vertAlign == 2) {
        cellStyle.addProperty("style:vertical-align", "bottom");
    }

    QString cellStyleName = m_mainStyles->lookup(cellStyle, QString("cell"));

    //emit sigTableCellStart( m_row, leftCellNumber, rowSpan, colSpan, cellRect, m_currentTable->name, brcTop, brcBottom, brcLeft, brcRight, m_tap->rgshd[ m_column ] );
    //start table cell in content
    m_bodyWriter->startElement("table:table-cell");
    m_cellOpen = true;
    m_bodyWriter->addAttribute("table:style-name", cellStyleName.toUtf8());
    if(rowSpan > 1) {
        m_bodyWriter->addAttribute("table:number-rows-spanned", rowSpan);
    }
    if(colSpan > 1) {
        m_bodyWriter->addAttribute("table:number-columns-spanned", colSpan);
        m_colSpan = colSpan;
    }
    else {
        //if we don't set it to colSpan, we still need to (re)set it to a known value
        m_colSpan = 1;
    }
}

void KWordTableHandler::tableCellEnd()
{
    kDebug(30513);
    //end table cell in content
    // but only if we actually opened a cell
    if(m_cellOpen) {
        m_bodyWriter->endElement();//table:table-cell
        m_cellOpen = false;
    }
    if(m_colSpan > 1) {
        for(int i = 1; i < m_colSpan; i++) {
            m_bodyWriter->startElement("table:covered-table-cell");
            m_bodyWriter->endElement();
        }
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
    for (unsigned int i = 0; i < size; i++)
    {
        if (m_cellEdges[i] == cellEdge)  {
            kDebug(30513) << cellEdge <<" -> found";
            return;
        }
        //insert it in the right place if necessary
        if(m_cellEdges[i] > cellEdge) {
            m_cellEdges.insert(i, cellEdge);
            kDebug(30513) << cellEdge <<" -> added. Size=" << size+1;
            return;
        }
    }
    //add it at the end if this edge is larger than all the rest
    m_cellEdges.append(cellEdge);
    kDebug(30513) << cellEdge <<" -> added. Size=" << size+1;
}

// Lookup a cell edge from the cache of cell edges
// And return the column number
int KWord::Table::columnNumber(int cellEdge) const
{
    kDebug(30513) ;
    for (unsigned int i = 0; i < (unsigned int)m_cellEdges.size(); i++)
    {
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
    return qMax( m_tap->dyaRowHeight / 20.0, 20.0);
}

#include "tablehandler.moc"
