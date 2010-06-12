/* This file is part of the KDE project
   Copyright 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2003 Philipp Müller <philipp.mueller@gmx.de>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "SheetPrint.h"
#include "SheetPrint_p.h"

#include "PrintSettings.h"
#include "Region.h"
#include "RowColumnFormat.h"
#include "Sheet.h"

using namespace KSpread;

void SheetPrint::Private::calculateHorizontalPageParameters( int _column )
{
    // Zoom the print width ONCE here, instead for each column width.
    const double printWidth = m_settings->printWidth() / m_settings->zoom();

    float offset = 0.0;

    //Are these the edges of the print range?
    const QRect printRange = m_settings->printRegion().lastRange();
#if 0
    if (_column == printRange.left() || _column == printRange.right()) {
        if (_column > m_maxCheckedNewPageX)
            m_maxCheckedNewPageX = _column;
        return;
    }

    //We don't check beyond the print range
    if ( _column < printRange.left() || _column > printRange.right() )
    {
        if( _column > m_maxCheckedNewPageX )
            m_maxCheckedNewPageX = _column;
        if ( _column > printRange.right() )
        {
            if ( m_lnewPageListX.last().endItem()==0 )
                m_lnewPageListX.last().setEndItem( printRange.right() );
        }
        return;
    }
#endif

    // Check if the pre-calculated width matches the repeated columns setting.
    const bool repetitions = m_settings->repeatedColumns().first != 0;
    if (repetitions != (m_dPrintRepeatColumnsWidth == 0.0)) {
        // Existing column repetitions, but their pre-calculated width is zero?
        // Must be the first run. Or the other way around? Seem to be orphaned.
        // Either way, seems they do not match. Calculate them!
        updateRepeatedColumnsWidth();
    }

    // The end of the last item (zero, if list is empty).
    const int end = m_lnewPageListX.empty() ? 0 : m_lnewPageListX.last().endItem();

    //If _column is greater than the last entry, we need to calculate the result
    if ( _column > end &&
         _column > m_maxCheckedNewPageX ) //this columns hasn't been calculated before
    {
        int startCol = end + 1;
        int col = startCol;
        double x = m_pSheet->columnFormat( col )->width();

        // Add a new page.
        m_lnewPageListX.append(PrintNewPageEntry(startCol));

        //Add repeated column width, when necessary
        const QPair<int, int> repeatedColumns = m_settings->repeatedColumns();
        if ( col > repeatedColumns.first )
        {
            x += m_dPrintRepeatColumnsWidth;
            offset = m_dPrintRepeatColumnsWidth;
        }
        kDebug() << "startCol:" << startCol << "col:" << col << "x:" << x
                 << "offset:" << offset << repeatedColumns;

        while ((col <= _column) && (col < printRange.right())) {
            kDebug() << "loop:" << "startCol:" << startCol << "col:" << col
                     << "x:" << x << "offset:" << offset;
            // end of page?
            if (x > printWidth || m_pSheet->columnFormat(col)->hasPageBreak()) {
                //Now store into the previous entry the enditem and the width
                m_lnewPageListX.last().setEndItem(col - 1);
                m_lnewPageListX.last().setSize(x - m_pSheet->columnFormat(col)->width());
                m_lnewPageListX.last().setOffset(offset);

                //start a new page
                m_lnewPageListX.append(PrintNewPageEntry(col));
                startCol = col;
                x = m_pSheet->columnFormat(col)->width();
                if (col >= repeatedColumns.first) {
                    kDebug() << "col >= repeatedColumns.first:" << col << repeatedColumns.first;
                    x += m_dPrintRepeatColumnsWidth;
                    offset = m_dPrintRepeatColumnsWidth;
                }
            }
            col++;
            x += m_pSheet->columnFormat( col )->width();
        }

        // Iterate to the end of the page.
        while (m_lnewPageListX.last().endItem() == 0) {
            kDebug() << "loop to end" << "col:" << col << "x:" << x << "offset:" << offset
                        << "m_maxCheckedNewPageX:" << m_maxCheckedNewPageX;
            if (x > printWidth || m_pSheet->columnFormat(col)->hasPageBreak()) {
                // Now store into the previous entry the enditem and the width
                m_lnewPageListX.last().setEndItem(col - 1);
                m_lnewPageListX.last().setSize(x - m_pSheet->columnFormat(col)->width());
                m_lnewPageListX.last().setOffset(offset);

                if (col - 1 > m_maxCheckedNewPageX) {
                    m_maxCheckedNewPageX = col - 1;
                }
                return;
            }
            ++col;
            x += m_pSheet->columnFormat(col)->width();
        }
    }

    kDebug() << "m_maxCheckedNewPageX:" << m_maxCheckedNewPageX;
    if (_column > m_maxCheckedNewPageX) {
        m_maxCheckedNewPageX = _column;
        m_lnewPageListX.last().setEndItem( _column );
    }
}

void SheetPrint::Private::calculateVerticalPageParameters( int _row )
{
    // Zoom the print height ONCE here, instead for each row height.
    const double printHeight = m_settings->printHeight() / m_settings->zoom();

    float offset = 0.0;

    //Are these the edges of the print range?
    const QRect printRange = m_settings->printRegion().lastRange();
#if 0
    if (_row == printRange.top() || _row == printRange.bottom()) {
        if (_row > m_maxCheckedNewPageY)
            m_maxCheckedNewPageY = _row;
        return;
    }

     //beyond the print range it's always false
    if ( _row < printRange.top() || _row > printRange.bottom() )
    {
        if( _row > m_maxCheckedNewPageY )
            m_maxCheckedNewPageY = _row;
        if ( _row > printRange.bottom() )
        {
            if ( m_lnewPageListY.last().endItem()==0 )
                m_lnewPageListY.last().setEndItem( printRange.bottom() );
        }
        return;
    }
#endif

    // Check if the pre-calculated height matches the repeated rows setting.
    const bool repetitions = m_settings->repeatedRows().first != 0;
    if (repetitions != (m_dPrintRepeatRowsHeight == 0.0)) {
        // Existing row repetitions, but their pre-calculated height is zero?
        // Must be the first run. Or the other way around? Seem to be orphaned.
        // Either way, seems they do not match. Calculate them!
        updateRepeatedRowsHeight();
    }

    // The end of the last item (zero, if list is empty).
    const int end = m_lnewPageListY.empty() ? 0 : m_lnewPageListY.last().endItem();

    //If _column is greater than the last entry, we need to calculate the result
    if ( _row > end &&
         _row > m_maxCheckedNewPageY ) //this columns hasn't been calculated before
    {
        int startRow = end + 1;
        int row = startRow;
        double y = m_pSheet->rowFormat( row )->height();

        // Add a new page.
        m_lnewPageListY.append(PrintNewPageEntry(startRow));

        //Add repeated row height, when necessary
        const QPair<int, int> repeatedRows = m_settings->repeatedRows();
        if ( row > repeatedRows.first )
        {
            y += m_dPrintRepeatRowsHeight;
            offset = m_dPrintRepeatRowsHeight;
        }

        while ( ( row <= _row ) && ( row < printRange.bottom() ) )
        {
            // end of page?
            if (y > printHeight || m_pSheet->rowFormat(row)->hasPageBreak()) {
                //Now store into the previous entry the enditem and the width
                m_lnewPageListY.last().setEndItem(row - 1);
                m_lnewPageListY.last().setSize(y - m_pSheet->rowFormat(row)->height());
                m_lnewPageListY.last().setOffset(offset);

                //start a new page
                m_lnewPageListY.append(PrintNewPageEntry(row));
                startRow = row;
                y = m_pSheet->rowFormat(row)->height();
                if (row >= repeatedRows.first) {
                    y += m_dPrintRepeatRowsHeight;
                    offset = m_dPrintRepeatRowsHeight;
                }
            }
            row++;
            y += m_pSheet->rowFormat( row )->height();
        }

        // Iterate to the end of the page.
        while (m_lnewPageListY.last().endItem() == 0) {
            if (y > printHeight || m_pSheet->rowFormat(row)->hasPageBreak()) {
                // Now store into the previous entry the enditem and the width
                m_lnewPageListY.last().setEndItem(row - 1);
                m_lnewPageListY.last().setSize(y - m_pSheet->rowFormat(row)->height());
                m_lnewPageListY.last().setOffset(offset);

                if (row - 1 > m_maxCheckedNewPageY) {
                    m_maxCheckedNewPageY = row - 1;
                }
                return;
            }
            ++row;
            y += m_pSheet->rowFormat(row)->height();
        }
    }

    if( _row > m_maxCheckedNewPageY )
    {
        m_maxCheckedNewPageY = _row;
        m_lnewPageListY.last().setEndItem( _row );
    }
}

void SheetPrint::Private::calculateZoomForPageLimitX()
{
    kDebug() <<"Calculating zoom for X limit";
    const int horizontalPageLimit = m_settings->pageLimits().width();
    if( horizontalPageLimit == 0 )
        return;

    const double origZoom = m_settings->zoom();

    if( m_settings->zoom() < 1.0 ) {
        q->updateHorizontalPageParameters( 0 ); // clear all parameters
        m_settings->setZoom( 1.0 );
    }

    QRect printRange = m_pSheet->usedArea(true);
    calculateHorizontalPageParameters(printRange.right());
    int currentPages = m_lnewPageListX.count();

    if (currentPages <= horizontalPageLimit)
        return;

    //calculating a factor for scaling the zoom down makes it lots faster
    double factor = (double)horizontalPageLimit/(double)currentPages +
                    1-(double)currentPages/((double)currentPages+1); //add possible error;
    kDebug() <<"Calculated factor for scaling m_settings->zoom():" << factor;
    m_settings->setZoom( m_settings->zoom()*factor );

    kDebug() <<"New exact zoom:" << m_settings->zoom();

    if (m_settings->zoom() < 0.01)
        m_settings->setZoom( 0.01 );
    if (m_settings->zoom() > 1.0)
        m_settings->setZoom( 1.0 );

    m_settings->setZoom( (((int)(m_settings->zoom()*100 + 0.5))/100.0) );

    kDebug() <<"New rounded zoom:" << m_settings->zoom();

    q->updateHorizontalPageParameters( 0 ); // clear all parameters
    calculateHorizontalPageParameters(printRange.right());
    currentPages = m_lnewPageListX.count();

    kDebug() <<"Number of pages with this zoom:" << currentPages;

    while( ( currentPages > horizontalPageLimit ) && ( m_settings->zoom() > 0.01 ) )
    {
        m_settings->setZoom( m_settings->zoom() - 0.01 );
        q->updateHorizontalPageParameters( 0 ); // clear all parameters
        calculateHorizontalPageParameters(printRange.right());
        currentPages = m_lnewPageListX.count();
        kDebug() <<"Looping -0.01; current zoom:" << m_settings->zoom();
    }

    if ( m_settings->zoom() < origZoom )
    {
        // Trigger an update of the vertical page parameters.
        q->updateVerticalPageParameters(0); // clear all parameters
        calculateVerticalPageParameters(printRange.bottom());
    }
    else
        m_settings->setZoom( origZoom );
}

void SheetPrint::Private::calculateZoomForPageLimitY()
{
    kDebug() <<"Calculating zoom for Y limit";
    const int verticalPageLimit = m_settings->pageLimits().height();
    if( verticalPageLimit == 0 )
        return;

    const double origZoom = m_settings->zoom();

    if( m_settings->zoom() < 1.0 ) {
        q->updateVerticalPageParameters( 0 ); // clear all parameters
        m_settings->setZoom( 1.0 );
    }

    QRect printRange = m_pSheet->usedArea(true);
    calculateVerticalPageParameters(printRange.bottom());
    int currentPages = m_lnewPageListY.count();

    if (currentPages <= verticalPageLimit)
        return;

    double factor = (double)verticalPageLimit/(double)currentPages +
                    1-(double)currentPages/((double)currentPages+1); //add possible error
    kDebug() <<"Calculated factor for scaling m_settings->zoom():" << factor;
    m_settings->setZoom( m_settings->zoom()*factor );

    kDebug() <<"New exact zoom:" << m_settings->zoom();

    if (m_settings->zoom() < 0.01)
        m_settings->setZoom( 0.01 );
    if (m_settings->zoom() > 1.0)
        m_settings->setZoom( 1.0 );

    m_settings->setZoom( (((int)(m_settings->zoom()*100 + 0.5))/100.0) );

    kDebug() <<"New rounded zoom:" << m_settings->zoom();

    q->updateVerticalPageParameters( 0 ); // clear all parameters
    calculateVerticalPageParameters(printRange.bottom());
    currentPages = m_lnewPageListY.count();

    kDebug() <<"Number of pages with this zoom:" << currentPages;

    while( ( currentPages > verticalPageLimit ) && ( m_settings->zoom() > 0.01 ) )
    {
        m_settings->setZoom( m_settings->zoom() - 0.01 );
        q->updateVerticalPageParameters( 0 ); // clear all parameters
        calculateVerticalPageParameters(printRange.bottom());
        currentPages = m_lnewPageListY.count();
        kDebug() <<"Looping -0.01; current zoom:" << m_settings->zoom();
    }

    if ( m_settings->zoom() < origZoom )
    {
        // Trigger an update of the horizontal page parameters.
        q->updateHorizontalPageParameters(0); // clear all parameters
        calculateHorizontalPageParameters(printRange.right());
    }
    else
        m_settings->setZoom( origZoom );
}

void SheetPrint::Private::updateRepeatedColumnsWidth()
{
    m_dPrintRepeatColumnsWidth = 0.0;
    const QPair<int, int> repeatedColumns = m_settings->repeatedColumns();
    if( repeatedColumns.first != 0 )
    {
        for( int i = repeatedColumns.first; i <= repeatedColumns.second; i++ )
        {
            m_dPrintRepeatColumnsWidth += m_pSheet->columnFormat( i )->width();
        }
    }
}

void SheetPrint::Private::updateRepeatedRowsHeight()
{
    m_dPrintRepeatRowsHeight = 0.0;
    const QPair<int, int> repeatedRows = m_settings->repeatedRows();
    if ( repeatedRows.first != 0 )
    {
        for ( int i = repeatedRows.first; i <= repeatedRows.second; i++)
        {
            m_dPrintRepeatRowsHeight += m_pSheet->rowFormat( i )->height();
        }
    }
}

bool PrintNewPageEntry::operator==( PrintNewPageEntry const & entry ) const
{
    return m_iStartItem == entry.m_iStartItem;
}
