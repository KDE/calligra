/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
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

// Local
#include "Function.h"

#include "Value.h"

using namespace KSpread;

class Function::Private
{
public:
  QString name;
    QString alternateName;
  FunctionPtr ptr;
  int paramMin, paramMax;
  bool acceptArray;
  bool ne;   // need FunctionExtra* when called ?
};

Function::Function( const QString& name, FunctionPtr ptr )
    : d( new Private )
{
  d->name = name;
  d->ptr = ptr;
  d->acceptArray = false;
  d->paramMin = 1;
  d->paramMax = 1;
  d->ne = false;
}

Function::~Function()
{
  delete d;
}

QString Function::name() const
{
  return d->name;
}

QString Function::alternateName() const
{
    return d->alternateName;
}

void Function::setAlternateName(const QString &name)
{
    d->alternateName = name;
}

void Function::setParamCount (int min, int max)
{
  d->paramMin = min;
  d->paramMax = (max == 0) ? min : max;
}

bool Function::paramCountOkay (int count)
{
  // less than needed
  if (count < d->paramMin) return false;
  // no upper limit
  if (d->paramMax == -1) return true;
  // more than needed
  if (count > d->paramMax) return false;
  // okay otherwise
  return true;
}

void Function::setAcceptArray (bool accept) {
  d->acceptArray = accept;
}

bool Function::needsExtra () {
  return d->ne;
}
void Function::setNeedsExtra (bool extra) {
  d->ne = extra;
}

Value Function::exec (valVector args, ValueCalc *calc, FuncExtra *extra)
{
  // check number of parameters
  if (!paramCountOkay (args.count()))
    return Value::errorVALUE();

  if( extra )
    extra->function = this;

  // do we need to perform array expansion ?
  bool mustExpandArray = false;
  if (!d->acceptArray)
    for (int i = 0; i < args.count(); ++i) {
      if (args[i].isArray())
        mustExpandArray = true;
    }

  if( !d->ptr ) return Value::errorVALUE();

  // perform the actual array expansion if need be

  if (mustExpandArray) {
    // compute number of rows/cols of the result
    int rows = 0;
    int cols = 0;
    for (int i = 0; i < args.count(); ++i) {
      int x = 1;
      if (extra) x = extra->ranges[i].rows();
      if (x > rows) rows = x;
      if (extra) x = extra->ranges[i].columns();
      if (x > cols) cols = x;
    }
    // allocate the resulting array
    Value res( Value::Array );
    // perform the actual computation for each element of the array
    for (int row = 0; row < rows; ++row)
      for (int col = 0; col < cols; ++col) {
        // fill in the parameter vector
        valVector vals (args.count());
        FuncExtra extra2 = *extra;
        for (int i = 0; i < args.count(); ++i) {
          int r = extra->ranges[i].rows();
          int c = extra->ranges[i].columns();
          vals[i] = args[i].isArray() ?
              args[i].element (col % c, row % r): args[i];

          // adjust the FuncExtra structure to refer to the correct cells
          extra2.ranges[i].col1 += col;
          extra2.ranges[i].row1 += row;
          extra2.ranges[i].col2 = extra2.ranges[i].col1;
          extra2.ranges[i].row2 = extra2.ranges[i].row1;
        }
        // execute the function on each element
        res.setElement (col, row, exec (vals, calc, &extra2));
      }
    return res;
  }
  else
    // call the function
    return (*d->ptr) (args, calc, extra);
}
