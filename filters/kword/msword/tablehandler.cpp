/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "tablehandler.h"

#include <wv2/word97_generated.h>

#include <kdebug.h>
#include <KoRect.h>
#include <q3tl.h>
//Added by qt3to4:
#include <Q3ValueList>

KWordTableHandler::KWordTableHandler()
{
    tableEnd();
}

// Called by Document before invoking the table-row-functors
void KWordTableHandler::tableStart( KWord::Table* table )
{
    Q_ASSERT( table );
    Q_ASSERT( !table->name.isEmpty() );
    m_currentTable = table;
#warning "port it: qHeapSort"
    //qHeapSort( table->m_cellEdges );
#if 0
    for (unsigned int i = 0; i < table->m_cellEdges.size(); i++)
        kDebug(30513) << table->m_cellEdges[i] << endl;
#endif
    m_row = -1;
    m_currentY = 0;
}

void KWordTableHandler::tableEnd()
{
    m_currentTable = 0L; // we don't own it, the table-queue does!
    m_row = -2;
    m_column = -2;
    // Warning: if doing more here, check that it's still ok to call this from the ctor
}

void KWordTableHandler::tableRowStart( wvWare::SharedPtr<const wvWare::Word97::TAP> tap )
{
    if ( m_row == -2 )
    {
        kWarning(30513) << "tableRowStart: tableStart not called previously!" << endl;
        return;
    }
    Q_ASSERT( m_currentTable );
    Q_ASSERT( !m_currentTable->name.isEmpty() );
    m_row++;
    m_column = -1;
    m_tap = tap;
    kDebug(30513) << "tableRowStart row=" << m_row << endl;
}

void KWordTableHandler::tableRowEnd()
{
    kDebug(30513) << "tableRowEnd" << endl;
    m_currentY += rowHeight();
}

void KWordTableHandler::tableCellStart()
{
    Q_ASSERT( m_tap );
    if ( !m_tap )
        return;
    m_column++;
    int nbCells = m_tap->itcMac;
    Q_ASSERT( m_column < nbCells );
    if ( m_column >= nbCells )
        return;

    // Get table cell descriptor
    const wvWare::Word97::TC& tc = m_tap->rgtc[ m_column ];

    int left = m_tap->rgdxaCenter[ m_column ]; // in DXAs
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
    if ( tc.fVertRestart )
    {
        //kDebug(30513) << "fVertRestart is set!" << endl;
        // This cell is the first one of a series of vertically merged cells ->
        // we want to find out its size.
        Q3ValueList<KWord::Row>::Iterator it = m_currentTable->rows.at( m_row + 1 );
        for( ; it != m_currentTable->rows.end(); ++it )  {
            // Find cell right below us in row (*it), if any
            KWord::TAPptr tapBelow = (*it).tap;
            const wvWare::Word97::TC* tcBelow = 0L;
            for ( int c = 0; !tcBelow && c < tapBelow->itcMac ; ++c )
            {
                 if ( QABS( tapBelow->rgdxaCenter[ c ] - left ) <= 3
                      && QABS( tapBelow->rgdxaCenter[ c + 1 ] - right ) <= 3 ) {
                     tcBelow = &tapBelow->rgtc[ c ];
                     //kDebug(30513) << "found cell below, at (Word) column " << c << " fVertMerge:" << tcBelow->fVertMerge << endl;
                 }
            }
            if ( tcBelow && tcBelow->fVertMerge && !tcBelow->fVertRestart )
                ++rowSpan;
            else
                break;
        }
        //kDebug(30513) << "rowSpan=" << rowSpan << endl;
    }
    // Skip cells that are part of a vertically merged cell, KWord doesn't want them
    // The MSWord spec says they must be empty anyway (and we'll get a warning if not).
    if ( tc.fVertMerge && !tc.fVertRestart )
        return;

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

    Q_ASSERT( rightCellNumber >= leftCellNumber ); // you'd better be...
    int colSpan = rightCellNumber - leftCellNumber; // the resulting number of merged cells

    KoRect cellRect( left / 20.0, // left
                     m_currentY, // top
                     ( right - left ) / 20.0, // width
                     rowHeight() ); // height

    kDebug(30513) << " tableCellStart row=" << m_row << " WordColumn=" << m_column << " colSpan=" << colSpan << " (from " << leftCellNumber << " to " << rightCellNumber << " for KWord) rowSpan=" << rowSpan << " cellRect=" << cellRect << endl;

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

    emit sigTableCellStart( m_row, leftCellNumber, rowSpan, colSpan, cellRect, m_currentTable->name, brcTop, brcBottom, brcLeft, brcRight, m_tap->rgshd[ m_column ] );
}

void KWordTableHandler::tableCellEnd()
{
    kDebug(30513) << " tableCellEnd" << endl;
    emit sigTableCellEnd();
}


// Add cell edge into the cache of cell edges for a given table.
void KWord::Table::cacheCellEdge(int cellEdge)
{
    uint size = m_cellEdges.size();
    // Do we already know about this edge?
    for (unsigned int i = 0; i < size; i++)
    {
        if (m_cellEdges[i] == cellEdge)  {
            kDebug(30513) << k_funcinfo << cellEdge << " -> found" << endl;
            return;
        }
    }

    // Add the edge to the array.
    m_cellEdges.resize(size + 1, Q3GArray::SpeedOptim);
    m_cellEdges[size] = cellEdge;
    kDebug(30513) << k_funcinfo << cellEdge << " -> added. Size=" << size+1 << endl;
}

// Lookup a cell edge from the cache of cell edges
// And return the column number
int KWord::Table::columnNumber(int cellEdge) const
{
    for (unsigned int i = 0; i < m_cellEdges.size(); i++)
    {
        if (m_cellEdges[i] == cellEdge)
            return i;
    }
    // This can't happen, if cacheCellEdge has been properly called
    kWarning(30513) << "Column not found for cellEdge x=" << cellEdge << " - BUG." << endl;
    return 0;
}

double KWordTableHandler::rowHeight() const
{
    return qMax( m_tap->dyaRowHeight / 20.0, 20.0);
}

#include "tablehandler.moc"
