/* Sidewinder - Portable library for spreadsheet
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

#include "format.h"

#include "ustring.h"


using namespace Sidewinder;

class FormatFont::Private
{
public:
  bool null;
  UString fontFamily;
  double fontSize;
  bool bold;
  bool italic;
  bool underline;
  bool strikeout;
  bool subscript;
  bool superscript;
};

FormatFont::FormatFont()
{
  d = new FormatFont::Private();
  d->null        = true;
  d->fontFamily  = "Arial";
  d->fontSize    = 11;
  d->bold        = false;
  d->italic      = false;
  d->underline   = false;
  d->strikeout   = false;
  d->subscript   = false;
  d->superscript = false;
}

FormatFont::~FormatFont()
{
  delete d;
}

FormatFont::FormatFont( const FormatFont& f )
{
  d = new FormatFont::Private();
  assign( f );
}

FormatFont& FormatFont::operator=( const FormatFont& f )
{
  return assign( f );
}

FormatFont& FormatFont::assign( const FormatFont& f )
{
  d->null        = f.isNull();
  d->fontFamily  = f.fontFamily();
  d->fontSize    = f.fontSize();
  d->bold        = f.bold();
  d->italic      = f.italic();
  d->underline   = f.underline();
  d->strikeout   = f.strikeout();
  d->subscript   = f.subscript();
  d->superscript = f.superscript();
  
  return *this;
}

bool FormatFont::isNull() const
{
  return d->null;
}

UString FormatFont::fontFamily() const
{
  return d->fontFamily;
}

void FormatFont::setFontFamily( const UString& fontFamily )
{
  d->fontFamily = fontFamily;
  d->null = false;
}

double FormatFont::fontSize() const
{
  return d->fontSize;
}
  
void FormatFont::setFontSize( double fs )
{
  d->fontSize = fs;
  d->null = false;
}

bool FormatFont::bold() const
{
  return d->bold;
}

void FormatFont::setBold( bool b )
{
  d->bold = b;
  d->null = false;
}

bool FormatFont::italic() const
{
  return d->italic;
}

void FormatFont::setItalic( bool b )
{
  d->italic = b;
  d->null = false;
}

bool FormatFont::underline() const
{
  return d->underline;
}

void FormatFont::setUnderline( bool b )
{
  d->underline = b;
  d->null = false;
}

bool FormatFont::strikeout() const
{
  return d->strikeout;
}

void FormatFont::setStrikeout( bool s )
{
  d->strikeout = s;
  d->null = false;
}

bool FormatFont::subscript() const
{
  return d->subscript;
}

void FormatFont::setSubscript( bool s )
{
  d->subscript = s;
  d->null = false;

  // mutually exclusive
  if( d->subscript && d->superscript )
    d->superscript = false;
}

bool FormatFont::superscript() const
{
  return d->superscript;
}

void FormatFont::setSuperscript( bool s )
{
  d->superscript = s;
  d->null = false;

  // mutually exclusive
  if( d->superscript && d->subscript )
    d->subscript = false;
}

class FormatAlignment::Private
{
public:
  bool null;
  unsigned alignX;
  unsigned alignY;
  bool wrap;
  unsigned indentLevel;
  unsigned rotationAngle;
};

FormatAlignment::FormatAlignment()
{
  d         = new FormatAlignment::Private;
  d->null   = true;
  d->alignX = Format::Left;
  d->alignY = Format::Middle;
}

// destructor
FormatAlignment::~FormatAlignment()
{
  delete d;
}

// copy constructor
FormatAlignment::FormatAlignment( const FormatAlignment& align )
{
  d = new FormatAlignment::Private;
  assign( align );
}

// assignment operator
FormatAlignment& FormatAlignment::operator=( const FormatAlignment& align )
{
  return assign( align );
}

// assign from another alignment
FormatAlignment& FormatAlignment::assign( const FormatAlignment& align )
{
  d->null         = align.isNull();
  d->alignX        = align.alignX();
  d->alignY        = align.alignY();
  d->wrap          = align.wrap();
  d->indentLevel   = align.indentLevel();
  d->rotationAngle = align.rotationAngle();
  return *this;
}

unsigned FormatAlignment::alignX() const
{
  return d->alignX;
}

void FormatAlignment::setAlignX( unsigned xa )
{
  d->alignX = xa;
  d->null = false;
}

unsigned FormatAlignment::alignY() const
{
  return d->alignY;
}

void FormatAlignment::setAlignY( unsigned ya )
{
  d->alignY = ya;
  d->null = false;
}

bool FormatAlignment::wrap() const
{
  return d->wrap;
}

void FormatAlignment::setWrap( bool w )
{
  d->wrap = w;
  d->null = false;
}

unsigned FormatAlignment::indentLevel() const
{
  return d->indentLevel;
}

void FormatAlignment::setIndentLevel( unsigned i )
{
  d->indentLevel = i;
  d->null = false;
}

unsigned FormatAlignment::rotationAngle() const
{
  return d->rotationAngle;
}

void FormatAlignment::setRotationAngle( unsigned r )
{
  d->rotationAngle = r;
  d->null = false;
}

class FormatBorder::Private
{
public:
  bool null;
  unsigned leftBorderStyle;
  Color leftBorderColor;
  unsigned rightBorderStyle;
  Color rightBorderColor;
  unsigned topBorderStyle;
  Color topBorderColor;
  unsigned bottomBorderStyle;
  Color bottomBorderColor;
};

// constructor
FormatBorder::FormatBorder()
{
  d                    = new FormatBorder::Private;
  d->null              = true;
  d->leftBorderStyle   = 0;
  d->leftBorderColor   = Color( 0, 0, 0);
  d->rightBorderStyle  = 0;
  d->rightBorderColor  = Color( 0, 0, 0);
  d->topBorderStyle    = 0;
  d->topBorderColor    = Color( 0, 0, 0);
  d->bottomBorderStyle = 0;
  d->bottomBorderColor = Color( 0, 0, 0);
}

// destructor
FormatBorder::~FormatBorder()
{
  delete d;
}

// copy constructor
FormatBorder::FormatBorder( const FormatBorder& border )
{
  d = new FormatBorder::Private;
  assign( border );
}

// assignment operator
FormatBorder& FormatBorder::operator=( const FormatBorder& border )
{
  return assign( border );
}

// assign from another alignment
FormatBorder& FormatBorder::assign( const FormatBorder& border )
{
  d->null              = border.isNull();
  d->leftBorderStyle   = border.leftBorderStyle();
  d->leftBorderColor   = border.leftBorderColor();
  d->rightBorderStyle  = border.rightBorderStyle();
  d->rightBorderColor  = border.rightBorderColor();
  d->topBorderStyle    = border.topBorderStyle();
  d->topBorderColor    = border.topBorderColor();
  d->bottomBorderStyle = border.bottomBorderStyle();
  d->bottomBorderColor = border.bottomBorderColor();
  return *this;
}

bool FormatBorder::isNull() const
{
  return d->null;
}

unsigned FormatBorder::leftBorderStyle() const
{
  return d->leftBorderStyle;
}

void FormatBorder::setLeftBorderStyle( unsigned s )
{
  d->leftBorderStyle = s;
  d->null = false;
}

Color FormatBorder::leftBorderColor() const
{
  return d->leftBorderColor;
}

void FormatBorder::setLeftBorderColor( const Color& color )
{
  d->leftBorderColor = color;
  d->null = false;
}

unsigned FormatBorder::rightBorderStyle() const
{
  return d->rightBorderStyle;
}

void FormatBorder::setRightBorderStyle( unsigned s )
{
  d->rightBorderStyle = s;
  d->null = false;
}

Color FormatBorder::rightBorderColor() const
{
  return d->rightBorderColor;
}

void FormatBorder::setRightBorderColor( const Color& color )
{
  d->rightBorderColor = color;
  d->null = false;
}

unsigned FormatBorder::topBorderStyle() const
{
  return d->topBorderStyle;
}

void FormatBorder::setTopBorderStyle( unsigned s )
{
  d->topBorderStyle = s;
  d->null = false;
}

Color FormatBorder::topBorderColor() const
{
  return d->topBorderColor;
}

void FormatBorder::setTopBorderColor( const Color& color )
{
  d->topBorderColor = color;
  d->null = false;
}

unsigned FormatBorder::bottomBorderStyle() const
{
  return d->bottomBorderStyle;
}

void FormatBorder::setBottomBorderStyle( unsigned s )
{
  d->bottomBorderStyle = s;
  d->null = false;
}

Color FormatBorder::bottomBorderColor() const
{
  return d->bottomBorderColor;
}

void FormatBorder::setBottomBorderColor( const Color& color )
{
  d->bottomBorderColor = color;
  d->null = false;
}

// helper class for Format class
class Format::Private
{
public:
    FormatFont font;
    FormatAlignment alignment;
};


// create an empty format
Format::Format()
{
  d = new Format::Private;
}

// destructor
Format::~Format()
{
  delete d;
}

// copy constructor
Format::Format( const Format& f )
{
  d = new Format::Private;
  assign( f );
}

// assignment operator
Format& Format::operator=( const Format& f )
{
  return assign( f );
}

// assign from another format
Format& Format::assign( const Format& f )
{
  d->font = f.font();
  d->alignment = f.alignment();
  return *this;
}

bool FormatAlignment::isNull() const
{
  return d->null;
}

FormatFont& Format::font()
{
  return d->font;
}

FormatFont& Format::font() const
{
  return d->font;
}

FormatAlignment& Format::alignment()
{
  return d->alignment;
}

FormatAlignment& Format::alignment() const
{
  return d->alignment;
}

// merge f into current format
Format& Format::apply( const Format& f )
{
  if( !f.alignment().isNull() )
    alignment() = f.alignment();
  if( !f.font().isNull() )
    font() = f.font();
  
  return *this;
}

class FormatStorage::Private
{
public:
  int i;
};

FormatStorage::FormatStorage()
{
  d = new FormatStorage::Private;
}

FormatStorage::~FormatStorage()
{
  delete d;
}

Format FormatStorage::format( unsigned row, unsigned column )
{
  return Format();
}

void FormatStorage::setFormat( unsigned row, unsigned column, 
  const Format& format )
{
}

void FormatStorage::columnInserted( unsigned column )
{
}

void FormatStorage::columnDeleted( unsigned column )
{
}

void FormatStorage::rowInserted( unsigned row )
{
}

void FormatStorage::rowDeleted( unsigned row )
{
}


