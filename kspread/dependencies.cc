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

#include "formula.h"

#include "kspread_cell.h"
#include "kspread_sheet.h"

#include <qmap.h>

//rows x cols in one cell-chunk; bigger values lead to slower updating
//of range-dependencies, lower values will increase memory usage
#define CELLCHUNK_ROWS 128
#define CELLCHUNK_COLS 16

namespace KSpread {


/** d-pointer of DependencyManager */
struct DependencyList {
  /** dependencies of each cell */
  QMap<KSpreadPoint, RangeList> dependencies;
  /** list of cells (but NOT ranges) that depend on a cell */
  QMap<KSpreadPoint, QValueList<KSpreadPoint> > cellDeps;
  /** all range dependencies splitted into cell-chunks (TODO: describe) */
  QMap<KSpreadPoint, QValueList<RangeDependency> > rangeDeps;
};

};

using namespace KSpread;

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

void DependencyManager::cellChanged (const KSpreadPoint &cell)
{
  KSpreadCell *c = cell.cell();
  
  //if the cell contains the circle error, we mustn't do anything
  if (c->testFlag (KSpreadCell::Flag_CircularCalculation))
    return;

  //don't re-generate dependencies if we're updating dependencies
  if ( !(c->testFlag (KSpreadCell::Flag_Progress)))
    generateDependencies (cell);

  processDependencies (cell);
}

void DependencyManager::rangeChanged (const KSpreadRange &range)
{
  generateDependencies (range);
  processDependencies (range);
}

void DependencyManager::rangeListChanged (const RangeList &rangeList)
{
  generateDependencies (rangeList);
  processDependencies (rangeList);
}

void DependencyManager::addDependency (const KSpreadPoint &cell1,
    const KSpreadPoint &cell2)
{
  deps->dependencies[cell1].cells.push_back (cell2);
  deps->cellDeps[cell2].push_back (cell1);
}

void DependencyManager::addRangeDependency (const RangeDependency &rd)
{
  KSpreadPoint cell;
  cell.table = sheet;
  cell.setRow (rd.cellrow);
  cell.setColumn (rd.cellcolumn);
  deps->dependencies[cell].ranges.push_back (rd.range);
  
  QValueList<KSpreadPoint> leadings = leadingCells (rd.range);
  QValueList<KSpreadPoint>::iterator it;
  for (it = leadings.begin(); it != leadings.end(); ++it)
    deps->rangeDeps[*it].push_back (rd);
}

void DependencyManager::removeDependencies (const KSpreadPoint &cell)
{
  //look if the cell has any dependencies
  if (!deps->dependencies.contains (cell))
    return;  //it doesn't - nothing more to do

  //first we remove cell-dependencies
  QValueList<KSpreadPoint> cells = deps->dependencies[cell].cells;
  QValueList<KSpreadPoint>::iterator it1;
  for (it1 = cells.begin();
      it1 != cells.end(); ++it1)
  {
    if (!deps->cellDeps.contains (*it1))
      continue;  //this should never happen
    
    //we no longer depend on this cell
    //ASSUMPTION: each cell is contained no more than once
    QValueList<KSpreadPoint> cells = deps->cellDeps[*it1];
    QValueList<KSpreadPoint>::iterator cit = cells.find (cell);
    if (cit != cells.end())
      cells.erase (cit);
  }
  
  //then range-dependencies are removed
  QValueList<KSpreadRange> ranges = deps->dependencies[cell].ranges;
  QValueList<KSpreadRange>::iterator it2;
  QValueList<KSpreadPoint> leads;
  for (it2 = ranges.begin(); it2 != ranges.end(); ++it2)
  {
    //we construct a list of cell-chunks containing a range and merge it
    //with lists generated from all previous ranges (duplicates are removed)
    QValueList<KSpreadPoint> leadings = leadingCells (*it2);
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
        if (((*it3).cellrow == cell.row()) &&
            ((*it3).cellcolumn == cell.column()))
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

void DependencyManager::generateDependencies (const KSpreadPoint &cell)
{
  //get rid of old dependencies first
  removeDependencies (cell);
  
  //new dependencies only need to be generated if the cell contains a formula
  KSpreadCell *c = sheet->cellAt (cell.column(), cell.row());
  if (!c->isFormula())
    return;
    
  //now we need to generate dependencies
  RangeList rl = getDependencies (cell);
  
  //now that we have the new dependencies, we put them into our data structures
  //and we're done
  QValueList<KSpreadPoint>::iterator it1;
  QValueList<KSpreadRange>::iterator it2;
  
  for (it1 = rl.cells.begin(); it1 != rl.cells.end(); ++it1)
    addDependency (cell, *it1);
  for (it2 = rl.ranges.begin(); it2 != rl.ranges.end(); ++it2)
  {
    RangeDependency rd;
    rd.cellrow = cell.row();
    rd.cellcolumn = cell.column();
    rd.range = *it2;
    addRangeDependency (rd);
  }
}

void DependencyManager::generateDependencies (const KSpreadRange &range)
{
  for (int row = range.startRow(); row <= range.endRow(); row++)
    for (int col = range.startCol(); col <= range.endCol(); col++)
    {
      KSpreadPoint c;
      c.setRow (row);
      c.setColumn (col);
      c.table = sheet;
      generateDependencies (c);
    }
}

void DependencyManager::generateDependencies (const RangeList &rangeList)
{
  QValueList<KSpreadPoint>::const_iterator it1;
  QValueList<KSpreadRange>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    generateDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    generateDependencies (*it2);
}

void DependencyManager::processDependencies (const KSpreadPoint &cell) const
{
  if (deps->cellDeps.contains (cell))
  {
    QValueList<KSpreadPoint> d = deps->cellDeps[cell];
    QValueList<KSpreadPoint>::iterator it;
    for (it = d.begin(); it != d.end(); ++it)
      updateCell (*it);
  }
  
  processRangeDependencies (cell);
}

void DependencyManager::processRangeDependencies (const KSpreadPoint &cell) const
{
  KSpreadPoint leading = leadingCell (cell);
  QValueList<RangeDependency>::iterator it;
  if (!deps->rangeDeps.count (leading))
    return;  //no range dependencies in this cell chunk
  
  for (it = deps->rangeDeps[leading].begin();
      it != deps->rangeDeps[leading].end(); ++it)
  {
    //process all range dependencies, and for each range including the modified cell,
    //recalc the depending cell
    if ((*it).range.contains (cell))
    {
      KSpreadPoint c;
      c.setRow ((*it).cellrow);
      c.setColumn ((*it).cellcolumn);
      c.table = sheet;
      updateCell (c);
    }
  }
}

void DependencyManager::processDependencies (const KSpreadRange &range) const
{
  //each cell's dependencies need to be updated - that cannot be helped - having a range
  //only helps with range dependencies
  for (int row = range.startRow(); row <= range.endRow(); row++)
    for (int col = range.startCol(); col <= range.endCol(); col++)
    {
      KSpreadPoint c;
      c.setRow (row);
      c.setColumn (col);
      c.table = sheet;
      if (deps->cellDeps.contains (c))
      {
        QValueList<KSpreadPoint> d = deps->cellDeps[c];
        QValueList<KSpreadPoint>::iterator it;
        for (it = d.begin(); it != d.end(); ++it)
          updateCell (*it);
      }
    }
  
  processRangeDependencies (range);
}

void DependencyManager::processRangeDependencies (const KSpreadRange &range) const
{
  //TODO: some optimization, so that we don't recompute cells depending of huge
  //ranges more than once (now we recompute them once per cell-chunk used by their dependency)
  //This will probably happen as a part of splitting this into dep manager
  //and recalc manager
  
  QValueList<KSpreadPoint> leadings = leadingCells (range);
  QValueList<KSpreadPoint>::iterator it;
  for (it = leadings.begin(); it != leadings.end(); ++it)
  {
    if (!deps->rangeDeps.count (*it))
      continue;  //no range dependencies in this cell chunk
    QValueList<RangeDependency>::iterator it2;
    for (it2 = deps->rangeDeps[*it].begin(); it2 != deps->rangeDeps[*it].end(); ++it2)
    {
      //process all range dependencies, and for each range intersecting with our range,
      //recalc the depending cell
      if ((*it2).range.intersects (range))
      {
        KSpreadPoint c;
        c.setRow ((*it2).range.startRow());
        c.setColumn ((*it2).range.startCol());
        c.table = sheet;
        updateCell (c);
      }
    }
  }
}

void DependencyManager::processDependencies (const RangeList &rangeList) const
{
  QValueList<KSpreadPoint>::const_iterator it1;
  QValueList<KSpreadRange>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    processDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    processDependencies (*it2);
}

void DependencyManager::updateCell (const KSpreadPoint &cell) const
{
  KSpreadCell *c = cell.cell();
  
  //prevent infinite recursion (circular dependencies)
  if (c->testFlag (KSpreadCell::Flag_Progress))
  {
    kdError(36001) << "ERROR: Circle" << endl;
    c->setFlag(KSpreadCell::Flag_CircularCalculation);
    KSpreadValue v;
    v.setError ( "####" );
    c->setValue (v);
    //clear the computing-dependencies flag
    c->clearFlag (KSpreadCell::Flag_Progress);
    return;
  }
  
  //set the computing-dependencies flag
  c->setFlag (KSpreadCell::Flag_Progress);
  
  //mark the cell as calc-dirty
  c->setCalcDirtyFlag();
  
  //recalculate the cell
  c->calc (false);
  
  //clear the computing-dependencies flag
  c->clearFlag (KSpreadCell::Flag_Progress);
}

KSpreadPoint DependencyManager::leadingCell (const KSpreadPoint &cell) const
{
  KSpreadPoint c;
  c.setRow (cell.row() - cell.row() % CELLCHUNK_ROWS + 1);
  c.setColumn (cell.column() - cell.column() % CELLCHUNK_COLS + 1);
  c.table = cell.table;
  return c;
}

QValueList<KSpreadPoint> DependencyManager::leadingCells (const KSpreadRange &range) const
{
  QValueList<KSpreadPoint> cells;
  KSpreadPoint cell1, cell2, cell;
  cell1.table = cell2.table = sheet;
  
  cell1.setRow (range.startRow());
  cell1.setColumn (range.startCol());
  cell2.setRow (range.endRow());
  cell2.setColumn (range.endCol());
  cell1 = leadingCell (cell1);
  cell2 = leadingCell (cell2);
  for (int row = cell1.row(); row <= cell2.row(); row += CELLCHUNK_ROWS)
    for (int col = cell1.column(); col <= cell2.column();
        col += CELLCHUNK_COLS)
    {
      cell.setRow (row);
      cell.setColumn (col);
      cells.push_back (cell);
    }
  return cells;
}

RangeList DependencyManager::getDependencies (const KSpreadPoint &cell) const
{
  RangeList rl;
  KSpreadCell *c = cell.cell();
  if (!c->isFormula())
    return rl;   //not a formula -> no dependencies

  //TODO: when the new parser is in use, KSpreadCell will hold a Formula
  //instance, hence we'll be able to use that one directly
  Formula formula;
  formula.setExpression (c->text());
  
  //now that we have the formula, we ask it to give us the dependencies
  rl = formula.getDependencies ();
  return rl;
}

