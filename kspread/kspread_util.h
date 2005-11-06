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

#include "kspread_global.h"
#include "kspread_value.h"
#include <koffice_export.h>
class KSpreadMap;
class KSpreadSheet;
class KLocale;

class QFont;
class QPen;
class QDomElement;
class QDomDocument;

namespace KSpread
{
class Cell;
}

struct KSPREAD_EXPORT KSpreadPoint
{
public:
  KSpreadPoint() { pos.setX( -1 ); sheet = 0; columnFixed = false; rowFixed = false; }
  KSpreadPoint( const QString& );
  KSpreadPoint( const QString&, KSpreadMap*, KSpreadSheet* default_sheet = 0 );
  KSpreadPoint( const KSpreadPoint& c ) {
    pos = c.pos;
    sheet = c.sheet; sheetName = c.sheetName;
    columnFixed = c.columnFixed;
    rowFixed = c.rowFixed;
  }

  bool isValid() const { return ( pos.x() >= 0 && ( sheet != 0 || sheetName.isEmpty() ) ); }
  bool isSheetKnown() const { return ( !sheetName.isEmpty() && sheet != 0 ); }

  KSpread::Cell* cell() const;

  bool operator== (const KSpreadPoint &cell) const;
  bool operator< (const KSpreadPoint &cell) const;

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
  KSpreadSheet* sheet() const { return m_sheet; }

private:
  */
  KSpreadSheet* sheet;
  QString sheetName;
  QPoint pos;
  bool columnFixed;
  bool rowFixed;

private:
  void init( const QString& );
};

struct KSPREAD_EXPORT KSpreadRange
{
  KSpreadRange();
  KSpreadRange( const QString& );
  KSpreadRange( const QString&, KSpreadMap*, KSpreadSheet* default_sheet = 0 );
  KSpreadRange( const KSpreadRange& r ) {
    sheet = r.sheet;
    sheetName = r.sheetName;
    range = r.range;
  }
  KSpreadRange( const KSpreadPoint& ul, const KSpreadPoint& lr )
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

  /** Fills a KSpreadPoint with info (row,column,sheet) about the first point in the range */
  void getStartPoint(KSpreadPoint* pt);
  /** Fills a KSpreadPoint with info (row,column,sheet) about the last point the range */
  void getEndPoint(KSpreadPoint* pt);

  int startRow () const { return range.top(); };
  int startCol () const { return range.left(); };
  int endRow () const { return range.bottom(); };
  int endCol () const { return range.right(); };

  void setRange(QRect& newRange) {range=newRange;}
  void setRange(int newStartCol, int newStartRow, int newEndCol, int newEndRow)
  { range=QRect(newStartCol,newStartRow,newEndCol-newStartCol,newEndRow-newStartRow); }

  /** does this range contain the given cell? */
  bool contains (const KSpreadPoint &cell) const;
  /** do these two ranges have at least one common cell? */
  bool intersects (const KSpreadRange &r) const;

  /** returns a string representation of this range, ie.
     " SheetName! [StartCell] : [EndCell] " */
  QString toString();

  KSpreadSheet* sheet;
  QString sheetName;
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
  QValueList<KSpreadPoint> cells;
  QValueList<KSpreadRange> ranges;
};


/**
 * KSpreadRangeIterator
 *
 * Class to simplify the process of iterating through each cell in a
 * range that has already been allocated
 */
class KSpreadRangeIterator
{
public:
  /**
   * Contstruct the iterator with the rectangular cell area and which
   * sheet the area is on
   */
  KSpreadRangeIterator(QRect _range, KSpreadSheet* _sheet);
  ~KSpreadRangeIterator();

  /**
   * @return the first allocated cell in the area
   */
  KSpread::Cell* first();

  /**
   * @return the next allocated cell in the area after the previous one
   * retrieved, or NULL if it was the last one.
   */
  KSpread::Cell* next();
private:

  QRect range;
  KSpreadSheet* sheet;
  QPoint current;
};

//helper functions for the formatting
bool formatIsDate (FormatType fmt);
bool formatIsTime (FormatType fmt);
bool formatIsFraction (FormatType fmt);



KSPREAD_EXPORT QString util_rangeName( const QRect &_area );
KSPREAD_EXPORT QString util_rangeName( KSpreadSheet *_sheet, const QRect &_area );
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


#endif
