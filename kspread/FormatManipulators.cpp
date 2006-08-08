/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

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

#include "FormatManipulators.h"

using namespace KSpread;

/***************************************************************************
  class FormatManipulator
****************************************************************************/

FormatManipulator::FormatManipulator()
  : m_properties( 0 )
  , m_size( 0 )
  , m_bold( -1 )
  , m_italic( -1 )
  , m_strike( -1 )
  , m_underline( -1 )
  , m_angle( 0 )
  , m_precision( 0 )
  , m_currencyType( 0 )
  , m_indent( 0.0 )
  , m_multiRow( false )
  , m_verticalText( false )
  , m_dontPrintText( false )
  , m_notProtected( false )
  , m_hideAll( false )
  , m_hideFormula( false )
  , m_topBorderPen( QPen(QColor(), 0, Qt::NoPen) )
  , m_bottomBorderPen( QPen(QColor(), 0, Qt::NoPen) )
  , m_leftBorderPen( QPen(QColor(), 0, Qt::NoPen) )
  , m_rightBorderPen( QPen(QColor(), 0, Qt::NoPen) )
  , m_horizontalPen( QPen(QColor(), 0, Qt::NoPen) )
  , m_verticalPen( QPen(QColor(), 0, Qt::NoPen) )
  , m_fallDiagonalPen( QPen(QColor(), 0, Qt::NoPen) )
  , m_goUpDiagonalPen( QPen(QColor(), 0, Qt::NoPen) )
{
}

FormatManipulator::~FormatManipulator()
{
  QLinkedList<layoutCell>::Iterator it2;
  for ( it2 = m_lstFormats.begin(); it2 != m_lstFormats.end(); ++it2 )
  {
    delete (*it2).l;
  }
  m_lstFormats.clear();

  for ( it2 = m_lstRedoFormats.begin(); it2 != m_lstRedoFormats.end(); ++it2 )
  {
    delete (*it2).l;
  }
  m_lstRedoFormats.clear();

  QLinkedList<layoutColumn>::Iterator it3;
  for ( it3 = m_lstColFormats.begin(); it3 != m_lstColFormats.end(); ++it3 )
  {
    delete (*it3).l;
  }
  m_lstColFormats.clear();

  for ( it3 = m_lstRedoColFormats.begin(); it3 != m_lstRedoColFormats.end(); ++it3 )
  {
    delete (*it3).l;
  }
  m_lstRedoColFormats.clear();

  QLinkedList<layoutRow>::Iterator it4;
  for ( it4 = m_lstRowFormats.begin(); it4 != m_lstRowFormats.end(); ++it4 )
  {
    delete (*it4).l;
  }
  m_lstRowFormats.clear();

  for ( it4 = m_lstRedoRowFormats.begin(); it4 != m_lstRedoRowFormats.end(); ++it4 )
  {
    delete (*it4).l;
  }
  m_lstRedoRowFormats.clear();
}

bool FormatManipulator::preProcessing ()
{
  if (m_reverse)
    copyFormat (m_lstRedoFormats, m_lstRedoColFormats, m_lstRedoRowFormats);
  else
    copyFormat (m_lstFormats, m_lstColFormats, m_lstRowFormats);
  return true;
}

bool FormatManipulator::process (Element *element)
{
  // see what is selected; if nothing, take marker position
  QRect range = element->rect();

  if (!m_reverse) {

    int top = range.top();
    int left = range.left();
    int bottom = range.bottom();
    int right  = range.right();

    // create cells in rows if complete columns selected
    // TODO: Tomas says: this sounds like a bad idea
    Cell * cell;
    if ( element->isColumn() )
    {
      for ( RowFormat * row = m_sheet->firstRow(); row; row = row->next() )
      {
        if ( !row->isDefault() )
        {
          for ( int col = left; col <= right; ++col )
          {
            cell = m_sheet->nonDefaultCell( col, row->row() );
          }
        }
      }
    }

    // complete rows selected ?
    if ( element->isRow() )
    {
      for ( int row = top; row <= bottom; ++row )
      {
        cell = m_sheet->getFirstCellRow( row );
        while ( cell )
        {
          prepareCell( cell );
          cell = m_sheet->getNextCellRight( cell->column(), row );
        }
        RowFormat * rowFormat = m_sheet->nonDefaultRowFormat(row);
        doWork(rowFormat, row==top, row==bottom, false, false);
      }
    }
    // complete columns selected ?
    else if ( element->isColumn() )
    {
      for ( int col = left; col <= right; ++col )
      {
        cell = m_sheet->getFirstCellColumn( col );
        while ( cell )
        {
          prepareCell( cell );
          cell = m_sheet->getNextCellDown( col, cell->row() );
        }
        ColumnFormat * colFormat = m_sheet->nonDefaultColumnFormat( col );
        doWork(colFormat, false, false, col==left, col==right);
      }

      for ( RowFormat * rowFormat = m_sheet->firstRow(); rowFormat; rowFormat = rowFormat->next() )
      {
        if ( !rowFormat->isDefault() && testCondition( rowFormat ) )
        {
          for ( int col = left; col <= right; ++col )
          {
            cell = m_sheet->nonDefaultCell(col, rowFormat->row() );
            doWork(cell->format(), false, false, col==left, col==right );
          }
        }
      }
    }
    // cell region selected
    else
    {
      for ( int col = left; col <= right; ++col )
      {
        for ( int row = top; row <= bottom; ++row )
        {
          cell = m_sheet->nonDefaultCell(col,row);
          if ( !cell->isPartOfMerged() )
          {
            doWork(cell->format(), row==top, row==bottom, col==left, col==right);
          }
        }
      }
    }
  }
  else
  {  // undoing
    if( element->isColumn() )
    {
      QLinkedList<layoutColumn>::Iterator it2;
      for ( it2 = m_lstColFormats.begin(); it2 != m_lstColFormats.end(); ++it2 )
      {
        ColumnFormat * col = m_sheet->nonDefaultColumnFormat( (*it2).col );
        col->copy( *(*it2).l );
      }
    }
    else if( element->isRow() )
    {
      QLinkedList<layoutRow>::Iterator it2;
      for ( it2 = m_lstRowFormats.begin(); it2 != m_lstRowFormats.end(); ++it2 )
      {
        RowFormat * row = m_sheet->nonDefaultRowFormat( (*it2).row );
        row->copy( *(*it2).l );
      }
    }

    QLinkedList<layoutCell>::Iterator it2;
    for ( it2 = m_lstFormats.begin(); it2 != m_lstFormats.end(); ++it2 )
    {
      Cell *cell = m_sheet->nonDefaultCell( (*it2).col,(*it2).row );
      cell->format()->copy( *(*it2).l );
      cell->setLayoutDirtyFlag();
    }
    m_sheet->setRegionPaintDirty( *this );
  }
  return true;
}

void FormatManipulator::copyFormat(QLinkedList<layoutCell> & list,
                                   QLinkedList<layoutColumn> & listCol,
                                   QLinkedList<layoutRow> & listRow)
{
  QLinkedList<layoutCell>::Iterator end = list.end();
  for (QLinkedList<layoutCell>::Iterator it2 = list.begin(); it2 != end; ++it2)
  {
    delete (*it2).l;
  }
  list.clear();

  Cell * cell;
  Region::ConstIterator endOfList(cells().constEnd());
  for (Region::ConstIterator it = cells().constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect();
    int bottom = range.bottom();
    int right  = range.right();

    if ( (*it)->isColumn() )
    {
      for ( int col = range.left(); col <= right; ++col )
      {
        layoutColumn tmplayout;
        tmplayout.col = col;
        tmplayout.l = new ColumnFormat( m_sheet, col );
        tmplayout.l->copy( *(m_sheet->columnFormat( col )) );
        listCol.append(tmplayout);

        cell = m_sheet->getFirstCellColumn( col );
        while ( cell )
        {
          if ( cell->isPartOfMerged() )
          {
            cell = m_sheet->getNextCellDown( col, cell->row() );
            continue;
          }

          layoutCell tmplayout;
          tmplayout.col = col;
          tmplayout.row = cell->row();
          tmplayout.l = new Format( m_sheet, 0 );
          tmplayout.l->copy( *(m_sheet->cellAt( tmplayout.col, tmplayout.row )->format()) );
          list.append(tmplayout);

          cell = m_sheet->getNextCellDown( col, cell->row() );
        }
      }
      /*
      Cell * cell = m_sheet->firstCell();
      for( ; cell; cell = cell->nextCell() )
      {
      int col = cell->column();
      if ( range.left() <= col && right >= col
      && !cell->isPartOfMerged())
      {
      layoutCell tmplayout;
      tmplayout.col = cell->column();
      tmplayout.row = cell->row();
      tmplayout.l = new Format( m_sheet, 0 );
      tmplayout.l->copy( *(m_sheet->cellAt( tmplayout.col, tmplayout.row )) );
      list.append(tmplayout);
    }
    }
      */
    }
    else if ((*it)->isRow())
    {
      for ( int row = range.top(); row <= bottom; ++row )
      {
        layoutRow tmplayout;
        tmplayout.row = row;
        tmplayout.l = new RowFormat( m_sheet, row );
        tmplayout.l->copy( *(m_sheet->rowFormat( row )) );
        listRow.append(tmplayout);

        cell = m_sheet->getFirstCellRow( row );
        while ( cell )
        {
          if ( cell->isPartOfMerged() )
          {
            cell = m_sheet->getNextCellRight( cell->column(), row );
            continue;
          }
          layoutCell tmplayout;
          tmplayout.col = cell->column();
          tmplayout.row = row;
          tmplayout.l = new Format( m_sheet, 0 );
          tmplayout.l->copy( *(m_sheet->cellAt( cell->column(), row )->format()) );
          list.append(tmplayout);

          cell = m_sheet->getNextCellRight( cell->column(), row );
        }
      }
      /*
      Cell * cell = m_sheet->firstCell();
      for( ; cell; cell = cell->nextCell() )
      {
      int row = cell->row();
      if ( range.top() <= row && bottom >= row
      && !cell->isPartOfMerged())
      {
      layoutCell tmplayout;
      tmplayout.col = cell->column();
      tmplayout.row = cell->row();
      tmplayout.l = new Format( m_sheet, 0 );
      tmplayout.l->copy( *(m_sheet->cellAt( tmplayout.col, tmplayout.row )) );
      list.append(tmplayout);
    }
    }
      */
    }
    else
    {
      for ( int row = range.top(); row <= bottom; ++row )
        for ( int col = range.left(); col <= right; ++col )
      {
        Cell * cell = m_sheet->nonDefaultCell( col, row );
        if ( !cell->isPartOfMerged() )
        {
          layoutCell tmplayout;
          tmplayout.col = col;
          tmplayout.row = row;
          tmplayout.l = new Format( m_sheet, 0 );
          tmplayout.l->copy( *(m_sheet->cellAt( col, row )->format()) );
          list.append(tmplayout);
        }
      }
    }
  }
}

bool FormatManipulator::testCondition(RowFormat* row)
{
  for (quint32 property = Style::SHAlign;
       property <= Style::SHideFormula;
       property *= 2)
  {
    if (m_properties & property)
    {
      return ( row->hasProperty((Style::FlagsSet) property) );
    }
  }
  return false;
}

void FormatManipulator::doWork(Format* format,
                               bool isTop, bool isBottom,
                               bool isLeft, bool isRight)
{
  // SetSelectionFontWorker
  // SetSelectionSizeWorker
  if (m_properties & Style::SFontFamily)
  {
    if ( !m_font.isEmpty() )
      format->setTextFontFamily( m_font );
  }
  if (m_properties & Style::SFontSize)
  {
    if ( m_size > 0 )
      format->setTextFontSize( m_size );
  }
  if (m_properties & Style::SFontFlag)
  {
    if ( m_italic >= 0 )
      format->setTextFontItalic( (bool)m_italic );
    if ( m_bold >= 0 )
      format->setTextFontBold( (bool)m_bold );
    if ( m_underline >= 0 )
      format->setTextFontUnderline( (bool)m_underline );
    if ( m_strike >= 0 )
      format->setTextFontStrike( (bool)m_strike );
  }
  // SetSelectionAngleWorker
  if (m_properties & Style::SAngle)
  {
    format->setAngle( m_angle );
  }
  // SetSelectionTextColorWorker
  if (m_properties & Style::STextPen)
  {
    format->setTextColor( m_textColor );
  }
  // SetSelectionBgColorWorker
  if (m_properties & Style::SBackgroundColor)
  {
    format->setBgColor( m_backgroundColor );
  }
  // SetSelectionBorderAllWorker
  if (m_properties & Style::SLeftBorder)
  {
    if (isLeft)
    {
      if (m_leftBorderPen.color().isValid())
      {
        format->setLeftBorderPen(m_leftBorderPen);
      }
    }
    else
    {
      if (m_verticalPen.color().isValid())
      {
        format->setLeftBorderPen(m_verticalPen);
      }
    }
  }
  if (m_properties & Style::SRightBorder)
  {
    if (isRight)
    {
      if (m_rightBorderPen.color().isValid())
      {
        format->setRightBorderPen(m_rightBorderPen);
      }
    }
    else
    {
      if (m_verticalPen.color().isValid())
      {
        format->setRightBorderPen(m_verticalPen);
      }
    }
  }
  if (m_properties & Style::STopBorder)
  {
    if (isTop)
    {
      if (m_topBorderPen.color().isValid())
      {
        format->setTopBorderPen(m_topBorderPen);
      }
    }
    else
    {
      if (m_horizontalPen.color().isValid())
      {
        format->setTopBorderPen(m_horizontalPen);
      }
    }
  }
  if (m_properties & Style::SBottomBorder)
  {
    if (isBottom)
    {
      if (m_bottomBorderPen.color().isValid())
      {
        format->setBottomBorderPen(m_bottomBorderPen);
      }
    }
    else
    {
      if (m_horizontalPen.color().isValid())
      {
        format->setBottomBorderPen(m_horizontalPen);
      }
    }
  }
  if (m_properties & Style::SFallDiagonal)
  {
    format->setFallDiagonalPen(m_fallDiagonalPen);
  }
  if (m_properties & Style::SGoUpDiagonal)
  {
    format->setGoUpDiagonalPen(m_goUpDiagonalPen);
  }
  // SetSelectionAlignWorker
  if (m_properties & Style::SHAlign)
  {
    format->setAlign( m_horAlign );
  }
  // SetSelectionAlignYWorker
  if (m_properties & Style::SVAlign)
  {
    format->setAlignY( m_verAlign );
  }
  if (m_properties & Style::SPrefix)
  {
    format->setPrefix(m_prefix);
  }
  if (m_properties & Style::SPostfix)
  {
    format->setPostfix(m_postfix);
  }
  if (m_properties & Style::SBackgroundBrush)
  {
    format->setBackGroundBrush(m_backgroundBrush);
  }
  if (m_properties & Style::SFloatFormat)
  {
    format->setFloatFormat(m_floatFormat);
  }
  if (m_properties & Style::SFloatColor)
  {
    format->setFloatColor(m_floatColor);
  }
  if (m_properties & Style::SMultiRow)
  {
    format->setMultiRow(m_multiRow);
  }
  if (m_properties & Style::SVerticalText)
  {
    format->setVerticalText(m_verticalText);
  }
  if (m_properties & Style::SPrecision)
  {
    format->setPrecision(m_precision);
  }
  if (m_properties & Style::SFormatType)
  {
    format->setFormatType(m_formatType);
    if (m_formatType == Money_format)
    {
      format->setCurrency(m_currencyType, m_currencySymbol);
    }
    else if (m_formatType == Percentage_format)
    {
      format->setFormatType(Percentage_format);
    }
  }
  if (m_properties & Style::SComment)
  {
    format->setComment(m_comment);
  }
  if (m_properties & Style::SIndent)
  {
    format->setIndent(m_indent);
  }
  if (m_properties & Style::SDontPrintText)
  {
    format->setDontPrintText(m_dontPrintText);
  }
  if (m_properties & Style::SCustomFormat)
  {
    //TODO
  }
  if (m_properties & Style::SNotProtected)
  {
    format->setNotProtected(m_notProtected);
  }
  if (m_properties & Style::SHideAll)
  {
    format->setHideAll(m_hideAll);
  }
  if (m_properties & Style::SHideFormula)
  {
    format->setHideFormula(m_hideFormula);
  }
}

void FormatManipulator::prepareCell(Cell* cell)
{
  for (quint32 property = Style::SHAlign;
       property <= Style::SHideFormula;
       property *= 2)
  {
    if (m_properties & property)
    {
      cell->format()->clearProperty((Style::FlagsSet) property);
      cell->format()->clearNoFallBackProperties((Style::FlagsSet) property);
    }
  }
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
  if ( !m_reverse )
  {
    cell->format()->setIndent( cell->format()->getIndent(cell->column(), cell->row()) + m_sheet->doc()->indentValue() );
  }
  else // m_reverse
  {
    cell->format()->setIndent( cell->format()->getIndent(cell->column(), cell->row()) - m_sheet->doc()->indentValue() );
  }
  return true;
}

bool IncreaseIndentManipulator::process( Format* format )
{
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

  processHelper( cell->format(), col, row );
  return true;
}

bool BorderColorManipulator::process( Format* format )
{
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
  return true;
}

void BorderColorManipulator::processHelper( Format* const format, int col, int row )
{
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



/***************************************************************************
  class StyleApplicator
****************************************************************************/

StyleApplicator::StyleApplicator()
  : Manipulator()
  , m_style(0)
{
}

bool StyleApplicator::process( Cell* cell )
{
  if ( !m_reverse )
  {
    if ( m_firstrun )
    {
      m_undoData[cell->column()][cell->row()] = cell->format()->style();
      m_undoData[cell->column()][cell->row()]->addRef();
    }

    if ( m_style )
    {
      kDebug() << "Aplying style at " << cell->name() << endl;
      cell->format()->setStyle( m_style );
    }
    else
    {
      kDebug() << "Aplying default style at " << cell->name() << endl;
      cell->defaultStyle();
    }
  }
  else // m_reverse
  {
    cell->format()->setStyle( m_undoData[cell->column()][cell->row()] );
    m_undoData[cell->column()][cell->row()]->release();
  }
  return true;
}

bool StyleApplicator::process( Format* format )
{
  if ( !m_reverse )
  {
    if ( m_firstrun )
    {
      if ( dynamic_cast<ColumnFormat*>(format) )
      {
        const ColumnFormat* columnFormat = dynamic_cast<ColumnFormat*>(format);
        m_undoData[columnFormat->column()][0] = columnFormat->style();
        m_undoData[columnFormat->column()][0]->addRef();
      }
      else
      {
        const RowFormat* rowFormat = dynamic_cast<RowFormat*>(format);
        m_undoData[0][rowFormat->row()] = rowFormat->style();
        m_undoData[0][rowFormat->row()]->addRef();
      }
    }

    if ( m_style )
    {
     format->setStyle( m_style );
    }
    else
    {
      format->defaultStyleFormat();
    }
  }
  else // m_reverse
  {
    if ( dynamic_cast<ColumnFormat*>(format) )
    {
      ColumnFormat* const columnFormat = dynamic_cast<ColumnFormat*>(format);
      columnFormat->setStyle( m_undoData[columnFormat->column()][0] );
      m_undoData[columnFormat->column()][0]->release();
    }
    else
    {
      RowFormat* const rowFormat = dynamic_cast<RowFormat*>(format);
      rowFormat->setStyle( m_undoData[0][rowFormat->row()] );
      m_undoData[0][rowFormat->row()]->release();
    }
  }
  return true;
}

QString StyleApplicator::name() const
{
  if ( m_style )
  {
    return i18n( "Apply Style" );
  }
  else
  {
    return i18n( "Apply Default Style" );
  }
}
