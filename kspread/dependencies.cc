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

#include "docbase.h"
#include "kspread_cell.h"
#include "kspread_sheet.h"

#include <qmap.h>

//rows x cols in one cell-chunk; bigger values lead to slower updating
//of range-dependencies, lower values will increase memory usage
#define CELLCHUNK_ROWS 128
#define CELLCHUNK_COLS 16

namespace KSpread {


/** d-pointer of DependencyManager */
class DependencyList {
 public:
  DependencyList (KSpreadSheet *s);
  ~DependencyList () { reset (); };
  /** clear internal structures */
  void reset ();
  
  /** generate list of dependencies of a cell */
  void generateDependencies (const KSpreadPoint &cell);
  /** generate list of dependencies of a range */
  void generateDependencies (const KSpreadRange &range);
  /** generate list of dependencies of a range list */
  void generateDependencies (const RangeList &rangeList);

  /** update cells dependending on a given cell */
  void processDependencies (const KSpreadPoint &cell);
  /** update cells dependending on a cell in a given range */
  void processDependencies (const KSpreadRange &range);
  /** update cells dependending on a given range-list */
  void processDependencies (const RangeList &rangeList);

  /** get dependencies of a cell */
  RangeList getDependencies (const KSpreadPoint &cell);
  /** get cells depending on this cell, either through normal or range dependency */
  QValueList<KSpreadPoint> getDependants (const KSpreadPoint &cell);

 protected:
  /** update structures: cell 1 depends on cell 2 */
  void addDependency (const KSpreadPoint &cell1, const KSpreadPoint &cell2);
  /** update structures: cell depends on a range */
  void addRangeDependency (const RangeDependency &rd);
  /** remove all dependencies of a cell */
  void removeDependencies (const KSpreadPoint &cell);
  
  /** update all cells depending on a range containing this cell */
  void processRangeDependencies (const KSpreadPoint &cell);

  /** update all cells depending on a range intersecting with this range */
  void processRangeDependencies (const KSpreadRange &range);
  
  /** update one cell due to changed dependencies */
  void updateCell (const KSpreadPoint &cell) const;

  /** return a leading cell for a given cell (used to store range
  dependencies effectively) */
  KSpreadPoint leadingCell (const KSpreadPoint &cell) const;
  /** list of leading cells of all cell chunks that this range belongs to */
  QValueList<KSpreadPoint> leadingCells (const KSpreadRange &range) const;
  /** retrieve a list of cells that a given cell depends on */
  RangeList computeDependencies (const KSpreadPoint &cell) const;
  
  /** KSpreadSheet whose dependencies are managed by this instance */
  KSpreadSheet *sheet;
  
  /** dependencies of each cell */
  QMap<KSpreadPoint, RangeList> dependencies;
  /** list of cells (but NOT ranges) that depend on a cell */
  QMap<KSpreadPoint, QValueList<KSpreadPoint> > cellDeps;
  /** all range dependencies splitted into cell-chunks (TODO: describe) */
  QMap<KSpreadPoint, QValueList<RangeDependency> > rangeDeps;
};

}

using namespace KSpread;

DependencyManager::DependencyManager (KSpreadSheet *s)
{
  deps = new DependencyList (s);
}

DependencyManager::~DependencyManager ()
{
  delete deps;
  deps = 0;
}

void DependencyManager::reset ()
{
  deps->reset();
}

void DependencyManager::cellChanged (const KSpreadPoint &cell)
{
  KSpreadCell *c = cell.cell();
  
  // empty or default cell? do nothing
  if( c->isDefault() )
    return;
  
  //if the cell contains the circle error, we mustn't do anything
  if (c->testFlag (KSpreadCell::Flag_CircularCalculation))
    return;
  
  kdDebug(36001) << "updating dependencies for cell (" <<
      c->row() << "," << c->column() << ")" << endl;
  
  //don't re-generate dependencies if we're updating dependencies
  if ( !(c->testFlag (KSpreadCell::Flag_Progress)))
    deps->generateDependencies (cell);

  deps->processDependencies (cell);
}

void DependencyManager::rangeChanged (const KSpreadRange &range)
{
  deps->generateDependencies (range);
  deps->processDependencies (range);
}

void DependencyManager::rangeListChanged (const RangeList &rangeList)
{
  deps->generateDependencies (rangeList);
  deps->processDependencies (rangeList);
}

RangeList DependencyManager::getDependencies (const KSpreadPoint &cell)
{
  return deps->getDependencies (cell);
}

QValueList<KSpreadPoint> DependencyManager::getDependants (const KSpreadPoint &cell)
{
  return deps->getDependants (cell);
}

DependencyList::DependencyList (KSpreadSheet *s)
    : sheet (s)
{
}

void DependencyList::reset ()
{
  dependencies.clear();
  cellDeps.clear();
  rangeDeps.clear();
}

RangeList DependencyList::getDependencies (const KSpreadPoint &cell)
{
  RangeList rl;
  //look if the cell has any dependencies
  if (!dependencies.contains (cell))
    return rl;  //it doesn't - return an empty list
  
  //the cell does have dependencies - return them!
  return dependencies[cell];
}

QValueList<KSpreadPoint> DependencyList::getDependants (const KSpreadPoint &cell)
{
  QValueList<KSpreadPoint> list;
  
  //cell dependencies go first
  if (cellDeps.contains (cell))
    list = cellDeps[cell];
  
  //next, append range dependencies
  KSpreadPoint leading = leadingCell (cell);
  QValueList<RangeDependency>::iterator it;
  if (!rangeDeps.count (leading))
    return list;  //no range dependencies in this cell chunk -> nothing more to do
  
  for (it = rangeDeps[leading].begin();
      it != rangeDeps[leading].end(); ++it)
  {
    //process all range dependencies, and for each range including the questioned cell,
    //add the depending cell to the list
    if ((*it).range.contains (cell))
    {
      KSpreadPoint c;
      c.setRow ((*it).cellrow);
      c.setColumn ((*it).cellcolumn);
      c.sheet = (*it).cellsheet;
      list.push_back (c);
    }
  }
  
  return list;
}

void DependencyList::addDependency (const KSpreadPoint &cell1,
    const KSpreadPoint &cell2)
{
  kdDebug(36001) << "Dep. manager: added a dependency" << endl;
  
  //cell2 can be in another sheet (inter-sheet dependency)
  KSpreadSheet *sh = cell2.sheet;
  if (!sh)
    sh = sheet;

  dependencies[cell1].cells.push_back (cell2);
  sh->dependencies()->deps->cellDeps[cell2].push_back (cell1);
}

void DependencyList::addRangeDependency (const RangeDependency &rd)
{
  //target range can be in another sheet (inter-sheet dependency)
  KSpreadSheet *sh = rd.range.sheet;
  if (!sh)
    sh = sheet;
  
  KSpreadPoint cell;
  cell.sheet = rd.cellsheet;
  cell.setRow (rd.cellrow);
  cell.setColumn (rd.cellcolumn);
  dependencies[cell].ranges.push_back (rd.range);
  
  QValueList<KSpreadPoint> leadings = leadingCells (rd.range);
  QValueList<KSpreadPoint>::iterator it;
  for (it = leadings.begin(); it != leadings.end(); ++it)
    sh->dependencies()->deps->rangeDeps[*it].push_back (rd);
}

void DependencyList::removeDependencies (const KSpreadPoint &cell)
{
  //look if the cell has any dependencies
  if (!dependencies.contains (cell))
    return;  //it doesn't - nothing more to do
  
  //first we remove cell-dependencies
  QValueList<KSpreadPoint> cells = dependencies[cell].cells;
  QValueList<KSpreadPoint>::iterator it1;
  for (it1 = cells.begin(); it1 != cells.end(); ++it1)
  {
    //get sheet-pointer - needed to handle inter-sheet dependencies correctly
    KSpreadSheet *sh = (*it1).sheet;
    if (!sh)
      sh = sheet;
    
    if (!sh->dependencies()->deps->cellDeps.contains (*it1))
      continue;  //this should never happen
    
    //we no longer depend on this cell
    QValueList<KSpreadPoint>::iterator cit;
    cit = sh->dependencies()->deps->cellDeps[*it1].find (cell);
    if (cit != sh->dependencies()->deps->cellDeps[*it1].end())
      sh->dependencies()->deps->cellDeps[*it1].erase (cit);
  }
  
  //then range-dependencies are removed
  QValueList<KSpreadRange> ranges = dependencies[cell].ranges;
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
  {
    //get sheet-pointer - needed to handle inter-sheet dependencies correctly
    KSpreadSheet *sh = (*it1).sheet;
    if (!sh)
      sh = sheet;

    if (sh->dependencies()->deps->rangeDeps.contains (*it1))
    {
      QValueList<RangeDependency>::iterator it3;
      QValueList<RangeDependency> rdeps =
          sh->dependencies()->deps->rangeDeps[*it1];
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
        sh->dependencies()->deps->rangeDeps.erase (*it1);
    }
  }
  
  //finally, remove the entry about this cell
  dependencies[cell].cells.clear();
  dependencies[cell].ranges.clear();
  dependencies.erase (cell);
}

void DependencyList::generateDependencies (const KSpreadPoint &cell)
{
  //get rid of old dependencies first
  removeDependencies (cell);

  //new dependencies only need to be generated if the cell contains a formula
  KSpreadCell *c = sheet->cellAt (cell.column(), cell.row());
  if( c->isDefault() )
    return;
  if (!c->isFormula())
    return;
  
  //now we need to generate dependencies
  RangeList rl = computeDependencies (cell);
  
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
    rd.cellsheet = sheet;
    rd.range = *it2;
    if (rd.range.sheet == 0) rd.range.sheet = sheet;
    addRangeDependency (rd);
  }
}

void DependencyList::generateDependencies (const KSpreadRange &range)
{
  for (int row = range.startRow(); row <= range.endRow(); row++)
    for (int col = range.startCol(); col <= range.endCol(); col++)
    {
      KSpreadPoint c;
      c.setRow (row);
      c.setColumn (col);
      c.sheet = sheet;
      generateDependencies (c);
    }
}

void DependencyList::generateDependencies (const RangeList &rangeList)
{
  QValueList<KSpreadPoint>::const_iterator it1;
  QValueList<KSpreadRange>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    generateDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    generateDependencies (*it2);
}

void DependencyList::processDependencies (const KSpreadPoint &cell)
{
  if (cellDeps.contains (cell))
  {
    QValueList<KSpreadPoint> d = cellDeps[cell];
    QValueList<KSpreadPoint>::iterator it;
    for (it = d.begin(); it != d.end(); ++it)
      updateCell (*it);
  }
  
  processRangeDependencies (cell);
}

void DependencyList::processRangeDependencies (const KSpreadPoint &cell)
{
  KSpreadPoint leading = leadingCell (cell);
  QValueList<RangeDependency>::iterator it;
  if (!rangeDeps.count (leading))
    return;  //no range dependencies in this cell chunk

  for (it = rangeDeps[leading].begin();
      it != rangeDeps[leading].end(); ++it)
  {
    //process all range dependencies, and for each range including the modified cell,
    //recalc the depending cell
    if ((*it).range.contains (cell))
    {
      KSpreadPoint c;
      c.setRow ((*it).cellrow);
      c.setColumn ((*it).cellcolumn);
      c.sheet = (*it).cellsheet;
      updateCell (c);
    }
  }
}

void DependencyList::processDependencies (const KSpreadRange &range)
{
  //each cell's dependencies need to be updated - that cannot be helped - having a range
  //only helps with range dependencies
  for (int row = range.startRow(); row <= range.endRow(); row++)
    for (int col = range.startCol(); col <= range.endCol(); col++)
    {
      KSpreadPoint c;
      c.setRow (row);
      c.setColumn (col);
      c.sheet = sheet;
      if (cellDeps.contains (c))
      {
        QValueList<KSpreadPoint> d = cellDeps[c];
        QValueList<KSpreadPoint>::iterator it;
        for (it = d.begin(); it != d.end(); ++it)
          updateCell (*it);
      }
    }
  
  processRangeDependencies (range);
}

void DependencyList::processRangeDependencies (const KSpreadRange &range)
{
  //TODO: some optimization, so that we don't recompute cells depending of huge
  //ranges more than once (now we recompute them once per cell-chunk used by their dependency)
  //This will probably happen as a part of splitting this into dep manager
  //and recalc manager
  
  QValueList<KSpreadPoint> leadings = leadingCells (range);
  QValueList<KSpreadPoint>::iterator it;
  for (it = leadings.begin(); it != leadings.end(); ++it)
  {
    if (!rangeDeps.count (*it))
      continue;  //no range dependencies in this cell chunk
    QValueList<RangeDependency>::iterator it2;
    for (it2 = rangeDeps[*it].begin(); it2 != rangeDeps[*it].end(); ++it2)
    {
      //process all range dependencies, and for each range intersecting with our range,
      //recalc the depending cell
      if ((*it2).range.intersects (range))
      {
        KSpreadPoint c;
        c.setRow ((*it2).range.startRow());
        c.setColumn ((*it2).range.startCol());
        c.sheet = sheet;
        updateCell (c);
      }
    }
  }
}

void DependencyList::processDependencies (const RangeList &rangeList)
{
  QValueList<KSpreadPoint>::const_iterator it1;
  QValueList<KSpreadRange>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    processDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    processDependencies (*it2);
}

void DependencyList::updateCell (const KSpreadPoint &cell) const
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
  kdDebug() << "Updating depending cell (" <<
      c->row() << "," << c->column() << ")" << endl;
  //set the computing-dependencies flag
  c->setFlag (KSpreadCell::Flag_Progress);
  
  //mark the cell as calc-dirty
  c->setCalcDirtyFlag();
  
  //recalculate the cell
  c->calc (false);
  
  //clear the computing-dependencies flag
  c->clearFlag (KSpreadCell::Flag_Progress);
}

KSpreadPoint DependencyList::leadingCell (const KSpreadPoint &cell) const
{
  KSpreadPoint c;
  c.setRow (cell.row() - cell.row() % CELLCHUNK_ROWS + 1);
  c.setColumn (cell.column() - cell.column() % CELLCHUNK_COLS + 1);
  c.sheet = cell.sheet;
  return c;
}

QValueList<KSpreadPoint> DependencyList::leadingCells (const KSpreadRange &range) const
{
  QValueList<KSpreadPoint> cells;
  KSpreadPoint cell1, cell2, cell;
  
  cell1.setRow (range.startRow());
  cell1.setColumn (range.startCol());
  cell2.setRow (range.endRow());
  cell2.setColumn (range.endCol());
  cell1.sheet = range.sheet;
  cell2.sheet = range.sheet;
  
  cell1 = leadingCell (cell1);
  cell2 = leadingCell (cell2);
  for (int row = cell1.row(); row <= cell2.row(); row += CELLCHUNK_ROWS)
    for (int col = cell1.column(); col <= cell2.column();
        col += CELLCHUNK_COLS)
    {
      cell.setRow (row);
      cell.setColumn (col);
      cell.sheet = range.sheet;
      cells.push_back (cell);
    }
  return cells;
}

RangeList DependencyList::computeDependencies (const KSpreadPoint &cell) const
{
  KSpreadCell *c = cell.cell();
  if (!c->isFormula())
    return RangeList();   //not a formula -> no dependencies

  QString expr = c->text();
  kdDebug(36001) << "Retrieving dependencies for cell with text \"" <<
    expr << "\"" << endl;

  //TODO: when the new parser is in use, KSpreadCell will hold a Formula
  //instance, hence we'll be able to use that one directly
  Tokens tokens = Formula::scan( expr );  

  //return empty list if the tokens aren't valid
  if (!tokens.valid())
    return RangeList();   
  
  RangeList rl;
  for( unsigned i = 0; i < tokens.count(); i++ )
  {
    Token token = tokens[i];
    Token::Type tokenType = token.type();
    
    //parse each cell/range and put it to our RangeList
    if (tokenType == Token::Cell)
    {
      QString text = token.text();
      KSpreadPoint cell (text, sheet->workbook(), sheet);
      if (cell.isValid())
        rl.cells.push_back (cell);
    }
    else if (tokenType == Token::Range)
    {
      QString text = token.text();
      KSpreadRange range (text, sheet->workbook(), sheet);
      if (range.isValid())
        rl.ranges.push_back (range);
    }
  }
  
  return rl;
}

