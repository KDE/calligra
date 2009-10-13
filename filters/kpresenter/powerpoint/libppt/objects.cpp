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
#include <QColor>

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

  /**
  * @brief Vector of lines
  *
  */
  std::vector<UString> text;

  /**
  * @brief Vector of style names for the text lines
  *
  */
  std::vector<QString> styleNames;
  /**
  * @brief Simple class holding bullet properties
  *
  */
  class BulletProperties {

      public:
      BulletProperties() :
        bullet(false),
        bulletChar(0),
        bulletFont(0),
        bulletSize(0)
       { }

      /**
      * @brief Should there be a bullet
      *
      */
      bool bullet;

      /**
      * @brief Character to be displayed for a bullet
      *
      */
      QChar bulletChar;

      /**
      * @brief Font for the bullet character
      *
      */
      unsigned int bulletFont;

      /**
      * @brief Bullet's color
      *
      */
      QColor bulletColor;

      /**
      * @brief Bullet's size
      *
      * Size is in percents
      */
      unsigned int bulletSize;

    /**
      * @brief Position in text this style applies from
      *
      * If all lines in text were concatenated to one long text, the position
      * applies only to that text, not to a specific line.
      */
      unsigned int start;

      /**
      * @brief For how many characters should this flag be applied to
      */
      unsigned int length;
  };

  /**
  * @brief Vector containing all the text properties that should be applied
  * to texts
  */
  std::vector<BulletProperties> bulletProperties;

  /**
  * @brief Positions where lines start
  *
  * This is added here to make it a bit faster to count where bullet flags
  * are applied
  */
  std::vector<unsigned int> lineStartPositions;

  /**
  * @brief To what position should the next text property be applied to
  * if all the lines were concatenated into one big string.
  */
  unsigned int nextFlagPosition;
};


TextObject::TextObject(): Object()
{
  d = new Private;
  d->nextFlagPosition = 0;
}

TextObject::~TextObject()
{
  delete d;
}

unsigned int TextObject::type() const
{
  return d->type;
}

unsigned int TextObject::listSize() const
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


QChar TextObject::bulletChar ( unsigned int index ) const
{
  int pos = findTextPropery(index);
  if (pos >= 0) {
      return d->bulletProperties[pos].bulletChar;
  }
  return QChar();
}

unsigned int TextObject::bulletFont( unsigned int index ) const
{
  int pos = findTextPropery(index);
  if (pos >= 0) {
      return d->bulletProperties[pos].bulletFont;
  }
  return 0;
}

unsigned int TextObject::bulletSize( unsigned int index ) const
{
  int pos = findTextPropery(index);
  if (pos >= 0) {
      return d->bulletProperties[pos].bulletSize;
  }
  return 0;
}

QString TextObject::bulletColor( unsigned int index ) const
{
  int pos = findTextPropery(index);
  if (pos < 0) {
      return "";
  }

  return d->bulletProperties[pos].bulletColor.name();
}

void TextObject::setBulletChar ( unsigned int index, int value )
{
  if (index < d->bulletProperties.size()) {
    d->bulletProperties[index].bulletChar = QChar(((value>>0) & 0xff),((value >>8) & 0xff));
  }
}

void TextObject::setBulletFont( unsigned int index, int value )
{
  if (index < d->bulletProperties.size()) {
    d->bulletProperties[index].bulletFont = value;
  }
}

void TextObject::setBulletColor( unsigned int index, int value )
{
  if (index < d->bulletProperties.size()) {
    d->bulletProperties[index].bulletColor = QColor(((value>>0) & 0xff),((value >>8) & 0xff),((value >>16) & 0xff));
  }
}

void TextObject::setBulletSize( unsigned int index, int value )
{
  if (index < d->bulletProperties.size()) {
    d->bulletProperties[index].bulletSize = value;
  }
}

int TextObject::findTextPropery( unsigned int index ) const
{
  //Check if we have enough lines
  if (index >= d->lineStartPositions.size()) {
      return -1;
  }

  //Then find the right text property that has a starting point that matches
  //With line's starting point
  for(unsigned int i=0;i<d->bulletProperties.size();i++) {
    if (d->lineStartPositions[index] >= d->bulletProperties[i].start &&
      d->lineStartPositions[index] < d->bulletProperties[i].start + d->bulletProperties[i].length) {
      return i;
    }
  }
  return -1;
}

unsigned int TextObject::addBulletProperty( int length )
{
    TextObject::Private::BulletProperties newFlag;

    //If length of the bullet is -1 then it corresponds to the total
    //length of rest of the unflagged text (starting from d->nextFlagPosition)
    if (length == -1) {
      int totalLength = 0;
      for(unsigned int i=0;i<d->text.size();i++) {
          totalLength += d->text[i].length();
      }

      newFlag.length = totalLength - d->nextFlagPosition;
    }
    else {
      newFlag.length = length;
    }

    //Use the next flag position for our starting point
    newFlag.start = d->nextFlagPosition;

    //and set the position for the next flag
    d->nextFlagPosition += newFlag.length;
    d->bulletProperties.push_back( newFlag );
    return d->bulletProperties.size()-1;
}

bool TextObject::bulletFlag( unsigned int index ) const
{
  int pos = findTextPropery(index);

  if (pos >= 0) {
      return d->bulletProperties[pos].bullet;
  }
  return false;
}

void TextObject::setBulletFlag( unsigned int index, bool value )
{
  if (index < d->bulletProperties.size()) {
    d->bulletProperties[index].bullet = value;
  }
}


void TextObject::setType( unsigned int type )
{
  d->type = type;
}

UString TextObject::text( unsigned int index ) const
{
    if (index >= d->text.size()) {
        return UString();
    }
    return d->text[index];
}

void TextObject::setLineStyleName( unsigned int index, QString name )
{
    if (index < d->styleNames.size()) {
        d->styleNames[index] = name;
    }
}

QString TextObject::lineStyleName( unsigned int index )
{
    if (index < d->styleNames.size()) {
        return d->styleNames[index];
    }

    return QString();
}

void TextObject::setText( const UString& text )
{
    if (d->text.size() > 0) {
        //New start position is the end position of previous line
        //We'll also compensate +1 since we have removed the line breaks
        d->lineStartPositions.push_back(d->lineStartPositions.back() + d->text.back().length() + 1);
    } else {
        d->lineStartPositions.push_back(0);
    }

    d->text.push_back(text);
    d->styleNames.push_back(QString(""));
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

  // in group nesting, coordinate transformations may occur
  // these values define the transformation
  double xoffset;
  double yoffset;
  double xscale;
  double yscale;

  Private() :xoffset(0), yoffset(0), xscale(25.4/576), yscale(25.4/576) {
  }
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

double
GroupObject::getXOffset() const
{
  return d->xoffset;
}

void
GroupObject::setXOffset(double xoffset)
{
  d->xoffset = xoffset;
}

double
GroupObject::getYOffset() const
{
  return d->yoffset;
}

void
GroupObject::setYOffset(double yoffset)
{
  d->yoffset = yoffset;
}

double
GroupObject::getXScale() const
{
  return d->xscale;
}

void
GroupObject::setXScale(double xscale) const
{
  d->xscale = xscale;
}

double
GroupObject::getYScale() const
{
  return d->yscale;
}

void
GroupObject::setYScale(double yscale) const
{
  d->yscale = yscale;
}

class DrawObject::Private
{
public:
  unsigned shape;
  bool isVerFlip;
  bool isHorFlip;

  /**
  * @brief Name of the style corresponding this object
  *
  * This is usually generated by KoGenStyles
  */
  QString styleName;
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

QString DrawObject::styleName() const
{
  return d->styleName;
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

void DrawObject::setStyleName( const QString &name )
{
  d->styleName = name;
}

class TextFont::Private
{
public:

  /**
  * @brief Font's name
  */
  UString fontName;

  /**
  * @brief Font's charset
  *
  * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
  * for more information
  */
  int charset;

  /**
  * @brief Font's clip precision
  *
  * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
  * for more information
  */
  int clipPrecision;

  /**
  * @brief Font's quality
  *
  * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
  * for more information
  */
  int quality;

  /**
  * @brief Font's pitch and family
  *
  * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
  * for more information
  */
  int pitchAndFamily;
};

TextFont::TextFont()
{
  d = new Private;
  d->fontName = "";
  d->charset = 0;
  d->clipPrecision = 0;
  d->quality = 0;
  d->pitchAndFamily = 0;
}

TextFont::TextFont(const UString &fontName,
                   int charset,
                   int clipPrecision,
                   int quality,
                   int pitchAndFamily)
{
  d = new Private;
  d->fontName = fontName;
  d->charset = charset;
  d->clipPrecision = clipPrecision;
  d->quality = quality;
  d->pitchAndFamily = pitchAndFamily;
}

TextFont::TextFont(const TextFont &source)
{
    d = new Private;
    d->fontName = source.name();
    d->charset = source.charset();
    d->clipPrecision = source.clipPrecision();
    d->quality = source.quality();
    d->pitchAndFamily = source.pitchAndFamily();
}


TextFont::~TextFont()
{
  delete d;
  d = 0;
}

UString TextFont::name() const
{
  return d->fontName;
}

int TextFont::charset() const
{
  return d->charset;
}

int TextFont::clipPrecision() const
{
  return d->clipPrecision;
}

int TextFont::quality() const
{
  return d->quality;
}

int TextFont::pitchAndFamily() const
{
  return d->pitchAndFamily;
}



class TextFontCollection::Private
{
public:
  std::vector<TextFont> fonts;
};

TextFontCollection::TextFontCollection()
{
  d = new Private;
}

TextFontCollection::~TextFontCollection()
{
  delete d;
}

unsigned TextFontCollection::listSize() const
{
  return d->fonts.size();
}

void TextFontCollection::addFont(const TextFont &font)
{
  return d->fonts.push_back(font);
}

const TextFont TextFontCollection::getFont(unsigned index) const
{
  if (index < listSize()) {
      return d->fonts[index];
  }

  return TextFont();
}

