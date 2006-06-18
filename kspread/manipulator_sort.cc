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


#include "manipulator_sort.h"

#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "valuecalc.h"
#include "valueconverter.h"

#include <klocale.h>

using namespace KSpread;

SortManipulator::SortManipulator ()
{
  m_changeformat = false;
  m_rows = true;
  m_skipfirst = false;
  m_cs = false;
  m_usecustomlist = false;

  m_name = i18n ("Sort Data");
}

SortManipulator::~SortManipulator ()
{
}

bool SortManipulator::process (Element* element)
{
  // process one element - rectangular range
  
  // here we perform the actual sorting, remember the new ordering and
  // let AbstractDFManipulator::process do the rest of the work
  // the new ordering is used in newValue and newFormat to return proper
  // values
  
  // sort
  sort (element);

  // set values
  return AbstractDFManipulator::process (element);
}

void SortManipulator::addSortBy (int v, bool asc)
{
  m_sortby.push_back (v);
  m_sortorder.push_back (asc);
}

void SortManipulator::clearSortOrder ()
{
  m_sortby.clear ();
  m_sortorder.clear ();
}

Value SortManipulator::newValue (Element *element, int col, int row,
    bool *parse, FormatType *)
{
  QRect range = element->rect();
  int colidx = col - range.left();
  int rowidx = row - range.top();
  if (m_rows)  // sort rows
    rowidx = sorted[rowidx];
  else
    colidx = sorted[colidx];

  // have to return stored value, to avoid earlier calls disrupting latter ones
  Value val = oldData[colidx][rowidx].val;
  QString text = oldData[colidx][rowidx].text;
  *parse = false;
  if (!text.isEmpty()) {
    val = Value (text);
    *parse = true;
  }
  return val;
}

Format SortManipulator::newFormat (Element *element, int col, int row)
{
  QRect range = element->rect();
  int colidx = col - range.left();
  int rowidx = row - range.top();
  if (m_changeformat) {
    if (m_rows)  // sort rows
      rowidx = sorted[rowidx];
    else
      colidx = sorted[colidx];
  }
 
  // have to return stored format, to avoid earlier calls disrupting latter ones
  return formats[colidx][rowidx].format;
}

void SortManipulator::sort (Element *element)
{
  // we'll use insert-sort to sort
  QRect range = element->rect();
  int max = m_rows ? range.bottom() : range.right();
  int min = m_rows ? range.top() : range.left();
  int count = max - min + 1;
  
  // initially, all values are at their original positions
  sorted.clear ();
  for (int i = 0; i < count; ++i) sorted[i] = i;
  
  // for each position, find the lowest value and move it there
  int start = m_skipfirst ? 1 : 0;
  for (int i = start; i < count - 1; ++i) {
    int lowest = i;
    for (int j = i+1; j < count; ++j)
      if (shouldReorder (element, sorted[lowest], sorted[j]))
        lowest = j;
    // move lowest to start
    int tmp = sorted[i];
    sorted[i] = sorted[lowest];
    sorted[lowest] = tmp;
  }
  
  // that's all - process will take care of the rest, together with our
  // newValue/newFormat
}

bool SortManipulator::shouldReorder (Element *element, int first, int second)
{
  // we use ValueCalc::natural* to compare
  // indexes are real indexes, we don't use the sorted array here

  ValueCalc *calc = m_sheet->doc()->calc();
  ValueConverter *conv = m_sheet->doc()->converter();

  QRect range = element->rect();
  int firstrow = range.top();
  int firstcol = range.left();

  QList<int>::iterator it = m_sortby.begin();
  QList<bool>::iterator it2 = m_sortorder.begin();
  for (; it != m_sortby.end(); ++it, ++it2) {
    int which = *it;
    int ascending = *it2;
    // figure out coordinates of the cells
    int row1 = firstrow + (m_rows ? first : which);
    int row2 = firstrow + (m_rows ? second : which);
    int col1 = firstcol + (m_rows ? which : first);
    int col2 = firstcol + (m_rows ? which : second);
    Value val1 = m_sheet->value (col1, row1);
    Value val2 = m_sheet->value (col2, row2);
    // empty values always go to the end, so if second value is empty and
    // first one is not, we don't need to reorder
    if ((!val1.isEmpty()) && val2.isEmpty())
      return false;
    
    // custom list ?
    if (m_usecustomlist) {
      QString s1 = conv->asString (val1).asString().toLower();
      QString s2 = conv->asString (val2).asString().toLower();
      QStringList::iterator it;
      int pos1 = -1, pos2 = -1;
      int pos = 0;
      // Try to locate our two strings in the list. If both are there, assume
      // ordering as specified by the list.
      for (it = m_customlist.begin(); it != m_customlist.end(); ++it) {
        if ((pos1 == -1) && ((*it).toLower() == s1))
          pos1 = pos;
        if ((pos2 == -1) && ((*it).toLower() == s2))
          pos2 = pos;
        pos++;
      }
      if ((pos1 >= 0) && (pos2 >= 0) && (pos1 != pos2))
        // both are in the list, not the same
        return (pos1 > pos2);
    }
    
    if (calc->naturalGreater (val1, val2, m_cs))
      // first one greater - must reorder if ascending, don't reorder if not
      return ascending;
    if (calc->naturalLower (val1, val2, m_cs))
      // first one lower - don't reorder if ascending, reorder if not
      return !ascending;
    // equal - don't know yet, continue
  }

  // no difference found, they're the same - no need to reorder
  return false;
}
