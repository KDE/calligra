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

#ifndef SWINDER_FORMAT_H
#define SWINDER_FORMAT_H

#include "ustring.h"
#include "defs.h"

namespace Swinder
{

/**
 * Defines font information for cell format.
 *
 * Class FormatFont defines the font family, size and other attributes
 * for use in cell format.
 * 
 */

class FormatFont
{
public:

  /**
   * Creates a default font information.
   */
  FormatFont();
  
  /**
   * Destroys the font information
   */
  ~FormatFont();
  
  /**
   * Creates a copy of font information.
   */
  FormatFont( const FormatFont& );
  
  /**
   * Assigns from another font information.
   */
  FormatFont& operator=( const FormatFont& );
  
  /**
   * Assigns from another font information.
   */
  FormatFont& assign( const FormatFont& );
  
  /**
   * Returns true if it is a default font information.
   */
  bool isNull() const;
  
  /**
   * Returns the name of font family, e.g "Helvetica".
   */
  UString fontFamily() const;
  
  /**
   * Sets a new family for the font information.
   */
  void setFontFamily( const UString& fontFamily );
  
  /**
   * Returns the size of font (in points).
   */
  double fontSize() const;
  
  /**
   * Sets the size of font (in points).
   */
  void setFontSize( double fs );
  
  /**
   * Returns the color of the font.
   */
  Color color() const;
  
  /**
   * Sets the color of the font.
   */
  void setColor( const Color& color );
  
  /**
   * Returns true if bold has been set.
   */
  bool bold() const;
  
  /**
   * If b is true, bold is set on; otherwise bold is set off.
   */
  void setBold( bool b );

  /**
   * Returns true if italic has been set.
   */
  bool italic() const;
  
  /**
   * If i is true, italic is set on; otherwise italic is set off.
   */
  void setItalic( bool i );
  
  /**
   * Returns true if underline has been set.
   */
  bool underline() const;
  
  /**
   * If u is true, underline is set on; otherwise underline is set off.
   */
  void setUnderline( bool u );
  
  /**
   * Returns true if strikeout has been set.
   */
  bool strikeout() const;
  
  /**
   * If s is true, strikeout is set on; otherwise strikeout is set off.
   */
  void setStrikeout( bool s );
  
  /**
   * Returns true if subscript has been set.
   */
  bool subscript() const;
  
  /**
   * If s is true, subscript is set on; otherwise subscript is set off.
   */
  void setSubscript( bool s );
  
  /**
   * Returns true if superscript has been set.
   */
  bool superscript() const;
  
  /**
   * If s is true, superscript is set on; otherwise superscript is set off.
   */
  void setSuperscript( bool s );
  
private:  
  class Private;
  Private *d;
};

/**
 * Defines alignment information for cell format.
 *
 * Class FormatAlignment defines the horizontal and vertical alignment
 * for the text inside a cell.
 * 
 */

class FormatAlignment
{
public:

  /**
   * Creates a default alignment information.
   */
  FormatAlignment();
  
  /**
   * Destroys the alingment information
   */
  ~FormatAlignment();
  
  /**
   * Creates a copy of alignment information.
   */
  FormatAlignment( const FormatAlignment& );
  
  /**
   * Assigns from another alignment information.
   */
  FormatAlignment& operator=( const FormatAlignment& );
  
  /**
   * Assigns from another alignment information.
   */
  FormatAlignment& assign( const FormatAlignment& );
  
  /**
   * Returns true if it is a default alignment information.
   */
  bool isNull() const;
  
  /**
   * Returns horizontal alignment. Possible values are
   * Format::Left, Format::Right and Format::Center.
   *
   * \sa setAlignX
   */
  unsigned alignX() const;
  
  /**
   * Sets the horizontal alignment.
   *
   * \sa alignX
   */
  void setAlignX( unsigned xa );
  
  /**
   * Returns horizontal alignment. Possible values are
   * Format::Top, Format::Middle and Format::Bottom.
   *
   * \sa setAlignY
   */  
  unsigned alignY() const;
  
  /**
   * Sets the horizontal alignment.
   *
   * \sa alignY
   */
   void setAlignY( unsigned xa );
   
  /**
   * Returns true if the text should be wrapped at right border.
   *
   * \sa setWrap
   */  
   bool wrap() const;
   
  /**
   * Sets whether the text should be wrapped at right border.
   *
   * \sa setWrap
   */  
   void setWrap( bool w );
   
  /**
   * Returns the indentation level.
   *
   * \sa setIndentLevel
   */  
   unsigned indentLevel() const;
   
  /**
   * Sets the indentation level.
   *
   * \sa indentLevel
   */  
   void setIndentLevel( unsigned i );
   
  /**
   * Returns the text rotation angle.
   *
   * \sa setRotationAngle
   */  
   unsigned rotationAngle() const;
   
  /**
   * Sets the text rotation angle.
   *
   * \sa rotationAngle
   */  
   void setRotationAngle( unsigned r );
       
private:  
  class Private;
  Private *d;
};

/**
 * Defines borders information for cell.
 * 
 */

class FormatBorders
{
public:

  /**
   * Creates a default border information.
   */
  FormatBorders();
  
  /**
   * Destroys the border information
   */
  ~FormatBorders();
  
  /**
   * Creates a copy of border information.
   */
  FormatBorders( const FormatBorders& );
  
  /**
   * Assigns from another border information.
   */
  FormatBorders& operator=( const FormatBorders& );
  
  /**
   * Assigns from another border information.
   */
  FormatBorders& assign( const FormatBorders& );
  
  /**
   * Returns true if it is a default border information.
   */
  bool isNull() const;
  
  /**
   * Returns pen style, width and color for left border.
   *
   * \sa setLeftBorder
   */
  const Pen& leftBorder() const;
  
  /**
   * Sets pen style, width and color for left border.
   *
   * \sa leftBorder
   */
  void setLeftBorder( const Pen& pen );
  
  /**
   * Returns pen style, width and color for right border.
   *
   * \sa setRightBorder
   */
  const Pen& rightBorder() const;
  
  /**
   * Sets pen style, width and color for right border.
   *
   * \sa rightBorder
   */
  void setRightBorder( const Pen& pen );
  
  /**
   * Returns pen style, width and color for top border.
   *
   * \sa setTopBorder
   */
  const Pen& topBorder() const;
  
  /**
   * Sets pen style, width and color for top border.
   *
   * \sa topBorder
   */
  void setTopBorder( const Pen& pen );
  
  /**
   * Returns pen style, width and color for bottom border.
   *
   * \sa setBottomBorder
   */
  const Pen& bottomBorder() const;
  
  /**
   * Sets pen style, width and color for bottom border.
   *
   * \sa bottomBorder
   */
  void setBottomBorder( const Pen& pen );
  
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
  
  bool isNull() const;
  
  /** 
   * Returns a constant reference to the formatting information of this format.
   */
  FormatFont& font() const;
  
  /** 
   * Sets new font information for this format.
   */
  void setFont( const FormatFont& font );
  
  /** 
   * Returns a constant reference to the alignment information of this format.
   */
  FormatAlignment& alignment() const;
  
  /** 
   * Sets new alignment information for this format.
   */
  void setAlignment( const FormatAlignment& alignment );
  
  /** 
   * Returns a reference to the borders information of this format.
   */
  FormatBorders& borders() const;
  
  /** 
   * Sets new borders information for this format.
   */
  void setBorders( const FormatBorders& border );
  
  enum { Left, Center, Right };
  
  enum { Top, Middle, Bottom };
  
  /**
   * Applies another format to this format. Basically this will merge
   * the formatting information of f into the current format.
   * For example, if current format is "Bold, Italic" and f is
   * "Left border", the current format would become "Bold, Italic, left border".
   *
   * If parts of the formatting information in f are already specified in the
   * current format, then it will override the current format.
   * For example, if current format is "Bold, right-aligned" and f is "Italic",
   * the result is "Italic, right-aligned".
   *
   */
  Format& apply( const Format& f );

protected:
  class Private;
  Private* d; // can't never be 0

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

#endif // SWINDER_FORMAT_H

