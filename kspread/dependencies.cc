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
class DependencyList {
 public:
  DependencyList (Sheet *s);
  ~DependencyList () { reset (); };
  /** clear internal structures */
  void reset ();

  /** handle the fact that a cell has been changed */
  void cellChanged (const Point &cell);

  /** generate list of dependencies of a cell */
  void generateDependencies (const Point &cell);
  /** generate list of dependencies of a range */
  void generateDependencies (const Range &range);
  /** generate list of dependencies of a range list */
  void generateDependencies (const RangeList &rangeList);

  /** update cells dependending on a given cell */
  void processDependencies (const Point &cell);
  /** update cells dependending on a cell in a given range */
  void processDependencies (const Range &range);
  /** update cells dependending on a given range-list */
  void processDependencies (const RangeList &rangeList);

  /** get dependencies of a cell */
  RangeList getDependencies (const Point &cell);
  /** get cells depending on this cell, either through normal or range dependency */
  QValueList<Point> getDependants (const Point &cell);

  void areaModified (const QString &name);
  protected:
  /** update structures: cell 1 depends on cell 2 */
  void addDependency (const Point &cell1, const Point &cell2);
  /** update structures: cell depends on a range */
  void addRangeDependency (const RangeDependency &rd);
  /** remove all dependencies of a cell */
  void removeDependencies (const Point &cell);
  
  /** update all cells depending on a range containing this cell */
  void processRangeDependencies (const Point &cell);

  /** update all cells depending on a range intersecting with this range */
  void processRangeDependencies (const Range &range);
  
  /** update one cell due to changed dependencies */
  void updateCell (const Point &cell) const;

  /** return a leading cell for a given cell (used to store range
  dependencies effectively) */
  Point leadingCell (const Point &cell) const;
  /** list of leading cells of all cell chunks that this range belongs to */
  QValueList<Point> leadingCells (const Range &range) const;
  /** retrieve a list of cells that a given cell depends on */
  RangeList computeDependencies (const Point &cell) const;
  
  /** Sheet whose dependencies are managed by this instance */
  Sheet *sheet;
  
  /** dependencies of each cell */
  QMap<Point, RangeList> dependencies;
  /** list of cells (but NOT ranges) that depend on a cell */
  QMap<Point, QValueList<Point> > cellDeps;
  /** all range dependencies splitted into cell-chunks (TODO: describe) */
  QMap<Point, QValueList<RangeDependency> > rangeDeps;
  /** list of cells referencing a given named area */
  QMap<QString, QMap<Point, bool> > areaDeps;
};

} // namespace KSpread

using namespace KSpread;

DependencyManager::DependencyManager (Sheet *s)
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

void DependencyManager::cellChanged (const Point &cell)
{
  deps->cellChanged (cell);
  Cell *c = cell.cell();

  // empty or default cell? do nothing
  if( c->isDefault() )
    return;
  
  //if the cell contains the circle error, we mustn't do anything
  if (c->testFlag (Cell::Flag_CircularCalculation))
    return;
  
  //don't re-generate dependencies if we're updating dependencies
  if ( !(c->testFlag (Cell::Flag_Progress)))
    deps->generateDependencies (cell);

  deps->processDependencies (cell);
}

void DependencyManager::rangeChanged (const Range &range)
{
  deps->generateDependencies (range);
  deps->processDependencies (range);
}

void DependencyManager::rangeListChanged (const RangeList &rangeList)
{
  deps->generateDependencies (rangeList);
  deps->processDependencies (rangeList);
}

void DependencyManager::areaModified (const QString &name)
{
  deps->areaModified (name);
}

RangeList DependencyManager::getDependencies (const Point &cell)
{
  return deps->getDependencies (cell);
}

QValueList<Point> DependencyManager::getDependants (const Point &cell)
{
  return deps->getDependants (cell);
}

DependencyList::DependencyList (Sheet *s)
    : sheet (s)
{
}

void DependencyList::reset ()
{
  dependencies.clear();
  cellDeps.clear();
  rangeDeps.clear();
}

void DependencyList::cellChanged (const Point &cell)
{
  Cell *c = cell.cell();

  // empty or default cell? do nothing
  if( c->isDefault() )
    return;
  
  //if the cell contains the circle error, we mustn't do anything
  if (c->testFlag (Cell::Flag_CircularCalculation))
    return;
  
  //don't re-generate dependencies if we're updating dependencies
  if ( !(c->testFlag (Cell::Flag_Progress)))
    generateDependencies (cell);

  processDependencies (cell);
}

RangeList DependencyList::getDependencies (const Point &cell)
{
  RangeList rl;
  //look if the cell has any dependencies
  if (!dependencies.contains (cell))
    return rl;  //it doesn't - return an empty list
  
  //the cell does have dependencies - return them!
  return dependencies[cell];
}

QValueList<Point> DependencyList::getDependants (const Point &cell)
{
  QValueList<Point> list;
  
  //cell dependencies go first
  if (cellDeps.contains (cell))
    list = cellDeps[cell];
  
  //next, append range dependencies
  Point leading = leadingCell (cell);
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
      Point c;
      c.setRow ((*it).cellrow);
      c.setColumn ((*it).cellcolumn);
      c.sheet = (*it).cellsheet;
      list.push_back (c);
    }
  }
  
  return list;
}

void DependencyList::areaModified (const QString &name)
{
  // since area names are something like aliases, modifying an area name
  // basically means that all cells referencing this area should be treated
  // as modified - that will retrieve updated area ranges and also update
  // everything as necessary ...
  if (!areaDeps.contains (name))
    return;
  
  QMap<Point, bool>::iterator it;
  for (it = areaDeps[name].begin(); it != areaDeps[name].end(); ++it)
  {
    Cell *c = it.key().cell();
    // this forces the cell to regenerate everything - new range dependencies
    // and so on
    c->setValue (c->value ());
  }
}

void DependencyList::addDependency (const Point &cell1,
    const Point &cell2)
{
  //cell2 can be in another sheet (inter-sheet dependency)
  Sheet *sh = cell2.sheet;
  if (!sh)
    sh = sheet;

  dependencies[cell1].cells.push_back (cell2);
  sh->dependencies()->deps->cellDeps[cell2].push_back (cell1);
}

void DependencyList::addRangeDependency (const RangeDependency &rd)
{
  //target range can be in another sheet (inter-sheet dependency)
  Sheet *sh = rd.range.sheet;
  if (!sh)
    sh = sheet;
  
  Point cell;
  cell.sheet = rd.cellsheet;
  cell.setRow (rd.cellrow);
  cell.setColumn (rd.cellcolumn);
  dependencies[cell].ranges.push_back (rd.range);
  
  QValueList<Point> leadings = leadingCells (rd.range);
  QValueList<Point>::iterator it;
  for (it = leadings.begin(); it != leadings.end(); ++it)
    sh->dependencies()->deps->rangeDeps[*it].push_back (rd);

  // the target range could be a named area ...
  if (!rd.range.namedArea.isNull())
    areaDeps[rd.range.namedArea][cell] = true;
}

void DependencyList::removeDependencies (const Point &cell)
{
  //look if the cell has any dependencies
  if (!dependencies.contains (cell))
    return;  //it doesn't - nothing more to do

  //first we remove cell-dependencies
  QValueList<Point> cells = dependencies[cell].cells;
  QValueList<Point>::iterator it1;
  for (it1 = cells.begin(); it1 != cells.end(); ++it1)
  {
    //get sheet-pointer - needed to handle inter-sheet dependencies correctly
    Sheet *sh = (*it1).sheet;
    if (!sh)
      sh = sheet;
    
    if (!sh->dependencies()->deps->cellDeps.contains (*it1))
      continue;  //this should never happen
    
    //we no longer depend on this cell
    QValueList<Point>::iterator cit;
    cit = sh->dependencies()->deps->cellDeps[*it1].find (cell);
    if (cit != sh->dependencies()->deps->cellDeps[*it1].end())
      sh->dependencies()->deps->cellDeps[*it1].erase (cit);
  }
  
  //then range-dependencies are removed
  QValueList<Range> ranges = dependencies[cell].ranges;
  QValueList<Range>::iterator it2;
  QValueList<Point> leads;
  for (it2 = ranges.begin(); it2 != ranges.end(); ++it2)
  {
    //we construct a list of cell-chunks containing a range and merge it
    //with lists generated from all previous ranges (duplicates are removed)
    QValueList<Point> leadings = leadingCells (*it2);
    for (it1 = leadings.begin(); it1 != leadings.end(); ++it1)
      if (!leads.contains (*it1))
        leads.push_back (*it1);
  }
  for (it1 = leads.begin(); it1 != leads.end(); ++it1)
  {
    //get sheet-pointer - needed to handle inter-sheet dependencies correctly
    Sheet *sh = (*it1).sheet;
    if (!sh)
      sh = sheet;

    if (sh->dependencies()->deps->rangeDeps.contains (*it1))
    {
      QValueList<RangeDependency>::iterator it3;
      it3 = sh->dependencies()->deps->rangeDeps[*it1].begin();
      //erase all range dependencies of this cell in this cell-chunk
      while (it3 != sh->dependencies()->deps->rangeDeps[*it1].end())
        if (((*it3).cellrow == cell.row()) &&
            ((*it3).cellcolumn == cell.column()))
          it3 = sh->dependencies()->deps->rangeDeps[*it1].erase (it3);
        else
          ++it3;
      //erase the list if we no longer need it
      if (sh->dependencies()->deps->rangeDeps[*it1].empty())
        sh->dependencies()->deps->rangeDeps.erase (*it1);
    }
  }
  
  // remove information about named area dependencies
  QMap<QString, QMap<Point, bool> >::iterator itr;
  for (itr = areaDeps.begin(); itr != areaDeps.end(); ++itr) {
    if (itr.data().contains (cell))
      itr.data().remove (cell);
  }

  // finally, remove the entry about this cell
  dependencies[cell].cells.clear();
  dependencies[cell].ranges.clear();
  dependencies.erase (cell);
}

void DependencyList::generateDependencies (const Point &cell)
{
  //get rid of old dependencies first
  removeDependencies (cell);

  //new dependencies only need to be generated if the cell contains a formula
  Cell *c = sheet->cellAt (cell.column(), cell.row());
  if( c->isDefault() )
    return;
  if (!c->isFormula())
    return;
 
  //now we need to generate dependencies
  RangeList rl = computeDependencies (cell);
  
  //now that we have the new dependencies, we put them into our data structures
  //and we're done
  QValueList<Point>::iterator it1;
  QValueList<Range>::iterator it2;
  
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

void DependencyList::generateDependencies (const Range &range)
{
  for (int row = range.startRow(); row <= range.endRow(); row++)
    for (int col = range.startCol(); col <= range.endCol(); col++)
    {
      Point c;
      c.setRow (row);
      c.setColumn (col);
      c.sheet = sheet;
      generateDependencies (c);
    }
}

void DependencyList::generateDependencies (const RangeList &rangeList)
{
  QValueList<Point>::const_iterator it1;
  QValueList<Range>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    generateDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    generateDependencies (*it2);
}

void DependencyList::processDependencies (const Point &cell)
{
  if (cellDeps.contains (cell))
  {
    QValueList<Point> d = cellDeps[cell];
    QValueList<Point>::iterator it;
    for (it = d.begin(); it != d.end(); ++it)
      updateCell (*it);
  }
  
  processRangeDependencies (cell);
}

void DependencyList::processRangeDependencies (const Point &cell)
{
  Point leading = leadingCell (cell);
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
      Point c;
      c.setRow ((*it).cellrow);
      c.setColumn ((*it).cellcolumn);
      c.sheet = (*it).cellsheet;
      updateCell (c);
    }
  }
}

void DependencyList::processDependencies (const Range &range)
{
  //each cell's dependencies need to be updated - that cannot be helped - having a range
  //only helps with range dependencies
  for (int row = range.startRow(); row <= range.endRow(); row++)
    for (int col = range.startCol(); col <= range.endCol(); col++)
    {
      Point c;
      c.setRow (row);
      c.setColumn (col);
      c.sheet = sheet;
      if (cellDeps.contains (c))
      {
        QValueList<Point> d = cellDeps[c];
        QValueList<Point>::iterator it;
        for (it = d.begin(); it != d.end(); ++it)
          updateCell (*it);
      }
    }
  
  processRangeDependencies (range);
}

void DependencyList::processRangeDependencies (const Range &range)
{
  //TODO: some optimization, so that we don't recompute cells depending of huge
  //ranges more than once (now we recompute them once per cell-chunk used by their dependency)
  //This will probably happen as a part of splitting this into dep manager
  //and recalc manager
  
  QValueList<Point> leadings = leadingCells (range);
  QValueList<Point>::iterator it;
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
        Point c;
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
  QValueList<Point>::const_iterator it1;
  QValueList<Range>::const_iterator it2;
  
  for (it1 = rangeList.cells.begin(); it1 != rangeList.cells.end(); ++it1)
    processDependencies (*it1);
  for (it2 = rangeList.ranges.begin(); it2 != rangeList.ranges.end(); ++it2)
    processDependencies (*it2);
}

void DependencyList::updateCell (const Point &cell) const
{
  Cell *c = cell.cell();
  //prevent infinite recursion (circular dependencies)
  if (c->testFlag (Cell::Flag_Progress))
  {
    kdError(36001) << "ERROR: Circle" << endl;
    c->setFlag(Cell::Flag_CircularCalculation);
    Value v;
    v.setError ( "####" );
    c->setValue (v);
    //clear the computing-dependencies flag
    c->clearFlag (Cell::Flag_Progress);
    return;
  }
  //set the computing-dependencies flag
  c->setFlag (Cell::Flag_Progress);
  
  //mark the cell as calc-dirty
  c->setCalcDirtyFlag();
  
  //recalculate the cell
  c->calc (false);
  
  //clear the computing-dependencies flag
  c->clearFlag (Cell::Flag_Progress);
}

Point DependencyList::leadingCell (const Point &cell) const
{
  Point c;
  c.setRow (cell.row() - cell.row() % CELLCHUNK_ROWS + 1);
  c.setColumn (cell.column() - cell.column() % CELLCHUNK_COLS + 1);
  c.sheet = cell.sheet;
  return c;
}

QValueList<Point> DependencyList::leadingCells (const Range &range) const
{
  QValueList<Point> cells;
  Point cell1, cell2, cell;
  
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

RangeList DependencyList::computeDependencies (const Point &cell) const
{
  Cell *c = cell.cell();

  // Not a formula -> no dependencies
  if (!c->isFormula())
    return RangeList();

  // Broken formula -> meaningless dependencies
  // (tries to avoid c->formula() being null)
  if (c->hasError())
    return RangeList();

  Tokens tokens = c->formula()->tokens();

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
      Point cell (text, sheet->workbook(), sheet);
      if (cell.isValid())
        rl.cells.push_back (cell);
    }
    else if (tokenType == Token::Range)
    {
      QString text = token.text();
      Range range (text, sheet->workbook(), sheet);
      if (range.isValid())
        rl.ranges.push_back (range);
    }
  }
  
  return rl;
}

