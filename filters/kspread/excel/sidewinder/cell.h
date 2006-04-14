/* Swinder - Portable library for spreadsheet 
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_CELL_H
#define SWINDER_CELL_H

#include "ustring.h"
#include "format.h"
#include "value.h"

namespace Swinder
{

class Workbook;
class Sheet;

// don't export this private class
class CellPrivate;

class Cell
{
public:

  Cell( Sheet* sheet, unsigned column, unsigned row );
  
  virtual ~Cell();
  
  Sheet* sheet();
  
  unsigned column() const;
  
  unsigned row() const;
  
  UString name() const;
  
  static UString name( unsigned column, unsigned row );
  
  UString columnLabel() const;
  
  static UString columnLabel( unsigned column );
  
  Value value() const;
  
  void setValue( const Value& value );
  
  UString formula() const;
  
  void setFormula( const UString& formula );
  
  Format format() const;
  
  void setFormat( const Format& format );
  
  unsigned columnSpan() const;
  
  void setColumnSpan( unsigned span );
  
  unsigned rowSpan() const;
  
  void setRowSpan( unsigned span );  
  
private:
  // no copy or assign
  Cell( const Cell& );
  Cell& operator=( const Cell& );
  
  CellPrivate *d;
};

} // namespace Swinder


#endif // SWINDER_CELL_H

