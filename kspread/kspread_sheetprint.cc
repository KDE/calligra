/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>,
   2003 Philipp Müller <philipp.mueller@gmx.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kspread_sheetprint.h"
#include "kspread_sheet.h"
#include "kspread_doc.h"
#include "kspread_undo.h"

#include <koDocumentInfo.h>

#include <kmessagebox.h>
#include <kprinter.h>
#include <kdebug.h>

#include <qregexp.h>

#include <pwd.h>
#include <unistd.h>

#include "kspread_sheetprint.moc"

#define NO_MODIFICATION_POSSIBLE \
do { \
  KMessageBox::error( 0, i18n ( "You cannot change a protected sheet" ) ); return; \
} while(0)

KSpreadSheetPrint::KSpreadSheetPrint( KSpreadSheet* sheet )
{
    m_pSheet = sheet;
    m_pDoc = m_pSheet->doc();

    m_bPrintGrid = false;
    m_bPrintCommentIndicator = false;
    m_bPrintFormulaIndicator = false;

    m_leftBorder = 20.0;
    m_rightBorder = 20.0;
    m_topBorder = 20.0;
    m_bottomBorder = 20.0;
    m_paperFormat = PG_DIN_A4;
    m_paperWidth = PG_A4_WIDTH;
    m_paperHeight = PG_A4_HEIGHT;
    m_orientation = PG_PORTRAIT;
    m_printRange = QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) );
    m_lnewPageListX.append( 1 );
    m_lnewPageListY.append( 1 );
    m_dPrintRepeatColumnsWidth = 0.0;
    m_dPrintRepeatRowsHeight = 0.0;
    m_printRepeatColumns = qMakePair( 0, 0 );
    m_printRepeatRows = qMakePair( 0, 0 );

    calcPaperSize();
}

KSpreadSheetPrint::~KSpreadSheetPrint()
{
  // nothing todo yet
}

void KSpreadSheetPrint::print( QPainter &painter, KPrinter *_printer )
{
    kdDebug(36001)<<"PRINTING ...."<<endl;

    // Override the current grid pen setting, when set to disable
    QPen gridPen;
    bool oldShowGrid = m_pSheet->getShowGrid();
    m_pSheet->setShowGrid( m_bPrintGrid );
    if ( !m_bPrintGrid )
    {
        gridPen = m_pDoc->defaultGridPen();
        QPen nopen;
        nopen.setStyle( NoPen );
        m_pDoc->setDefaultGridPen( nopen );
    }

    //
    // Find maximum right/bottom cell with content
    //
    QRect cell_range;
    cell_range.setCoords( 1, 1, 1, 1 );

    KSpreadCell* c = m_pSheet->firstCell();
    for( ;c; c = c->nextCell() )
    {
        if ( c->needsPrinting() )
        {
            if ( c->column() > cell_range.right() )
                cell_range.setRight( c->column() );
            if ( c->row() > cell_range.bottom() )
                cell_range.setBottom( c->row() );
        }
    }

    // Now look at the children
    QPtrListIterator<KoDocumentChild> cit( m_pDoc->children() );
    double dummy;
    int i;
    for( ; cit.current(); ++cit )
    {
        QRect bound = cit.current()->boundingRect();
        i = m_pSheet->leftColumn( bound.right(), dummy );
        if ( i > cell_range.right() )
            cell_range.setRight( i );
        i = m_pSheet->topRow( bound.bottom(), dummy );
        if ( i > cell_range.bottom() )
            cell_range.setBottom( i );
    }

    // Adjust to the print range
    if ( cell_range.top() < m_printRange.top() ) cell_range.setTop( m_printRange.top() );
    if ( cell_range.left() < m_printRange.left() ) cell_range.setLeft( m_printRange.left() );
    if ( cell_range.bottom() > m_printRange.bottom()-1 ) cell_range.setBottom( m_printRange.bottom()-1 );
    if ( cell_range.right() > m_printRange.right()-1 ) cell_range.setRight( m_printRange.right()-1 );

    //If we have repeated columns/rows, children get an offset on pages
    double offsetX = 0;
    double offsetY = 0;
    double currentOffsetX = 0;
    double currentOffsetY = 0;
    //Calculate offsetX for repeated columns
    if ( m_printRepeatColumns.first != 0 )
    {
        //When we need repeated columns, reservate space for them
        for ( int i = m_printRepeatColumns.first; i <= m_printRepeatColumns.second; i++)
            offsetX += m_pSheet->columnFormat( i )->dblWidth();
    }
    //Calculate offsetY for repeated rows
    if ( m_printRepeatRows.first != 0 )
    {
        //When we need repeated rows, reservate space for them
        for ( int i = m_printRepeatRows.first; i <= m_printRepeatRows.second; i++)
            offsetY += m_pSheet->rowFormat( i )->dblHeight();
    }

    //
    // Find out how many pages need printing
    // and which cells to print on which page.
    //
    QValueList<QRect> page_list;
    QValueList<KoRect> page_frame_list;
    QValueList<KoPoint> page_frame_list_offset;

    // How much space is on every page for table content ?
    QRect rect;
    rect.setCoords( 0, 0, (int)( MM_TO_POINT ( printableWidth()  )),
                          (int)( MM_TO_POINT ( printableHeight() )) );

    // Up to this row everything is already handled, starting with the print range
    int bottom = m_printRange.top()-1;
    // Start of the next page
    int top = bottom + 1;
    // Calculate all pages, but if we are embedded, print only the first one
    while ( bottom <= cell_range.bottom() /* && page_list.count() == 0 */ )
    {
        // Up to this column everything is already printed, starting with the print range
        int right = m_printRange.left()-1;
        // Start of the next page
        int left = right + 1;
        while ( right <= cell_range.right() )
        {
            QRect page_range;
            page_range.setLeft( left );
            page_range.setTop( top );

            int col = left;
            double x = m_pSheet->columnFormat( col )->dblWidth();

            //Check if we have to repeat some columns
            if ( ( m_printRepeatColumns.first != 0 ) && ( col > m_printRepeatColumns.first ) )
            {
                //When we need repeated columns, reservate space for them
                x += offsetX;
                currentOffsetX = offsetX;
            }

            //Count the columns which still fit on the page
            while ( ( x < rect.width() ) && ( col <= m_printRange.right() ) )
            {
                col++;
                x += m_pSheet->columnFormat( col )->dblWidth();
            }

            // We want to print at least one column
            if ( col == left )
                col = left + 1;
            page_range.setRight( col - 1 );

            int row = top;
            double y = m_pSheet->rowFormat( row )->dblHeight();

            //Check if we have to repeat some rows
            if ( ( m_printRepeatRows.first != 0 ) && ( row > m_printRepeatRows.first ) )
            {
                //When we need repeated rows, reservate space for them
                y += offsetY;
                currentOffsetY = offsetY;
            }

            //Count the rows, which still fit on the page
            while ( ( y < rect.height() ) && ( row <= m_printRange.bottom() ) )
            {
                row++;
                y += m_pSheet->rowFormat( row )->dblHeight();
            }

            // We want to print at least one row
            if ( row == top )
                row = top + 1;
            page_range.setBottom( row - 1 );

            right = page_range.right();
            bottom = page_range.bottom();

            //
            // Test wether there is anything on the page at all.
            //

            // Look at the cells
            bool empty = TRUE;
            for( int r = page_range.top(); empty && ( r <= page_range.bottom() ); ++r )
                for( int c = page_range.left(); empty && ( c <= page_range.right() ); ++c )
                    if ( m_pSheet->cellAt( c, r )->needsPrinting() )
                        empty = FALSE;

            // Look for children
            QRect intView = QRect( QPoint( m_pDoc->zoomItX( m_pSheet->dblColumnPos( page_range.left() ) ),
                                           m_pDoc->zoomItY( m_pSheet->dblRowPos( page_range.top() ) ) ),
                                   QPoint( m_pDoc->zoomItX( m_pSheet->dblColumnPos( col-1 ) +
                                                            m_pSheet->columnFormat( col-1 )->dblWidth() ),
                                           m_pDoc->zoomItY( m_pSheet->dblRowPos( row-1 ) +
                                                            m_pSheet->rowFormat( row-1 )->dblHeight() ) ) );
            QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
            for( ; empty && it.current(); ++it )
            {
                QRect bound = it.current()->boundingRect();
                if ( bound.intersects( intView ) )
                    empty = FALSE;
            }

            //Append page when there is something to print
            if ( !empty )
            {
                KoRect view = KoRect( KoPoint( m_pSheet->dblColumnPos( page_range.left() ),
                                               m_pSheet->dblRowPos( page_range.top() ) ),
                                      KoPoint( m_pSheet->dblColumnPos( col-1 ) +
                                               m_pSheet->columnFormat( col-1 )->dblWidth(),
                                               m_pSheet->dblRowPos( row-1 ) +
                                               m_pSheet->rowFormat( row-1 )->dblHeight() ) );
                page_list.append( page_range );
                page_frame_list.append( view );
                page_frame_list_offset.append( KoPoint( currentOffsetX, currentOffsetY ) );
            }

            left = right + 1;
        }

        top = bottom + 1;
    }

    kdDebug(36001) << "PRINTING " << page_list.count() << " pages" << endl;
    m_uprintPages = page_list.count();

    if ( page_list.count() == 0 )
    {
        KMessageBox::information( 0, i18n("Nothing to print.") );
        if ( !m_bPrintGrid )
        {
            // Restore the grid pen
            m_pDoc->setDefaultGridPen( gridPen );
        }
        m_pSheet->setShowGrid( oldShowGrid );

        //abort printing
        _printer->abort();

        return;
    }

    int pagenr = 1;

    //
    // Print all pages in the list
    //

    QValueList<QRect>::Iterator it = page_list.begin();
    QValueList<KoRect>::Iterator fit = page_frame_list.begin();
    QValueList<KoPoint>::Iterator fito = page_frame_list_offset.begin();
    int w;
    for( ; it != page_list.end(); ++it, ++fit, ++fito, ++pagenr )
    {
        // print head line
        QFont font( "Times", 10 );
        painter.setFont( font );
        QFontMetrics fm = painter.fontMetrics();
        w = fm.width( headLeft( pagenr, m_pSheet->tableName() ) );
        if ( w > 0 )
            painter.drawText( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) ),
                              m_pDoc->zoomItY( MM_TO_POINT ( 10.0 ) ),
                              headLeft( pagenr, m_pSheet->tableName() ) );
        w = fm.width( headMid( pagenr, m_pSheet->tableName() ) );
        if ( w > 0 )
            painter.drawText( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) +
                                               ( MM_TO_POINT ( printableWidth() ) -
                                                (float)w ) / 2.0 ),
                              m_pDoc->zoomItY( MM_TO_POINT ( 10.0 )),
                              headMid( pagenr, m_pSheet->tableName() ) );
        w = fm.width( headRight( pagenr, m_pSheet->tableName() ) );
        if ( w > 0 )
            painter.drawText( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) +
                                               MM_TO_POINT ( printableWidth()) - (float)w ),
                              m_pDoc->zoomItY( MM_TO_POINT ( 10.0 )),
                              headRight( pagenr, m_pSheet->tableName() ) );

        // print foot line
        w = fm.width( footLeft( pagenr, m_pSheet->tableName() ) );
        if ( w > 0 )
            painter.drawText( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() )),
                              m_pDoc->zoomItY( MM_TO_POINT ( paperHeight() - 10.0 )),
                              footLeft( pagenr, m_pSheet->tableName() ) );
        w = fm.width( footMid( pagenr, m_pSheet->tableName() ) );
        if ( w > 0 )
            painter.drawText( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() )+
                                               ( MM_TO_POINT ( printableWidth() ) -
                                                (float)w ) / 2.0 ),
                              m_pDoc->zoomItY( MM_TO_POINT  ( paperHeight() - 10.0 ) ),
                              footMid( pagenr, m_pSheet->tableName() ) );
        w = fm.width( footRight( pagenr, m_pSheet->tableName() ) );
        if ( w > 0 )
            painter.drawText( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) +
                                                MM_TO_POINT ( printableWidth() ) -
                                                (float)w ),
                              m_pDoc->zoomItY( MM_TO_POINT ( paperHeight() - 10.0 ) ),
                              footRight( pagenr, m_pSheet->tableName() ) );

        painter.translate( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) ),
                           m_pDoc->zoomItY( MM_TO_POINT ( topBorder()  ) ) );

        // Print the page
        printPage( painter, *it, *fit, *fito );

        painter.translate( - m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) ),
                           - m_pDoc->zoomItY( MM_TO_POINT ( topBorder()  ) ) );

        if ( pagenr < (int)page_list.count() )
            _printer->newPage();
    }

    if ( !m_bPrintGrid )
    {
	// Restore the grid pen
	m_pDoc->setDefaultGridPen( gridPen );
    }
    m_pSheet->setShowGrid( oldShowGrid );
}

void KSpreadSheetPrint::printPage( QPainter &_painter, const QRect& page_range, const KoRect& view, const KoPoint _childOffset )
{
      /*kdDebug(36001) << "Rect x=" << page_range.left() << " y=" << page_range.top() << ", w="
      << page_range.width() << " h="  << page_range.height() << "  offsetx: "<< _childOffset.x()
      << "  offsety: " << _childOffset.y() <<"  view-x: "<<view.x()<< endl;*/

    //Don't paint on the page borders
    QRegion clipRegion( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) ),
                        m_pDoc->zoomItY( MM_TO_POINT ( leftBorder() ) ),
                        m_pDoc->zoomItX( view.width() ),
                        m_pDoc->zoomItY( view.height() ) );
    _painter.setClipRegion( clipRegion );

    //
    // Draw the cells.
    //
    double xpos;
    double xpos_Start;
    double ypos;
    double ypos_Start;
    KSpreadCell *cell;
    RowFormat *row_lay;
    ColumnFormat *col_lay;

    //Check if we have to repeat some rows and columns
    xpos = 0.0;
    ypos = 0.0;
    xpos_Start = 0.0;
    ypos_Start = 0.0;
    if ( m_printRepeatColumns.first != 0 && page_range.left() > m_printRepeatColumns.first &&
         m_printRepeatRows.first != 0    && page_range.top() > m_printRepeatRows.first )
    {
        for ( int y = m_printRepeatRows.first; y <= m_printRepeatRows.second; y++ )
        {
            row_lay = m_pSheet->rowFormat( y );
            xpos = 0.0;

            for ( int x = m_printRepeatColumns.first; x <= m_printRepeatColumns.second; x++ )
            {
                col_lay = m_pSheet->columnFormat( x );

                cell = m_pSheet->cellAt( x, y );
                KoRect r( 0.0, 0.0, view.width(), view.height() );

                bool paintBordersRight = ( x == m_printRepeatColumns.second );
                bool paintBordersBottom = ( y == m_printRepeatRows.second );

                cell->paintCell( r, _painter, NULL,
                                 KoPoint( xpos, ypos ),
                                 QPoint( x, y ), paintBordersRight, paintBordersBottom );

                xpos += col_lay->dblWidth();
            }

            ypos += row_lay->dblHeight();
        }
        ypos_Start = ypos;
        xpos_Start = xpos;
        //Don't let obscuring cells and children overpaint this area
        clipRegion -= QRegion ( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) ),
                                m_pDoc->zoomItY( MM_TO_POINT ( topBorder() ) ),
                                m_pDoc->zoomItX( xpos ),
                                m_pDoc->zoomItY( ypos ) );
        _painter.setClipRegion( clipRegion );
    }

    //Check if we have to repeat some rows
    xpos = xpos_Start;
    ypos = 0.0;
    if ( m_printRepeatRows.first != 0 && page_range.top() > m_printRepeatRows.first )
    {
        for ( int y = m_printRepeatRows.first; y <= m_printRepeatRows.second; y++ )
        {
            row_lay = m_pSheet->rowFormat( y );
            xpos = xpos_Start;

            for ( int x = page_range.left(); x <= page_range.right(); x++ )
            {
                col_lay = m_pSheet->columnFormat( x );

                cell = m_pSheet->cellAt( x, y );
                KoRect r( 0.0, 0.0, view.width() + xpos, view.height() );

                bool paintBordersRight = ( x == page_range.right() );
                bool paintBordersBottom = ( y == m_printRepeatRows.second );

                cell->paintCell( r, _painter, NULL,
                                 KoPoint( xpos, ypos ),
                                 QPoint( x, y ), paintBordersRight, paintBordersBottom );

                xpos += col_lay->dblWidth();
            }

            ypos += row_lay->dblHeight();
        }
        ypos_Start = ypos;
        //Don't let obscuring cells and children overpaint this area
        clipRegion -= QRegion( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() + xpos_Start ) ),
                               m_pDoc->zoomItY( MM_TO_POINT ( topBorder() ) ),
                               m_pDoc->zoomItX( xpos - xpos_Start ),
                               m_pDoc->zoomItY( ypos ) );
        _painter.setClipRegion( clipRegion );
    }

    //Check if we have to repeat some columns
    xpos = 0.0;
    ypos = ypos_Start;
    if ( m_printRepeatColumns.first != 0 && page_range.left() > m_printRepeatColumns.first )
    {
        for ( int y = page_range.top(); y <= page_range.bottom(); y++ )
        {
            row_lay = m_pSheet->rowFormat( y );
            xpos = 0.0;

            for ( int x = m_printRepeatColumns.first; x <= m_printRepeatColumns.second; x++ )
            {
                col_lay = m_pSheet->columnFormat( x );

                cell = m_pSheet->cellAt( x, y );
                KoRect r( 0.0, 0.0, view.width() + xpos, view.height() + ypos );

                bool paintBordersRight = ( x == m_printRepeatColumns.second );
                bool paintBordersBottom = ( y == page_range.bottom() );

                cell->paintCell( r, _painter, NULL,
                                 KoPoint( xpos, ypos ),
                                 QPoint( x, y ), paintBordersRight, paintBordersBottom );

                xpos += col_lay->dblWidth();
            }

            ypos += row_lay->dblHeight();
        }
        xpos_Start = xpos;
        //Don't let obscuring cells and children overpaint this area
        clipRegion -= QRegion( m_pDoc->zoomItX( MM_TO_POINT ( leftBorder() ) ),
                               m_pDoc->zoomItY( MM_TO_POINT ( topBorder() + ypos_Start ) ),
                               m_pDoc->zoomItX( xpos ),
                               m_pDoc->zoomItY( ypos - ypos_Start ) );
        _painter.setClipRegion( clipRegion );
    }

    //Print the cells
    xpos = xpos_Start;
    ypos = ypos_Start;
    for ( int y = page_range.top(); y <= page_range.bottom(); y++ )
    {
        row_lay = m_pSheet->rowFormat( y );
        xpos = xpos_Start;

        for ( int x = page_range.left(); x <= page_range.right(); x++ )
        {
            col_lay = m_pSheet->columnFormat( x );

            cell = m_pSheet->cellAt( x, y );
            KoRect r( 0.0, 0.0, view.width() + xpos, view.height() + ypos );

            bool paintBordersRight = ( x == page_range.right() );
            bool paintBordersBottom = ( y == page_range.bottom() );

            cell->paintCell( r, _painter, NULL,
                             KoPoint( xpos, ypos ),
                             QPoint( x, y ), paintBordersRight, paintBordersBottom );

            xpos += col_lay->dblWidth();
        }

        ypos += row_lay->dblHeight();
    }

    //
    // Draw the children
    //
    QRect zoomedView = m_pDoc->zoomRect( view );
    QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
    QRect bound;
    for( ; it.current(); ++it )
    {
//        QString tmp=QString("Testing child %1/%2 %3/%4 against view %5/%6 %7/%8")
//        .arg(it.current()->contentRect().left())
//        .arg(it.current()->contentRect().top())
//        .arg(it.current()->contentRect().right())
//        .arg(it.current()->contentRect().bottom())
//        .arg(view.left()).arg(view.top()).arg(zoomedView.right()).arg(zoomedView.bottom());
//        kdDebug(36001)<<tmp<<" offset "<<_childOffset.x()<<"/"<<_childOffset.y()<<endl;

        bound = it.current()->boundingRect();
        if ( ( ( KSpreadChild* )it.current() )->table() == m_pSheet &&
             bound.intersects( zoomedView ) )
        {
            _painter.save();

            _painter.translate( -zoomedView.left() + m_pDoc->zoomItX( _childOffset.x() ),
                                -zoomedView.top()  + m_pDoc->zoomItY( _childOffset.y() ) );
            bound.moveBy( -bound.x(), -bound.y() );

            it.current()->transform( _painter );
            it.current()->document()->paintEverything( _painter,
                                                       bound,
                                                       it.current()->isTransparent() );
            _painter.restore();
        }
    }
}

bool KSpreadSheetPrint::isOnNewPageX( int _column )
{
    //Are these the edges of the print range?
    if ( _column == m_printRange.left() || _column == m_printRange.right() + 1 )
        return true;

    //beyond the print range it's always false
    if ( _column < m_printRange.left() || _column > m_printRange.right() )
        return false;

    //If we start, then add the printrange
    if ( m_lnewPageListX.empty() )
        m_lnewPageListX.append( m_printRange.left() ); //Add the first entry

    //Now check if we find the column already
    if ( m_lnewPageListX.findIndex( _column ) != -1 )
        return true;

    //If _column is greater than the last entry, we need to calculate the result
    if ( _column > m_lnewPageListX.last() ) //this columns hasn't been calculated before
    {
        int col = m_lnewPageListX.last();
        float x = m_pSheet->columnFormat( col )->mmWidth();
        //Add repeated column width, when necessary
        if ( col > m_printRepeatColumns.first )
            x += m_dPrintRepeatColumnsWidth;

        while ( ( col <= _column ) && ( col < m_printRange.right() ) )
        {
            if ( x > printableWidth() )
            {
                //We found a new page, so add it to the list
                m_lnewPageListX.append( col );
                if ( col == _column )
                    return TRUE;
                else
                {
                    x = m_pSheet->columnFormat( col )->mmWidth();
                    if ( col >= m_printRepeatColumns.first )
                        x += m_dPrintRepeatColumnsWidth;
                }
            }

            col++;
            x += m_pSheet->columnFormat( col )->mmWidth();
        }
    }
    return FALSE;
}

bool KSpreadSheetPrint::isOnNewPageY( int _row )
{
    //Are these the edges of the print range?
    if ( _row == m_printRange.top() || _row == m_printRange.bottom() + 1 )
        return true;

     //beyond the print range it's always false
    if ( _row < m_printRange.top() || _row > m_printRange.bottom() )
        return false;

    //If we start, then add the printrange
    if ( m_lnewPageListY.empty() )
        m_lnewPageListY.append( m_printRange.top() ); //Add the first entry

    //Now check if we find the row already
    if ( m_lnewPageListY.findIndex( _row ) != -1 )
        return true;

    //If _column is greater than the last entry, we need to calculate the result
    if ( _row > m_lnewPageListY.last() ) //this columns hasn't been calculated before
    {
        int row = m_lnewPageListY.last();
        float y = m_pSheet->rowFormat( row )->mmHeight();
        //Add repeated row height, when necessary
        if ( row > m_printRepeatRows.first )
            y += m_dPrintRepeatRowsHeight;
        while ( ( row <= _row ) && ( row < m_printRange.bottom() ) )
        {
            if ( y > printableHeight() )
            {
                //We found a new page, so add it to the list
                m_lnewPageListY.append( row );

                if ( row == _row )
                    return TRUE;
                else
                {
                    y = m_pSheet->rowFormat( row )->mmHeight();
                    if ( row >= m_printRepeatRows.first )
                        y += m_dPrintRepeatRowsHeight;
                }
            }
            row++;
            y += m_pSheet->rowFormat( row )->mmHeight();
        }
    }

    return FALSE;
}

void KSpreadSheetPrint::updateNewPageListX( int _col )
{
    //If the new range is after the first entry, we need to delete the whole list
    if ( m_lnewPageListX.first() != m_printRange.left() )
    {
        m_lnewPageListX.clear();
        m_lnewPageListX.append( m_printRange.left() );
        return;
    }

    if ( _col < m_lnewPageListX.last() )
    {
        //Find the page entry for this column
        QValueList<int>::iterator it;
        it = m_lnewPageListX.find( _col );
        while ( ( it == m_lnewPageListX.end() ) && _col > 0 )
        {
            _col--;
            it = m_lnewPageListX.find( _col );
        }

        //Remove later pages
        while ( it != m_lnewPageListX.end() )
            it = m_lnewPageListX.remove( it );

        //Add default page when list is now empty
        if ( m_lnewPageListX.empty() )
            m_lnewPageListX.append( m_printRange.left() );
    }
}

void KSpreadSheetPrint::updateNewPageListY( int _row )
{
    //If the new range is after the first entry, we need to delete the whole list
    if ( m_lnewPageListY.first() != m_printRange.top() )
    {
        m_lnewPageListY.clear();
        m_lnewPageListY.append( m_printRange.top() );
        return;
    }

    if ( _row < m_lnewPageListY.last() )
    {
        //Find the page entry for this row
        QValueList<int>::iterator it;
        it = m_lnewPageListY.find( _row );
        while ( ( it == m_lnewPageListY.end() ) && _row > 0 )
        {
            _row--;
            it = m_lnewPageListY.find( _row );
        }

        //Remove later pages
        while ( it != m_lnewPageListY.end() )
            it = m_lnewPageListY.remove( it );

        //Add default page when list is now empty
        if ( m_lnewPageListY.empty() )
            m_lnewPageListY.append( m_printRange.top() );
    }
}

void KSpreadSheetPrint::definePrintRange( KSpreadSelection* selectionInfo )
{
    if ( !selectionInfo->singleCellSelection() )
    {
        if ( !m_pDoc->undoBuffer()->isLocked() )
        {
             KSpreadUndoAction* undo = new KSpreadUndoDefinePrintRange( m_pSheet->doc(), m_pSheet );
             m_pDoc->undoBuffer()->appendUndo( undo );
        }

        setPrintRange( selectionInfo->selection() );
    }
}

void KSpreadSheetPrint::resetPrintRange ()
{
    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
         KSpreadUndoAction* undo = new KSpreadUndoDefinePrintRange( m_pSheet->doc(), m_pSheet );
         m_pDoc->undoBuffer()->appendUndo( undo );
    }

    setPrintRange( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) );
}

void KSpreadSheetPrint::replaceHeadFootLineMacro ( QString &_text, const QString &_search, const QString &_replace )
{
    if ( _search != _replace )
        _text.replace ( QRegExp( "<" + _search + ">" ), "<" + _replace + ">" );
}

QString KSpreadSheetPrint::localizeHeadFootLine ( const QString &_text )
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro ( tmp, "page",   i18n("page") );
    replaceHeadFootLineMacro ( tmp, "pages",  i18n("pages") );
    replaceHeadFootLineMacro ( tmp, "file",   i18n("file") );
    replaceHeadFootLineMacro ( tmp, "name",   i18n("name") );
    replaceHeadFootLineMacro ( tmp, "time",   i18n("time") );
    replaceHeadFootLineMacro ( tmp, "date",   i18n("date") );
    replaceHeadFootLineMacro ( tmp, "author", i18n("author") );
    replaceHeadFootLineMacro ( tmp, "email",  i18n("email") );
    replaceHeadFootLineMacro ( tmp, "org",    i18n("org") );
    replaceHeadFootLineMacro ( tmp, "sheet",  i18n("sheet") );

    return tmp;
}


QString KSpreadSheetPrint::delocalizeHeadFootLine ( const QString &_text )
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro ( tmp, i18n("page"),   "page" );
    replaceHeadFootLineMacro ( tmp, i18n("pages"),  "pages" );
    replaceHeadFootLineMacro ( tmp, i18n("file"),   "file" );
    replaceHeadFootLineMacro ( tmp, i18n("name"),   "name" );
    replaceHeadFootLineMacro ( tmp, i18n("time"),   "time" );
    replaceHeadFootLineMacro ( tmp, i18n("date"),   "date" );
    replaceHeadFootLineMacro ( tmp, i18n("author"), "author" );
    replaceHeadFootLineMacro ( tmp, i18n("email"),  "email" );
    replaceHeadFootLineMacro ( tmp, i18n("org"),    "org" );
    replaceHeadFootLineMacro ( tmp, i18n("sheet"),  "sheet" );

    return tmp;
}


KoHeadFoot KSpreadSheetPrint::headFootLine() const
{
    KoHeadFoot hf;
    hf.headLeft  = m_headLeft;
    hf.headRight = m_headRight;
    hf.headMid   = m_headMid;
    hf.footLeft  = m_footLeft;
    hf.footRight = m_footRight;
    hf.footMid   = m_footMid;

    return hf;
}


void KSpreadSheetPrint::setHeadFootLine( const QString &_headl, const QString &_headm, const QString &_headr,
                                         const QString &_footl, const QString &_footm, const QString &_footr )
{
    if ( m_pSheet->isProtected() )
        NO_MODIFICATION_POSSIBLE;

    m_headLeft  = _headl;
    m_headRight = _headr;
    m_headMid   = _headm;
    m_footLeft  = _footl;
    m_footRight = _footr;
    m_footMid   = _footm;

    m_pDoc->setModified( TRUE );
}

void KSpreadSheetPrint::setPaperOrientation( KoOrientation _orient )
{
    if ( m_pSheet->isProtected() )
        NO_MODIFICATION_POSSIBLE;

    m_orientation = _orient;
    calcPaperSize();
    updatePrintRepeatColumnsWidth();
    updatePrintRepeatRowsHeight();
    updateNewPageListX( m_printRange.left() ); //Reset the list
    updateNewPageListY( m_printRange.top() ); //Reset the list

    if( m_pSheet->isShowPageBorders() )
        emit sig_updateView( m_pSheet );
}


KoPageLayout KSpreadSheetPrint::paperLayout() const
{
    KoPageLayout pl;
    pl.format = m_paperFormat;
    pl.orientation = m_orientation;
    pl.ptWidth  =  m_paperWidth;
    pl.ptHeight =  m_paperHeight;
    pl.ptLeft   =  m_leftBorder;
    pl.ptRight  =  m_rightBorder;
    pl.ptTop    =  m_topBorder;
    pl.ptBottom =  m_bottomBorder;
    return pl;
}


void KSpreadSheetPrint::setPaperLayout( float _leftBorder, float _topBorder,
                                        float _rightBorder, float _bottomBorder,
                                        KoFormat _paper,
                                        KoOrientation _orientation )
{
    if ( m_pSheet->isProtected() )
        NO_MODIFICATION_POSSIBLE;

    m_leftBorder   = _leftBorder;
    m_rightBorder  = _rightBorder;
    m_topBorder    = _topBorder;
    m_bottomBorder = _bottomBorder;
    m_paperFormat  = _paper;

    setPaperOrientation( _orientation ); //calcPaperSize() is done here already

//    QPtrListIterator<KoView> it( views() );
//    for( ;it.current(); ++it )
//    {
//        KSpreadView *v = static_cast<KSpreadView *>( it.current() );
          // We need to trigger the appropriate repaintings in the cells near the
          // border of the page. The easiest way for this is to turn the borders
          // off and on (or on and off if they were off).
//        bool bBorderWasShown = v->activeTable()->isShowPageBorders();
//        v->activeTable()->setShowPageBorders( !bBorderWasShown );
//        v->activeTable()->setShowPageBorders( bBorderWasShown );
//    }

    m_pDoc->setModified( TRUE );
}

void KSpreadSheetPrint::setPaperLayout( float _leftBorder, float _topBorder,
                                        float _rightBorder, float _bottomBorder,
                                        const QString& _paper,
                                        const QString& _orientation )
{
    if ( m_pSheet->isProtected() )
        NO_MODIFICATION_POSSIBLE;

    KoFormat f = paperFormat();
    KoOrientation o = orientation();

    QString paper( _paper );
    if ( paper[0].isDigit() ) // Custom format
    {
        const int i = paper.find( 'x' );
        if ( i < 0 )
        {
            // We have nothing useful, so assume ISO A4
            f = PG_DIN_A4;
        }
        else
        {
            f = PG_CUSTOM;
            m_paperWidth  = paper.left(i).toFloat();
            m_paperHeight = paper.mid(i+1).toFloat();
            if ( m_paperWidth < 10.0 )
                m_paperWidth = PG_A4_WIDTH;
            if ( m_paperHeight < 10.0 )
                m_paperWidth = PG_A4_HEIGHT;
        }
    }
    else
    {
        f = KoPageFormat::formatFromString( paper );
        if ( f == PG_CUSTOM )
            // We have no idea about height or width, therefore assume ISO A4
            f = PG_DIN_A4;
    }

    if ( _orientation == "Portrait" )
        o = PG_PORTRAIT;
    else if ( _orientation == "Landscape" )
        o = PG_LANDSCAPE;

    setPaperLayout( _leftBorder, _topBorder, _rightBorder, _bottomBorder, f, o );
}

void KSpreadSheetPrint::calcPaperSize()
{
    if ( m_paperFormat != PG_CUSTOM )
    {
        m_paperWidth = KoPageFormat::width( m_paperFormat, m_orientation );
        m_paperHeight = KoPageFormat::height( m_paperFormat, m_orientation );
    }
}

QString KSpreadSheetPrint::paperFormatString()const
{
    if ( m_paperFormat == PG_CUSTOM )
    {
        QString tmp;
        tmp.sprintf( "%fx%f", m_paperWidth, m_paperHeight );
        return tmp;
    }

    return KoPageFormat::formatString( m_paperFormat );
}

const char* KSpreadSheetPrint::orientationString() const
{
    switch( m_orientation )
    {
    case KPrinter::Portrait:
        return "Portrait";
    case KPrinter::Landscape:
        return "Landscape";
    }

    kdWarning(36001)<<"KSpreadSheetPrint: Unknown orientation, using now portrait"<<endl;
    return 0;
}

QString KSpreadSheetPrint::completeHeading( const QString &_data, int _page, const QString &_table ) const
{
    QString page( QString::number( _page) );
    QString pages( QString::number( m_uprintPages ) );

    QString pathFileName(m_pDoc->url().path());
    if ( pathFileName.isNull() )
        pathFileName="";

    QString fileName(m_pDoc->url().fileName());
    if( fileName.isNull())
        fileName="";

    QString t(QTime::currentTime().toString());
    QString d(QDate::currentDate().toString());
    QString ta;
    if ( !_table.isEmpty() )
        ta = _table;

    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    QString full_name;
    QString email_addr;
    QString organization;
    QString tmp;
    if ( !authorPage )
        kdWarning() << "Author information not found in document Info !" << endl;
    else
    {
        full_name = authorPage->fullName();
        email_addr = authorPage->email();
        organization = authorPage->company();
    }

    char hostname[80];
    struct passwd *p;

    p = getpwuid(getuid());
    gethostname(hostname, sizeof(hostname));

    if(full_name.isEmpty())
 	full_name=p->pw_gecos;

    if( email_addr.isEmpty())
	email_addr = QString("%1@%2").arg(p->pw_name).arg(hostname);

    tmp = _data;
    int pos = 0;
    while ( ( pos = tmp.find( "<page>", pos ) ) != -1 )
        tmp.replace( pos, 6, page );
    pos = 0;
    while ( ( pos = tmp.find( "<pages>", pos ) ) != -1 )
        tmp.replace( pos, 7, pages );
    pos = 0;
    while ( ( pos = tmp.find( "<file>", pos ) ) != -1 )
        tmp.replace( pos, 6, pathFileName );
    pos = 0;
    while ( ( pos = tmp.find( "<name>", pos ) ) != -1 )
        tmp.replace( pos, 6, fileName );
    pos = 0;
    while ( ( pos = tmp.find( "<time>", pos ) ) != -1 )
        tmp.replace( pos, 6, t );
    pos = 0;
    while ( ( pos = tmp.find( "<date>", pos ) ) != -1 )
        tmp.replace( pos, 6, d );
    pos = 0;
    while ( ( pos = tmp.find( "<author>", pos ) ) != -1 )
        tmp.replace( pos, 8, full_name );
    pos = 0;
    while ( ( pos = tmp.find( "<email>", pos ) ) != -1 )
        tmp.replace( pos, 7, email_addr );
    pos = 0;
    while ( ( pos = tmp.find( "<org>", pos ) ) != -1 )
        tmp.replace( pos, 5, organization );
    pos = 0;
    while ( ( pos = tmp.find( "<sheet>", pos ) ) != -1 )
        tmp.replace( pos, 7, ta );

    return tmp;
}

void KSpreadSheetPrint::setPrintRange( QRect _printRange )
{
    if ( m_pSheet->isProtected() )
        NO_MODIFICATION_POSSIBLE;

    if ( m_printRange == _printRange )
        return;

    int oldLeft = m_printRange.left();
    int oldTop = m_printRange.top();
    m_printRange = _printRange;

    //Refresh calculation of stored page breaks, the lower one of old and new
    if ( oldLeft != _printRange.left() )
        updateNewPageListX( QMIN( oldLeft, _printRange.left() ) );
    if ( oldTop != _printRange.top() )
        updateNewPageListY( QMIN( oldTop, _printRange.top() ) );

    m_pDoc->setModified( true );

    emit sig_updateView( m_pSheet );
}

void KSpreadSheetPrint::setPrintGrid( bool _printGrid )
{
   if ( m_bPrintGrid == _printGrid )
        return;

    m_bPrintGrid = _printGrid;
    m_pDoc->setModified( true );
}

void KSpreadSheetPrint::setPrintCommentIndicator( bool _printCommentIndicator )
{
    if ( m_bPrintCommentIndicator == _printCommentIndicator )
        return;

    m_bPrintCommentIndicator = _printCommentIndicator;
    m_pDoc->setModified( true );
}

void KSpreadSheetPrint::setPrintFormulaIndicator( bool _printFormulaIndicator )
{
    if ( m_bPrintFormulaIndicator == _printFormulaIndicator )
        return;

    m_bPrintFormulaIndicator = _printFormulaIndicator;
    m_pDoc->setModified( true );
}

void KSpreadSheetPrint::updatePrintRepeatColumnsWidth()
{
    m_dPrintRepeatColumnsWidth = 0.0;
    if ( m_printRepeatColumns.first != 0 )
    {
        for ( int i = m_printRepeatColumns.first; i <= m_printRepeatColumns.second; i++)
        {
            m_dPrintRepeatColumnsWidth += m_pSheet->columnFormat( i )->mmWidth();
        }
    }
}


void KSpreadSheetPrint::updatePrintRepeatRowsHeight()
{
    m_dPrintRepeatRowsHeight += 0.0;
    if ( m_printRepeatRows.first != 0 )
    {
        for ( int i = m_printRepeatRows.first; i <= m_printRepeatRows.second; i++)
        {
            m_dPrintRepeatRowsHeight += m_pSheet->rowFormat( i )->mmHeight();
        }
    }
}


void KSpreadSheetPrint::setPrintRepeatColumns( QPair<int, int> _printRepeatColumns )
{
    //Bring arguments in order
    if ( _printRepeatColumns.first > _printRepeatColumns.second )
    {
        int tmp = _printRepeatColumns.first;
        _printRepeatColumns.first = _printRepeatColumns.second;
        _printRepeatColumns.second = tmp;
    }

    //If old are equal to the new setting, nothing is to be done at all
    if ( m_printRepeatColumns == _printRepeatColumns )
        return;

    int oldFirst = m_printRepeatColumns.first;
    m_printRepeatColumns = _printRepeatColumns;

    //Recalcualte the space needed for the repeated columns
    updatePrintRepeatColumnsWidth();

    //Refresh calculation of stored page breaks, the lower one of old and new
    updateNewPageListX( QMIN( oldFirst, _printRepeatColumns.first ) );

    //Refresh view, if page borders are shown
    if ( m_pSheet->isShowPageBorders() )
        emit sig_updateView( m_pSheet );

    m_pDoc->setModified( true );
}

void KSpreadSheetPrint::setPrintRepeatRows( QPair<int, int> _printRepeatRows )
{
    //Bring arguments in order
    if ( _printRepeatRows.first > _printRepeatRows.second )
    {
        int tmp = _printRepeatRows.first;
        _printRepeatRows.first = _printRepeatRows.second;
        _printRepeatRows.second = tmp;
    }

    //If old are equal to the new setting, nothing is to be done at all
    if ( m_printRepeatRows == _printRepeatRows )
        return;

    int oldFirst = m_printRepeatRows.first;
    m_printRepeatRows = _printRepeatRows;

    //Refresh calculation of stored page breaks, the lower one of old and new
    updateNewPageListY( QMIN( oldFirst, _printRepeatRows.first ) );

    //Recalcualte the space needed for the repeated rows
    updatePrintRepeatRowsHeight();

    //Refresh view, if page borders are shown
    if ( m_pSheet->isShowPageBorders() )
        emit sig_updateView( m_pSheet );

    m_pDoc->setModified( true );
}

void KSpreadSheetPrint::insertColumn( int col, int nbCol )
{
    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int left = m_printRange.left();
        int right = m_printRange.right();

        for( int i=0; i<=nbCol; i++ )
        {
            if ( left >= col ) left++;
            if ( right >= col ) right++;
        }
        //Validity checks
        if ( left > KS_colMax ) left = KS_colMax;
        if ( right > KS_colMax ) right = KS_colMax;
        setPrintRange( QRect( QPoint( left, m_printRange.top() ), QPoint( right, m_printRange.bottom() ) ) );
    }
}

void KSpreadSheetPrint::insertRow( int row, int nbRow )
{
    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int top = m_printRange.top();
        int bottom = m_printRange.bottom();

        for( int i=0; i<=nbRow; i++ )
        {
            if ( top >= row ) top++;
            if ( bottom >= row ) bottom++;
        }
        //Validity checks
        if ( top > KS_rowMax ) top = KS_rowMax;
        if ( bottom > KS_rowMax ) bottom = KS_rowMax;
        setPrintRange( QRect( QPoint( m_printRange.left(), top ), QPoint( m_printRange.right(), bottom ) ) );
    }
}

void KSpreadSheetPrint::removeColumn( int col, int nbCol )
{
    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int left = m_printRange.left();
        int right = m_printRange.right();

        for( int i=0; i<=nbCol; i++ )
        {
            if ( left > col ) left--;
            if ( right >= col ) right--;
        }
        //Validity checks
        if ( left < 1 ) left = 1;
        if ( right < 1 ) right = 1;
        setPrintRange( QRect( QPoint( left, m_printRange.top() ), QPoint( right, m_printRange.bottom() ) ) );
    }

    //update repeat columns, when it has been defined
    if ( m_printRepeatColumns.first != 0 )
    {
        int left = m_printRepeatColumns.first;
        int right = m_printRepeatColumns.second;

        for( int i=0; i<=nbCol; i++ )
        {
            if ( left > col ) left--;
            if ( right >= col ) right--;
        }
        //Validity checks
        if ( left < 1 ) left = 1;
        if ( right < 1 ) right = 1;
        setPrintRepeatColumns ( qMakePair( left, right ) );
    }
}

void KSpreadSheetPrint::removeRow( int row, int nbRow )
{
    //update print range, when it has been defined
    if ( m_printRange != QRect( QPoint(1, 1), QPoint(KS_colMax, KS_rowMax) ) )
    {
        int top = m_printRange.top();
        int bottom = m_printRange.bottom();

        for( int i=0; i<=nbRow; i++ )
        {
            if ( top > row ) top--;
            if ( bottom >= row ) bottom--;
        }
        //Validity checks
        if ( top < 1 ) top = 1;
        if ( bottom < 1 ) bottom = 1;
        setPrintRange( QRect( QPoint( m_printRange.left(), top ), QPoint( m_printRange.right(), bottom ) ) );
    }

    //update repeat rows, when it has been defined
    if ( m_printRepeatRows.first != 0 )
    {
        int top = m_printRepeatRows.first;
        int bottom = m_printRepeatRows.second;

        for( int i=0; i<=nbRow; i++ )
        {
            if ( top > row ) top--;
            if ( bottom >= row ) bottom--;
        }
        //Validity checks
        if ( top < 1 ) top = 1;
        if ( bottom < 1 ) bottom = 1;
        setPrintRepeatRows( qMakePair( top, bottom ) );
    }
}

