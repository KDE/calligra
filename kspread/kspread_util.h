/* This file is part of the KDE project
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

#include <koffice_export.h>

#include "kspread_global.h"
#include "kspread_value.h"

class QFont;
class QPen;
class QDomElement;
class QDomDocument;

class KLocale;

namespace KSpread
{
class Cell;
class Map;
class Sheet;

struct KSPREAD_EXPORT Point
{
public:
  Point() { pos.setX( -1 ); sheet = 0; columnFixed = false; rowFixed = false; }
  Point( const QString& );
  Point( const QString&, Map*, Sheet* default_sheet = 0 );
  Point( const Point& c ) {
    pos = c.pos;
    sheet = c.sheet; sheetName = c.sheetName;
    columnFixed = c.columnFixed;
    rowFixed = c.rowFixed;
  }

  bool isValid() const { return ( pos.x() >= 0 && ( sheet != 0 || sheetName.isEmpty() ) ); }
  bool isSheetKnown() const { return ( !sheetName.isEmpty() && sheet != 0 ); }

  Cell* cell() const;

  bool operator== (const Point &cell) const;
  bool operator< (const Point &cell) const;

  int row () const { return pos.y(); };
  int column () const { return pos.x(); };
  void setRow (int r) { pos.setY (r); };
  void setColumn (int c) { pos.setX (c); };



  /*
    TODO
  bool columnFixed() const { return m_columnFixed; }
  bool rowFixed() const { return m_rowFixed; }
  QPoint pos() const { return m_pos; }
  QString sheetName() const { return m_sheetName; }
  Sheet* sheet() const { return m_sheet; }

private:
  */
  Sheet* sheet;
  QString sheetName;
  QPoint pos;
  bool columnFixed;
  bool rowFixed;

private:
  void init( const QString& );
};

struct KSPREAD_EXPORT Range
{
  Range();
  Range( const QString& );
  Range( const QString&, Map*, Sheet* default_sheet = 0 );
  Range( const Range& r ) {
    sheet = r.sheet;
    sheetName = r.sheetName;
    range = r.range;
  }
  Range( const Point& ul, const Point& lr )
  {
    range = QRect( ul.pos, lr.pos );
    if ( ul.sheetName != lr.sheetName )
    {
      range.setLeft( -1 );
      return;
    }
    sheetName = ul.sheetName;
    sheet = ul.sheet;
    leftFixed = ul.columnFixed;
    rightFixed = lr.columnFixed;
    topFixed = ul.rowFixed;
    bottomFixed = lr.rowFixed;
  }

  bool isValid() const;
  bool isSheetKnown() const { return ( !sheetName.isEmpty() && sheet != 0 ); }

  /** Fills a Point with info (row,column,sheet) about the first point in the range */
  void getStartPoint(Point* pt);
  /** Fills a Point with info (row,column,sheet) about the last point the range */
  void getEndPoint(Point* pt);

  int startRow () const { return range.top(); };
  int startCol () const { return range.left(); };
  int endRow () const { return range.bottom(); };
  int endCol () const { return range.right(); };

  void setRange(QRect& newRange) {range=newRange;}
  void setRange(int newStartCol, int newStartRow, int newEndCol, int newEndRow)
  { range=QRect(newStartCol,newStartRow,newEndCol-newStartCol,newEndRow-newStartRow); }

  /** does this range contain the given cell? */
  bool contains (const Point &cell) const;
  /** do these two ranges have at least one common cell? */
  bool intersects (const Range &r) const;

  /** returns a string representation of this range, ie.
     " SheetName! [StartCell] : [EndCell] " */
  QString toString();

  Sheet* sheet;
  QString sheetName;
  QString namedArea;
  QRect range;
  bool leftFixed;
  bool rightFixed;
  bool topFixed;
  bool bottomFixed;
};

/**
range-list and cell-list
TODO: move to a separate file, improve structure, add iterators and all that
TODO: use this class instead of other means of range-walking all over KSpread
TODO: use this as selection
TODO: anything I forgot ;)
*/
struct RangeList {
  QValueList<Point> cells;
  QValueList<Range> ranges;
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

} // namespace KSpread

#endif
