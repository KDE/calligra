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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, US
*/

#include "presentation.h"
#include "powerpoint.h"

using namespace Libppt;

class Presentation::Private
{
public:
  int test;
};

Presentation::Presentation()
{
  d = new Private;
  d->test = 0;
}
  
Presentation::~Presentation()
{
  delete d;
}

void Presentation::clear()
{
  // TODO
}
  
bool Presentation::load( const char* filename )
{
  PPTReader* reader = new PPTReader;
  bool result = reader->load( this, filename );
  delete reader;
  return result;  
}
