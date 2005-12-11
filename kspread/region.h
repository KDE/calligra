/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#ifndef KSPREAD_REGION
#define KSPREAD_REGION

#include <qrect.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <koffice_export.h>

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
  friend class Manipulator;
  friend class Sheet;
public:
  /**
   * Constructor.
   * Creates an empty region.
   */
  Region();

  /**
   * Constructor.
   * Creates a region consisting of a point @param point
   */
  Region(const QPoint& point);

  /**
   * Constructor.
   * Creates a region consisting of a range @param range
   */
  Region(const QRect& range);

  /**
   * Constructor.
   * Creates a region consisting of the region defined in @param strRegion.
   * Uses the @param view to determine the sheet, if it's named in @param strRegion.
   */
  Region(const QString& strRegion, View* view);

  /**
   * Constructor.
   * Creates a copy of the region @param region.
   */
  Region(const Region& region);

  /**
   * Constructor.
   * Creates a region consisting of a point at the location @param col, @param row
   */
  Region(int col, int row);

  /**
   * Constructor.
   * Creates a region consisting of a range at the location @param col, @param row
   * and the size @param width, @param height
   */
  Region(int col, int row, int width, int height);

  /**
   * Destructor.
   */
  virtual ~Region();


  /**
   * @return the name of the region (e.g. "A1:A2")
   */
  QString name(const QString& relToSheetName = QString()) const;


  /**
   * @return true, if this region contains no elements
   */
  bool isEmpty() const;

  /**
   * @return true, if this region contains only a single point
   */
  bool isSingular() const;

  /**
   * @return true, if this region contains at least one valid point or one valid range
   */
  bool isValid() const;

  /**
   * @return true, if the colum @param col is selected. If column @param col
   * is not given, it returns true, if at least one column is selected.
   */
  bool isColumnSelected(uint col = 0) const;

  /**
   * @return true, if the row @param row is selected. If row @param row
   * is not given, it returns true, if at least one row is selected.
   */
  bool isRowSelected(uint row = 0) const;

  /**
   * @return true, if the at least on cell in column @param col is selected.
   */
  bool isColumnAffected(uint col) const;

  /**
   * @return true, if the at least on cell in row @param row is selected.
   */
  bool isRowAffected(uint row) const;


  /* TODO Stefan #2: Optimize! Adjacent Points/Ranges */
  /**
   * Adds the point @param point to this region.
   */
  void add(const QPoint& point);
  /**
   * Adds the range @param range to this region.
   */
  void add(const QRect& range);
  /**
   * Adds the region @param region to this region.
   */
  void add(const Region& region);

  /* TODO Stefan #3: Improve! */
  /**
   * Substracts the point @param point to this region.
   */
  void sub(const QPoint&);
  /**
   * Substracts the range @param range to this region.
   */
  void sub(const QRect&);

  /**
   * deletes all elements of the region. The result is an empty region.
   */
  void clear();


  /**
   * @return true, if the region contains the point @param point
   */
  bool contains(const QPoint& point) const;

  /**
   * @return true, if this region equals region @param region
   */
  bool operator==(const Region& region) const;


  /**
    * @param sRegion will be modified, if a valid sheet was found. The sheetname
    * will be removed.
    * @return sheet named in the @param sRegion or the active sheet of the view.
    */
  Sheet* filterSheetName(QString& sRegion);


  /**
   * @return the view to which this region belongs.
   */
  View* view() const;

  /**
   * sets the view to which this region belongs.
   */
  void setView(View*);

protected:
  /* TODO Stefan #4:
      Move the bits for fixing Points/Ranges to derived classes
      FixablePoint/FixableRange as they are needed for Selections only.
      This would save 4 bytes for each Element!
  */
  class Element;
  class Point;
  class Range;

  /**
   * @return the list of elements
   */
  QValueList<Element*>& cells() const;

private:
  /* TODO Stefan #5:
      Elaborate, if Private data class is really nescessary. A direct
      access to the list of Elements could make sense for derived
      classes (Selection!, Manipulator?)
  */
  class Private;
  Private *d;

public:
  /* TODO Stefan #6:
     Remove these typedefs/iterators. Create a manipulator or whatever for
     the cases, in which they are used.
  */
  typedef QValueList<Element*>::Iterator      Iterator;
  typedef QValueList<Element*>::ConstIterator ConstIterator;

  ConstIterator constBegin() const;
  ConstIterator constEnd() const;
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

  virtual QString name(const QString& /*relToSheetName */ = QString()) const { return QString(); }
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
 * bitmask: 4 bytes
 * sum: 20 bytes
 */
class Region::Point : public Region::Element
{
public:
  Point();
  Point(const QPoint&);
  Point(const QString&);
  virtual ~Point();

  virtual Type type() const { return Element::Point; }
  virtual bool isValid() const { return !m_point.isNull(); }
  virtual bool isColumn() const { return false; }
  virtual bool isRow() const { return false; }

  virtual bool contains(const QPoint&) const;
  virtual bool contains(const QRect&) const;

  virtual QString name(const QString& relToSheetName = QString()) const;

  virtual QRect rect() const { return QRect(m_point,m_point); }

  QPoint pos() const { return m_point; }

  bool columnFixed() const { return m_columnFixed; }
  bool rowFixed() const { return m_rowFixed; }

private:
  QPoint m_point;
  /* TODO Stefan #4 */
  bool m_columnFixed : 1;
  bool m_rowFixed    : 1;
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
 * bitmask: 4 bytes
 * sum: 28 bytes
 */
class Region::Range : public Region::Element
{
public:
  Range();
  Range(const QRect&);
  Range(const QString&);
  virtual ~Range();

  virtual Type type() const { return Element::Range; }
  virtual bool isValid() const { return !m_range.isNull(); }
  virtual bool isColumn() const { return (m_range.top() == 1 && m_range.bottom() == KS_rowMax); }
  virtual bool isRow() const { return (m_range.left() == 1 && m_range.right() == KS_colMax); }

  virtual bool contains(const QPoint&) const;
  virtual bool contains(const QRect&) const;

  virtual QString name(const QString& relToSheetName = QString()) const;

  virtual QRect rect() const { return m_range; }

  int width() const;
  int height() const;

private:
  QRect m_range;
  /* TODO Stefan #4 */
  bool m_leftFixed   : 1;
  bool m_rightFixed  : 1;
  bool m_topFixed    : 1;
  bool m_bottomFixed : 1;
};

} // namespace KSpread


/***************************************************************************
  kdDebug support
****************************************************************************/

#include <kdebug.h>

inline kdbgstream operator<<( kdbgstream str, const KSpread::Region& r )
{
  str << "Region = " << r.name();
  return str;
}

#endif // KSPREAD_REGION
