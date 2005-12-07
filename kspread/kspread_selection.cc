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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kspread_selection.h"
#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_global.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

using namespace KSpread;

Selection::Selection(View* view)
{
  m_marker = QPoint(1,1);
  m_cursorPosition = QPoint(1,1);
  m_anchor = QPoint(1,1);

  m_chooseMarker = QPoint(0,0);
  m_chooseAnchor = QPoint(0,0);
  m_chooseCursor = QPoint(0,0);

  m_chooseSheet = NULL;
  m_pView = view;
}

Selection::~Selection()
{
}

/******************************************
 * Functions dealing with the current selection
 */
/*
void Selection::unselect()
{
    // No selection? Then do nothing.
    if ( singleCellSelection() )
        return;

    QRect r = m_rctSelection;
    // Discard the selection
    Cell *cell = cellAt(marker());
    m_rctSelection.setCoords( m_marker.x(), m_marker.y(),
                              m_marker.x() + cell->mergedXCells(),
                              m_marker.y() + cell->mergedYCells() );

    // Emit signal so that the views can update.
//    emit sig_unselect( this, r );
}
*/

QPoint Selection::marker() const
{
  return m_marker;
}

QRect Selection::selection(bool extend) const
{
  int left, top, right, bottom;
  left = QMIN(m_anchor.x(), m_marker.x());
  top = QMIN(m_anchor.y(), m_marker.y());
  right = QMAX(m_anchor.x(), m_marker.x());
  bottom = QMAX(m_anchor.y(), m_marker.y());
  QRect selection(QPoint(left, top), QPoint(right, bottom));

  if (extend)
      return extendToMergedAreas(selection);
  return selection;
}

bool Selection::singleCellSelection() const
{
  const Cell* cell =
    m_pView->activeSheet()->cellAt(m_marker.x(), m_marker.y());

  QRect currentSelection = selection();
  return ((currentSelection.topLeft() == m_marker) &&
          (currentSelection.width() - 1 == cell->mergedXCells()) &&
          (currentSelection.height() - 1 == cell->mergedYCells()));
}

QRect Selection::selectionHandleArea() const
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
  const Cell* cell = m_pView->activeSheet()->cellAt(column, row);

  double xpos = m_pView->activeSheet()->dblColumnPos( column );
  double ypos = m_pView->activeSheet()->dblRowPos( row );
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

void Selection::setSelection( const QPoint &newMarker,
                                     const QPoint &newAnchor,
                                     Sheet *sheet )
{
  QRect oldSelection = selection();
  QPoint oldMarker = m_marker;
  m_marker = newMarker;
  m_anchor = newAnchor;

  QRect newSelection = selection();

  //kdDebug(36001) << "setSelection: anchor = " << newAnchor
  //		 << " marker = " << newMarker << endl;

  const Cell* cell = sheet->cellAt(newMarker);
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
       m_pView->activeSheet() == sheet )
    return;

  /* see if the cursor position is still valid */
  if (!setCursorPosition(m_cursorPosition))
  {
    setCursorPosition(newMarker);
  }

  m_pView->enableInsertColumn( !util_isRowSelected( newSelection ) );
  m_pView->enableInsertRow( !util_isColumnSelected( newSelection ) );
  m_pView->slotChangeSelection( sheet, oldSelection, oldMarker );
}

void Selection::setMarker( const QPoint &point,
                                  Sheet* sheet )
{
  QPoint topLeft(point);
  const Cell* cell = sheet->cellAt(topLeft);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint botRight(topLeft.x() + cell->mergedXCells(),
                  topLeft.y() + cell->mergedYCells());
  setSelection( topLeft, botRight, sheet );
}

QPoint Selection::selectionAnchor()const
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

  Sheet* sheet = m_pView->activeSheet();
  Cell* cell = sheet->cellAt(anchor);

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

bool Selection::setCursorPosition( const QPoint &position )
{
  const Cell* cell = m_pView->activeSheet()->cellAt(m_marker);

  QRect markerArea(m_marker, QPoint(m_marker.x() + cell->mergedXCells(),
                                    m_marker.y() + cell->mergedYCells()));

  if (markerArea.contains(position))
  {
    m_cursorPosition = position;
    return true;
  }
  return false;
}

QPoint Selection::cursorPosition()const
{
  return m_cursorPosition;
}

QRect Selection::getChooseRect()const
{
  QRect chooseRect;

  chooseRect.setLeft(QMIN(m_chooseMarker.x(), m_chooseAnchor.x()));
  chooseRect.setRight(QMAX(m_chooseMarker.x(), m_chooseAnchor.x()));
  chooseRect.setTop(QMIN(m_chooseMarker.y(), m_chooseAnchor.y()));
  chooseRect.setBottom(QMAX(m_chooseMarker.y(), m_chooseAnchor.y()));

  return chooseRect;
}



QRect Selection::extendToMergedAreas(QRect area) const
{
  const Cell *cell = m_pView->activeSheet()->
			    cellAt(area.left(), area.top());

  if( util_isColumnSelected(area) ||
      util_isRowSelected(area) )
    return area;

  else if ( !(cell->isObscured() && cell->isObscuringForced()) &&
            (cell->mergedXCells() + 1) >= area.width() &&
            (cell->mergedYCells() + 1) >= area.height())
  {
    /* if just a single cell is selected, we need to merge even when
       the obscuring isn't forced.  But only if this is the cell that
       is doing the obscuring -- we still want to be able to click on a cell
       that is being obscured.
    */
    area.setWidth(cell->mergedXCells() + 1);
    area.setHeight(cell->mergedYCells() + 1);
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
        cell = m_pView->activeSheet()->cellAt( x, y );
        if( cell->isForceExtraCells())
        {
          right=QMAX(right,cell->mergedXCells()+x);
          bottom=QMAX(bottom,cell->mergedYCells()+y);
        }
        else if ( cell->isObscured() && cell->isObscuringForced() )
        {
          cell = cell->obscuringCells().first();
          left=QMIN(left,cell->column());
          top=QMIN(top,cell->row());
          bottom=QMAX(bottom,cell->row() + cell->mergedYCells());
          right=QMAX(right,cell->column() + cell->mergedXCells());
        }
      }

    area.setCoords(left,top,right,bottom);
  }
  return area;
}
