/* Sidewinder - Portable library for spreadsheet 
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, US
*/

#ifndef SIDEWINDER_SHEET_H
#define SIDEWINDER_SHEET_H

#include "ustring.h"

namespace Sidewinder
{

class Workbook;
class Cell;

class Sheet
{
public:

  Sheet( Workbook* workbook );
  
  virtual ~Sheet();
  
  // get workbook that owns this sheet
  Workbook* workbook();
  
  void setName( const UString& name );
  
  UString name() const;
  
  // return cell at specified column and row 
  // automatically create the cell if previously there is no cell there
  // return NULL if no cell there _and_ autoCreate is false
  Cell* cell( unsigned column, unsigned row, bool autoCreate = true );

  bool visible() const;
  void setVisible( bool v );
  
  bool protect() const;
  void setProtect( bool p );
  
  double leftMargin() const;
  void setLeftMargin( double m );
  
  double rightMargin() const;
  void setRightMargin( double m );
  
  double topMargin() const;
  void setTopMargin( double m );
  
  double bottomMargin() const;
  void setBottomMargin( double m );

  unsigned maxRow() const;
  unsigned maxColumn() const;

private:
  // no copy or assign
  Sheet( const Sheet& );
  Sheet& operator=( const Sheet& );
  
  class Private;
  Private *d;  
};

};

#endif // SIDEWINDER_SHEET_H

