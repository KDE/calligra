/* libppt - library to read PowerPoint presentation
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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

#include "presentation.h"

#include <iostream>

int main( int argc, char** argv )
{
  if( argc < 2 )
  {
    std::cout << "Usage:" << std::endl;
    std::cout << "  testppt filename" << std::endl;
    return -1;
  }
  
  char* filename = argv[1];
  
  
  Libppt::Presentation* p;
  p = new Libppt::Presentation;
  p->load( filename );
  delete p; 
  
  return 0;
}
