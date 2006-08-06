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

#include <QColor>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstaticdeleter.h>

#include "Canvas.h"
#include "Cell.h"
#include "Doc.h"
#include "Map.h"
#include "Sheet.h"
#include "Format.h"
#include "Style.h"
#include "StyleManager.h"
#include "Undo.h"
#include "View.h"

#include "Manipulator.h"

using namespace KSpread;

//BEGIN NOTE Stefan: some words on operations
//
// 1. SubTotal
// a) Makes no sense to extend to non-contiguous selections (NCS) as
//    it refers to a change in one column.
// b) No special undo command available yet.
//
// 2. AutoSum
// a) should insert cell at the end of the selection, if the last
//    is not empty
// b) opens an editor, if the user's intention is fuzzy -> hard to
//    convert to NCS
//END

/***************************************************************************
  class Manipulator
****************************************************************************/

Manipulator::Manipulator()
  : Region(),
    KCommand(),
    m_sheet(0),
    m_creation(true),
    m_reverse(false),
    m_firstrun(true),
    m_format(true),
    m_register(true),
    m_protcheck(true)
{
}

Manipulator::~Manipulator()
{
}

void Manipulator::execute()
{
  if (!m_sheet)
  {
    kWarning() << "Manipulator::execute(): No explicit m_sheet is set. "
                << "Manipulating all sheets of the region." << endl;
  }

  // protection check if needed
  if (m_protcheck) {
    ProtectedCheck check;
    check.setSheet (m_sheet);
    Region::Iterator endOfList(cells().end());
    for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
      check.add ((*it)->rect());
    if (check.check ())
    {
      KMessageBox::error (0, i18n ("You cannot change a protected sheet"));
      return;
    }
  }
  
  bool successfully = true;
  successfully = preProcessing();
  if (!successfully)
  {
    kWarning() << "Manipulator::execute(): preprocessing was not successful!" << endl;
    return;   // do nothing if pre-processing fails
  }

  m_sheet->doc()->setModified(true);
  m_sheet->doc()->setUndoLocked(true);
  m_sheet->doc()->emitBeginOperation();
  m_sheet->setRegionPaintDirty( *this );

  // successfully = true;
  successfully = mainProcessing();
  if (!successfully)
  {
    kWarning() << "Manipulator::execute(): processing was not successful!" << endl;
  }

  successfully = true;
  successfully = postProcessing();
  if (!successfully)
  {
    kWarning() << "Manipulator::execute(): postprocessing was not successful!" << endl;
  }

  m_sheet->doc()->emitEndOperation();
  m_sheet->doc()->setUndoLocked(false);

  // add me to undo if needed
  if (m_firstrun && m_register)
  {
    // addCommand itself checks for undo lock
    m_sheet->doc()->addCommand (this);
    // if we add something to undo, then the document surely is modified ...
    m_sheet->doc()->setModified (true);
  }
  m_firstrun = false;
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

  QRect range = element->rect();
  if (m_format && element->isColumn())
  {
    for (int col = range.left(); col <= range.right(); ++col)
    {
      kDebug() << "Processing column " << col << "." << endl;
      ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
      process(format);
        // TODO Stefan: process cells with this property
    }
  }
  else if (m_format && element->isRow())
  {
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
      kDebug() << "Processing row " << row << "." << endl;
      RowFormat* format = sheet->nonDefaultRowFormat(row);
      process(format);
        // TODO Stefan: process cells with this property
    }
  }
  else
  {
    kDebug() << "Processing cell(s) at " << range << "." << endl;
    for (int col = range.left(); col <= range.right(); ++col)
    {
      sheet->enableScrollBarUpdates(false);
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell* cell = sheet->cellAt(col, row);
/* (Tomas) don't force working on obscurring cells - most manipulators don't want this, and those that do can do that manually ... Plus I think that no manipulator should do it anyway ...
        if ( cell->isPartOfMerged() )
        {
          cell = cell->obscuringCells().first();
        }
*/
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
      sheet->enableScrollBarUpdates(true);
      sheet->checkRangeVBorder(range.bottom());
    }
    sheet->checkRangeHBorder(range.right());
  }
  return true;
}

bool Manipulator::mainProcessing()
{
  bool successfully = true;
  Region::Iterator endOfList(cells().end());
  for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
  {
    successfully = successfully && process(*it);
  }
  return successfully;
}

void MacroManipulator::execute ()
{
  QList<Manipulator *>::iterator it;
  for (it = manipulators.begin(); it != manipulators.end(); ++it)
    (*it)->execute ();
  // add me to undo if needed - same as in Manipulator::execute
  if (m_firstrun && m_register)
  {
    m_sheet->doc()->addCommand (this);
    m_sheet->doc()->setModified (true);
  }
  m_firstrun = false;
}

void MacroManipulator::unexecute ()
{
  QList<Manipulator *>::iterator it;
  for (it = manipulators.begin(); it != manipulators.end(); ++it)
    (*it)->unexecute ();
}

void MacroManipulator::add (Manipulator *manipulator)
{
  manipulator->setRegisterUndo (false);
  manipulators.push_back (manipulator);
}

/***************************************************************************
  class ProtectedCheck
****************************************************************************/

ProtectedCheck::ProtectedCheck ()
{
}

ProtectedCheck::~ProtectedCheck ()
{
}

bool ProtectedCheck::check ()
{
  if (!m_sheet->isProtected())
    return false;

  bool prot = false;
  Region::Iterator endOfList(cells().end());
  for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
  {
    Region::Element *element = *it;
    QRect range = element->rect();

    for (int col = range.left(); col <= range.right(); ++col)
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell *cell = m_sheet->cellAt (col, row);
        if (!cell->format()->notProtected (col, row))
        {
          prot = true;
          break;
        }
      }
      if (prot) break;
    }
  }
  return prot;
}



/***************************************************************************
  class MergeManipulator
****************************************************************************/

MergeManipulator::MergeManipulator()
  : Manipulator(),
    m_merge(true),
    m_mergeHorizontal(false),
    m_mergeVertical(false),
    m_unmerger(0)
{
  m_protcheck = false;  // this manipulator does its own checking
}

MergeManipulator::~MergeManipulator()
{
  delete m_unmerger;
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

  QRect range = element->rect();
  int left   = range.left();
  int right  = range.right();
  int top    = range.top();
  int bottom = range.bottom();
  int height = range.height();
  int width  = range.width();

  bool doMerge = m_reverse ? (!m_merge) : m_merge;

  if (doMerge)
  {
    if (m_mergeHorizontal)
    {
      for (int row = top; row <= bottom; ++row)
      {
        int rows = 0;
        for (int col = left; col <= right; ++col)
        {
          Cell *cell = m_sheet->cellAt( col, row );
          if (cell->doesMergeCells())
          {
            rows = qMax(rows, cell->mergedYCells());
            cell->mergeCells( col, row, 0, 0 );
          }
        }
        Cell *cell = m_sheet->nonDefaultCell( left, row );
        if (!cell->isPartOfMerged())
        {
          cell->mergeCells( left, row, width - 1, rows );
        }
      }
    }
    else if (m_mergeVertical)
    {
      for (int col = left; col <= right; ++col)
      {
        int cols = 0;
        for (int row = top; row <= bottom; ++row)
        {
          Cell *cell = m_sheet->cellAt( col, row );
          if (cell->doesMergeCells())
          {
            cols = qMax(cols, cell->mergedXCells());
            cell->mergeCells( col, row, 0, 0 );
          }
        }
        Cell *cell = m_sheet->nonDefaultCell( col, top );
        if (!cell->isPartOfMerged())
        {
          cell->mergeCells( col, top, cols, height - 1);
        }
      }
    }
    else
    {
      Cell *cell = m_sheet->nonDefaultCell( left, top );
      cell->mergeCells( left, top, width - 1, height - 1);
    }
  }
  else // dissociate
  {
    for (int col = left; col <= right; ++col)
    {
      for (int row = top; row <= bottom; ++row)
      {
        Cell *cell = m_sheet->cellAt( col, row );
        if (!cell->doesMergeCells())
        {
          continue;
        }
        cell->mergeCells( col, row, 0, 0 );
      }
    }
  }

  return true;
}

QString MergeManipulator::name() const
{
  if (m_merge) // MergeManipulator
  {
    if (m_mergeHorizontal)
    {
      return i18n("Merge Cells Horizontally");
    }
    else if (m_mergeVertical)
    {
      return i18n("Merge Cells Vertically");
    }
    else
    {
      return i18n("Merge Cells");
    }
  }
  return i18n("Dissociate Cells");
}

bool MergeManipulator::preProcessing()
{
  if (isColumnOrRowSelected())
  {
    KMessageBox::information( 0, i18n( "Merging of columns or rows is not supported." ) );
    return false;
  }

  if (m_firstrun)
  {
    // reduce the region to the region occupied by merged cells
    Region mergedCells;
    ConstIterator endOfList = constEnd();
    for (ConstIterator it = constBegin(); it != endOfList; ++it)
    {
      Element* element = *it;
      QRect range = element->rect();
      int right = range.right();
      int bottom = range.bottom();
      for (int row = range.top(); row <= bottom; ++row)
      {
        for (int col = range.left(); col <= right; ++col)
        {
          Cell *cell = m_sheet->cellAt(col, row);
          if (cell->doesMergeCells())
          {
            QRect rect(col, row, cell->mergedXCells() + 1, cell->mergedYCells() + 1);
            mergedCells.add(rect);
          }
        }
      }
    }

    if (m_merge) // MergeManipulator
    {
      // we're in the manipulator's first execution
      // initialize the undo manipulator
      m_unmerger = new MergeManipulator();
      if (!m_mergeHorizontal && !m_mergeVertical)
      {
        m_unmerger->setReverse(true);
      }
      m_unmerger->setSheet(m_sheet);
      m_unmerger->setRegisterUndo(false);
      m_unmerger->add(mergedCells);
    }
    else // DissociateManipulator
    {
      clear();
      add(mergedCells);
    }
  }

  if (m_merge) // MergeManipulator
  {
    if (m_reverse) // dissociate
    {
    }
    else // merge
    {
      // Dissociate cells before merging the whole region.
      // For horizontal/vertical merging the cells stay
      // as they are. E.g. the region contains a merged cell
      // occupying two rows. Then the horizontal merge should
      // keep the height of two rows and extend the merging to the
      // region's width. In this case the unmerging is done while
      // processing each region element.
      if (!m_mergeHorizontal && !m_mergeVertical)
      {
        m_unmerger->execute();
      }
    }
  }
  return true;
}

bool MergeManipulator::postProcessing()
{
  if (m_merge) // MergeManipulator
  {
    if (m_reverse) // dissociate
    {
      // restore the old merge status
      if (m_mergeHorizontal || m_mergeVertical)
      {
        m_unmerger->execute();
      }
      else
      {
        m_unmerger->unexecute();
      }
    }
  }

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
  class RemovalManipulator
****************************************************************************/
bool RemovalManipulator::mainProcessing()
{
  QByteArray data;
  if (m_reverse)
  {
    data = m_undoData;
  }
  else
  {
    if ( m_firstrun )
    {
      return Manipulator::mainProcessing();
    }
    else
    {
      data = m_redoData;
    }
  }

  m_sheet->paste( data, boundingRect() );
  return true;
}

bool RemovalManipulator::preProcessing()
{
  if (m_reverse)
  {
    if ( m_redoData.isEmpty() )
    {
      saveCellRegion( m_redoData );
    }
  }
  else
  {
    if ( m_undoData.isEmpty() )
    {
      saveCellRegion( m_undoData );
    }
  }
  return true;
}

bool RemovalManipulator::postProcessing()
{
  if (m_sheet->getAutoCalc())
  {
    m_sheet->recalc();
  }
  return true;
}

void RemovalManipulator::saveCellRegion( QByteArray& data )
{
  QDomDocument document = m_sheet->saveCellRegion( *this );
  // Save to buffer
  QTextStream stream( &data, QIODevice::WriteOnly );
  stream.setCodec( "UTF-8" );
  stream << document;
}

bool CommentRemovalManipulator::process( Cell* cell )
{
  cell->format()->setComment( "" );
  return true;
}

bool ConditionRemovalManipulator::process( Cell* cell )
{
  cell->setConditionList( QLinkedList<Conditional>() );
  return true;
}



/***************************************************************************
  class ValidityManipulator
****************************************************************************/

ValidityManipulator::ValidityManipulator()
  : Manipulator()
{
  m_format = false;
}

bool ValidityManipulator::process( Cell* cell )
{
  if ( !cell->isObscured() )
  {
    if ( m_validity.m_restriction == Restriction::None )
    {
      cell->removeValidity();
    }
    else
    {
      *cell->validity(true) = m_validity;
    }
  }
  return true;
}

bool ValidityManipulator::preProcessing()
{
  if ( !m_reverse )
  {
    if ( m_firstrun )
    {
      // create undo
      ConstIterator endOfList = constEnd();
      for (ConstIterator it = constBegin(); it != endOfList; ++it)
      {
        Element* element = *it;
        QRect range = element->rect();
        int right = range.right();
        int bottom = range.bottom();
        for (int row = range.top(); row <= bottom; ++row)
        {
          for (int col = range.left(); col <= right; ++col)
          {
            if ( Validity* validity = m_sheet->cellAt(col, row)->validity() )
            {
              m_undoData[col][row] = *validity;
            }
          }
        }
      }
    }
  }
  else // m_reverse
  {
    // undo
    ConstIterator endOfList = constEnd();
    for (ConstIterator it = constBegin(); it != endOfList; ++it)
    {
      Element* element = *it;
      QRect range = element->rect();
      int right = range.right();
      int bottom = range.bottom();
      for (int row = range.top(); row <= bottom; ++row)
      {
        for (int col = range.left(); col <= right; ++col)
        {
          if ( m_undoData[col][row].m_restriction == Restriction::None )
          {
            *m_sheet->cellAt(col, row)->validity(true) = m_undoData[col][row];
          }
        }
      }
    }
  }
  return true;
}

QString ValidityManipulator::name() const
{
  if ( m_validity.m_restriction == Restriction::None )
  {
    return i18n( "Remove Validity" );
  }
  else
  {
    return i18n( "Add Validity" );
  }
}
