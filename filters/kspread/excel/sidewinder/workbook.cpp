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

#include "workbook.h"
#include "sheet.h"
#include "excel.h"

#include <iostream>
#include <vector>

using namespace Swinder;

class Workbook::Private
{
public:
  std::vector<Sheet*> sheets;
  bool autoCalc;
};

Workbook::Workbook()
{
  d = new Workbook::Private();
  d->autoCalc = true;
}

Workbook::~Workbook()
{
  clear();
  delete d;
}

void Workbook::clear()
{
  // FIXME use iterator
  for( unsigned i=0; i<sheetCount(); i++ )
  {
    Sheet* s = sheet( i );
    delete s;
    }
  d->sheets.clear();
}

bool Workbook::load( const char* filename )
{
  ExcelReader* reader = new ExcelReader;
  bool result = reader->load( this, filename );
  delete reader;
  return result;
}

bool Workbook::save( const char* filename )
{
  return true;
}

void Workbook::appendSheet( Sheet* sheet )
{
  d->sheets.push_back( sheet );
}

unsigned Workbook::sheetCount() const
{
  return d->sheets.size();
}

Sheet* Workbook::sheet( unsigned index )
{
  if( index >= sheetCount() ) return (Sheet*)0;
  return d->sheets[index];
}

bool Workbook::autoCalc() const
{
  return d->autoCalc;
}

void Workbook::setAutoCalc( bool a )
{
  d->autoCalc = a;
}

