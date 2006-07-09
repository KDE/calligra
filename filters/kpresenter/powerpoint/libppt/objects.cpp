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

#include "objects.h"
#include "ustring.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace Libppt;

class PropertyValue
{
public:
  enum { InvalidType,  IntType, DoubleType, StringType, BoolType, ColorType } type;
  bool b;
  int i;
  double d; 
  std::string s;
  Color c;

  PropertyValue()
  {
    type = InvalidType;
    b = false;
    i = 0;
    d = 0.0;
     
    
  }
};

class Object::Private
{
public:
  int id;
  double top;
  double left;
  double width;
  double height;
  bool background;
  std::map<std::string,PropertyValue> properties;
};

Object::Object()
{
  d = new Private;
  d->id = -1;
  d->top = 0.0;
  d->left = 0.0;
  d->width = 10.0;
  d->height = 3.0;
  d->background = false;
}

Object::~Object()
{
  delete d;
}

int Object::id() const
{
  return d->id;
}

void Object::setId( int id )
{
  d->id = id;
}

double Object::top() const
{
  return d->top;
}

double Object::left() const
{
  return d->left;
}

double Object::width() const
{
  return d->width;
}

double Object::height() const
{
  return d->height;
}

void Object::setTop( double top )
{
  d->top = top;
}

void Object::setLeft( double left )
{
  d->left = left;
}

void Object::setWidth( double width )
{
  d->width = width;
}

void Object::setHeight( double height)
{
  d->height = height;
}

bool Object::isBackground() const
{
  return d->background;
}

void Object::setBackground( bool bg )
{
  d->background = bg;
}

bool Object::hasProperty( std::string name )
{
  std::map<std::string,PropertyValue>::const_iterator i;
  i = d->properties.find( name );
  if( i ==  d->properties.end() )
    return false;
  else
    return true;
}

void Object::setProperty( std::string name, std::string value )
{
  PropertyValue pv;
  pv.type = PropertyValue::StringType;
  pv.s = value;
  d->properties[ name ] = pv;
}

void Object::setProperty( std::string name, int value )
 {
  PropertyValue pv;
  pv.type = PropertyValue::IntType;
  pv.i = value;
  d->properties[ name ] = pv;
}

void Object::setProperty( std::string name, double value )
{
  PropertyValue pv;
  pv.type = PropertyValue::DoubleType;
  pv.d = value;
  d->properties[ name ] = pv;
}

void Object::setProperty( std::string name, bool value )
{
  PropertyValue pv;
  pv.type = PropertyValue::BoolType;
  pv.b = value;
  d->properties[ name ] = pv;
}

void Object::setProperty( std::string name, Color value )
{
  PropertyValue pv;
  pv.type = PropertyValue::ColorType;
  pv.c = value;
  d->properties[ name ] = pv;
}

int Object::getIntProperty( std::string name )
{
  PropertyValue pv;
  pv = d->properties[ name ];
  if( pv.type == PropertyValue::IntType )
    return pv.i;
  else
    return 0;
}

double Object::getDoubleProperty( std::string name )
{
  PropertyValue pv;
  pv = d->properties[ name ];
  if( pv.type == PropertyValue::DoubleType )
    return pv.d;
  else
    return 0;
}

bool Object::getBoolProperty( std::string name )
{
  PropertyValue pv;
  pv = d->properties[ name ];
  if( pv.type == PropertyValue::BoolType )
    return pv.b;
  else
    return false;

}

std::string Object::getStrProperty( std::string name )
{
  PropertyValue pv;
  pv = d->properties[ name ];
  if( pv.type == PropertyValue::StringType )
    return pv.s;
  else
    return "NoString";  
}

Color Object::getColorProperty(std::string name)
{
  PropertyValue pv;
  pv = d->properties[ name ];
  if( pv.type == PropertyValue::ColorType )
    return pv.c;
  else 
    return Color(153,204,255); // #99ccff
 
} 

class TextObject::Private
{
public:
  unsigned type;
  std::vector<UString> text;  
  unsigned listSize; 
  std::vector<bool> bulletFlag; 
};

TextObject::TextObject(): Object()
{
  d = new Private;
}

TextObject::~TextObject()
{
  delete d;
}

unsigned TextObject::type() const
{
  return d->type;
}

unsigned TextObject::listSize() const
{
  return d->text.size();
}

const char* TextObject::typeAsString() const
{
  switch( d->type )
  {
    case  Title       : return "Title";
    case  Body        : return "Body";
    case  Notes       : return "Notes";
    case  NotUsed     : return "NotUsed";
    case  Other       : return "Other";
    case  CenterBody  : return "CenterBody";
    case  CenterTitle : return "CenterTitle";
    case  HalfBody    : return "HalfBody";
    case  QuarterBody : return "QuarterBody";
    default: break;
  }

  return "Unknown";
}

bool TextObject::bulletFlag( unsigned index ) const
{
  return d->bulletFlag[index];
}

void TextObject::setBulletFlag( bool flag )
{
  d->bulletFlag.push_back( flag );
}


void TextObject::setType( unsigned type )
{
  d->type = type;
}

UString TextObject::text( unsigned index) const
{
  return d->text[index];
}

void TextObject::setText( const UString& text )
{
  d->text.push_back( text );
}

void TextObject::convertFrom( Object* object )
{
  setId( object->id() );
  setLeft( object->left() );
  setTop( object->top() );
  setWidth( object->width() );
  setHeight( object->height() );

  if( object->isText() )
  {
    TextObject* textObj = static_cast<TextObject*>( object );
    setType( textObj->type() );
  // jgn lupa diganti :  setText( textObj->text() );
  }
}

class GroupObject::Private
{
public:
  std::vector<Object*> objects;
};

GroupObject::GroupObject()
{
  d = new Private;
}

GroupObject::~GroupObject()
{
  for( unsigned i=0; i<d->objects.size(); i++ )
    delete d->objects[i];
  delete d;
}

unsigned GroupObject::objectCount() const
{
  return d->objects.size();
}

Object* GroupObject::object( unsigned i )
{
  return d->objects[i];
}

void GroupObject::addObject( Object* object )
{
  d->objects.push_back( object );
}

void GroupObject::takeObject( Object* object )
{
  std::vector<Object*> result;
  for( unsigned i=0; i<d->objects.size(); i++ )
  {
    Object* obj = d->objects[i];
    if( obj != object ) 
      result.push_back( obj );
  }

  d->objects.clear();
  for( unsigned j=0; j<result.size(); j++ )
    d->objects.push_back( result[j] );
}

class DrawObject::Private
{
public:
  unsigned shape;
  bool isVerFlip; 
  bool isHorFlip;
};

DrawObject::DrawObject()
{
  d = new Private;
  d->shape = None;
}

DrawObject::~DrawObject()
{
  delete d;
}

unsigned DrawObject::shape() const
{
  return d->shape;
}

void DrawObject::setShape( unsigned s )
{
  d->shape = s;
}

bool DrawObject::isVerFlip() const
{
  return d->isVerFlip;
}

void DrawObject::setVerFlip( bool isVerFlip )
{
  d->isVerFlip = isVerFlip;
}

bool DrawObject::isHorFlip() const
{
  return d->isHorFlip;
}

void DrawObject::setHorFlip( bool isHorFlip )
{
  d->isHorFlip = isHorFlip;
}
