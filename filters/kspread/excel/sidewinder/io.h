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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, US
*/

#ifndef SWINDER_IO_H
#define SWINDER_IO_H

namespace Swinder
{

class Workbook;

class Reader
{
public:

  enum { Ok, CannotOpen, UnknownError };

  Reader(): res( Ok ){};
  
  virtual Workbook* load( const char* filename ) = 0;
    
  unsigned result(){ return res; }
  
  void setResult( unsigned r ){ res = r; }
  
private:

  unsigned res;
    
};

class Writer
{
public:
  enum { Ok, CannotCreate, UnknownError };
  
  Writer(): res( Ok ){};
  
  virtual void save( Workbook* workbook, const char* filename ) = 0;

  unsigned result(){ return res; }
  
  void setResult( unsigned r ){ res = r; }
  
private:

  unsigned res;
    
};

class ReaderFactory
{
public:
  static Reader* createReader( const char* mimetype );
};

class WriterFactory
{
public:
  static Writer* createWriter( const char* mimetype );
};

}; // namespace Swinder


#endif // SWINDER_WORKBOOK_H
