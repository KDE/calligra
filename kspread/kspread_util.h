/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kspread_util_h__
#define __kspread_util_h__

#include <qstring.h>
#include <qrect.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <koffice_export.h>

#include "kspread_global.h"
#include "kspread_value.h"

class QFont;
class QPen;
class QDomElement;
class QDomDocument;

class KLocale;

bool util_isPointValid(QPoint point);
bool util_isRectValid(QRect rect);

namespace KSpread
{
class Cell;
class Map;
class Sheet;

/**
* Represents the position of a single cell in a workbook.  Each position has a row, column and optionally an associated Sheet.
* Columns and rows can be marked as fixed.  This is for handling absolute coordinates in formulae (eg. in the
* formula "=$C$1" both the column (C) and row (1) are fixed.
*/
class KSPREAD_EXPORT Point
{
public:
  Point() { _pos.setX( -1 ); _sheet = 0; _columnFixed = false; _rowFixed = false; }
  Point( const QString& );
  Point( const QString&, Map*, Sheet* default_sheet = 0 );
  Point( const Point& c ) {
    _pos = c._pos;
    _sheet = c._sheet;
    _sheetName = c._sheetName;
    _columnFixed = c._columnFixed;
    _rowFixed = c._rowFixed;
  }

  bool isValid() const { return ( util_isPointValid(pos()) && ( _sheet != 0 || _sheetName.isEmpty() ) ); }
  bool isSheetKnown() const { return ( ! _sheetName.isEmpty() && _sheet != 0 ); }

  Cell* cell() const;

  bool operator== (const Point &cell) const;
  bool operator< (const Point &cell) const;

  int row () const { return _pos.y(); };
  int column () const { return _pos.x(); };
  void setRow (int r) { _pos.setY (r); };
  void setColumn (int c) { _pos.setX (c); };

  /**
  * Sets the sheet which this point lies on.
  */
  void      setSheet(Sheet* sheet);
  Sheet*    sheet() const;

  /**
  * Sets the name of the sheet which this point lies on.
  */
  void      setSheetName(QString name);
  QString   sheetName() const;

  /**
  * Sets the position of this point (in rows and columns)
  */
  void      setPos(QPoint pos);
  QPoint    pos() const;

  /**
  * Sets whether or not the column (x coordinate) of this point is fixed (ie. it represents an absolute
  * coordinate - eg. the column letter B in the formula "=$B30" is fixed)
  */
  void      setColumnFixed(bool colFixed);
  bool      columnFixed() const;

  /**
  * Sets whether or not the row (y coordinate) of this point is fixed (ie. it represents an absolute coordinate - eg. the row number 30 in the formula "=A$30" is fixed)
  */
  void      setRowFixed(bool rowFixed);
  bool      rowFixed() const;


private:
  Sheet* _sheet;
  QString _sheetName;
  QPoint _pos;
  bool _columnFixed;
  bool _rowFixed;

private:
  void init( const QString& );
};

/**
* Represents a region within a workbook.  The region has an area (the columns and rows that it includes) and
* optionally an associated @ref KSpread::Sheet
* A range is defined by four coordinates: Its left column, top row, right column and bottom row.  Each
* of these coordinates may be set as fixed, to represent absolute coordinates in formulae.
*/
class KSPREAD_EXPORT Range
{
public:
  Range();
  virtual ~Range() {}
  Range( const QString& );
  Range( const QString&, Map*, Sheet* default_sheet = 0 );
  Range( const Range& r );
  Range( const Point& ul, const Point& lr );

  /**
  * Returns true if this Range represents a valid region of a spreadsheet.
  * A range is valid if:
  * - It has an associated Sheet
  * - The area is non-negative (ie.
  * - The left-most column is non-negative
  * - The top-most row is non-negative
  */
  bool isValid() const;

  /** Returns true if this range has an associated Sheet or false otherwise */
  bool isSheetKnown() const { return ( !sheetName().isEmpty() && sheet() != 0 ); }

  /** Fills a Point with info (row,column,sheet) about the first point in the range */
  void getStartPoint(Point* pt);
  /** Fills a Point with info (row,column,sheet) about the last point the range */
  void getEndPoint(Point* pt);

  int startRow () const { return range().top(); };
  int startCol () const { return range().left(); };
  int endRow () const { return range().bottom(); };
  int endCol () const { return range().right(); };

  /**
  * Changes the area on the spreadsheet represented by this range
  * @param newRange The new area for this range.
  */
  virtual void setRange(const QRect& newRange) {_range=newRange;}

  void setRange(int newStartCol, int newStartRow, int newEndCol, int newEndRow)
  { _range=QRect(newStartCol,newStartRow,newEndCol-newStartCol,newEndRow-newStartRow); }

  /** Returns the area on the spreadsheet occupied by this range. */
  QRect range() const;

  /** Returns true if this range includes the specified cell */
  bool contains (const Point &cell) const;
  /**
  * Returns true if this range intersects Range @p r  (ie. there is at least one cell
  * which is common to both Ranges )
  */
  bool intersects (const Range &r) const;

  /**
  * Returns a string representation of this range as it would appear in a formula.
  * ie. In the format " SheetName! [StartCell] : [EndCell] "
  * The string representation uses $ characters to mark fixed parts of the range, eg. Sheet1!$A1:$A20
  */
  QString toString() const;

  /** Sets whether or not the left column is fixed . */
  void setLeftFixed(bool fixed);
  bool leftFixed() const;

  /** Sets whether or not the right column is fixed. */
  void setRightFixed(bool fixed);
  bool rightFixed() const;

  /** Sets whether or not the top row is fixed. */
  void setTopFixed(bool fixed);
  bool topFixed() const;

  /** Sets whether or not the bottom row is fixed. */
  void setBottomFixed(bool fixed);
  bool bottomFixed() const;

  /** Sets the Sheet object associated with this range.  The range can only span a single sheet. */
  void setSheet(Sheet* sheet);
  Sheet* sheet() const;

  /** Sets the name of the sheet associated with this range. */
  void setSheetName(QString sheetName);
  QString sheetName() const;

  /**
  * Returns the named area represented by this range or an empty string otherwise.
  * This is the name of the area which was passed as a QString to the Range constructor
  */
  QString namedArea() const;

  /**
  * Returns true if the other range occupies the same area on the same sheet as this range.
  */
  bool operator==(const Range& range) const;


private:
  Sheet* _sheet;
  QString _sheetName;
  QString _namedArea;
  QRect _range;
  bool _leftFixed;
  bool _rightFixed;
  bool _topFixed;
  bool _bottomFixed;
};

/**
range-list and cell-list
TODO: move to a separate file, improve structure, add iterators and all that
TODO: use this class instead of other means of range-walking all over KSpread
TODO: use this as selection
TODO: anything I forgot ;)
*/
struct RangeList {
  QLinkedList<Point> cells;
  QLinkedList<Range> ranges;
};


/**
 * RangeIterator
 *
 * Class to simplify the process of iterating through each cell in a
 * range that has already been allocated
 */
class RangeIterator
{
public:
  /**
   * Contstruct the iterator with the rectangular cell area and which
   * sheet the area is on
   */
  RangeIterator(QRect _range, Sheet* _sheet);
  ~RangeIterator();

  /**
   * @return the first allocated cell in the area
   */
  Cell* first();

  /**
   * @return the next allocated cell in the area after the previous one
   * retrieved, or NULL if it was the last one.
   */
  Cell* next();
private:

  QRect range;
  Sheet* sheet;
  QPoint current;
};

//helper functions for the formatting
bool formatIsDate (FormatType fmt);
bool formatIsTime (FormatType fmt);
bool formatIsFraction (FormatType fmt);



KSPREAD_EXPORT QString util_rangeName( const QRect &_area );
KSPREAD_EXPORT QString util_rangeName( Sheet *_sheet, const QRect &_area );
QString util_rangeColumnName( const QRect &_area);
QString util_rangeRowName( const QRect &_area);

/**
* Call this function to decode the text of a column label to an integer
* i.e. AA->27
*/
KSPREAD_EXPORT int util_decodeColumnLabelText( const QString &_col );
/**
* Call this function to encode an integer to the text of the column label
* i.e. 27->AA
*/
KSPREAD_EXPORT QString util_encodeColumnLabelText( int column );

bool util_isAllSelected(const QRect &selection);
bool util_isColumnSelected(const QRect &selection);
bool util_isRowSelected(const QRect &selection);
bool util_isRowOrColumnSelected( const QRect &selection );



bool util_validateSheetName(const QString &name);

QDomElement util_createElement( const QString & tagName, const QFont & font, QDomDocument & doc );
QDomElement util_createElement( const QString & tagname, const QPen & pen, QDomDocument & doc );
QFont       util_toFont( QDomElement & element );
QPen        util_toPen( QDomElement & element );
int         util_penCompare( QPen const & pen1, QPen const & pen2 );

QString convertRefToRange( const QString & sheet, const QRect & rect );
QString convertRefToBase( const QString & sheet, const QRect & rect );
QString convertRangeToRef( const QString & sheetName, const QRect & _area );

void insertBracket( QString & s );
QString convertOasisPenToString( const QPen & pen );
QPen convertOasisStringToPen( const QString &str );

//Return true when it's a reference to cell from sheet.
KSPREAD_EXPORT bool localReferenceAnchor( const QString &_ref );



namespace Oasis
{
  /**
   * Converts an OpenDocument representation of a formula/cell reference to a
   * localized formula/cell reference.
   * @param expr The expression to convert from OpenDocument format.
   * @param locale The locale to which the expression should be converted.
   */
  // TODO check visibility
  KSPREAD_EXPORT QString decodeFormula(const QString& expr, const KLocale* locale = 0);
  /**
   * Converts a localized formula/cell reference to an OpenDocument
   * representation of a formula/cell reference.
   * @param expr The expression to convert to OpenDocument format.
   * @param locale The locale from which the expression should be converted.
   */
  KSPREAD_EXPORT QString encodeFormula(const QString& expr, const KLocale* locale = 0);
}

} // namespace KSpread

#endif
