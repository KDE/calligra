/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include "kspread_selection.h"
#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_global.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

KSpreadSelection::KSpreadSelection(KSpreadView* view)
{
  m_marker = QPoint(1,1);
  m_cursorPosition = QPoint(1,1);
  m_anchor = QPoint(1,1);

  m_chooseMarker = QPoint(0,0);
  m_chooseAnchor = QPoint(0,0);
  m_chooseCursor = QPoint(0,0);

  m_chooseTable = NULL;
  m_pView = view;
}

KSpreadSelection::~KSpreadSelection()
{
}

/******************************************
 * Functions dealing with the current selection
 */
/*
void KSpreadSelection::unselect()
{
    // No selection? Then do nothing.
    if ( singleCellSelection() )
        return;

    QRect r = m_rctSelection;
    // Discard the selection
    KSpreadCell *cell = cellAt(marker());
    m_rctSelection.setCoords( m_marker.x(), m_marker.y(),
                              m_marker.x() + cell->extraXCells(),
                              m_marker.y() + cell->extraYCells() );

    // Emit signal so that the views can update.
//    emit sig_unselect( this, r );
}
*/

QPoint KSpreadSelection::marker() const
{
  return m_marker;
}

QRect KSpreadSelection::selection() const
{
  int left, top, right, bottom;
  left = QMIN(m_anchor.x(), m_marker.x());
  top = QMIN(m_anchor.y(), m_marker.y());
  right = QMAX(m_anchor.x(), m_marker.x());
  bottom = QMAX(m_anchor.y(), m_marker.y());
  QRect selection(QPoint(left, top), QPoint(right, bottom));

  return extendToMergedAreas(selection);
}

bool KSpreadSelection::singleCellSelection() const
{
  const KSpreadCell* cell =
    m_pView->activeTable()->cellAt(m_marker.x(), m_marker.y());

  QRect currentSelection = selection();
  return ((currentSelection.topLeft() == m_marker) &&
          (currentSelection.width() - 1 == cell->extraXCells()) &&
          (currentSelection.height() - 1 == cell->extraYCells()));
}

QRect KSpreadSelection::selectionHandleArea() const
{
  int column, row;

  // complete rows/columns are selected, use the marker.
  if ( util_isRowSelected(selection()) ||
       util_isColumnSelected(selection()) )
  {
    column = marker().x();
    row = marker().y();
  }
  else
  {
    column = selection().right();
    row = selection().bottom();
  }
  const KSpreadCell* cell = m_pView->activeTable()->cellAt(column, row);

  double xpos = m_pView->activeTable()->dblColumnPos( column );
  double ypos = m_pView->activeTable()->dblRowPos( row );
  double width = cell->dblWidth( column );
  double height = cell->dblHeight( row );

  QPoint rightBottom( m_pView->doc()->zoomItX( xpos + width ),
                      m_pView->doc()->zoomItY( ypos + height ) );

  QRect handle( ( rightBottom.x() - 2 ),
                ( rightBottom.y() - 2 ),
                ( 5 ),
                ( 5 ) );
  return handle;
}

void KSpreadSelection::setSelection( const QPoint &newMarker,
                                     const QPoint &newAnchor,
                                     KSpreadSheet *table )
{
  QRect oldSelection = selection();
  QPoint oldMarker = m_marker;
  m_marker = newMarker;
  m_anchor = newAnchor;

  QRect newSelection = selection();

  const KSpreadCell* cell = table->cellAt(newMarker);
  if (!util_isColumnSelected(newSelection) &&
      !util_isRowSelected(newSelection) &&
      cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    m_marker = QPoint(cell->column(), cell->row());
  }

  newSelection = selection();

  /* see if we've actually changed anything */
  if ( newSelection == oldSelection && newMarker == oldMarker &&
       m_pView->activeTable() == table )
    return;

  /* see if the cursor position is still valid */
  if (!setCursorPosition(m_cursorPosition))
  {
    setCursorPosition(newMarker);
  }

  m_pView->enableInsertColumn( !util_isRowSelected( newSelection ) );
  m_pView->enableInsertRow( !util_isColumnSelected( newSelection ) );
  m_pView->slotChangeSelection( table, oldSelection, oldMarker );
}

void KSpreadSelection::setMarker( const QPoint &point,
                                  KSpreadSheet* table )
{
  QPoint topLeft(point);
  const KSpreadCell* cell = table->cellAt(topLeft);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint botRight(topLeft.x() + cell->extraXCells(),
                  topLeft.y() + cell->extraYCells());
  setSelection( topLeft, botRight, table );
}

QPoint KSpreadSelection::selectionAnchor()const
{
  return m_anchor;
  /* the anchor is in the opposite corner of the selection rect from the marker */

  /* these show where the marker is */
/*
  bool atTop;
  bool atLeft;
  QRect anchorArea;

  atLeft = m_marker.x() == m_rctSelection.left();
  atTop = m_marker.y() == m_rctSelection.top();

  QPoint anchor(atLeft ? m_rctSelection.right() : m_rctSelection.left(),
                atTop ? m_rctSelection.bottom() : m_rctSelection.top());

  KSpreadSheet* table = m_pView->activeTable();
  KSpreadCell* cell = table->cellAt(anchor);

  if (cell->isObscured())
  {
    cell = cell->obscuringCells().first();
    anchorArea = QRect(QPoint(cell->column(), cell->row()), anchor);
  }
  else
  {
    anchorArea = QRect(anchor, anchor);
  }

  return anchorArea;
*/
}

bool KSpreadSelection::setCursorPosition( const QPoint &position )
{
  const KSpreadCell* cell = m_pView->activeTable()->cellAt(m_marker);

  QRect markerArea(m_marker, QPoint(m_marker.x() + cell->mergedXCells(),
                                    m_marker.y() + cell->mergedYCells()));

  if (markerArea.contains(position))
  {
    m_cursorPosition = position;
    return true;
  }
  return false;
}

QPoint KSpreadSelection::cursorPosition()const
{
  return m_cursorPosition;
}

QRect KSpreadSelection::getChooseRect()const
{
  QRect chooseRect;

  chooseRect.setLeft(QMIN(m_chooseMarker.x(), m_chooseAnchor.x()));
  chooseRect.setRight(QMAX(m_chooseMarker.x(), m_chooseAnchor.x()));
  chooseRect.setTop(QMIN(m_chooseMarker.y(), m_chooseAnchor.y()));
  chooseRect.setBottom(QMAX(m_chooseMarker.y(), m_chooseAnchor.y()));

  return chooseRect;
}



QRect KSpreadSelection::extendToMergedAreas(QRect area) const
{
  const KSpreadCell *cell = m_pView->activeTable()->
			    cellAt(area.left(), area.top());

  if( util_isColumnSelected(area) ||
      util_isRowSelected(area) )
    return area;

  else if ( !(cell->isObscured() && cell->isObscuringForced()) &&
            (cell->extraXCells() + 1) >= area.width() &&
            (cell->extraYCells() + 1) >= area.height())
  {
    /* if just a single cell is selected, we need to merge even when
       the obscuring isn't forced.  But only if this is the cell that
       is doing the obscuring -- we still want to be able to click on a cell
       that is being obscured.
    */
    area.setWidth(cell->extraXCells() + 1);
    area.setHeight(cell->extraYCells() + 1);
  }
  else
  {
    int top=area.top();
    int left=area.left();
    int bottom=area.bottom();
    int right=area.right();
    for ( int x = area.left(); x <= area.right(); x++ )
      for ( int y = area.top(); y <= area.bottom(); y++ )
      {
        cell = m_pView->activeTable()->cellAt( x, y );
        if( cell->isForceExtraCells())
        {
          right=QMAX(right,cell->extraXCells()+x);
          bottom=QMAX(bottom,cell->extraYCells()+y);
        }
        else if ( cell->isObscured() && cell->isObscuringForced() )
        {
          cell = cell->obscuringCells().first();
          left=QMIN(left,cell->column());
          top=QMIN(top,cell->row());
          bottom=QMAX(bottom,cell->row() + cell->extraYCells());
          right=QMAX(right,cell->column() + cell->extraXCells());
        }
      }

    area.setCoords(left,top,right,bottom);
  }
  return area;
}
