/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "AutoFormatCommand.h"

#include "CellStorage.h"
#include "Sheet.h"
#include "Style.h"
#include "Region.h"

#include <klocale.h>

using namespace KSpread;

AutoFormatCommand::AutoFormatCommand()
{
    m_name = i18n( "Auto-Format" );
}

AutoFormatCommand::~AutoFormatCommand()
{
}

void AutoFormatCommand::setStyles( const QList<Style*>& styles )
{
    m_styles = styles;
}

bool AutoFormatCommand::process( Element* element )
{
    Region::ConstIterator end(constEnd());
    for ( Region::ConstIterator it(constBegin()); it != end; ++it )
    {
        const QRect rect = (*it)->rect();
        // Top left corner
        if ( m_styles[0] && !m_styles[0]->isDefault() )
            m_sheet->cellStorage()->setStyle( Region(rect.topLeft()), *m_styles[0] );
        // Top column
        for( int col = rect.left() + 1; col <= rect.right(); ++col )
        {
            int pos = 1 + ( ( col - rect.left() - 1 ) % 2 );
            Cell cell( m_sheet, col, rect.top() );
            if ( !cell.isPartOfMerged() )
            {
                if ( m_styles[pos] && !m_styles[pos]->isDefault() )
                    m_sheet->cellStorage()->setStyle( Region(col, rect.top()), *m_styles[pos] );

                Style* style = ( col == rect.right() ) ? m_styles[2] : m_styles[1];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setTopBorderPen( style->topBorderPen() );
                    m_sheet->cellStorage()->setStyle( Region(col, rect.top()), tmpStyle );
                }

                style = ( col == rect.left() + 1 ) ? m_styles[1] : m_styles[2];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setLeftBorderPen( style->leftBorderPen() );
                    m_sheet->cellStorage()->setStyle( Region(col, rect.top()), tmpStyle );
                }
            }
        }

        if ( m_styles[3] )
        {
            Style tmpStyle;
            tmpStyle.setRightBorderPen( m_styles[3]->leftBorderPen() );
            m_sheet->cellStorage()->setStyle( Region(rect.topRight()), tmpStyle );
        }

        // Left row
        for ( int row = rect.top() + 1; row <= rect.bottom(); ++row )
        {
            int pos = 4 + ( ( row - rect.top() - 1 ) % 2 ) * 4;
            Cell cell( m_sheet, rect.left(), row );
            if ( !cell.isPartOfMerged() )
            {
                if ( m_styles[pos] && !m_styles[pos]->isDefault() )
                    m_sheet->cellStorage()->setStyle( Region(rect.left(), row), *m_styles[pos] );

                Style* style = ( row == rect.bottom() ) ? m_styles[8] : m_styles[4];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setLeftBorderPen( style->leftBorderPen() );
                    m_sheet->cellStorage()->setStyle( Region(rect.left(), row), tmpStyle );
                }

                style = ( row == rect.top() + 1 ) ? m_styles[4] : m_styles[8];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setTopBorderPen( style->topBorderPen() );
                    m_sheet->cellStorage()->setStyle( Region(rect.left(), row), tmpStyle );
                }
            }
        }

        // Body
        for ( int col = rect.left() + 1; col <= rect.right(); ++col )
        {
            for ( int row = rect.top() + 1; row <= rect.bottom(); ++row )
            {
                int pos = 5 + ( ( row - rect.top() - 1 ) % 2 ) * 4 + ( ( col - rect.left() - 1 ) % 2 );
                Cell cell( m_sheet, col, row );
                if ( !cell.isPartOfMerged() )
                {
                    if ( m_styles[pos] && !m_styles[pos]->isDefault() )
                        m_sheet->cellStorage()->setStyle( Region(col, row), *m_styles[pos] );

                    Style* style;
                    if ( col == rect.left() + 1 )
                        style = m_styles[ 5 + ( ( row - rect.top() - 1 ) % 2 ) * 4 ];
                    else
                        style = m_styles[ 6 + ( ( row - rect.top() - 1 ) % 2 ) * 4 ];

                    if ( style )
                    {
                        Style tmpStyle;
                        tmpStyle.setLeftBorderPen( style->leftBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(col, row), tmpStyle );
                    }

                    if ( row == rect.top() + 1 )
                        style = m_styles[ 5 + ( ( col - rect.left() - 1 ) % 2 ) ];
                    else
                        style = m_styles[ 9 + ( ( col - rect.left() - 1 ) % 2 ) ];

                    if ( style )
                    {
                        Style tmpStyle;
                        tmpStyle.setTopBorderPen( style->topBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(col, row), tmpStyle );
                    }
                }
            }
        }

        // Outer right border
        for ( int row = rect.top(); row <= rect.bottom(); ++row )
        {
            Cell cell( m_sheet, rect.right(), row );
            if ( !cell.isPartOfMerged() )
            {
                if ( row == rect.top() )
                {
                    if ( m_styles[3] )
                    {
                        Style tmpStyle;
                        tmpStyle.setRightBorderPen( m_styles[3]->leftBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(rect.right(), row), tmpStyle );
                    }
                }
                else if ( row == rect.right() )
                {
                    if ( m_styles[11] )
                    {
                        Style tmpStyle;
                        tmpStyle.setRightBorderPen( m_styles[11]->leftBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(rect.right(), row), tmpStyle );
                    }
                }
                else
                {
                    if ( m_styles[7] )
                    {
                        Style tmpStyle;
                        tmpStyle.setRightBorderPen( m_styles[7]->leftBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(rect.right(), row), tmpStyle );
                    }
                }
            }
        }

        // Outer bottom border
        for ( int col = rect.left(); col <= rect.right(); ++col )
        {
            Cell cell( m_sheet, col, rect.bottom() );
            if(!cell.isPartOfMerged())
            {
                if ( col == rect.left() )
                {
                    if ( m_styles[12] )
                    {
                        Style tmpStyle;
                        tmpStyle.setBottomBorderPen( m_styles[12]->topBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(col, rect.bottom()), tmpStyle );
                    }
                }
                else if ( col == rect.right() )
                {
                    if ( m_styles[14] )
                    {
                        Style tmpStyle;
                        tmpStyle.setBottomBorderPen( m_styles[14]->topBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(col, rect.bottom()), tmpStyle );
                    }
                }
                else
                {
                    if ( m_styles[13] )
                    {
                        Style tmpStyle;
                        tmpStyle.setBottomBorderPen( m_styles[13]->topBorderPen() );
                        m_sheet->cellStorage()->setStyle( Region(col, rect.bottom()), tmpStyle );
                    }
                }
            }
        }
    }
    return true;
}
