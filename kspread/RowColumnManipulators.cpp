/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "RowColumnManipulators.h"

#include <float.h>

#include "CellStorage.h"
#include "CellView.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "SheetView.h"
#include "StyleStorage.h"
#include "Value.h"

using namespace KSpread;

/***************************************************************************
  class ResizeColumnManipulator
****************************************************************************/

ResizeColumnManipulator::ResizeColumnManipulator()
{
}

ResizeColumnManipulator::~ResizeColumnManipulator()
{
}

bool ResizeColumnManipulator::process(Element* element)
{
  QRect range = element->rect();
  for (int col = range.right(); col >= range.left(); --col)
  {
    ColumnFormat *format = m_sheet->nonDefaultColumnFormat( col );
    format->setWidth( qMax( 2.0, m_reverse ? m_oldSize : m_newSize ) );
  }
  return true;
}



/***************************************************************************
  class ResizeRowManipulator
****************************************************************************/

ResizeRowManipulator::ResizeRowManipulator()
{
}

ResizeRowManipulator::~ResizeRowManipulator()
{
}

bool ResizeRowManipulator::process(Element* element)
{
  QRect range = element->rect();
  for (int row = range.bottom(); row >= range.top(); --row)
  {
    RowFormat* rl = m_sheet->nonDefaultRowFormat( row );
    rl->setHeight( qMax( 2.0, m_reverse ? m_oldSize : m_newSize ) );
  }
  return true;
}


/***************************************************************************
  class HideShowManipulator
****************************************************************************/

HideShowManipulator::HideShowManipulator()
  : m_manipulateColumns(false),
    m_manipulateRows(false)
{
}

HideShowManipulator::~HideShowManipulator()
{
}

bool HideShowManipulator::process(Element* element)
{
  QRect range = element->rect();
  if (m_manipulateColumns)
  {
    for (int col = range.left(); col <= range.right(); ++col)
    {
      ColumnFormat* format = m_sheet->nonDefaultColumnFormat(col);
      format->setHidden(!m_reverse);
    }
  }
  if (m_manipulateRows)
  {
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
      RowFormat* format = m_sheet->nonDefaultRowFormat(row);
      format->setHidden(!m_reverse);
    }
  }
  return true;
}

bool HideShowManipulator::preProcessing()
{
  Region region;
  ConstIterator endOfList = cells().constEnd();
  for (ConstIterator it = cells().constBegin(); it != endOfList; ++it)
  {
    if (m_reverse)
    {
      QRect range = (*it)->rect();
      if (m_manipulateColumns)
      {
        if (range.left() > 1)
        {
          int col;
          for (col = 1; col < range.left(); ++col)
          {
            if ( !m_sheet->columnFormat( col )->hidden() )
              break;
          }
          if (col == range.left())
          {
            region.add(QRect(1, 1, range.left()-1, KS_rowMax));
          }
        }
        for (int col = range.left(); col <= range.right(); ++col)
        {
          if ( m_sheet->columnFormat( col )->hidden() )
          {
            region.add(QRect(col, 1, 1, KS_rowMax));
          }
        }
      }
      if (m_manipulateRows)
      {
        if (range.top() > 1)
        {
          int row;
          for (row = 1; row < range.top(); ++row)
          {
            const RowFormat* format = m_sheet->rowFormat(row);
            if (!format->hidden())
            {
              break;
            }
          }
          if (row == range.top())
          {
            region.add(QRect(1, 1, KS_colMax, range.top()-1));
          }
        }
        for (int row = range.top(); row <= range.bottom(); ++row)
        {
          const RowFormat* format = m_sheet->rowFormat(row);
          if (format->hidden())
          {
            region.add(QRect(1, row, KS_colMax, 1));
          }
        }
      }
    }

    if (((*it)->isRow() && m_manipulateColumns) ||
           ((*it)->isColumn() && m_manipulateRows))
    {
      /*      KMessageBox::error( this, i18n( "Area is too large." ) );*/
      return false;
    }
  }

  if (m_reverse)
  {
    clear();
    add(region);
  }

  return true;
}

bool HideShowManipulator::postProcessing()
{
  if (m_manipulateColumns)
  {
    m_sheet->emitHideColumn();
  }
  if (m_manipulateRows)
  {
    m_sheet->emitHideRow();
  }

  return true;
}

QString HideShowManipulator::name() const
{
  QString name;
  if (m_reverse)
  {
    name = "Show ";
  }
  else
  {
    name = "Hide ";
  }
  if (m_manipulateColumns)
  {
    name += "Columns";
  }
  if (m_manipulateColumns && m_manipulateRows)
  {
    name += '/';
  }
  if (m_manipulateRows)
  {
    name += "Rows";
  }
  return name;
}

/***************************************************************************
  class AdjustColumnRowManipulator
****************************************************************************/

AdjustColumnRowManipulator::AdjustColumnRowManipulator()
  : Manipulator(),
    m_adjustColumn(false),
    m_adjustRow(false)
{
}

AdjustColumnRowManipulator::~AdjustColumnRowManipulator()
{
}

bool AdjustColumnRowManipulator::process(Element* element)
{
  Sheet* sheet = m_sheet; // TODO Stefan: element->sheet();
  if (m_sheet && sheet != m_sheet)
  {
    return true;
  }

  QMap<int,double> heights;
  QMap<int,double> widths;
  if (m_reverse)
  {
    heights = m_oldHeights;
    widths = m_oldWidths;
  }
  else
  {
    heights = m_newHeights;
    widths = m_newWidths;
  }

  QRect range = element->rect();
  if (m_adjustColumn)
  {
    if (element->isRow())
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell cell = sheet->cellStorage()->firstInRow( row );
        while ( !cell.isNull() )
        {
          int col = cell.column();
          if ( !cell.isEmpty() && !cell.isPartOfMerged())
          {
            if (widths.contains(col) && widths[col] != -1.0)
            {
              ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
              if ( qAbs(format->width() - widths[col] ) > DBL_EPSILON )
              {
                format->setWidth( qMax( 2.0, widths[col] ) );
              }
            }
          }
          cell = sheet->cellStorage()->nextInRow(col, row);
        }
      }
    }
    else
    {
      for (int col = range.left(); col <= range.right(); ++col)
      {
        if (widths.contains(col) && widths[col] != -1.0)
        {
          ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
          if ( qAbs(format->width() - widths[col] ) > DBL_EPSILON )
          {
            format->setWidth( qMax( 2.0, widths[col] ) );
          }
        }
      }
    }
  }
  if (m_adjustRow)
  {
    if (element->isColumn())
    {
      for (int col = range.left(); col <= range.right(); ++col)
      {
        Cell cell = sheet->cellStorage()->firstInColumn( col );
        while ( !cell.isNull() )
        {
          int row = cell.row();
          if ( !cell.isEmpty() && !cell.isPartOfMerged())
          {
            if (heights.contains(row) && heights[row] != -1.0)
            {
              RowFormat* format = sheet->nonDefaultRowFormat(row);
              if ( qAbs(format->height() - heights[row] ) > DBL_EPSILON )
              {
                format->setHeight( qMax( 2.0, heights[row] ) );
              }
            }
          }
          cell = sheet->cellStorage()->nextInColumn( col, row );
        }
      }
    }
    else
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        if (heights.contains(row) && heights[row] != -1.0)
        {
          RowFormat* format = sheet->nonDefaultRowFormat(row);
          if ( qAbs(format->height() - heights[row] ) > DBL_EPSILON )
          {
            format->setHeight( qMax( 2.0, heights[row] ) );
          }
        }
      }
    }
  }
  return true;
}

bool AdjustColumnRowManipulator::preProcessing()
{
  if (m_reverse)
  {
  }
  else
  {
    if (!m_newHeights.isEmpty() || !m_newWidths.isEmpty())
    {
      return true;
    }
//     createUndo();

    ConstIterator endOfList(cells().end());
    for (ConstIterator it = cells().begin(); it != endOfList; ++it)
    {
      Element* element = *it;
      QRect range = element->rect();
      if (element->isColumn())
      {
        for (int col = range.left(); col <= range.right(); ++col)
        {
          Cell cell = m_sheet->cellStorage()->firstInColumn( col );
          while ( !cell.isNull() )
          {
            int row = cell.row();
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                m_oldWidths[col] = m_sheet->columnFormat( col )->width();
              }
              if (!cell.isDefault() && !cell.isEmpty() && !cell.isPartOfMerged())
              {
                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                const RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->height();
              }
              if (!cell.isEmpty() && !cell.isPartOfMerged())
              {
                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                         m_newHeights[row]);
              }
            }
            cell = m_sheet->cellStorage()->nextInColumn( col, row );
          }
        }
      }
      else if (element->isRow())
      {
        for (int row = range.top(); row <= range.bottom(); ++row)
        {
          Cell cell = m_sheet->cellStorage()->firstInRow( row );
          while ( !cell.isNull() )
          {
            int col = cell.column();
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                m_oldWidths[col] = m_sheet->columnFormat( col )->width();
              }
              if (!cell.isDefault() && !cell.isEmpty() && !cell.isPartOfMerged())
              {
                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                const RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->height();
              }
              if (!cell.isDefault() && !cell.isEmpty() && !cell.isPartOfMerged())
              {
                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                         m_newHeights[row]);
              }
            }
            cell = m_sheet->cellStorage()->nextInRow(col, row);
          }
        }
      }
      else
      {
        Cell cell;
        for (int col = range.left(); col <= range.right(); ++col)
        {
          for ( int row = range.top(); row <= range.bottom(); ++row )
          {
            cell = Cell( m_sheet,  col, row );
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                m_oldWidths[col] = m_sheet->columnFormat( col )->width();
              }
              if (!cell.isDefault() && !cell.isEmpty() && !cell.isPartOfMerged())
              {
                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                const RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->height();
              }
              if (!cell.isDefault() && !cell.isEmpty() && !cell.isPartOfMerged())
              {
                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                         m_newHeights[row]);
              }
            }
          }
        }
      }
    }
  }
  return true;
}

double AdjustColumnRowManipulator::adjustColumnHelper( const Cell& cell )
{
    Q_ASSERT(!cell.isDefault());
  double long_max = 0.0;
  SheetView sheetView( cell.sheet() );
  CellView cellView( &sheetView, cell.column(), cell.row() ); // FIXME
  cellView.calculateTextParameters( cell );
  if ( cellView.textWidth() > long_max )
  {
    double indent = 0.0;
    Style::HAlign alignment = cell.style().halign();
    if (alignment == Style::HAlignUndefined)
    {
      if (cell.value().isNumber() || cell.isDate() || cell.isTime())
      {
        alignment = Style::Right;
      }
      else
      {
        alignment = Style::Left;
      }
    }

    if (alignment == Style::Left)
    {
      indent = cell.style().indentation();
    }
    long_max = indent + cellView.textWidth()
        + cell.style().leftBorderPen().width()
        + cell.style().rightBorderPen().width();
  }

  // add 4 because long_max is the length of the text
  // but column has borders
  if ( long_max == 0.0 )
  {
    return -1.0;
  }
  else
  {
    return long_max + 4;
  }
}

double AdjustColumnRowManipulator::adjustRowHelper( const Cell& cell )
{
    Q_ASSERT(!cell.isDefault());
  double long_max = 0.0;

  SheetView sheetView( cell.sheet() );
  CellView cellView( &sheetView, cell.column(), cell.row() ); // FIXME
  cellView.calculateTextParameters( cell );
  if ( cellView.textHeight() > long_max )
  {
    long_max = cellView.textHeight()
        + cell.style().topBorderPen().width()
        + cell.style().bottomBorderPen().width();
  }

  //  add 1 because long_max is the height of the text
  //  but row has borders
  if ( long_max == 0.0 )
  {
    return -1.0;
  }
  else
  {
    return long_max + 1;
  }
}

QString AdjustColumnRowManipulator::name() const
{
  if (m_adjustColumn && m_adjustRow)
  {
    return i18n("Adjust Columns/Rows");
  }
  else if (m_adjustColumn)
  {
    return i18n("Adjust Columns");
  }
  else
  {
    return i18n("Adjust Rows");
  }
}

/***************************************************************************
  class InsertDeleteColumnManipulator
****************************************************************************/

InsertDeleteColumnManipulator::InsertDeleteColumnManipulator()
    : Manipulator()
    , m_mode( Insert )
{
}

void InsertDeleteColumnManipulator::setReverse( bool reverse )
{
    m_reverse = reverse;
    m_mode = reverse ? Delete : Insert;
}

bool InsertDeleteColumnManipulator::process( Element* element )
{
    const QRect range = element->rect();
    const int pos = range.left();
    const int num = range.width();
    if ( !m_reverse ) // insertion
    {
        // insert rows
        m_sheet->insertColumns( pos, num );
        QVector< QPair<QPoint,Formula> > undoFormulas = m_sheet->formulaStorage()->insertColumns(pos, num);
        QVector< QPair<QPoint,Value> > undoValues = m_sheet->valueStorage()->insertColumns(pos, num);
        QVector< QPair<QPoint,QString> > undoLinks = m_sheet->linkStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,bool> > undoFusion = m_sheet->fusionStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->insertColumns(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->insertColumns(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoFormulas = undoFormulas;
            m_undoValues = undoValues;
            m_undoLinks = undoLinks;
            m_undoFusion = undoFusion;
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo deletion
        if ( m_mode == Delete )
        {
            for ( int i = 0; i < m_undoFormulas.count(); ++i )
                m_sheet->formulaStorage()->insert( m_undoFormulas[i].first.x(), m_undoFormulas[i].first.y(), m_undoFormulas[i].second );
            for ( int i = 0; i < m_undoValues.count(); ++i )
                m_sheet->valueStorage()->insert( m_undoValues[i].first.x(), m_undoValues[i].first.y(), m_undoValues[i].second );
            for ( int i = 0; i < m_undoLinks.count(); ++i )
                m_sheet->linkStorage()->insert( m_undoLinks[i].first.x(), m_undoLinks[i].first.y(), m_undoLinks[i].second );
            for ( int i = 0; i < m_undoFusion.count(); ++i )
                m_sheet->fusionStorage()->insert( Region(m_undoFusion[i].first.toRect()), m_undoFusion[i].second );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    else // deletion
    {
        // delete rows
        m_sheet->removeColumns( pos, num );
        QVector< QPair<QPoint,Formula> > undoFormulas = m_sheet->formulaStorage()->removeColumns(pos, num);
        QVector< QPair<QPoint,Value> > undoValues = m_sheet->valueStorage()->removeColumns(pos, num);
        QVector< QPair<QPoint,QString> > undoLinks = m_sheet->linkStorage()->removeColumns(pos, num);
        QList< QPair<QRectF,bool> > undoFusion = m_sheet->fusionStorage()->removeColumns(pos, num);
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->removeColumns(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->removeColumns(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->removeColumns(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->removeColumns(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoFormulas = undoFormulas;
            m_undoValues = undoValues;
            m_undoLinks = undoLinks;
            m_undoFusion = undoFusion;
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo insertion
        if ( m_mode == Insert )
        {
            for ( int i = 0; i < m_undoFormulas.count(); ++i )
                m_sheet->formulaStorage()->insert( m_undoFormulas[i].first.x(), m_undoFormulas[i].first.y(), m_undoFormulas[i].second );
            for ( int i = 0; i < m_undoValues.count(); ++i )
                m_sheet->valueStorage()->insert( m_undoValues[i].first.x(), m_undoValues[i].first.y(), m_undoValues[i].second );
            for ( int i = 0; i < m_undoLinks.count(); ++i )
                m_sheet->linkStorage()->insert( m_undoLinks[i].first.x(), m_undoLinks[i].first.y(), m_undoLinks[i].second );
            for ( int i = 0; i < m_undoFusion.count(); ++i )
                m_sheet->fusionStorage()->insert( Region(m_undoFusion[i].first.toRect()), m_undoFusion[i].second );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    return true;
}

bool InsertDeleteColumnManipulator::postProcessing()
{
    m_sheet->refreshMergedCell();
    m_sheet->recalc();
    return true;
}

QString InsertDeleteColumnManipulator::name() const
{
    if ( !m_reverse )
        return i18n( "Insert Columns" );
    else
        return i18n( "Remove Columns" );
}

/***************************************************************************
  class InsertDeleteRowManipulator
****************************************************************************/

InsertDeleteRowManipulator::InsertDeleteRowManipulator()
    : Manipulator()
    , m_mode( Insert )
{
}

void InsertDeleteRowManipulator::setReverse( bool reverse )
{
    m_reverse = reverse;
    m_mode = reverse ? Delete : Insert;
}

bool InsertDeleteRowManipulator::process( Element* element )
{
    const QRect range = element->rect();
    const int pos = range.top();
    const int num = range.height();
    if ( !m_reverse ) // insertion
    {
        // insert rows
        m_sheet->insertRows( pos, num );
        QVector< QPair<QPoint,Formula> > undoFormulas = m_sheet->formulaStorage()->insertRows(pos, num);
        QVector< QPair<QPoint,Value> > undoValues = m_sheet->valueStorage()->insertRows(pos, num);
        QVector< QPair<QPoint,QString> > undoLinks = m_sheet->linkStorage()->insertRows(pos, num);
        QList< QPair<QRectF,bool> > undoFusion = m_sheet->fusionStorage()->insertRows(pos, num);
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->insertRows(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->insertRows(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->insertRows(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->insertRows(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoFormulas = undoFormulas;
            m_undoValues = undoValues;
            m_undoLinks = undoLinks;
            m_undoFusion = undoFusion;
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo deletion
        if ( m_mode == Delete )
        {
            for ( int i = 0; i < m_undoFormulas.count(); ++i )
                m_sheet->formulaStorage()->insert( m_undoFormulas[i].first.x(), m_undoFormulas[i].first.y(), m_undoFormulas[i].second );
            for ( int i = 0; i < m_undoValues.count(); ++i )
                m_sheet->valueStorage()->insert( m_undoValues[i].first.x(), m_undoValues[i].first.y(), m_undoValues[i].second );
            for ( int i = 0; i < m_undoLinks.count(); ++i )
                m_sheet->linkStorage()->insert( m_undoLinks[i].first.x(), m_undoLinks[i].first.y(), m_undoLinks[i].second );
            for ( int i = 0; i < m_undoFusion.count(); ++i )
                m_sheet->fusionStorage()->insert( Region(m_undoFusion[i].first.toRect()), m_undoFusion[i].second );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    else // deletion
    {
        // delete rows
        m_sheet->removeRows( pos, num );
        QVector< QPair<QPoint,Formula> > undoFormulas = m_sheet->formulaStorage()->removeRows(pos, num);
        QVector< QPair<QPoint,Value> > undoValues = m_sheet->valueStorage()->removeRows(pos, num);
        QVector< QPair<QPoint,QString> > undoLinks = m_sheet->linkStorage()->removeRows(pos, num);
        QList< QPair<QRectF,bool> > undoFusion = m_sheet->fusionStorage()->removeRows(pos, num);
        QList< QPair<QRectF,SharedSubStyle> > undoStyles = m_sheet->styleStorage()->removeRows(pos, num);
        QList< QPair<QRectF,QString> > undoComment = m_sheet->commentStorage()->removeRows(pos, num);
        QList< QPair<QRectF,Conditions> > undoConditions = m_sheet->conditionsStorage()->removeRows(pos, num);
        QList< QPair<QRectF,Validity> > undoValidity = m_sheet->validityStorage()->removeRows(pos, num);

        // create undo for styles, comments, conditions, validity
        if ( m_firstrun )
        {
            m_undoFormulas = undoFormulas;
            m_undoValues = undoValues;
            m_undoLinks = undoLinks;
            m_undoFusion = undoFusion;
            m_undoStyles = undoStyles;
            m_undoComment = undoComment;
            m_undoConditions = undoConditions;
            m_undoValidity = undoValidity;
        }

        // undo insertion
        if ( m_mode == Insert )
        {
            for ( int i = 0; i < m_undoFormulas.count(); ++i )
                m_sheet->formulaStorage()->insert( m_undoFormulas[i].first.x(), m_undoFormulas[i].first.y(), m_undoFormulas[i].second );
            for ( int i = 0; i < m_undoValues.count(); ++i )
                m_sheet->valueStorage()->insert( m_undoValues[i].first.x(), m_undoValues[i].first.y(), m_undoValues[i].second );
            for ( int i = 0; i < m_undoLinks.count(); ++i )
                m_sheet->linkStorage()->insert( m_undoLinks[i].first.x(), m_undoLinks[i].first.y(), m_undoLinks[i].second );
            for ( int i = 0; i < m_undoFusion.count(); ++i )
                m_sheet->fusionStorage()->insert( Region(m_undoFusion[i].first.toRect()), m_undoFusion[i].second );
            for ( int i = 0; i < m_undoStyles.count(); ++i )
                m_sheet->styleStorage()->insert( m_undoStyles[i].first.toRect(), m_undoStyles[i].second );
            for ( int i = 0; i < m_undoComment.count(); ++i )
                m_sheet->commentStorage()->insert( Region(m_undoComment[i].first.toRect()), m_undoComment[i].second );
            for ( int i = 0; i < m_undoConditions.count(); ++i )
                m_sheet->conditionsStorage()->insert( Region(m_undoConditions[i].first.toRect()), m_undoConditions[i].second );
            for ( int i = 0; i < m_undoValidity.count(); ++i )
                m_sheet->validityStorage()->insert( Region(m_undoValidity[i].first.toRect()), m_undoValidity[i].second );
        }
    }
    return true;
}

bool InsertDeleteRowManipulator::postProcessing()
{
    m_sheet->refreshMergedCell();
    m_sheet->recalc();
    return true;
}

QString InsertDeleteRowManipulator::name() const
{
    if ( !m_reverse )
        return i18n( "Insert Rows" );
    else
        return i18n( "Remove Rows" );
}
