/* This file is part of the KDE project
   Copyright (C) 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef KSPREAD_REGION
#define KSPREAD_REGION

#include <QList>
#include <QRect>
#include <QString>

#include <kdebug.h>

#include <koffice_export.h>

#include "kspread_global.h"
#include "kspread_util.h"

namespace KSpread
{
class Manipulator;
class Sheet;
class View;

/**
 * The one for all class for points and ranges.
 */
class KSPREAD_EXPORT Region
{
protected:
  class Element;
  class Point;
  class Range;

public:
  /**
   * Constructor.
   * Creates an empty region.
   */
  Region();

  /**
   * Constructor.
   * Creates a region consisting of a point.
   * @param point the point's location
   * @param sheet the sheet the point belongs to
   */
  Region(const QPoint& point, Sheet* sheet = 0);

  /**
   * Constructor.
   * Creates a region consisting of a range.
   * @param range the range's location
   * @param sheet the sheet the range belongs to
   */
  Region(const QRect& range, Sheet* sheet = 0);

  /**
   * Constructor.
   * Creates a region consisting of the region defined in @p strRegion .
   * @param view used to determine the sheet, if it's named in the string
   * @param strRegion a string representing the region (e.g. "A1:B3")
   * @param sheet the sheet the region belongs to
   */
  Region(View* view, const QString& strRegion, Sheet* sheet = 0);

  /**
   * Copy Constructor.
   * Creates a copy of the region.
   * @param region the region to copy
   */
  Region(const Region& region);

  /**
   * Constructor.
   * Creates a region consisting of a point.
   * @param col the column of the point
   * @param row the row of the point
   * @param sheet the sheet the point belongs to
   */
  Region(int col, int row, Sheet* sheet = 0);

  /**
   * Constructor.
   * Creates a region consisting of a range at the location
   * @param col the column of the range' starting point
   * @param row the row of the range' starting point
   * @param width the width of the range
   * @param height the height of the range
   * @param sheet the sheet the range belongs to
   */
  Region(int col, int row, int width, int height, Sheet* sheet = 0);

  /**
   * Destructor.
   */
  virtual ~Region();



  /**
   * @param originSheet The name is created relative to this sheet.
   * @return the name of the region (e.g. "A1:A2")
   */
  QString name(Sheet* originSheet = 0) const;

  /**
   * @param sRegion will be modified, if a valid sheet was found. The sheetname
   * will be removed
   * @return sheet named in the @p sRegion or the active sheet of the view
   */
  Sheet* filterSheetName(QString& sRegion);



  /**
   * @return @c true, if this region contains no elements
   */
  bool isEmpty() const;

  /**
   * @return @c true, if this region contains only a single point
   */
  bool isSingular() const;

  /**
   * @return @c true, if this region is contiguous
   */
  bool isContiguous() const;

  /**
   * @return @c true, if this region contains at least one valid point or one valid range
   */
  bool isValid() const;

  /**
   * @param col the col to check
   * @return @c true, if the colum @p col is selected. If column @p col
   * is not given, it returns true, if at least one column is selected
   */
  bool isColumnSelected(uint col = 0) const;

  /**
   * @param row the row to check
   * @return @c true, if the row @p row is selected. If row @p row
   * is not given, it returns true, if at least one row is selected
   */
  bool isRowSelected(uint row = 0) const;

  /**
   * @return @c true,if at least one column or one row is selected
   */
  bool isColumnOrRowSelected() const;

  /**
   * @param col the col to check
   * @return @c true, if the at least one cell in column @p col is selected
   */
  bool isColumnAffected(uint col) const;

  /**
   * @param row the row to check
   * @return @c true, if the at least one cell in row @p row is selected
   */
  bool isRowAffected(uint row) const;

  /**
   * @param point the point's location
   * @param sheet the sheet the point belongs to
   * @return @c true, if the region contains the point @p point
   */
  bool contains(const QPoint& point, Sheet* sheet = 0) const;



  /* TODO Stefan #2: Optimize! Adjacent Points/Ranges */
  /**
   * Adds the point @p point to this region.
   * @param point the point's location
   * @param sheet the sheet the point belongs to
   */
  Element* add(const QPoint& point, Sheet* sheet = 0);
  /**
   * Adds the range @p range to this region.
   * @param range the range's location
   * @param sheet the sheet the range belongs to
   */
  Element* add(const QRect& range, Sheet* sheet = 0);
  /**
   * Adds the region @p region to this region.
   * @param region the range's location
   */
  Element* add(const Region& region);

  /* TODO Stefan #3: Improve! */
  /**
   * Substracts the point @p point from this region.
   * @param point the point's location
   */
  void sub(const QPoint& point);
  /**
   * Substracts the range @p range from this region.
   * @param range the range's location
   */
  void sub(const QRect& range);
  /**
   * Substracts the region @p region from this region.
   * @param region the region to substract
   */
  void sub(const Region& region);

  /**
   * @param point the point's location
   * @param sheet the sheet the point belongs to
   */
  virtual Element* eor(const QPoint& point, Sheet* sheet = 0);

  /**
   * Deletes all elements of the region. The result is an empty region.
   */
  virtual void clear();



  QRect boundingRect() const;



  /**
   * @param region the region to compare
   * @return @c true, if this region equals region @p region
   */
  bool operator==(const Region& region) const;

  /**
   * @param region the region to copy
   */
  void operator=(const Region& region);



  /**
   * @return the view to which this region belongs.
   */
  View* view() const;

  /**
   * Sets the view to which this region belongs.
   */
  void setView(View*);


  typedef QList<Element*>::Iterator      Iterator;
  typedef QList<Element*>::ConstIterator ConstIterator;

  ConstIterator constBegin() const;
  ConstIterator constEnd() const;

protected:
  /**
   * @return the list of elements
   */
  QList<Element*>& cells() const;

  /**
   * @param iterator the iterator to the element in whose front the new point
   * is inserted
   * @param point the location of the point to be inserted
   * @param multi @c true to allow multiple occurences of a point
   * @return an iterator to the added point or @p iterator, if @p point is not
   * valid or the point is already in the list
   */
  Iterator insert(Iterator iterator, const QPoint& point, Sheet*, bool multi = true);
  /**
   * @param iterator the iterator to the element in whose front the new range
   * is inserted
   * @param range the location of the range to be inserted
   * @param multi @c true to allow multiple occurences of a range
   * @return an iterator to the added range or @p iterator, if @p range is not
   * valid or the range is already in the list
   */
  Iterator insert(Iterator iterator, const QRect& range, Sheet*, bool multi = true);

  virtual Point* createPoint(const QPoint&) const;
  virtual Point* createPoint(const QString&) const;
  virtual Point* createPoint(const Point&) const;
  virtual Range* createRange(const QRect&) const;
  virtual Range* createRange(const QString&) const;
  virtual Range* createRange(const Range&) const;

private:
  class Private;
  Private *d;
};


/***************************************************************************
  class Region::Element
****************************************************************************/
/**
 * Base class for region elements, which can be points or ranges.
 * This class is used by KSpread::Region and could not be used outside of it.
 *
 * Size:
 * m_sheet: 4 bytes
 * vtable: 4 bytes
 * sum: 8 bytes
 */
class Region::Element
{
public:
  enum Type { Undefined, Point, Range };

  Element();
  virtual ~Element();

  virtual Type type() const { return Undefined; }
  virtual bool isValid() const { return false; }
  virtual bool isColumn() const { return false; }
  virtual bool isRow() const { return false; }

  virtual bool contains(const QPoint&) const { return false; }
  virtual bool contains(const QRect&) const { return false; }

  virtual QString name(Sheet* = 0) const { return QString(""); }
  virtual QRect rect() const { return QRect(); }

  Sheet* sheet() const { return m_sheet; }
  void setSheet(Sheet* sheet) { m_sheet = sheet; }

protected:
  /* TODO Stefan #6:
      Elaborate, if this pointer could be avoided by QDict or whatever in
      Region.
  */
  Sheet* m_sheet;
};


/***************************************************************************
  class Region::Point
****************************************************************************/

/**
 * A point in a region.
 * This class is used by KSpread::Region and could not be used outside of it.
 *
 * Size:
 * m_sheet: 4 bytes
 * vtable: 4 bytes
 * m_point: 8 bytes
 * sum: 16 bytes
 */
class Region::Point : public Region::Element
{
public:
  Point(const QPoint&);
  Point(const QString&);
  virtual ~Point();

  virtual Type type() const { return Element::Point; }
  virtual bool isValid() const { return (!m_point.isNull() && util_isPointValid(pos())); }
  virtual bool isColumn() const { return false; }
  virtual bool isRow() const { return false; }

  virtual bool contains(const QPoint&) const;
  virtual bool contains(const QRect&) const;

  virtual QString name(Sheet* originSheet = 0) const;

  virtual QRect rect() const { return QRect(m_point,m_point); }

  QPoint pos() const { return m_point; }

private:
  QPoint m_point;
};


/***************************************************************************
  class Region:.Range
****************************************************************************/

/**
 * A range in a region.
 * This class is used by KSpread::Region and could not be used outside of it.
 *
 * Size:
 * m_sheet: 4 bytes
 * vtable: 4 bytes
 * m_range: 16 bytes
 * sum: 24 bytes
 */
class Region::Range : public Region::Element
{
public:
  Range(const QRect&);
  Range(const QString&);
  virtual ~Range();

  virtual Type type() const { return Element::Range; }
  virtual bool isValid() const { return !m_range.isNull() && util_isRectValid(rect()); }
  virtual bool isColumn() const { return (m_range.normalized().top() == 1 && m_range.normalized().bottom() == KS_rowMax); }
  virtual bool isRow() const { return (m_range.normalized().left() == 1 && m_range.normalized().right() == KS_colMax); }

  virtual bool contains(const QPoint&) const;
  virtual bool contains(const QRect&) const;

  virtual QString name(Sheet* originSheet = 0) const;

  virtual QRect rect() const { return m_range; }

  int width() const;
  int height() const;

private:
  QRect m_range;
};

} // namespace KSpread


/***************************************************************************
  kDebug support
****************************************************************************/

inline kdbgstream operator<<( kdbgstream str, const KSpread::Region& r )
{
  str << "Region = " << r.name();
  return str;
}

#endif // KSPREAD_REGION
