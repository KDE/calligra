/* This file is part of the KDE project
   Copyright 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include <kdebug.h>

#include "Cell.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleStorage.h"

#include "StyleManipulators.h"

using namespace KSpread;

StyleManipulator::StyleManipulator()
    : m_horizontalPen( QPen(QColor(), 0, Qt::NoPen) )
    , m_verticalPen( QPen(QColor(), 0, Qt::NoPen) )
    , m_style( new Style() )
{
}

StyleManipulator::~StyleManipulator()
{
}

bool StyleManipulator::process(Element* element)
{
    const QRect range = element->rect();
    if ( !m_reverse ) // (re)do
    {
        if ( m_firstrun )
        {
            const QList< QPair<QRectF,QSharedDataPointer<SubStyle> > > rawUndoData
                    = m_sheet->styleStorage()->undoData( element->rect() );
            for ( int i = 0; i < rawUndoData.count(); ++i )
            {
//                 if ( m_style->hasAttribute( rawUndoData[i].second->type() ) ||
//                      rawUndoData[i].second->type() == Style::DefaultStyleKey ||
//                      rawUndoData[i].second->type() == Style::NamedStyleKey )
                    m_undoData << rawUndoData[i];
            }
        }
        // special handling for the border
        const QPen leftPen = m_style->leftBorderPen();
        const QPen rightPen = m_style->rightBorderPen();
        const QPen topPen = m_style->topBorderPen();
        const QPen bottomPen = m_style->bottomBorderPen();
        m_style->clearAttribute( Style::LeftPen );
        m_style->clearAttribute( Style::RightPen );
        m_style->clearAttribute( Style::TopPen );
        m_style->clearAttribute( Style::BottomPen );

        // use the horizontal/vertical pens
        if ( m_horizontalPen.style() != Qt::NoPen )
        {
            m_style->setTopBorderPen( m_horizontalPen );
            m_style->setBottomBorderPen( m_horizontalPen );
        }
        if ( m_verticalPen.style() != Qt::NoPen )
        {
            m_style->setLeftBorderPen( m_verticalPen );
            m_style->setRightBorderPen( m_verticalPen );
        }

        // set the actual style
        m_sheet->setStyle( Region(range), *m_style );

        // set the outer border styles
        Style style;
        if ( leftPen.style() != Qt::NoPen )
        {
            style.setLeftBorderPen( leftPen );
            m_sheet->setStyle( Region(QRect(range.left(), range.top(), 1, range.height())), style );
        }
        if ( rightPen.style() != Qt::NoPen )
        {
            style.clear();
            style.setRightBorderPen( rightPen );
            m_sheet->setStyle( Region(QRect(range.right(), range.top(), 1, range.height())), style );
        }
        if ( topPen.style() != Qt::NoPen )
        {
            style.clear();
            style.setTopBorderPen( topPen );
            m_sheet->setStyle( Region(QRect(range.left(), range.top(), range.width(), 1)), style );
        }
        if ( bottomPen.style() != Qt::NoPen )
        {
            style.clear();
            style.setBottomBorderPen( bottomPen );
            m_sheet->setStyle( Region(QRect(range.left(), range.bottom(), range.width(), 1)), style );
        }
    }
    return true;
}

bool StyleManipulator::mainProcessing()
{
    if ( !m_reverse )
    {
        if ( m_firstrun )
        {
            m_undoData.clear();
        }
    }
    else
    {
        Style style;
        style.setDefault();
        m_sheet->setStyle( *this, style );
        for ( int i = 0; i < m_undoData.count(); ++i )
        {
            m_sheet->styleStorage()->insert( m_undoData[i].first.toRect(), m_undoData[i].second );
        }
    }
    return Manipulator::mainProcessing();
}

bool StyleManipulator::postProcessing()
{
    m_sheet->addLayoutDirtyRegion( *this );
    return true;
}


/***************************************************************************
  class IncreaseIndentManipulator
****************************************************************************/

IncreaseIndentManipulator::IncreaseIndentManipulator()
  : Manipulator()
{
}

bool IncreaseIndentManipulator::process( Cell* cell )
{
    Style style;
    if ( !m_reverse )
    {
        style.setIndentation( cell->style(cell->column(), cell->row()).indentation() + m_sheet->doc()->indentValue() );
    }
    else // m_reverse
    {
        style.setIndentation( cell->style(cell->column(), cell->row()).indentation() - m_sheet->doc()->indentValue() );
    }
    cell->setStyle( style );
    return true;
}

bool IncreaseIndentManipulator::process( const Style& style )
{
#ifndef KSPREAD_NEW_STYLE_STORAGE
  if ( !m_reverse )
  {
    if ( dynamic_cast<ColumnFormat*>(format) )
    {
      const ColumnFormat* columnFormat = dynamic_cast<ColumnFormat*>(format);
      format->setIndent( format->getIndent( columnFormat->column(), 0 ) + m_sheet->doc()->indentValue() );
    }
    else
    {
      const RowFormat* rowFormat = dynamic_cast<RowFormat*>(format);
      format->setIndent( format->getIndent( 0, rowFormat->row() ) + m_sheet->doc()->indentValue() );
    }
  }
  else // m_reverse
  {
    if ( dynamic_cast<ColumnFormat*>(format) )
    {
      const ColumnFormat* columnFormat = dynamic_cast<ColumnFormat*>(format);
      format->setIndent( format->getIndent( columnFormat->column(), 0 ) - m_sheet->doc()->indentValue() );
    }
    else
    {
      const RowFormat* rowFormat = dynamic_cast<RowFormat*>(format);
      format->setIndent( format->getIndent( 0, rowFormat->row() ) - m_sheet->doc()->indentValue() );
    }
  }
#endif
  return true;
}

QString IncreaseIndentManipulator::name() const
{
  if ( !m_reverse )
  {
    return i18n( "Increase Indentation" );
  }
  else
  {
    return i18n( "Decrease Indentation" );
  }
}



/***************************************************************************
  class BorderColorManipulator
****************************************************************************/

BorderColorManipulator::BorderColorManipulator()
  : Manipulator()
{
}

bool BorderColorManipulator::process( Cell* cell )
{
  const int row = cell->row();
  const int col = cell->column();

  processHelper( cell->style( col, row ), col, row );
  return true;
}

bool BorderColorManipulator::process( const Style& style )
{
#ifndef KSPREAD_NEW_STYLE_STORAGE
  if ( dynamic_cast<ColumnFormat*>(format) )
  {
    ColumnFormat* const columnFormat = dynamic_cast<ColumnFormat*>(format);
    const int col = columnFormat->column();
    processHelper( columnFormat, col, 0 );
  }
  else
  {
    RowFormat* const rowFormat = dynamic_cast<RowFormat*>(format);
    const int row = rowFormat->row();
    processHelper( rowFormat, 0, row );
  }
#endif
  return true;
}

void BorderColorManipulator::processHelper( const Style& style, int col, int row )
{
#ifndef KSPREAD_NEW_STYLE_STORAGE
  if ( !m_reverse )
  {
    if ( m_firstrun )
    {
      if ( format->hasProperty( Style::STopBorder ) )
        m_undoData[col][row][Style::STopBorder] = format->topBorderColor( col, row );
      if ( format->hasProperty( Style::SLeftBorder ) )
        m_undoData[col][row][Style::SLeftBorder] = format->leftBorderColor( col, row );
      if ( format->hasProperty( Style::SFallDiagonal ) )
        m_undoData[col][row][Style::SFallDiagonal] = format->fallDiagonalColor( col, row );
      if ( format->hasProperty( Style::SGoUpDiagonal ) )
        m_undoData[col][row][Style::SGoUpDiagonal] = format->goUpDiagonalColor( col, row );
      if ( format->hasProperty( Style::SBottomBorder ) )
        m_undoData[col][row][Style::SBottomBorder] = format->bottomBorderColor( col, row );
      if ( format->hasProperty( Style::SRightBorder ) )
        m_undoData[col][row][Style::SRightBorder] = format->rightBorderColor( col, row );
    }

    if ( format->hasProperty( Style::STopBorder ) )
      format->setTopBorderColor( m_color );
    if ( format->hasProperty( Style::SLeftBorder ) )
      format->setLeftBorderColor( m_color );
    if ( format->hasProperty( Style::SFallDiagonal ) )
      format->setFallDiagonalColor( m_color );
    if ( format->hasProperty( Style::SGoUpDiagonal ) )
      format->setGoUpDiagonalColor( m_color );
    if ( format->hasProperty( Style::SBottomBorder ) )
      format->setBottomBorderColor( m_color );
    if ( format->hasProperty( Style::SRightBorder ) )
      format->setRightBorderColor( m_color );
  }
  else
  {
    if ( format->hasProperty( Style::STopBorder ) )
      format->setTopBorderColor( m_undoData[col][row][Style::STopBorder] );
    if ( format->hasProperty( Style::SLeftBorder ) )
      format->setLeftBorderColor( m_undoData[col][row][Style::SLeftBorder] );
    if ( format->hasProperty( Style::SFallDiagonal ) )
      format->setFallDiagonalColor( m_undoData[col][row][Style::SFallDiagonal] );
    if ( format->hasProperty( Style::SGoUpDiagonal ) )
      format->setGoUpDiagonalColor( m_undoData[col][row][Style::SGoUpDiagonal] );
    if ( format->hasProperty( Style::SBottomBorder ) )
      format->setBottomBorderColor( m_undoData[col][row][Style::SBottomBorder] );
    if ( format->hasProperty( Style::SRightBorder ) )
      format->setRightBorderColor( m_undoData[col][row][Style::SRightBorder] );
  }
#endif
}



/***************************************************************************
  class IncreasePrecisionManipulator
****************************************************************************/

IncreasePrecisionManipulator::IncreasePrecisionManipulator()
  : Manipulator()
{
  m_format = false;
}

bool IncreasePrecisionManipulator::process( Cell* cell )
{
  if ( !m_reverse )
  {
    cell->incPrecision();
  }
  else // m_reverse
  {
    cell->decPrecision();
  }
  return true;
}

QString IncreasePrecisionManipulator::name() const
{
  if ( !m_reverse )
  {
    return i18n( "Increase Precision" );
  }
  else
  {
    return i18n( "Decrease Precision" );
  }
}
