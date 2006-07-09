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
#include "slide.h"
#include "powerpoint.h"

#include <vector>

using namespace Libppt;

class Presentation::Private
{
public:
  Slide* masterSlide;
  std::vector<Slide*> slides;
};

Presentation::Presentation()
{
  d = new Private;
  d->masterSlide = 0;
}
  
Presentation::~Presentation()
{
  clear();
  delete d;
}

void Presentation::clear()
{
  // FIXME use iterator
  for( unsigned i=0; i<slideCount(); i++ )
    delete slide( i );
  d->slides.clear();
  delete d->masterSlide;
  d->masterSlide = 0;
}
  
bool Presentation::load( const char* filename )
{
  PPTReader* reader = new PPTReader;
  bool result = reader->load( this, filename );
  delete reader;
  return result;  
}

void Presentation::appendSlide( Slide* slide )
{
  d->slides.push_back( slide );
}

unsigned Presentation::slideCount() const
{
  return d->slides.size();
}

Slide* Presentation::slide( unsigned index )
{
  if( index >= slideCount() ) return (Slide*)0;
  return d->slides[index];
}


Slide* Presentation::masterSlide()
{
  return d->masterSlide;
}

void Presentation::setMasterSlide( Slide* masterSlide )
{
  delete d->masterSlide;
  d->masterSlide = masterSlide;
}
