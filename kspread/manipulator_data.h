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

/**
 * AbstractDataManipulator - provides storage of old cell data (for undo)
 * and has an abstract method for the actual setting of new values
 * */

namespace KSpread {

class AbstractDataManipulator : public Manipulator {
  public:
    AbstractDataManipulator ();
    ~AbstractDataManipulator ();
    virtual bool process (Element* element);
    /** this abstract method should return a value that will be set
    for a given cell */
  protected:
    /** row/col are relative to sheet, not element */
    virtual Value newValue (Element *element, int col, int row,
      bool *isFormula) = 0;
    /** preProcessing will store the old cell's data */
    virtual bool preProcessing ();
    // has to be like this, so that column/row changes take less memory
    // Value class can handle sparse arrays quite well, but still ...
    QMap<int, QMap<int, Value> > oldData;
    QMap<int, QMap<int, QString> > oldFormula;
};

class DataManipulator : public AbstractDataManipulator {
  
};


} // namespace KSpread

#endif  // KSPREAD_MANIPULATOR_DATA

