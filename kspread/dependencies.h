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

#include <q3valuelist.h>

#include "kspread_util.h"

namespace KSpread
{
class Sheet;

struct DependencyList;

/** Range dependency - stores information about one dependency of one cell on
one range of cells. */

struct RangeDependency {
  int cellrow, cellcolumn;
  Sheet *cellsheet;
  Range range;
};


/**
This class manages dependencies.
No need to inherit from DocBase here, at least not yet.
TODO: describe how it works and why there are two types of dependencies
*/

class DependencyManager {
 public:
  /** constructor */
   DependencyManager (Sheet *s);
  /** destructor */
  ~DependencyManager ();
 
  /** clear all data */
  void reset ();
  
  /** handle the fact that cell's contents have changed */
  void cellChanged (const Point &cell);
  /** handle the fact that a range has been changed */
  void rangeChanged (const Range &range);
  /** handle the fact that a range list has been changed */
  void rangeListChanged (const RangeList &rangeList);

  /** a named area was somehow modified */
  void areaModified (const QString &name);

  /** get dependencies of a cell */
  RangeList getDependencies (const Point &cell);
  /** get cells depending on this cell, either through normal or range dependency */
  QLinkedList<Point> getDependants (const Point &cell);
protected:
  
  /** local d-pointer */
  DependencyList *deps;
  friend class DependencyList;
};

//end of namespace
}

#endif // KSPREAD_DEPENDENCIES

