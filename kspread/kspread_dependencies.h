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

#ifndef _kspread_dependencies_h
#define _kspread_dependencies_h

#include <qvaluelist.h>

class KSpreadSheet;

struct DependencyList;

/**
Cell dependency - stores information about one dependency of a cell.
*/

struct CellInfo {
  int row, column;
};

/** one range */
struct Range {
  int startrow, startcol;
  int rows, cols;
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

class KSpreadDependencyManager {
 public:
  /** constructor */
   KSpreadDependencyManager (KSpreadSheet *s);
  /** destructor */
  ~KSpreadDependencyManager ();
 
  /** clear all data */
  void reset ();
  
  /** handle the fact that cell's contents have changed */
  void cellChanged (int row, int col);
  
 protected:
  /** update structures: cell 1 depends on cell 2 */
  void addDependency (int row1, int col1, int row2, int col2);
  /** update structures: cell depends on a range */
  void addRangeDependency (const RangeDependency &rd);
  /** remove all dependencies of a cell */
  void removeDependencies (int row, int col);
  
  /** update cell dependencies of a given cell */
  void processDependencies (int row, int col);
  /** update all cells depending on a range containing (row,col)  */
  void processRangeDependencies (int row, int col);

  /** retrieve a list of cells that this a given cell depends on */
  RangeList getDependencies (int row, int col);
  
  /** KSpreadSheet whose dependencies are managed by this instance */
  KSpreadSheet *sheet;
  
  /** local d-pointer */
  DependencyList *deps;
};

#endif //_kspread_dependencies_h

