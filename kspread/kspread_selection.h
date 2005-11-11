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

#ifndef __kspread_selection_h__
#define __kspread_selection_h__

#include <qrect.h>
#include "kspread_util.h"

namespace KSpread
{
class Sheet;
class View;

class KSPREAD_EXPORT Selection
{
public:

  Selection(View* view);

  virtual ~Selection();

  /**
   * Unselects all selected columns/rows/cells and redraws these cells.
   */
//  void unselect();


  /**
   * @return the marker point of the sheet.  Usually is the selected cell, or if
   * a region is selected it is the one point in the region where text would be
   * entered if the user started typing
   */
  QPoint marker() const;

  /**
   * @return the selection rectangle.  Note that this should now NEVER be
   * (0, 0, 0, 0).  Whether the selection is one cell or many this will give
   * the region.
   */
  QRect selection(bool extend = true) const;

  /**
   * @return a boolean indicator of whether a single cell is selected or not
   */
  bool singleCellSelection() const;

  /**
   * @return the area that the 'handle' of the selection is located in painting coordinates.
   */
  QRect selectionHandleArea() const;

  void setSelection( const QPoint &marker, const QPoint &anchor,
                     Sheet* sheet );

  void setMarker( const QPoint &point, Sheet* sheet );

  /**
   * @return the 'anchor' point of the selection -- i.e. the fixed corner
   */
   QPoint selectionAnchor()const;

  /**
   * The 'cursor position' is used to keep track of where on the spreadsheet the
   * user is with the arrow keys.  It's basically the same as the marker, except
   * when the selection moves into a merged area.  If we use the down arrow key to
   * enter a merged area 3 columns over, we want to come out the bottom 3 columns
   * over if we keep pressing the down arrow.
   *
   * If the given position is not within the merged cell area of the marker, this
   * function call will be ignored.
   *
   * @return true if the position was valid, false if it wasn't.  In other words,
   *         true if something changed, false if there is no change.
   */
  bool setCursorPosition(const QPoint &position);

  /**
   * @see setCursorPosition for a description of what the cursor position is
   *
   * You are probably wanting to use marker(), not this function
   */
  QPoint cursorPosition() const;


  void setChooseAnchor( const QPoint &chooseAnchor )
    { m_chooseAnchor = chooseAnchor; }
  void setChooseMarker( const QPoint &chooseMarker )
    { m_chooseMarker = chooseMarker; }
  void setChooseCursor( Sheet* sheet, const QPoint &chooseCursor )
    { m_chooseSheet = sheet; m_chooseCursor = chooseCursor; }
  void setChooseSheet(Sheet* sheet) { m_chooseSheet = sheet; }

  QRect getChooseRect()const;
  QPoint getChooseCursor()const { return m_chooseCursor; }
  QPoint getChooseMarker()const { return m_chooseMarker; }
  QPoint getChooseAnchor()const { return m_chooseAnchor; }

  Sheet* getChooseSheet()const { return m_chooseSheet; }


private: /* private data for the above functions on selections */
  /**
   * The rectangular area that is currently selected.
   * If complete columns are selected, then selection.bottom() == KS_rowMax and
   * selection.top()=1.
   * If complete rows are selected, then selection.right() == KS_colMax and
   * selection.left()=1.
   */
//  QRect m_rctSelection;
  QPoint m_marker;
  QPoint m_cursorPosition;
  QPoint m_anchor;

  QPoint m_chooseMarker;
  QPoint m_chooseAnchor;
  QPoint m_chooseCursor;

  Sheet* m_chooseSheet;
  View* m_pView;

private:
  /* helper functions */
  QRect extendToMergedAreas(QRect area) const;

};

} // namespace KSpread

#endif
