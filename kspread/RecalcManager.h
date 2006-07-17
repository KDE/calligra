/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_RECALC_MANAGER
#define KSPREAD_RECALC_MANAGER

namespace KSpread
{
class Cell;
class DependencyManager;
class Sheet;

/**
 * \class RecalcManager
 * \brief Manages the recalculations of cells containing a formula.
 *
 * If a cell region was changed, the cells, that are refering to this
 * region, are updated.
 * The recalculations of a cell region are triggered by cell value
 * changes. The recalculation itself changes the value of a cell, so
 * we get a cascade of recalculation events.
 *
 * The recalculations of a sheet or the map is based on the following:
 * A cell could refer to other cells, which need to be recalculated
 * before. The order of recalculation is determined by the depth of
 * references, i.e. first the cells, which do not refer to other cells,
 * are recalculated. Cells refering to those are next. Then cells, which
 * refer to the ones in the last step follow and so on until all cells
 * have been updated.
 * Cell value changes are blocked while doing this, i.e. they do not
 * trigger a new recalculation event.
 */
class RecalcManager
{
public:
  /**
   * Creates a RecalcManager. It is used for a whole map.
   *
   * \param depManager The DependencyManger which provides
   * the necessary cell dependencies.
   */
  RecalcManager(DependencyManager* depManager);

  /**
   * Destructor.
   */
  virtual ~RecalcManager();

  /**
   * Recalculates the cells refering to cells in \p region .
   * The updated cells trigger new recalculations until all
   * dependants have been updated.
   *
   * \todo This works fine for the case in which a single cell
   * was changed, but leads to problems with cell regions. If one of
   * the cells in the changed region refers to another one in the same
   * region, this cell gets recalculated twice. One time in the current
   * recalculation event and one time in the event triggered be the
   * update of the refered cell. Use a similar approach as for sheets/map
   * in the other direction.
   *
   * \see recalcRegion()
   */
  void regionChanged(const Region& region);

  /**
   * Recalculates the sheet \p sheet .
   * First the reference cell depths are computed. Then the cells are
   * recalculated sorted by the reference depth in ascending order.
   * \see recalc()
   */
  void recalcSheet(Sheet* const sheet);

  /**
   * Recalculates the whole map.
   * First the reference cell depth are computed. Then the cells are
   * recalculated sorted by the reference depth in ascending order.
   * \see recalc()
   */
  void recalcMap();

  /**
   * Prints out the cell depths in the current recalculation event.
   */
  void dump() const;

protected:
  /**
   * Computes the reference depth.
   * Depth means the maximum depth of all cells this cell depends on plus one,
   * while a cell, which do not refer to other cells, has a depth
   * of zero.
   *
   * E.g. A1: '=1.0'; A2: '=A1+A1'; A3: '=A1+A1+A2';
   *      depth(A1) = 0; depth(A2) = 1; depth(A3) = 2
   */
  int computeDepth(Cell* cell) const;

  /**
   * Iterates over the map of cell with their reference depths
   * and calls recalcCell().
   * \see recalcCell()
   */
  void recalc();

  /**
   * Used in the recalculation events for changed regions.
   * Calls recalcCell() for each position in \p region .
   * \see recalcCell()
   */
  void recalcRegion(const Region& region);

  /**
   * Recalculates a cell.
   * Checks for circular recalculations and sets an error, if those
   * occur.
   */
  void recalcCell(Cell* cell) const;

private:
  class Private;
  Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_RECALC_MANAGER
