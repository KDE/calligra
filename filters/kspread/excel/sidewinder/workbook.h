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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_WORKBOOK_H
#define SWINDER_WORKBOOK_H

#include "format.h"

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

  
  bool isPasswordProtected() const;  
  void setPasswordProtected( bool p );
  
  void setFormat(int index, const Format& format);
  const Format& format(int index) const;
  
private:
  // no copy or assign
  Workbook( const Workbook& );
  Workbook& operator=( const Workbook& );
  
  class Private;
  Private* d;
};

}


#endif // SWINDER_WORKBOOK_H
