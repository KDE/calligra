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

#include "slide.h"
#include "presentation.h"
#include "ustring.h"
#include "objects.h"

#include <vector>
#include <iostream>


using namespace Libppt;

class Slide::Private
{
public:
  Presentation* presentation;
  UString title;
  GroupObject* rootObject;

  double pageWidth; 
  double pageHeight;
};

Slide::Slide( Presentation* pr )
{
  d = new Private;
  d->presentation = pr;
  d->rootObject = new GroupObject;
  d->pageWidth = 0.0; 
  d->pageHeight = 0.0;
}
  
Slide::~Slide()
{
  delete d->rootObject;
  delete d;
}

void Slide::clear()
{
  d->title = UString::null;
  setRootObject( 0 );
  d->rootObject = new GroupObject;
}

UString Slide::title() const
{
  return d->title;
}

void Slide::setTitle( const UString& t )
{
  UChar* s = new UChar[t.length()];
  int len = 0;

  // filter crazy characters
  for( int i=0; i<t.length(); i++ )
    if( t[i] != UChar(11) )
      s[len++] = t[i];

  d->title = UString( s, len );
  delete [] s;
}

GroupObject *Slide::rootObject()
{
  return d->rootObject;
}

void Slide::setRootObject( GroupObject* root )
{
  delete d->rootObject;
  d->rootObject = root;
}

TextObject* recursiveSearch( GroupObject* group, unsigned placeId )
{
  if( group )
    for( unsigned i=0; i<group->objectCount(); i++ )
    {
      Object* object = group->object(i);
      if( object->isText() )
      {
        TextObject* textObject = static_cast<TextObject*>(object);
        if( textObject)
          if( textObject->id() == (int)placeId )
            return textObject;
      }
      if( object->isGroup() )
        return recursiveSearch( static_cast<GroupObject*>(object), placeId );
    }

  return 0;
}


TextObject* Slide::textObject( unsigned placeId )
{
  return recursiveSearch( d->rootObject, placeId );
}

double Slide::pageWidth() const
{ 
  return d->pageWidth; 
}

void Slide::setPageWidth( double pageWidth ) 
{  
  d->pageWidth = pageWidth; 
}

double Slide::pageHeight() const
{
  return d->pageHeight; 
}

void Slide::setPageHeight( double pageHeight ) 
{
  d->pageHeight = pageHeight; 
}
