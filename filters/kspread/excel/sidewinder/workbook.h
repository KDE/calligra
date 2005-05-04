/* Swinder - Portable library for spreadsheet 
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>

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

#ifndef SWINDER_WORKBOOK_H
#define SWINDER_WORKBOOK_H

namespace Swinder
{

class Sheet;

class Workbook
{
public:

  /*
   * Constructs a new workbook.
   */

  Workbook();
  
  /*
   * Destroys the workbook.
   */
  
  virtual ~Workbook();
  
  /*
   * Clears the workbook, i.e. makes it as if it is just constructed.
   */
  void clear();
  
  /*
   * Loads the workbook from file. Returns false if error occurred.
   */
  bool load( const char* filename );
  
  /*
   * Saves the workbook to file. Returns false if error occurred.
   */
  bool save( const char* filename );
  
  /*
   * Appends a new sheet.
   */
  void appendSheet( Sheet* sheet );
  
  /*
   * Returns the number of worksheet in this workbook. A newly created
   * workbook has no sheet, i.e. sheetCount() returns 0.
   */
  unsigned sheetCount() const;
  
  /*
   * Returns a worksheet at given index. If index is invalid (e.g. larger
   * than total number of worksheet), this function returns NULL.
   */
  Sheet* sheet( unsigned index );
  
  /*
   * Returns true if automatic calculation is enabled.
   */
  bool autoCalc() const;
  
  /*
   * Sets the automatic calculation.
   */
  void setAutoCalc( bool a );
  
private:
  // no copy or assign
  Workbook( const Workbook& );
  Workbook& operator=( const Workbook& );
  
  class Private;
  Private* d;
};

}


#endif // SWINDER_WORKBOOK_H
