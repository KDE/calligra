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


#ifndef KSPREAD_MANIPULATOR_SORT
#define KSPREAD_MANIPULATOR_SORT



#include "DataManipulators.h"

namespace KSpread {


/** SortManipulator - sorts the range that it gets, using various criteria */

class KSPREAD_EXPORT SortManipulator : public AbstractDFManipulator {
  public:
    SortManipulator ();
    virtual ~SortManipulator ();
    
    virtual bool process (Element* element);

    /** true if rows are to be sorted, false if columns are */
    void setSortRows (bool v) { m_rows = v; };
    /** skip first row/column, as it contains headers */
    void setSkipFirst (bool v) { m_skipfirst = v; };
    /** case sensitive sorting */
    void setCaseSensitive (bool v) { m_cs = v; };
    /** set whether cell formats should be moved with data */
    void setCopyFormat (bool v) { setChangeFormat (v); };
    /** set whether we will use a custom list */
    void setUseCustomList (bool v) { m_usecustomlist = v; };
    /** set a custom list that will be used */
    void setCustomList (QStringList l) { m_customlist = l; };

    /** Add new sort type. Indexed from 0. Sort types are used in order
    in which they're added. */
    void addSortBy (int v, bool asc);
    void clearSortOrder ();
  
  protected:
    virtual Value newValue (Element *element, int col, int row,
        bool *parse, FormatType *fmtType);
    virtual Format *newFormat (Element *element, int col, int row);

    /** sort the data, filling the "sorted" structure */
    void sort (Element *element);
    bool shouldReorder (Element *element, int first, int second);

    bool m_rows, m_skipfirst, m_usecustomlist, m_cs;
    QStringList m_customlist;
    QList<int> m_sortby;
    QList<bool> m_sortorder;
    
    /** sorted order - which row/column will move to where */
    QMap<int, int> sorted;
};

} // namespace KSpread

#endif  // KSPREAD_MANIPULATOR_SORT
