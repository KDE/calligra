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

#include <float.h>

#include <qcolor.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstaticdeleter.h>

#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_view.h"

#include "manipulator.h"

using namespace KSpread;

// TODO Stefan: SubTotal	(neither NCS nor undo)
// TODO Stefan: Conditional	(neither NCS nor undo)
// TODO Stefan: Validity	(undo doesn't work)

// TODO Stefan: AutoSum ?
// TODO Stefan: FillSelection ?
// TODO Stefan: Default ?

// TODO Stefan: Copy
// TODO Stefan: Cut
// TODO Stefan: Delete
// TODO Stefan: Paste (Special)

// TODO Stefan: SortInc
// TODO Stefan: SortDec

// TODO Stefan: DeleteColumn
// TODO Stefan: DeleteRow
// TODO Stefan: InsertColumn
// TODO Stefan: InsertRow

// TODO Stefan: RemoveComment	(works, but not a manipulator yet)
// TODO Stefan: ClearText	(works, but not a manipulator yet)
// TODO Stefan: ClearValidity	(works, but not a manipulator yet)

// TODO Stefan: more ????

// TODO Stefan: maybe make the undos optional

/***************************************************************************
  class Manipulator
****************************************************************************/

Manipulator::Manipulator()
  : Region(),
    KCommand(),
    m_sheet(0),
    m_creation(true),
    m_reverse(false)
{
}

Manipulator::~Manipulator()
{
}

void Manipulator::execute()
{
  if (!m_sheet)
  {
    kdWarning() << "Manipulator::execute(): No explicit m_sheet is set. "
                << "Manipulating all sheets of the region." << endl;
  }

  m_sheet->doc()->emitBeginOperation();

  bool successfully = true;
  successfully = preProcessing();
  if (!successfully)
  {
    kdWarning() << "Manipulator::execute(): preprocessing was not successful!" << endl;
  }

  successfully = true;
  Region::Iterator endOfList(cells().end());
  for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
  {
    successfully &= process(*it);
  }

  if (!successfully)
  {
    kdWarning() << "Manipulator::execute(): processing was not successful!" << endl;
  }

  successfully = true;
  postProcessing();
  if (!successfully)
  {
    kdWarning() << "Manipulator::execute(): postprocessing was not successful!" << endl;
  }

  m_sheet->doc()->emitEndOperation(*this);
}

void Manipulator::unexecute()
{
  m_reverse = !m_reverse;
  execute();
  m_reverse = !m_reverse;
}

bool Manipulator::process(Element* element)
{
  Sheet* sheet = m_sheet; // TODO Stefan: element->sheet();
  if (m_sheet && sheet != m_sheet)
  {
    return true;
  }

  QRect range = element->rect().normalize();
  if (element->isColumn())
  {
    for (int col = range.left(); col <= range.right(); ++col)
    {
      kdDebug() << "Processing column " << col << "." << endl;
      ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
      process(format);
        // TODO Stefan: process cells with this property
    }
  }
  else if (element->isRow())
  {
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
      kdDebug() << "Processing row " << row << "." << endl;
      RowFormat* format = sheet->nonDefaultRowFormat(row);
      process(format);
        // TODO Stefan: process cells with this property
    }
  }
  else
  {
    kdDebug() << "Processing cell(s) at " << range << "." << endl;
    for (int col = range.left(); col <= range.right(); ++col)
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell* cell = sheet->cellAt(col, row);
        if ( cell->isObscuringForced() )
        {
          cell = cell->obscuringCells().first();
        }

        //if (testCondition(cell))
        {
          if (cell == sheet->defaultCell() && m_creation)
          {
            Style* style = sheet->doc()->styleManager()->defaultStyle();
            cell = new Cell(sheet, style, col, row);
            sheet->insertCell(cell);
          }

          if (!process(cell))
          {
            return false;
          }
        }
      }
    }
  }
  return true;
}



/***************************************************************************
  class FormatManipulator
****************************************************************************/

FormatManipulator::FormatManipulator()
{
  m_properties = 0;
  // initialize pens with invalid color
  m_topBorderPen = QPen(QColor(), 0, Qt::NoPen);
  m_bottomBorderPen = QPen(QColor(), 0, Qt::NoPen);
  m_leftBorderPen = QPen(QColor(), 0, Qt::NoPen);
  m_rightBorderPen = QPen(QColor(), 0, Qt::NoPen);
  m_horizontalPen = QPen(QColor(), 0, Qt::NoPen);
  m_verticalPen = QPen(QColor(), 0, Qt::NoPen);
  m_fallDiagonalPen = QPen(QColor(), 0, Qt::NoPen);
  m_goUpDiagonalPen = QPen(QColor(), 0, Qt::NoPen);
}

FormatManipulator::~FormatManipulator()
{
  QValueList<layoutCell>::Iterator it2;
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

  QValueList<layoutColumn>::Iterator it3;
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

  QValueList<layoutRow>::Iterator it4;
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

void FormatManipulator::execute()
{
  m_sheet->doc()->undoLock();
  m_sheet->doc()->emitBeginOperation();
  copyFormat( m_lstFormats, m_lstColFormats, m_lstRowFormats );

  Region::Iterator endOfList(cells().end());
  for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
  {
    // see what is selected; if nothing, take marker position
    QRect range = (*it)->rect().normalize();

    int top = range.top();
    int left = range.left();
    int bottom = range.bottom();
    int right  = range.right();

    // create cells in rows if complete columns selected
    Cell * cell;
    if ( (*it)->isColumn() )
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
    if ( (*it)->isRow() )
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
    else if ( (*it)->isColumn() )
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
          if ( !cell->isObscuringForced() )
          {
            cell->setDisplayDirtyFlag();
            doWork(cell->format(), row==top, row==bottom, col==left, col==right);
            cell->clearDisplayDirtyFlag();
          }
        }
      }
    }
  } // for Region::Elements

  m_sheet->doc()->emitEndOperation(*this);
  m_sheet->doc()->undoUnlock();
}

void FormatManipulator::unexecute()
{
  m_sheet->doc()->undoLock();
  m_sheet->doc()->emitBeginOperation();
  copyFormat( m_lstRedoFormats, m_lstRedoColFormats, m_lstRedoRowFormats );
  Region::ConstIterator endOfList(cells().constEnd());
  for (Region::ConstIterator it = cells().constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalize();
    if( (*it)->isColumn() )
    {
      QValueList<layoutColumn>::Iterator it2;
      for ( it2 = m_lstColFormats.begin(); it2 != m_lstColFormats.end(); ++it2 )
      {
        ColumnFormat * col = m_sheet->nonDefaultColumnFormat( (*it2).col );
        col->copy( *(*it2).l );
      }
    }
    else if( (*it)->isRow() )
    {
      QValueList<layoutRow>::Iterator it2;
      for ( it2 = m_lstRowFormats.begin(); it2 != m_lstRowFormats.end(); ++it2 )
      {
        RowFormat * row = m_sheet->nonDefaultRowFormat( (*it2).row );
        row->copy( *(*it2).l );
      }
    }

    QValueList<layoutCell>::Iterator it2;
    for ( it2 = m_lstFormats.begin(); it2 != m_lstFormats.end(); ++it2 )
    {
      Cell *cell = m_sheet->nonDefaultCell( (*it2).col,(*it2).row );
      cell->format()->copy( *(*it2).l );
      cell->setLayoutDirtyFlag();
      cell->setDisplayDirtyFlag();
      m_sheet->updateCell( cell, (*it2).col, (*it2).row );
    }
  }

  m_sheet->doc()->emitEndOperation( *this );
  m_sheet->doc()->undoUnlock();
}

void FormatManipulator::copyFormat(QValueList<layoutCell> & list,
                                   QValueList<layoutColumn> & listCol,
                                   QValueList<layoutRow> & listRow)
{
  QValueList<layoutCell>::Iterator end = list.end();
  for (QValueList<layoutCell>::Iterator it2 = list.begin(); it2 != end; ++it2)
  {
      delete (*it2).l;
  }
  list.clear();

  Cell * cell;
  Region::ConstIterator endOfList(cells().constEnd());
  for (Region::ConstIterator it = cells().constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalize();
    int bottom = range.bottom();
    int right  = range.right();

    if ( (*it)->isColumn() )
    {
      /* Don't need to go through the loop twice...
        for (int i = range.left(); i <= right; ++i)
        {
        layoutColumn tmplayout;
        tmplayout.col = i;
        tmplayout.l = new ColumnFormat( m_sheet, i );
        tmplayout.l->copy( *(m_sheet->columnFormat( i )) );
        listCol.append(tmplayout);
        }
      */
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
          if ( cell->isObscuringForced() )
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
            && !cell->isObscuringForced())
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
          if ( cell->isObscuringForced() )
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
            && !cell->isObscuringForced())
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
          if ( !cell->isObscuringForced() )
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
  for (Q_UINT32 property = Format::PAlign;
       property <= Format::PHideFormula;
       property *= 2)
  {
    if (m_properties & property)
    {
      return ( row->hasProperty((Format::Properties) property) );
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
  if (m_properties & Format::PFont)
  {
    if ( !m_font.isEmpty() )
      format->setTextFontFamily( m_font );
    if ( m_size > 0 )
      format->setTextFontSize( m_size );
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
  if (m_properties & Format::PAngle)
  {
    format->setAngle( m_angle );
  }
  // SetSelectionTextColorWorker
  if (m_properties & Format::PTextPen)
  {
    format->setTextColor( m_textColor );
  }
  // SetSelectionBgColorWorker
  if (m_properties & Format::PBackgroundColor)
  {
    format->setBgColor( m_backgroundColor );
  }
  // SetSelectionBorderAllWorker
  if (m_properties & Format::PLeftBorder)
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
  if (m_properties & Format::PRightBorder)
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
  if (m_properties & Format::PTopBorder)
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
  if (m_properties & Format::PBottomBorder)
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
  if (m_properties & Format::PFallDiagonal)
  {
    format->setFallDiagonalPen(m_fallDiagonalPen);
  }
  if (m_properties & Format::PGoUpDiagonal)
  {
    format->setGoUpDiagonalPen(m_goUpDiagonalPen);
  }
  // SetSelectionAlignWorker
  if (m_properties & Format::PAlign)
  {
    format->setAlign( m_horAlign );
  }
  // SetSelectionAlignYWorker
  if (m_properties & Format::PAlignY)
  {
    format->setAlignY( m_verAlign );
  }
  if (m_properties & Format::PPrefix)
  {
    format->setPrefix(m_prefix);
  }
  if (m_properties & Format::PPostfix)
  {
    format->setPostfix(m_postfix);
  }
  if (m_properties & Format::PBackgroundBrush)
  {
    format->setBackGroundBrush(m_backgroundBrush);
  }
  if (m_properties & Format::PFloatFormat)
  {
    format->setFloatFormat(m_floatFormat);
  }
  if (m_properties & Format::PFloatColor)
  {
    format->setFloatColor(m_floatColor);
  }
  if (m_properties & Format::PMultiRow)
  {
    format->setMultiRow(m_multiRow);
  }
  if (m_properties & Format::PVerticalText)
  {
    format->setVerticalText(m_verticalText);
  }
  if (m_properties & Format::PPrecision)
  {
    format->setPrecision(m_precision);
  }
  if (m_properties & Format::PFormatType)
  {
    format->setFormatType(m_formatType);
  }
  if (m_properties & Format::PComment)
  {
    format->setComment(m_comment);
  }
  if (m_properties & Format::PIndent)
  {
    format->setIndent(m_indent);
  }
  if (m_properties & Format::PDontPrintText)
  {
    format->setDontPrintText(m_dontPrintText);
  }
  if (m_properties & Format::PCustomFormat)
  {
    //TODO
  }
  if (m_properties & Format::PNotProtected)
  {
    format->setNotProtected(m_notProtected);
  }
  if (m_properties & Format::PHideAll)
  {
    format->setHideAll(m_hideAll);
  }
  if (m_properties & Format::PHideFormula)
  {
    format->setHideFormula(m_hideFormula);
  }
}

void FormatManipulator::prepareCell(Cell* cell)
{
  for (Q_UINT32 property = Format::PAlign;
       property <= Format::PHideFormula;
       property *= 2)
  {
    if (m_properties & property)
    {
      cell->format()->clearProperty((Format::Properties) property);
      cell->format()->clearNoFallBackProperties((Format::Properties) property);
    }
  }
}



/***************************************************************************
  class MergeManipulator
****************************************************************************/

MergeManipulator::MergeManipulator()
  : Manipulator()
{
}

MergeManipulator::~MergeManipulator()
{
}

bool MergeManipulator::process(Element* element)
{
  if (element->type() != Element::Range || element->isRow() || element->isColumn())
  {
    // TODO Stefan: remove these elements?!
    return true;
  }

  // sanity check
  if( m_sheet->isProtected() || m_sheet->workbook()->isProtected() )
  {
    return false;
  }

  QRect range = element->rect().normalize();

  if (!m_reverse)
  {
    Cell *cell = m_sheet->nonDefaultCell( range.topLeft() );
    cell->forceExtraCells( range.topLeft().x(), range.topLeft().y(),
                          range.width() - 1, range.height() - 1);
  }
  else
  {
    QPoint marker(range.topLeft());
    Cell *cell = m_sheet->nonDefaultCell( marker );
    if(!cell->isForceExtraCells())
    {
      return true;
    }

    cell->forceExtraCells( marker.x() ,marker.y(), 0, 0 );
  }

  return true;
}

QString MergeManipulator::name() const
{
  if (Region::name().isEmpty())
  {
    return i18n("Merge Cells");
  }
  else
  {
    return i18n("Merge Cells %1").arg(Region::name());
  }
}

bool MergeManipulator::preProcessing()
{
  if ( isRowSelected() || isColumnSelected() )
  {
    KMessageBox::information( 0, i18n( "Merging of columns or rows is not supported." ) );
    return false;
  }
  return true;
}

bool MergeManipulator::postProcessing()
{
  if (!m_reverse)
  {
    if (m_sheet->getAutoCalc())
    {
      m_sheet->recalc();
    }
  }
  else
  {
    m_sheet->refreshMergedCell();
  }
  return true;
}



/***************************************************************************
  class DissociateManipulator
****************************************************************************/

DissociateManipulator::DissociateManipulator()
  : MergeManipulator()
{
  m_reverse = true;
}

DissociateManipulator::~DissociateManipulator()
{
}

QString DissociateManipulator::name() const
{
  if (Region::name().isEmpty())
  {
    return i18n("Dissociate Cells");
  }
  else
  {
    return i18n("Dissociate Cells %1").arg(Region::name());
  }
}

bool DissociateManipulator::preProcessing()
{
  if (!m_reverse)
  {
  }
  else
  {
    Region unmergedCells;
    ConstIterator endOfList(cells().end());
    for (ConstIterator it = cells().begin(); it != endOfList; ++it)
    {
      Element* element = *it;
      if (element->type() == Element::Point)
      {
        QRect range = element->rect().normalize();
        QPoint marker(range.topLeft());

        Cell *cell = m_sheet->nonDefaultCell( marker );
        QRect rect(marker.x(), marker.y(), cell->mergedXCells() + 1, cell->mergedYCells() + 1);
        unmergedCells.add(rect);
      }
    }
    add(unmergedCells);
  }
  return true;
}



/***************************************************************************
  class DilationManipulator
****************************************************************************/

DilationManipulator::DilationManipulator()
  : Manipulator()
{
}

DilationManipulator::~DilationManipulator()
{
}

void DilationManipulator::execute()
{
  Region extendedRegion;
  ConstIterator end(cells().constEnd());
  for (ConstIterator it = cells().constBegin(); it != end; ++it)
  {
    Element* element = *it;
  QRect area = element->rect().normalize();

  ColumnFormat *col;
  RowFormat *rl;
  //look at if column is hiding.
  //if it's hiding refreshing column+1 (or column -1 )
  int left = area.left();
  int right = area.right();
  int top = area.top();
  int bottom = area.bottom();

  if ( right < KS_colMax )
  {
    do
    {
      right++;
      col = m_sheet->nonDefaultColumnFormat( right );
    } while ( col->isHide() && right != KS_colMax );
  }
  if ( left > 1 )
  {
    do
    {
      left--;
      col = m_sheet->nonDefaultColumnFormat( left );
    } while ( col->isHide() && left != 1);
  }

  if ( bottom < KS_rowMax )
  {
    do
    {
      bottom++;
      rl = m_sheet->nonDefaultRowFormat( bottom );
    } while ( rl->isHide() && bottom != KS_rowMax );
  }

  if ( top > 1 )
  {
    do
    {
      top--;
      rl = m_sheet->nonDefaultRowFormat( top );
    } while ( rl->isHide() && top != 1);
  }

  area.setLeft(left);
  area.setRight(right);
  area.setTop(top);
  area.setBottom(bottom);

  extendedRegion.add(area, element->sheet());
  }
  clear();
  add(extendedRegion);
}

void DilationManipulator::unexecute()
{
  kdError() << "DilationManipulator::unexecute(): "
            << "An undo of dilating a region is not possible." << endl;
}



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
  QRect range = element->rect().normalize();
  for (int col = range.right(); col >= range.left(); --col)
  {
    ColumnFormat *format = m_sheet->nonDefaultColumnFormat( col );
    format->setDblWidth( QMAX( 2.0, m_reverse ? m_oldSize : m_newSize ) );
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
  QRect range = element->rect().normalize();
  for (int row = range.bottom(); row >= range.top(); --row)
  {
    RowFormat* rl = m_sheet->nonDefaultRowFormat( row );
    rl->setDblHeight( QMAX( 2.0, m_reverse ? m_oldSize : m_newSize ) );
  }
  return true;
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

  QRect range = element->rect().normalize();
  if (m_adjustColumn)
  {
    if (element->isRow())
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell* cell = sheet->getFirstCellRow( row );
        while ( cell )
        {
          int col = cell->column();
          if ( !cell->isEmpty() && !cell->isObscured())
          {
            if (widths.contains(col) && widths[col] != -1.0)
            {
              ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
              if ( kAbs(format->dblWidth() - widths[col] ) > DBL_EPSILON )
              {
                format->setDblWidth( QMAX( 2.0, widths[col] ) );
              }
            }
          }
          cell = sheet->getNextCellRight(col, row);
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
          if ( kAbs(format->dblWidth() - widths[col] ) > DBL_EPSILON )
          {
            format->setDblWidth( QMAX( 2.0, widths[col] ) );
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
        Cell* cell = sheet->getFirstCellColumn( col );
        while ( cell )
        {
          int row = cell->row();
          if ( !cell->isEmpty() && !cell->isObscured())
          {
            if (heights.contains(row) && heights[row] != -1.0)
            {
              RowFormat* format = sheet->nonDefaultRowFormat(row);
              if ( kAbs(format->dblHeight() - heights[row] ) > DBL_EPSILON )
              {
                format->setDblHeight( QMAX( 2.0, heights[row] ) );
              }
            }
          }
          cell = sheet->getNextCellDown( col, row );
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
          if ( kAbs(format->dblHeight() - heights[row] ) > DBL_EPSILON )
          {
            format->setDblHeight( QMAX( 2.0, heights[row] ) );
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
      QRect range = element->rect().normalize();
      if (element->isColumn())
      {
        for (int col = range.left(); col <= range.right(); ++col)
        {
          Cell* cell = m_sheet->getFirstCellColumn( col );
          while ( cell )
          {
            int row = cell->row();
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                ColumnFormat* format = m_sheet->columnFormat(col);
                m_oldWidths[col] = format->dblWidth();
              }
              if (!cell->isEmpty() && !cell->isObscured())
              {
                m_newWidths[col] = QMAX(adjustColumnHelper(cell, col, row),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->dblHeight();
              }
              if (!cell->isEmpty() && !cell->isObscured())
              {
                m_newHeights[row] = QMAX(adjustRowHelper(cell, col, row),
                                        m_newHeights[row]);
              }
            }
            cell = m_sheet->getNextCellDown( col, row );
          }
        }
      }
      else if (element->isRow())
      {
        for (int row = range.top(); row <= range.bottom(); ++row)
        {
          Cell* cell = m_sheet->getFirstCellRow( row );
          while ( cell )
          {
            int col = cell->column();
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                ColumnFormat* format = m_sheet->columnFormat(col);
                m_oldWidths[col] = format->dblWidth();
              }
              if (cell != m_sheet->defaultCell() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newWidths[col] = QMAX(adjustColumnHelper(cell, col, row),
                                        m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->dblHeight();
              }
              if (cell != m_sheet->defaultCell() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newHeights[row] = QMAX(adjustRowHelper(cell, col, row),
                                        m_newHeights[row]);
              }
            }
            cell = m_sheet->getNextCellRight(col, row);
          }
        }
      }
      else
      {
        Cell* cell;
        for (int col = range.left(); col <= range.right(); ++col)
        {
          for ( int row = range.top(); row <= range.bottom(); ++row )
          {
            cell = m_sheet->cellAt( col, row );
            if (m_adjustColumn)
            {
              if (!m_newWidths.contains(col))
              {
                m_newWidths[col] = -1.0;
                ColumnFormat* format = m_sheet->columnFormat(col);
                m_oldWidths[col] = format->dblWidth();
              }
              if (cell != m_sheet->defaultCell() && !cell->isEmpty() && !cell->isObscured())
              {
                  m_newWidths[col] = QMAX(adjustColumnHelper(cell, col, row),
                                          m_newWidths[col] );
              }
            }
            if (m_adjustRow)
            {
              if (!m_newHeights.contains(row))
              {
                m_newHeights[row] = -1.0;
                RowFormat* format = m_sheet->rowFormat(row);
                m_oldHeights[row] = format->dblHeight();
              }
              if (cell != m_sheet->defaultCell() && !cell->isEmpty() && !cell->isObscured())
              {
                m_newHeights[row] = QMAX(adjustRowHelper(cell, col, row),
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

double AdjustColumnRowManipulator::adjustColumnHelper(Cell* cell, int col, int row )
{
  double long_max = 0.0;
  cell->calculateTextParameters( m_sheet->painter(), col, row );
  if ( cell->textWidth() > long_max )
  {
    double indent = 0.0;
    Format::Align alignment = cell->format()->align(cell->column(), cell->row());
    if (alignment == Format::Undefined)
    {
      if (cell->value().isNumber() || cell->isDate() || cell->isTime())
      {
        alignment = Format::Right;
      }
      else
      {
        alignment = Format::Left;
      }
    }

    if (alignment == Format::Left)
    {
      indent = cell->format()->getIndent( cell->column(), cell->row() );
    }
    long_max = indent + cell->textWidth()
             + cell->format()->leftBorderWidth( cell->column(), cell->row() )
             + cell->format()->rightBorderWidth( cell->column(), cell->row() );
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

double AdjustColumnRowManipulator::adjustRowHelper(Cell* cell, int col, int row)
{
  double long_max = 0.0;
  cell->calculateTextParameters( m_sheet->painter(), col, row);
  if ( cell->textHeight() > long_max )
  {
      long_max = cell->textHeight()
               + cell->format()->topBorderWidth(col, row)
               + cell->format()->bottomBorderWidth(col, row);
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
  QRect range = element->rect().normalize();
  if (m_manipulateColumns)
  {
    for (int col = range.left(); col <= range.right(); ++col)
    {
      ColumnFormat* format = m_sheet->nonDefaultColumnFormat(col);
      format->setHide(!m_reverse);
    }
  }
  if (m_manipulateRows)
  {
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
      RowFormat* format = m_sheet->nonDefaultRowFormat(row);
      format->setHide(!m_reverse);
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
      QRect range = (*it)->rect().normalize();
      if (m_manipulateColumns)
      {
        if (range.left() > 1)
        {
          int col;
          for (col = 1; col < range.left(); ++col)
          {
            ColumnFormat* format = m_sheet->columnFormat(col);
            if (!format->isHide())
            {
              break;
            }
          }
          if (col == range.left())
          {
            region.add(QRect(1, 1, range.left()-1, KS_rowMax));
          }
        }
        for (int col = range.left(); col <= range.right(); ++col)
        {
          ColumnFormat* format = m_sheet->columnFormat(col);
          if (format->isHide())
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
            RowFormat* format = m_sheet->rowFormat(row);
            if (!format->isHide())
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
          RowFormat* format = m_sheet->rowFormat(row);
          if (format->isHide())
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
    name += "/";
  }
  if (m_manipulateRows)
  {
    name += "Rows";
  }
  return name;
}




/***************************************************************************
  class ManipulatorManager
****************************************************************************/

ManipulatorManager* ManipulatorManager::m_self = 0;
static KStaticDeleter<ManipulatorManager> staticManipulatorManagerDeleter;

ManipulatorManager* ManipulatorManager::self()
{
  if (!m_self)
  {
    staticManipulatorManagerDeleter.setObject(m_self, new ManipulatorManager());
  }
  return m_self;
}

ManipulatorManager::ManipulatorManager()
{
}

ManipulatorManager::~ManipulatorManager()
{
}

Manipulator* ManipulatorManager::create(const QString& type)
{
  if (type == "bgcolor")
  {
    kdDebug() << "Background color manipulator created." << endl;
//     return new FontColorManipulator();
  }
  else if (type == "textcolor")
  {
    kdDebug() << "Text color manipulator created." << endl;
//     return new FontColorManipulator();
  }

  // no manipulator of this type found
  kdError() << "Unknown manipulator!" << endl;
  return 0;
}
