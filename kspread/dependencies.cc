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


#include "dependencies.h"

#include "kspread_cell.h"
#include "kspread_sheet.h"

#include <qmap.h>

//rows x cols in one cell-chunk; bigger values lead to slower updating
//of range-dependencies, lower values will increase memory usage
#define CELLCHUNK_ROWS 100
#define CELLCHUNK_COLS 10

namespace KSpread {


/** d-pointer of DependencyManager */
struct DependencyList {
  /** dependencies of each cell */
  QMap<CellInfo, RangeList> dependencies;
  /** list of cells (but NOT ranges) that depend on a cell */
  QMap<CellInfo, QValueList<CellInfo> > cellDeps;
  /** all range dependencies splitted into cell-chunks (TODO: describe) */
  QMap<CellInfo, QValueList<RangeDependency> > rangeDeps;
};

bool CellInfo::operator== (const CellInfo &cell) const
{
  return ((row == cell.row) && (column == cell.column));
}

bool CellInfo::operator< (const CellInfo &cell) const
{
  return (row < cell.row) ? true :
      ((row == cell.row) && (column < cell.column));
}

DependencyManager::DependencyManager (KSpreadSheet *s)
    : sheet (s)
{
  deps = new DependencyList;
}

DependencyManager::~DependencyManager ()
{
  reset ();
  delete deps;
}

void DependencyManager::reset ()
{
  deps->dependencies.clear();
  deps->cellDeps.clear();
  deps->rangeDeps.clear();
}

void DependencyManager::cellChanged (const CellInfo &cell)
{
  generateDependencies (cell);
  processDependencies (cell);
}

void DependencyManager::rangeChanged (const Range &range)
{
  generateDependencies (range);
  processDependencies (range);
}

void DependencyManager::rangeListChanged (const RangeList &rangeList)
{
  generateDependencies (rangeList);
  processDependencies (rangeList);
}

void DependencyManager::addDependency (const CellInfo &cell1,
    const CellInfo &cell2)
{
  deps->dependencies[cell1].cells.push_back (cell2);
  deps->cellDeps[cell2].push_back (cell1);
}

void DependencyManager::addRangeDependency (const RangeDependency &rd)
{
  CellInfo cell;
  cell.row = rd.cellrow;
  cell.column = rd.cellcolumn;
  deps->dependencies[cell].ranges.push_back (rd.range);
  
  QValueList<CellInfo> leadings = leadingCells (rd.range);
  QValueList<CellInfo>::iterator it;
  for (it = leadings.begin(); it != leadings.end(); ++it)
    deps->rangeDeps[*it].push_back (rd);
}

void DependencyManager::removeDependencies (const CellInfo &cell)
{
  //look if the cell has any dependencies
  if (!deps->dependencies.contains (cell))
    return;  //it doesn't - nothing more to do

  //first we remove cell-dependencies
  QValueList<CellInfo> cells = deps->dependencies[cell].cells;
  QValueList<CellInfo>::iterator it1;
  for (it1 = cells.begin();
      it1 != cells.end(); ++it1)
  {
    if (!deps->cellDeps.contains (*it1))
      continue;  //this should never happen
    
    //we no longer depend on this cell
    //ASSUMPTION: each cell is contained no more than once
    QValueList<CellInfo> cells = deps->cellDeps[*it1];
    QValueList<CellInfo>::iterator cit = cells.find (cell);
    if (cit != cells.end())
      cells.erase (cit);
  }
  
  //then range-dependencies are removed
  QValueList<Range> ranges = deps->dependencies[cell].ranges;
  QValueList<Range>::iterator it2;
  QValueList<CellInfo> leads;
  for (it2 = ranges.begin(); it2 != ranges.end(); ++it2)
  {
    //we construct a list of cell-chunks containing a range and merge it
    //with lists generated from all previous ranges (duplicates are removed)
    QValueList<CellInfo> leadings = leadingCells (*it2);
    for (it1 = leadings.begin(); it1 != leadings.end(); ++it1)
      if (!leads.contains (*it1))
        leads.push_back (*it1);
  }
  for (it1 = leads.begin(); it1 != leads.end(); ++it1)
    if (deps->rangeDeps.contains (*it1))
    {
      QValueList<RangeDependency>::iterator it3;
      QValueList<RangeDependency> rdeps = deps->rangeDeps[*it1];
      it3 = rdeps.begin();
      //erase all range dependencies of this cell in this cell-chunk
      while (it3 != rdeps.end())
        if (((*it3).cellrow == cell.row) &&
            ((*it3).cellcolumn == cell.column))
          it3 = rdeps.erase (it3);
        else
          ++it3;
      //erase the list if we no longer need it
      if (rdeps.empty())
        deps->rangeDeps.erase (*it1);
    }
  
  //finally, remove the entry about this cell
  deps->dependencies[cell].cells.clear();
  deps->dependencies[cell].ranges.clear();
  deps->dependencies.erase (cell);
}

void DependencyManager::generateDependencies (const CellInfo &cell)
{
  //get rid of old dependencies first
  removeDependencies (cell);
  
  //new dependencies only need to be generated if the cell contains a formula
  KSpreadCell *c = sheet->cellAt (cell.column, cell.row);
  if (!c->isFormula())
    return;
    
  //now we need to generate dependencies
  RangeList rl = getDependencies (cell);
  
  //now that we have the new dependencies, we put them into our data structures
  //and we're done
  QValueList<CellInfo>::iterator it1;
  QValueList<Range>::iterator it2;
  
  for (it1 = rl.cells.begin(); it1 != rl.cells.end(); ++it1)
    addDependency (cell, *it1);
  for (it2 = rl.ranges.begin(); it2 != rl.ranges.end(); ++it2)
  {
    RangeDependency rd;
    rd.cellrow = cell.row;
    rd.cellcolumn = cell.column;
    rd.range = *it2;
    addRangeDependency (rd);
  }
}

void DependencyManager::generateDependencies (const Range &range)
{
  for (int row = range.startrow; row < range.startrow + range.rows; row++)
    for (int col = range.startcol; col < range.startcol + range.cols; col++)
    {
      CellInfo ci;
      ci.row = row;
      ci.column = col;
      generateDependencies (ci);
    }
}

void DependencyManager::generateDependencies (const RangeList &rangeList)
{
  QValueList<CellInfo>::const_iterator it1;
  QValueList<Range>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    generateDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    generateDependencies (*it2);
}

void DependencyManager::processDependencies (const CellInfo &cell) const
{
  //TODO
  
  processRangeDependencies (cell);
}

void DependencyManager::processRangeDependencies (const CellInfo &cell) const
{
  //TODO
}

void DependencyManager::processDependencies (const Range &range) const
{
  //TODO
  
  processRangeDependencies (range);
}

void DependencyManager::processRangeDependencies (const Range &range) const
{
  //TODO
}

void DependencyManager::processDependencies (const RangeList &rangeList) const
{
  QValueList<CellInfo>::const_iterator it1;
  QValueList<Range>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    processDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    processDependencies (*it2);
}

void DependencyManager::updateCell (const CellInfo &cell)
{
  //prevent infinite recursion (circular dependencies)
  
  //set the computing-dependencies flag
  
  //recalculate the cell
  
  //clear the computing-dependencies flag
  
}

CellInfo DependencyManager::leadingCell (const CellInfo &cell) const
{
  CellInfo c;
  c.row = cell.row - cell.row % CELLCHUNK_ROWS + 1;
  c.column = cell.column - cell.column % CELLCHUNK_COLS + 1;
  return c;
}

QValueList<CellInfo> DependencyManager::leadingCells (const Range &range) const
{
  QValueList<CellInfo> cells;
  CellInfo cell1, cell2, cell;
  
  cell1.row = range.startrow;
  cell1.column = range.startcol;
  cell2.row = range.startrow + range.rows - 1;
  cell2.column = range.startcol + range.cols - 1;
  cell1 = leadingCell (cell1);
  cell2 = leadingCell (cell2);
  for (int row = cell1.row; row <= cell2.row; row += CELLCHUNK_ROWS)
    for (int col = cell1.column; col <= cell2.column; col += CELLCHUNK_COLS)
    {
      cell.row = row;
      cell.column = col;
      cells.push_back (cell);
    }
  return cells;
}

RangeList DependencyManager::getDependencies (const CellInfo &cell) const
{
  //TODO
}


//KSpread namespace end
};

