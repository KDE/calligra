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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KSPREAD_DEPENDENCIES
#define KSPREAD_DEPENDENCIES

#include <qvaluelist.h>

#include "kspread_util.h"

class KSpreadSheet;

// KSpread namespace
namespace KSpread {


struct DependencyList;

/** Range dependency - stores information about one dependency of one cell on
one range of cells. */

struct RangeDependency {
  int cellrow, cellcolumn;
  KSpreadRange range;
};


/**
This class manages dependencies.
TODO: describe how it works and why there are two types of dependencies
*/

class DependencyManager {
 public:
  /** constructor */
   DependencyManager (KSpreadSheet *s);
  /** destructor */
  ~DependencyManager ();
 
  /** clear all data */
  void reset ();
  
  /** handle the fact that cell's contents have changed */
  void cellChanged (const KSpreadPoint &cell);
  /** handle the fact that a range has been changed */
  void rangeChanged (const KSpreadRange &range);
  /** handle the fact that a range list has been changed */
  void rangeListChanged (const RangeList &rangeList);
 protected:
  /** update structures: cell 1 depends on cell 2 */
  void addDependency (const KSpreadPoint &cell1, const KSpreadPoint &cell2);
  /** update structures: cell depends on a range */
  void addRangeDependency (const RangeDependency &rd);
  /** remove all dependencies of a cell */
  void removeDependencies (const KSpreadPoint &cell);

  /** generate list of dependencies of a cell */
  void generateDependencies (const KSpreadPoint &cell);
  /** generate list of dependencies of a range */
  void generateDependencies (const KSpreadRange &range);
  /** generate list of dependencies of a range list */
  void generateDependencies (const RangeList &rangeList);

  /** update cells dependending on a given cell */
  void processDependencies (const KSpreadPoint &cell) const;
  /** update all cells depending on a range containing this cell */
  void processRangeDependencies (const KSpreadPoint &cell) const;

  /** update cells dependending on a cell in a given range */
  void processDependencies (const KSpreadRange &range) const;
  /** update all cells depending on a range intersecting with this range */
  void processRangeDependencies (const KSpreadRange &range) const;
  
  /** update cells dependending on a given range-list */
  void processDependencies (const RangeList &rangeList) const;

  /** update one cell due to changed dependencies */
  void updateCell (const KSpreadPoint &cell) const;

  /** return a leading cell for a given cell (used to store range
  dependencies effectively) */
  KSpreadPoint leadingCell (const KSpreadPoint &cell) const;
  /** list of leading cells of all cell chunks that this range belongs to */
  QValueList<KSpreadPoint> leadingCells (const KSpreadRange &range) const;
  /** retrieve a list of cells that a given cell depends on */
  RangeList getDependencies (const KSpreadPoint &cell) const;
  
  /** KSpreadSheet whose dependencies are managed by this instance */
  KSpreadSheet *sheet;
  
  /** local d-pointer */
  DependencyList *deps;
};

//end of namespace
};

#endif // KSPREAD_DEPENDENCIES

