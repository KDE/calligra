/* This file is part of the KDE project
   Copyright 2007 Tomas Mecir <mecirt@gmail.com>

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

#ifndef KSPREAD_NUMBER_H
#define KSPREAD_NUMBER_H

#include <QSharedDataPointer>

#include "kspread_export.h"

namespace KSpread {

/**
The Number class holds a single number. The class will support integers, doubles, complex numbers, and eventually also high-precision numbers from GnuMP.

The class will be able to format itself into a string, using provided locale settings.

Out-of-class methods for computations will be provided, so that objects of the class may be used just like regular number variables, and so that the caller needs not worry whether the number is an integer, double, complex, or anything else.
*/

class KSPREAD_EXPORT Number {
  public:
    // TODO
  private:
    class Private;
    QSharedDataPointer<Private> d;

};  // class Number

} // namespace KSpread

#endif // KSPREAD_NUMBER_H
