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

#ifndef _dependencies_h
#define _dependencies_h

#include <qvaluelist.h>

class KSpreadSheet;

// KSpread namespace
namespace KSpread {


struct DependencyList;

/**
Cell dependency - stores information about one dependency of a cell.
*/

struct CellInfo {
  int row, column;
  
  inline bool operator== (const CellInfo &cell) const;
  inline bool operator< (const CellInfo &cell) const;
};

/** one range */
struct Range {
  int startrow, startcol;
  int rows, cols;
  
  /** does this range contain the given cell? */
  inline bool contains (const CellInfo &cell);
  /** do these two ranges have at least one common cell? */
  bool intersects (const Range &range);
};

/**
range-list and cell-list
TODO: move to a separate file, improve structure, add iterators and all that 
TODO: use this class instead of other means of range-walking all over KSpread
TODO: use this as selection
TODO: anything I forgot ;)
*/
struct RangeList {
  QValueList<CellInfo> cells;
  QValueList<Range> ranges;
};

/** Range dependency - stores information about one dependency of one cell on
one range of cells. */

struct RangeDependency {
  int cellrow, cellcolumn;
  Range range;
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
  void cellChanged (const CellInfo &cell);
  /** handle the fact that a range has been changed */
  void rangeChanged (const Range &range);
  /** handle the fact that a range list has been changed */
  void rangeListChanged (const RangeList &rangeList);
 protected:
  /** update structures: cell 1 depends on cell 2 */
  void addDependency (const CellInfo &cell1, const CellInfo &cell2);
  /** update structures: cell depends on a range */
  void addRangeDependency (const RangeDependency &rd);
  /** remove all dependencies of a cell */
  void removeDependencies (const CellInfo &cell);

  /** generate list of dependencies of a cell */
  void generateDependencies (const CellInfo &cell);
  /** generate list of dependencies of a range */
  void generateDependencies (const Range &range);
  /** generate list of dependencies of a range list */
  void generateDependencies (const RangeList &rangeList);

  /** update cells dependending on a given cell */
  void processDependencies (const CellInfo &cell) const;
  /** update all cells depending on a range containing this cell */
  void processRangeDependencies (const CellInfo &cell) const;

  /** update cells dependending on a cell in a given range */
  void processDependencies (const Range &range) const;
  /** update all cells depending on a range intersecting with this range */
  void processRangeDependencies (const Range &range) const;
  
  /** update cells dependending on a given range-list */
  void processDependencies (const RangeList &rangeList) const;

  /** update one cell due to changed dependencies */
  void updateCell (const CellInfo &cell) const;

  /** return a leading cell for a given cell (used to store range
  dependencies effectively) */
  CellInfo leadingCell (const CellInfo &cell) const;
  /** list of leading cells of all cell chunks that this range belongs to */
  QValueList<CellInfo> leadingCells (const Range &range) const;
  /** retrieve a list of cells that a given cell depends on */
  RangeList getDependencies (const CellInfo &cell) const;
  
  /** KSpreadSheet whose dependencies are managed by this instance */
  KSpreadSheet *sheet;
  
  /** local d-pointer */
  DependencyList *deps;
};

//end of namespace
};

#endif //_dependencies_h

