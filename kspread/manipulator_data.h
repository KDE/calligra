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


#ifndef KSPREAD_MANIPULATOR_DATA
#define KSPREAD_MANIPULATOR_DATA

#include "manipulator.h"
#include "kspread_value.h"

namespace KSpread {


/**
 * AbstractDataManipulator - provides storage of old cell data (for undo)
 * and has an abstract method for the actual setting of new values
 */

class AbstractDataManipulator : public Manipulator {
  public:
    AbstractDataManipulator ();
    virtual ~AbstractDataManipulator ();
    virtual bool process (Element* element);
    /** this abstract method should return a value that will be set
    for a given cell */
  protected:
    /** Return new value. row/col are relative to sheet, not element.
    If the function sets *parse to true, the value will be treated as an
    user-entered string and parsed by Cell. */
    virtual Value newValue (Element *element, int col, int row,
      bool *parse) = 0;
    /** preProcessing will store the old cell's data */
    virtual bool preProcessing ();
    // has to be like this, so that column/row changes take less memory
    // Value class can handle sparse arrays quite well, but still ...
    QMap<int, QMap<int, Value> > oldData;
    QMap<int, QMap<int, QString> > oldFormula;
};


/**
 * DataManipulator - allows setting values on one range.
 * If multiple ranges are selected, they all get set to the same values
 */

class DataManipulator : public AbstractDataManipulator {
  public:
    DataManipulator ();
    virtual ~DataManipulator ();
    void setParsing (bool val) { m_parsing = val; };
    /** set the values for the range. Can be either a single value, or
    a value array */
    void setValue (Value val) { data = val; };
  protected:
    virtual Value newValue (Element *element, int col, int row, bool *);
    
    Value data;
    bool m_parsing : 1;
};


} // namespace KSpread

#endif  // KSPREAD_MANIPULATOR_DATA

