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

#ifndef SIDEWINDER_FORMAT_H
#define SIDEWINDER_FORMAT_H

#include "ustring.h"

namespace Sidewinder
{


class FormatFont
{
public:

  FormatFont();
  
  ~FormatFont();
  
  FormatFont( const FormatFont& );
  
  FormatFont& operator=( const FormatFont& );
  
  FormatFont& assign( const FormatFont& );
  
  bool isNull() const;
  
  UString fontFamily() const;
  
  void setFontFamily( const UString& fontFamily );
  
  double fontSize() const;
  
  void setFontSize( double fs );
  
  bool bold() const;
  
  void setBold( bool b );

  bool italic() const;
  
  void setItalic( bool i );
  
  bool underline() const;
  
  void setUnderline( bool u );
  
  bool strikeout() const;
  
  void setStrikeout( bool s );
  
  bool subscript() const;
  
  void setSubscript( bool s );
  
  bool superscript() const;
  
  void setSuperscript( bool s );
  
private:  
  class Private;
  Private *d;
};

class FormatAlignment
{
public:

  FormatAlignment();
  
  ~FormatAlignment();
  
  FormatAlignment( const FormatAlignment& );
  
  FormatAlignment& operator=( const FormatAlignment& );
  
  FormatAlignment& assign( const FormatAlignment& );
  
  bool isNull() const;
  
  unsigned alignX() const;
  
  void setAlignX( unsigned xa );
  
  unsigned alignY() const;
  
  void setAlignY( unsigned xa );
    
private:  
  class Private;
  Private *d;
};

/**
 * Defines format of cell.
 *
 * Class Format defines possible formatting for use in cells or ranges.
 * Basically, Format might consist of one or more "pieces". Each piece
 * specifies only one type of formatting, e.g whether the text should
 * be shown in bold or not, which borders should the cells/ranges have, 
 * and so on.
 *
 * A complex formatting can be decomposed into different pieces. For example,
 * formatting like "Font is Arial 10 pt, background color is blue,
 " formula is hidden" could be a combination of three simple formatting pieces 
 * as: (1) font is "Arial 10pt", (2) background pattern is 100%, blue
 * and (3) cell is protected, formula is hidden. This also means
 * that one format might be applied to another format. An example of this is
 * "Font is Helvetica" format and "Left border, 1pt, blue" format will yields
 * something like "Font is Helvetica, with left border of blue 1pt". 
 * Use Format::apply to do such format merging.
 * 
 */
 
class FormatData;

class Format
{
public:

  /**
   * Creates a default format.
   */
  Format();
  
  /**
   * Destroys the format.
   */
  ~Format();
  
  /** 
   * Creates a copy from another format.
   */
  Format( const Format& f );

  /** 
   * Assigns from another format.
   */
  Format& operator= ( const Format& f );

  /** 
   * Assigns from another value. 
   */
  Format& assign( const Format& f );
  
  FormatFont& font();
  
  FormatFont& font() const;
  
  FormatAlignment& alignment();
  
  FormatAlignment& alignment() const;
  
  enum { Left, Center, Right };
  
  /**
   * Applies another format to this format. Basically this will merge
   * the formatting information of f into the current format.
   * For example, if current format is "Bold, Italic" and f is
   * "Left border", the current format would become "Bold, Italic, left border".
   *         
   */
  Format& apply( const Format& f );

protected:
  class Data;
  Data* d; // can't never be 0

};

class FormatStorage
{
public:
  FormatStorage();
  
  virtual ~FormatStorage();
  
  Format format( unsigned row, unsigned column );
  
  void setFormat( unsigned row, unsigned column, const Format& format );
  
  void columnInserted( unsigned column );

  void columnDeleted( unsigned column );
  
  void rowInserted( unsigned row );

  void rowDeleted( unsigned row );

private:
  // no copy or assign
  FormatStorage( const FormatStorage& );
  FormatStorage& operator=( const FormatStorage& );
  
  class Private;
  Private* d;
};


};

#endif // SIDEWINDER_FORMAT_H

