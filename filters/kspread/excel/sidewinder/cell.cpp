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

#include "cell.h"

#include "ustring.h"
#include "format.h"
#include "value.h"

#include <iostream>

namespace Swinder
{

class CellPrivate
{
public:
  Sheet* sheet;
  unsigned column;  
  unsigned row;     
  UString formula;
  Value value;
  Format* format;
  int formatIndex;
  unsigned columnSpan;
  unsigned rowSpan;

  static UString columnNames[256];

  CellPrivate(Sheet* s, unsigned column, unsigned row);  
  ~CellPrivate() { delete format; }
};

}

using namespace Swinder;

UString CellPrivate::columnNames[256];

  
CellPrivate::CellPrivate(Sheet* s, unsigned c, unsigned r):
sheet(s), column(c), row(r), format(0), formatIndex(-1), columnSpan(0), rowSpan(0)
{
}

Cell::Cell( Sheet* sheet, unsigned column, unsigned row )
{
  d = new CellPrivate(sheet, column, row);
}

Cell::~Cell()
{
  delete d;
}

Sheet* Cell::sheet()
{
  return d->sheet;
}

unsigned Cell::column() const
{
  return d->column;
}

unsigned Cell::row() const
{
  return d->row;
}

UString Cell::name() const
{
  return name( column(), row() ); 
}

UString Cell::name( unsigned column, unsigned row )
{
	// column=0, row=0 is "A1"
  return columnLabel( column ) + UString::from( row + 1 );  
}

UString Cell::columnLabel() const
{
  return columnLabel( column() );
}


UString Cell::columnLabel( unsigned column )
{
  UString label;
  
  // Excel has only up to 256 columns, so we cache those
  if(column < 256 )
  {
    label = CellPrivate::columnNames[column];

    // cache is not ready, so construct it first
    if(label.isEmpty())
    {
      for(unsigned c = 0; c < 26; c++)
        CellPrivate::columnNames[c] = UString(UChar('A'+c));
      for(unsigned d = 0; d < 256-26; d++)
      {
        char buf[3] = { 'A'+(d/26), 'A'+(d%26), 0};
        CellPrivate::columnNames[d+26] = UString(buf);
      }
      
      label = CellPrivate::columnNames[column];
    }
  }
  else
  {
    // otherwise, find with slower method
      
    // how many characters for the column name?  
    unsigned digits = 1;
    unsigned offset = 0;
    for( unsigned limit = 26; column-offset >= limit; limit *= 26, digits++ )
	    offset += limit;

    // extreme case e.g. column 4294967295 is "AATYHWUR" (8 characters)
    if(digits < 9)
    {
      char ref[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      
      // from the last character
      char* colp = ref + 9 - 1;
      for( unsigned c = column - offset; digits; --digits, c/=26, colp-- )
        *colp = char('A' + (c%26));
      
      label = UString((const char*)(colp+1));
    }
  }
  
  return label;
}

const Value& Cell::value() const
{
  return d->value;
}

void Cell::setValue( const Value& value )
{
  d->value = value;
}

const UString& Cell::formula() const
{
  return d->formula;
}

void Cell::setFormula( const UString& formula )
{
  d->formula = formula;
}

int Cell::formatIndex() const
{
  return d->formatIndex;
}

void Cell::setFormatIndex( int index )
{
  d->formatIndex = index;
}

Format Cell::format() const
{
  if(!d->format)
    d->format = new Format();
    
  return Format(*d->format);
}

void Cell::setFormat( const Format& format )
{
  if(!d->format)
    d->format = new Format();
    
  (*d->format) = format;
}

unsigned Cell::columnSpan() const
{
  return d->columnSpan;
}

void Cell::setColumnSpan( unsigned span )
{
  if( span < 1 ) return;
  d->columnSpan = span;
}

unsigned Cell::rowSpan() const
{
  return d->rowSpan;
}

void Cell::setRowSpan( unsigned span )
{
  if( span < 1 ) return;
  d->rowSpan = span;
}
