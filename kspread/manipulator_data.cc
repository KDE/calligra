/* This file is part of the KDE project
   Copyright (C) 2006 Tomas Mecir <mecirt@gmail.com>

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


#include "manipulator_data.h"

#include "kspread_cell.h"
#include "kspread_sheet.h"

using namespace KSpread;

AbstractDataManipulator::AbstractDataManipulator ()
{
  m_format = false;
}

AbstractDataManipulator::~AbstractDataManipulator ()
{
  oldData.clear ();
  oldFormula.clear ();
}

bool AbstractDataManipulator::process (Element* element)
{
  QRect range = element->rect().normalize();
  for (int col = range.left(); col <= range.right(); ++col)
    for (int row = range.top(); row <= range.bottom(); ++row) {
      Value val;
      QString formula;
      bool isFormula = false;
      if (m_reverse) {
        // reverse - use the stored value
        if (oldData.contains (col) && oldData[col].contains (row)) {
          val = oldData[col][row];
        } else if (oldFormula.contains (col) &&
                   oldFormula[col].contains (row)) {
          formula = oldFormula[col][row];
          isFormula = true;
        }
      } else {
        val = newValue (element, col, row, &isFormula);
        if (isFormula)
          formula = val.asString();
      }
      
      // we have the data - set it !
      if (isFormula) {
        Cell *cell = m_sheet->nonDefaultCell (col, row);
        cell->setCellText (cell->decodeFormula (formula));
      } else {
        Cell *cell = m_sheet->cellAt (col, row);
        if ((!val.isEmpty()) || cell)  // nothing if value and cell both empty
          m_sheet->nonDefaultCell (col, row)->setValue (val);
      }
    }
  return true;
}

bool AbstractDataManipulator::preProcessing ()
{
  // not the first run - data already stored ...
  if (!m_firstrun) return true;
  
  Region::Iterator endOfList(cells().end());
  for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalize();
    for (int col = range.left(); col <= range.right(); ++col)
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell* cell = m_sheet->cellAt(col, row);
        if (cell != m_sheet->defaultCell())  // non-default cell - remember it
        {
          if (cell->isFormula())
          {
            QString f = cell->encodeFormula();
            oldFormula[col][row] = f;
          }
          else {
            Value val = m_sheet->value (col, row);
            if (!val.isEmpty())
              oldData[col][row] = val;
          }
        }
      }
  }
  return true;
}

DataManipulator::DataManipulator ()
{
}

DataManipulator::~DataManipulator ()
{
}

Value DataManipulator::newValue (Element *element, int col, int row,
    bool *isFormula)
{
  *isFormula = false;
  QRect range = element->rect().normalize();
  int colidx = range.left() - col;
  int rowidx = range.top() - row;
  return data.element (colidx, rowidx);
}

