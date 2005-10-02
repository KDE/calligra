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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#include "format.h"

#include "ustring.h"


using namespace Swinder;

class FormatFont::Private
{
public:
  bool null : 1 ;
  bool bold  : 1;
  bool italic : 1;
  bool underline : 1;
  bool strikeout : 1;
  bool subscript : 1;
  bool superscript : 1;
  UString fontFamily;
  double fontSize;
  Color color;
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
  d->color       = f.color();
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

Color FormatFont::color() const
{
  return d->color;
}

void FormatFont::setColor( const Color& c )
{
  d->color = c;
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
  d = new FormatAlignment::Private;
  d->null          = true;
  d->alignX        = Format::Left;
  d->alignY        = Format::Middle;
  d->wrap          = false;
  d->indentLevel   = 0;
  d->rotationAngle = 0;
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
  d->null          = align.isNull();
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

class FormatBorders::Private
{
public:
  bool null;
  Pen leftBorder;
  Pen rightBorder;
  Pen topBorder;
  Pen bottomBorder;
};

// constructor
FormatBorders::FormatBorders()
{
  d = new FormatBorders::Private;
  d->null = true;
}

// destructor
FormatBorders::~FormatBorders()
{
  delete d;
}

// copy constructor
FormatBorders::FormatBorders( const FormatBorders& border )
{
  d = new FormatBorders::Private;
  assign( border );
}

// assignment operator
FormatBorders& FormatBorders::operator=( const FormatBorders& border )
{
  return assign( border );
}

// assign from another alignment
FormatBorders& FormatBorders::assign( const FormatBorders& border )
{
  d->null         = border.isNull();
  d->leftBorder   = border.leftBorder();
  d->rightBorder  = border.rightBorder();
  d->topBorder    = border.topBorder();
  d->bottomBorder = border.bottomBorder();
  return *this;
}

bool FormatBorders::isNull() const
{
  return d->null;
}

const Pen& FormatBorders::leftBorder() const
{
  return d->leftBorder;
}

void FormatBorders::setLeftBorder( const Pen& pen )
{
  d->leftBorder = pen;
  d->null = false;
}

const Pen& FormatBorders::rightBorder() const
{
  return d->rightBorder;
}

void FormatBorders::setRightBorder( const Pen& pen )
{
  d->rightBorder = pen;
  d->null = false;
}

const Pen& FormatBorders::topBorder() const
{
  return d->topBorder;
}

void FormatBorders::setTopBorder( const Pen& pen )
{
  d->topBorder = pen;
  d->null = false;
}

const Pen& FormatBorders::bottomBorder() const
{
  return d->bottomBorder;
}

void FormatBorders::setBottomBorder( const Pen& pen )
{
  d->bottomBorder = pen;
  d->null = false;
}

// helper class for Format class
class Format::Private
{
public:
  FormatFont font;
  FormatAlignment alignment;
  FormatBorders borders;
  UString valueFormat;
};

// create an empty format
Format::Format()
{
  d = new Format::Private;
  d->valueFormat = "General";
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
  d->borders = f.borders();
  d->valueFormat = f.valueFormat();
  return *this;
}

bool FormatAlignment::isNull() const
{
  return d->null;
}

bool Format::isNull() const
{
  return d->font.isNull() && d->alignment.isNull() && d->borders.isNull();
}

FormatFont& Format::font() const
{
  return d->font;
}

void Format::setFont( const FormatFont& font )
{
  d->font = font;
}

FormatAlignment& Format::alignment() const
{
  return d->alignment;
}

void Format::setAlignment( const FormatAlignment& alignment )
{
  d->alignment = alignment;
}

FormatBorders& Format::borders() const
{
  return d->borders;
}

void Format::setBorders( const FormatBorders& borders )
{
  d->borders = borders;
}

UString Format::valueFormat() const
{
  return d->valueFormat;
}

void Format::setValueFormat( const UString& valueFormat )
{
  d->valueFormat = valueFormat;
}

// merge f into current format
Format& Format::apply( const Format& f )
{
  if( !f.alignment().isNull() )
    alignment() = f.alignment();
  if( !f.font().isNull() )
    font() = f.font();
  if( !f.borders().isNull() )
    borders() = f.borders();
  if( f.valueFormat().isEmpty() || f.valueFormat() == "General" )
    setValueFormat( f.valueFormat() );
  
  return *this;
}
