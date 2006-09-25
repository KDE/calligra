/* This file is part of the KDE project
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>

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

#ifndef KSPREAD_DEPENDENCIES
#define KSPREAD_DEPENDENCIES

#include <QLinkedList>

#include "Region.h"
#include "Util.h"

namespace KSpread
{
class Region;
class Sheet;

/**
 * This class manages cell dependencies.
 */
class KSPREAD_EXPORT DependencyManager
{
  friend class RecalcManager;

public:
  /** constructor */
   DependencyManager();
  /** destructor */
  ~DependencyManager ();

  /** clear all data */
  void reset ();

  /** handle the fact that cell's contents have changed */
  void regionChanged (const Region& region);

  /** a named area was somehow modified */
  void areaModified (const QString &name);

  /** Updates the whole map. */
  void updateAllDependencies(const Map* map);

  /** get dependencies of a cell */
  Region getDependencies (const Point &cell);

  /**
   * \return cells depending on \p cell
   */
  Region getDependents(const Cell* cell);

  /**
   * Adjusts formulas after cut & paste operations or column/row insertions/deletions.
   *
   * \param movedRegion the region, that was moved
   * \param destination the new upper left corner of the region
   */
  void regionMoved( const Region& movedRegion, const Region::Point& destination );

protected:
  QMap<Region::Point, Region> dependencies() const;

  /**
   * \param cell the cell which formula should  be altered
   * \param oldLocation the location/range, that was cut
   * \param offset the relative movement and new sheet, if applicable
   *
   * \see regionMoved()
   */
  void updateFormula( Cell* cell, const Region::Element* oldLocation, const Region::Point& offset );

  class Private;
  Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_DEPENDENCIES
