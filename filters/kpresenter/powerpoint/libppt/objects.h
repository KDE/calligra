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

#ifndef LIBPPT_OBJECTS
#define LIBPPT_OBJECTS

#include "ustring.h"
#include <string>


namespace Libppt
{

class Color
{
public:
  unsigned red, green, blue;
  Color(){ red = green = blue = 0; }
  Color( unsigned r, unsigned g, unsigned b )
    { red = r; green = g; blue = b; }
  Color( const Color& c )
    { red = c.red; green = c.green; blue = c.blue; }
  Color& operator=( const Color& c )
    { red = c.red; green = c.green; blue = c.blue; return *this; }
};

class Object
{
public:
  Object();
  virtual ~Object();

  int id() const;
  void setId( int id );

  virtual bool isText() const { return false; }
  virtual bool isPicture() const { return false; }
  virtual bool isGroup() const { return false; }
  virtual bool isDrawing() const { return false; }

  // all is in mm

  double top() const;
  double left() const;
  double width() const;
  double height() const;
  void setTop( double top );
  void setLeft( double left );
  void setWidth( double width );
  void setHeight( double height );

  bool isBackground() const;
  void setBackground( bool bg );
  

  bool hasProperty( std::string name );

  void setProperty( std::string name, int value );
  void setProperty( std::string name, double value );
  void setProperty( std::string name, std::string value );
  void setProperty( std::string name, bool value );  
  void setProperty( std::string name, Color value );
  void setProperty( std::string name, const char* value )
    { setProperty( name, std::string(value) ); }

  int getIntProperty( std::string name );
  double getDoubleProperty( std::string name );
  bool getBoolProperty( std::string name );
  std::string getStrProperty( std::string name );
  Color getColorProperty(std::string name); 

private:
  // no copy or assign
  Object( const Object& );
  Object& operator=( const Object& );
  
  class Private;
  Private* d;
};

class TextObject: public Object
{
public:

  enum { 
    Title       = 0, 
    Body        = 1, 
    Notes       = 2, 
    NotUsed     = 3,
    Other       = 4,  // text in a shape
    CenterBody  = 5,  // subtitle in title slide
    CenterTitle = 6,  // title in title slide
    HalfBody    = 7,  // body in two-column slide
    QuarterBody = 8   // body in four-body slide
  };

  TextObject();
  virtual ~TextObject();
  virtual bool isText() const { return true; }
  unsigned type() const;
  void setType( unsigned type );
  const char* typeAsString() const;
  UString text(unsigned index) const;
  void setText( const UString& text );
  unsigned listSize() const;
  bool bulletFlag(unsigned index) const;
  void setBulletFlag( bool flag ) ;
  void convertFrom( Object* object );

private:
  // no copy or assign
  TextObject( const TextObject& );
  TextObject& operator=( const TextObject& );
  
  class Private;
  Private* d;
};

class GroupObject: public Object
{
public:
  GroupObject();
  virtual ~GroupObject();
  virtual bool isGroup() const { return true; }
  unsigned objectCount() const;
  Object* object( unsigned index );
  void addObject( Object* object );
  void takeObject( Object* object );

private:
  // no copy or assign
  GroupObject( const GroupObject& );
  GroupObject& operator=( const GroupObject& );
  
  class Private;
  Private* d;

};

class DrawObject: public Object
{
public:

  enum {
    None = 0,
    Rectangle,
    RoundRectangle,
    Circle,
    Ellipse,
    Diamond,
    RightArrow,
    LeftArrow,
    UpArrow,
    DownArrow, 
    IsoscelesTriangle,
    RightTriangle,
    Parallelogram,
    Trapezoid,
    Hexagon,
    Octagon,
    Line,
    Smiley,
    Heart,
    FreeLine
  };

  DrawObject();
  virtual ~DrawObject();
  virtual bool isDrawing() const { return true; }
   
  unsigned shape() const;
  void setShape( unsigned s );

  bool isVerFlip() const;
  void setVerFlip( bool vFlip );
  bool isHorFlip() const;
  void setHorFlip( bool hFlip );  

private:
  // no copy or assign
  DrawObject( const DrawObject& );
  DrawObject& operator=( const DrawObject& );
  
  class Private;
  Private* d;
};

}

#endif /* LIBPPT_OBJECTS */
