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
#include "kspread_global.h"
#include "kspread_value.h"

namespace KSpread {


/**
 * AbstractDataManipulator - provides storage of old cell data (for undo)
 * and has an abstract method for the actual setting of new values
 */

struct ADMStorage {
  Value val;
  QString text;
  FormatType format;
};

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
      bool *parse, FormatType *fmtType) = 0;
    /** preProcessing will store the old cell's data */
    virtual bool preProcessing ();
    QMap<int, QMap<int, ADMStorage> > oldData;
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
    /** If set, all cells shall be switched to this format. Will do
    nothing if parsing is true. */
    void setFormat (FormatType fmtType) { m_format = fmtType; };
  protected:
    virtual Value newValue (Element *element, int col, int row, bool *,
        FormatType *);
    
    Value data;
    FormatType m_format;
    bool m_parsing : 1;
};

class ArrayFormulaManipulator : public AbstractDataManipulator {
  public:
    ArrayFormulaManipulator ();
    virtual ~ArrayFormulaManipulator ();
    void setText (const QString text) { m_text = text; };
  protected:
    virtual Value newValue (Element *element, int col, int row, bool *,
        FormatType *);
    QString cellRef, m_text;
};


/** class ProtectedCheck can be used to check, whether a particular
  range is protected or not */
class ProtectedCheck : public Region {
  public:
    ProtectedCheck ();
    virtual ~ProtectedCheck ();
    void setSheet (Sheet *sheet) { m_sheet = sheet; };
    bool check ();
  protected:
    Sheet *m_sheet;
};


} // namespace KSpread

#endif  // KSPREAD_MANIPULATOR_DATA

