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
 */
class RecalcManager
{
public:
  RecalcManager(DependencyManager* depManager);
  virtual ~RecalcManager();

  void regionChanged(const Region& region);
  void recalcSheet(Sheet* const sheet);
  void recalcMap();

  void dump() const;

protected:
  int computeDepth(Cell* cell) const;
  void recalc();
  void recalcRegion(const Region& region);
  void recalcCell(Cell* cell) const;

private:
  class Private;
  Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_RECALC_MANAGER
