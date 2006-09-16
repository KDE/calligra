/* This file is part of the KDE project
   Copyright (C) 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <QRegExp>
#include <QStringList>

#include <kdebug.h>

#include "Cell.h"
#include "Doc.h"
#include "Global.h"
#include "Map.h"
#include "Sheet.h"
#include "Util.h"

#include "Region.h"

namespace KSpread
{

class Region::Private
{
public:
  Private()
    : map(0),
      cells(QList<Element*>())
  {
  }

  Map* map;
  QList<Element*> cells;
};


/***************************************************************************
  class Region
****************************************************************************/

Region::Region()
{
  d = new Private();
}

Region::Region(Map* map, const QString& string, Sheet* fallbackSheet)
{
  d = new Private();
  d->map = map;

  if (string.isEmpty())
  {
    return;
  }
  QStringList substrings = string.split(';');
  QStringList::ConstIterator end = substrings.constEnd();
  for (QStringList::ConstIterator it = substrings.constBegin(); it != end; ++it)
  {
    QString sRegion = *it;

    Sheet* sheet = filterSheetName(sRegion);
    if (!sheet)
    {
      sheet = fallbackSheet;
    }

    int delimiterPos = sRegion.indexOf(':');
    if (delimiterPos > -1)
    {
      // range
      Point ul(sRegion.left(delimiterPos));
      Point lr(sRegion.mid(delimiterPos + 1));

      if (ul.isValid() && lr.isValid())
      {
        Range* range = createRange(sRegion);
        range->setSheet(sheet);
        d->cells.append(range);
      }
      else if (ul.isValid())
      {
        Point* point = createPoint(sRegion.left(delimiterPos));
        point->setSheet(sheet);
        d->cells.append(point);
      }
      else // lr.isValid()
      {
        Point* point = createPoint(sRegion.right(delimiterPos + 1));
        point->setSheet(sheet);
        d->cells.append(point);
      }
    }
    else
    {
      // single cell
      Point* point = createPoint(sRegion);
      point->setSheet(sheet);
      d->cells.append(point);
    }
  }
}

Region::Region(const QRect& rect, Sheet* sheet)
{
  d = new Private();

  if (rect.isNull())
  {
    kError(36001) << "Region::Region(const QRect&): QRect is empty!" << endl;
    return;
  }
  add(rect, sheet);
}

Region::Region(const QPoint& point, Sheet* sheet)
{
  d = new Private();

  if (point.isNull())
  {
    kError(36001) << "Region::Region(const QPoint&): QPoint is empty!" << endl;
    return;
  }
  add(point, sheet);
}

Region::Region(const Region& list)
{
  d = new Private();
  d->map = list.d->map;

  ConstIterator end(list.d->cells.constEnd());
  for (ConstIterator it = list.d->cells.constBegin(); it != end; ++it)
  {
    Element *element = *it;
    if (element->type() == Element::Point)
    {
      Point* point = static_cast<Point*>(element);
      d->cells.append(createPoint(*point));
    }
    else
    {
      Range* range = static_cast<Range*>(element);
      d->cells.append(createRange(*range));
    }
  }
}

Region::Region(int x, int y, Sheet* sheet)
{
  d = new Private();

  if (!isValid(QPoint(x,y)))
  {
    kError(36001) << "Region::Region(" << x << ", " << y << "): Coordinates are invalid!" << endl;
    return;
  }
  add(QPoint(x,y), sheet);
}

Region::Region(int x, int y, int width, int height, Sheet* sheet)
{
  d = new Private();

  if (!isValid(QRect(x,y,width,height)))
  {
    kError(36001) << "Region::Region(" << x << ", " << y << ", " << width << ", " << height << "): Dimensions are invalid!" << endl;
    return;
  }
  add(QRect(x,y,width,height), sheet);
}


Region::~Region()
{
  d->cells.clear();
  delete d;
}

Map* Region::map() const
{
  Q_ASSERT(d->map);
  return d->map;
}

void Region::setMap(Map* map)
{
  d->map = map;
}

bool Region::isValid() const
{
  ConstIterator end = d->cells.constEnd();
  for (ConstIterator it = d->cells.constBegin(); it != end; ++it)
  {
    if (!(*it)->isValid())
    {
      return false;
    }
  }
  return true;
}

bool Region::isSingular() const
{
  if (d->cells.isEmpty() || d->cells.count() > 1 || (*d->cells.constBegin())->type() != Element::Point)
  {
    return false;
  }
  return true;
}

bool Region::isContiguous() const
{
  if (d->cells.count() != 1 || !isValid())
  {
    return false;
  }
  return true;
}

QString Region::name(Sheet* originSheet) const
{
  QStringList names;
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    names += element->name(originSheet);
  }
  return names.isEmpty() ? "" : names.join(";");
}

Region::Element* Region::add(const QPoint& point, Sheet* sheet)
{
  if (!isValid(point))
  {
    return 0;
  }
  return insert(d->cells.count(), point, sheet, false);
}

Region::Element* Region::add(const QRect& range, Sheet* sheet)
{
  const QRect normalizedRange = range.normalized();
  if (normalizedRange.width() == 0 || normalizedRange.height() == 0)
  {
    return 0;
  }
  if (normalizedRange.size() == QSize(1,1))
  {
    return add(normalizedRange.topLeft(), sheet);
  }
  return insert(d->cells.count(), normalizedRange, sheet, false);
}

Region::Element* Region::add(const Region& region)
{
  ConstIterator endOfList(region.d->cells.constEnd());
  for (ConstIterator it = region.d->cells.constBegin(); it != endOfList; ++it)
  {
    add((*it)->rect(), (*it)->sheet());
  }
  return d->cells.isEmpty() ? 0 : d->cells.last();
}

void Region::sub(const QPoint& point, Sheet* sheet)
{
  // TODO Stefan: Improve!
  Iterator endOfList(d->cells.end());
  for (Iterator it = d->cells.begin(); it != endOfList; ++it)
  {
    Element *element = *it;
    if (element->sheet() != sheet)
    {
      continue;
    }
    if (element->rect() == QRect(point,point))
    {
      delete element;
      d->cells.removeAll(element);
      break;
    }
  }
}

void Region::sub(const QRect& range, Sheet* sheet)
{
  const QRect normalizedRange = range.normalized();
  // TODO Stefan: Improve!
  Iterator endOfList(d->cells.end());
  for (Iterator it = d->cells.begin(); it != endOfList; ++it)
  {
    Element *element = *it;
    if (element->sheet() != sheet)
    {
      continue;
    }
    if (element->rect() == normalizedRange)
    {
      delete element;
      d->cells.removeAll(element);
      break;
    }
  }
}

void Region::sub(const Region& region)
{
  ConstIterator endOfList(region.constEnd());
  for (ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    if (element->type() == Element::Point)
    {
      Point* point = static_cast<Point*>(element);
      sub(point->pos());
    }
    else
    {
      sub(element->rect());
    }
  }
}

Region::Element* Region::eor(const QPoint& point, Sheet* sheet)
{
  bool containsPoint = false;

  int index = 0;
  while (index < d->cells.count())
  {
    if (!d->cells[index]->contains(point))
    {
      ++index;
      continue;
    }
    containsPoint = true;
    int x = point.x();
    int y = point.y();
    QRect fullRange = d->cells[index]->rect();
    delete d->cells.takeAt(index);

    // top range
    int left = fullRange.left();
    int top = fullRange.top();
    int width = fullRange.width();
    int height = y - top;
    if (height > 0)
    {
      insert(index, QRect(left, top, width, height), sheet);
    }
    // left range
    left = fullRange.left();
    top = y;
    width = qMax(0, x - left);
    height = 1;
    if (width > 0)
    {
      insert(index, QRect(left, top, width, height), sheet);
    }
    // right range
    left = qMin(x+1, fullRange.right());
    top = y;
    width = qMax(0, fullRange.right() - x);
    height = 1;
    if (width > 0)
    {
      insert(index, QRect(left, top, width, height), sheet);
    }
    // bottom range
    left = fullRange.left();
    top = y+1;
    width = fullRange.width();
    height = qMax(0, fullRange.bottom() - y);
    if (height > 0)
    {
      insert(index, QRect(left, top, width, height), sheet);
    }
    return d->cells[index];
  }

  if (!containsPoint)
  {
    return add(point, sheet);
  }
  return 0;
}

Region::Element* Region::insert(int pos, const QPoint& point, Sheet* sheet, bool multi)
{
  if (point.x() < 1 || point.y() < 1)
  {
    return 0;
  }

  bool containsPoint = false;
//   bool adjacentPoint = false;
//   QRect neighbour;

  // we don't have to check for occurrences?
  if (multi)
  {
    Point* rpoint = createPoint(point);
    rpoint->setSheet(sheet);
    if (pos > d->cells.count())
      pos = d->cells.count();
    d->cells.insert(pos, rpoint);
    return d->cells[pos];
  }

  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    if (sheet && sheet != element->sheet())
    {
      continue;
    }
    if (element->contains(point))
    {
      containsPoint = true;
      break;
    }
/*    else
    {
      neighbour = element->rect();
      neighbour.setTopLeft(neighbour.topLeft() - QPoint(1,1));
      neighbour.setBottomRight(neighbour.bottomRight() + QPoint(1,1));
      if (neighbour.contains(point))
      {
        adjacentPoint = true; // TODO Stefan: Implement!
        break;
      }
    }*/
  }
  if ( !containsPoint )
  {
    Point* rpoint = createPoint(point);
    rpoint->setSheet(sheet);
    if (pos > d->cells.count())
      pos = d->cells.count();
    d->cells.insert(pos, rpoint);
    return d->cells[pos];
  }
  return 0;
}

Region::Element* Region::insert(int pos, const QRect& range, Sheet* sheet, bool multi)
{
  const QRect normalizedRange = range.normalized();
  if (normalizedRange.size() == QSize(1,1))
  {
    return insert(pos, normalizedRange.topLeft(), sheet);
  }

  if (multi)
  {
    Range* rrange = createRange(normalizedRange);
    rrange->setSheet(sheet);
    if (pos > d->cells.count())
      pos = d->cells.count();
    d->cells.insert(pos, rrange);
    return d->cells[pos];
  }

  bool containsRange = false;

  for (int index = 0; index < d->cells.count(); ++index)
  {
    if (sheet && sheet != d->cells[index]->sheet())
    {
      ++index;
      continue;
    }
    if (d->cells[index]->contains(normalizedRange))
    {
      containsRange = true;
    }
    else if (normalizedRange.contains(d->cells[index]->rect()))
    {
      delete d->cells.takeAt(index--);
      continue;
    }
  }
  if ( !containsRange )
  {
    Range* rrange = createRange(normalizedRange);
    rrange->setSheet(sheet);
    if (pos > d->cells.count())
      pos = d->cells.count();
    d->cells.insert(pos, rrange);
    return d->cells[pos];
  }
  return 0;
}

QSet<int> Region::columnsSelected() const
{
  QSet<int> result;
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    if ((*it)->isColumn())
    {
      const QRect range = (*it)->rect();
      const int right = range.right();
      for (int col = range.left(); col <= right; ++col)
      {
        result << col;
      }
    }
  }
  return result;
}

QSet<int> Region::rowsSelected() const
{
  QSet<int> result;
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    if ((*it)->isRow())
    {
      const QRect range = (*it)->rect();
      const int bottom = range.bottom();
      for (int row = range.top(); row <= bottom; ++row)
      {
        result << row;
      }
    }
  }
  return result;
}

QSet<int> Region::columnsAffected() const
{
  QSet<int> result;
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    const QRect range = (*it)->rect();
    const int right = range.right();
    for (int col = range.left(); col <= right; ++col)
    {
      result << col;
    }
  }
  return result;
}

QSet<int> Region::rowsAffected() const
{
  QSet<int> result;
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    const QRect range = (*it)->rect();
    const int bottom = range.bottom();
    for (int row = range.top(); row <= bottom; ++row)
    {
      result << row;
    }
  }
  return result;
}

bool Region::isColumnSelected(uint col) const
{
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    QRect region = element->rect();
    if ((col == 0 || ((int)col >= region.left() && (int)col <= region.right())) &&
         region.top() == 1 && region.bottom() == KS_rowMax)
    {
      return true;
    }
  }
  return false;
}

bool Region::isRowSelected(uint row) const
{
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    QRect region = element->rect();
    if ((row == 0 || ((int)row >= region.top() && (int)row <= region.bottom())) &&
         region.left() == 1 && region.right() == KS_colMax)
    {
      return true;
    }
  }
  return false;
}

bool Region::isColumnOrRowSelected() const
{
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    QRect region = element->rect();
    if ((region.top() == 1 && region.bottom() == KS_rowMax) ||
        (region.left() == 1 && region.right() == KS_colMax))
    {
      return true;
    }
  }
  return false;
}

bool Region::contains(const QPoint& point, Sheet* sheet) const
{
  if (d->cells.isEmpty())
  {
    return false;
  }
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    if (element->contains(point))
    {
      if (sheet && element->sheet() != sheet)
      {
        return false;
      }
      return true;
    }
  }
  return false;
}

bool Region::isEmpty() const
{
  return d->cells.isEmpty();
}

void Region::clear()
{
  for (int i = 0; i < d->cells.count(); ++i)
  {
    delete d->cells.takeAt(0);
  }
}

QRect Region::boundingRect() const
{
  int left   = KS_colMax;
  int right  = 1;
  int top    = KS_rowMax;
  int bottom = 1;
  Region::ConstIterator endOfList = cells().constEnd();
  for (Region::ConstIterator it = cells().constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect();
    if (range.left() < left)
    {
      left = range.left();
    }
    if (range.right() > right)
    {
      right = range.right();
    }
    if (range.top() < top)
    {
      top = range.top();
    }
    if (range.bottom() > bottom)
    {
      bottom = range.bottom();
    }
  }
  return QRect(left, top, right-left+1, bottom-top+1);
}

Region::ConstIterator Region::constBegin() const
{
  return d->cells.constBegin();
}

Region::ConstIterator Region::constEnd() const
{
  return d->cells.constEnd();
}

bool Region::isValid(const QPoint& point)
{
  if ( point.x() < 1 || point.y() < 1 ||
       point.x() > KS_colMax ||  point.y() > KS_rowMax )
    return false;
  else
    return true;
}

bool Region::isValid(const QRect& rect)
{
  if ( !isValid( rect.topLeft() ) || !isValid( rect.bottomRight() ) ||
       rect.width() == 0 || rect.height() == 0 )
    return false;
  else
    return true;
}

QList<Region::Element*>& Region::cells() const
{
  return d->cells;
}

bool Region::operator==(const Region& other) const
{
  ConstIterator endOfList(d->cells.constEnd());
  ConstIterator endOfOtherList(other.d->cells.constEnd());
  ConstIterator it = d->cells.constBegin();
  ConstIterator it2 = other.d->cells.constBegin();
  while (it != endOfList && it2 != endOfOtherList)
  {
    if ((*it++)->rect() != (*it2++)->rect())
    {
      return false;
    }
  }
  return true;
}

void Region::operator=(const Region& other)
{
  d->map = other.d->map;
  clear();
  ConstIterator end(other.d->cells.constEnd());
  for (ConstIterator it = other.d->cells.constBegin(); it != end; ++it)
  {
    Element *element = *it;
    if (element->type() == Element::Point)
    {
      Point* point = static_cast<Point*>(element);
      d->cells.append(createPoint(*point));
    }
    else
    {
      Range* range = static_cast<Range*>(element);
      d->cells.append(createRange(*range));
    }
  }
}

Sheet* Region::filterSheetName(QString& sRegion)
{
  Sheet* sheet = 0;
  int delimiterPos = sRegion.indexOf( '!' );
  if (delimiterPos > -1)
  {
    QString sheetName = sRegion.left(delimiterPos);
    // remove the '!'
    sRegion = sRegion.right(sRegion.length() - delimiterPos - 1);
    sheet = d->map->findSheet(sheetName);
  }
  return sheet;
}

Region::Point* Region::createPoint(const QPoint& point) const
{
  return new Point(point);
}

Region::Point* Region::createPoint(const QString& string) const
{
  return new Point(string);
}

Region::Point* Region::createPoint(const Point& point) const
{
  return new Point(point);
}

Region::Range* Region::createRange(const QRect& rect) const
{
  return new Range(rect);
}

Region::Range* Region::createRange(const QString& string) const
{
  return new Range(string);
}

Region::Range* Region::createRange(const Range& range) const
{
  return new Range(range);
}

/***************************************************************************
  class Element
****************************************************************************/

Region::Element::Element()
    : m_sheet(0)
{
}

Region::Element::~Element()
{
}


/***************************************************************************
  class Point
****************************************************************************/

Region::Point::Point(const QPoint& point)
  : Region::Element(),
    m_point(point)
{
}

Region::Point::Point(const QString& sCell)
  : Region::Element(),
    m_point()
{
    uint length = sCell.length();

    if (length == 0)
    {
      kDebug(36001) << "Region::Point::init: length = 0" << endl;
      return;
    }

    QString string = sCell;//Region::filterSheetName(sCell);

    uint p = 0;

    // Fixed ?
    if (string[0] == '$')
    {
        p++;
    }

    // Malformed ?
    if (p == length)
    {
      kDebug(36001) << "Region::Point::init: no point after '$' (string: '" << string.mid(p) << "'" << endl;
        return;
    }

    if (string[p] < 'A' || string[p] > 'Z')
    {
        if (string[p] < 'a' || string[p] > 'z')
        {
          kDebug(36001) << "Region::Point::init: wrong first character in point (string: '" << string.mid(p) << "'" << endl;
            return;
        }
    }
    //default is error
    int x = -1;
    //search for the first character != text
    int result = string.indexOf( QRegExp("[^A-Za-z]+"), p );

    //get the colomn number for the character between actual position and the first non text charakter
    if ( result != -1 )
    {
        x = util_decodeColumnLabelText( string.mid( p, result - p ) ); // x is defined now
    }
    else  // If there isn't any, then this is not a point -> return
    {
      kDebug(36001) << "Region::Point::init: no number in string (string: '" << string.mid( p, result ) << "')" << endl;
        return;
    }
    p = result;

    //limit is KS_colMax
    if ( x > KS_colMax )
    {
      kDebug(36001) << "Region::Point::init: column value too high (col: " << x << ")" << endl;
        return;
    }

    // Malformed ?
    if (p == length)
    {
        kDebug(36001) << "Region::Point::init: p==length after cols" << endl;
        return;
    }

    if (string[p] == '$')
    {
        p++;
  // Malformed ?
        if ( p == length )
        {
            kDebug(36001) << "Region::Point::init: p==length after $ of row" << endl;
            return;
        }
    }

    uint p2 = p;
    while ( p < length )
    {
        if (!QChar(string[p++]).isDigit())
        {
            kDebug(36001) << "Region::Point::init: no number" << endl;
            return;
        }
    }

    bool ok;
    int y = string.mid( p2, p-p2 ).toInt( &ok );
    if ( !ok )
    {
        kDebug(36001) << "Region::Point::init: Invalid number (string: '" << string.mid( p2, p-p2 ) << "'" << endl;
        return;
    }
    if ( y > KS_rowMax )
    {
        kDebug(36001) << "Region::Point::init: row value too high (row: " << y << ")" << endl;
        return;
    }
    if ( y <= 0 )
    {
        kDebug(36001) << "Region::Point::init: y <= 0" << endl;
        return;
    }

    m_point = QPoint(x, y);
}

Region::Point::~Point()
{
}

QString Region::Point::name(Sheet* originSheet) const
{
  QString name = "";
  if (m_sheet && m_sheet != originSheet)
  {
    name = m_sheet->sheetName() + '!';
  }
  return name + Cell::name(m_point.x(), m_point.y());
}

bool Region::Point::contains(const QPoint& point) const
{
    return (m_point == point);
}

bool Region::Point::contains(const QRect& range) const
{
    return (range.width() == 1) && (range.height() == 1) && (range.topLeft() == m_point);
}

Cell* Region::Point::cell() const
{
  return m_sheet->cellAt(m_point);
}

/***************************************************************************
  class Range
****************************************************************************/

Region::Range::Range(const QRect& rect)
  : Region::Element(),
      m_range(rect)
{
}

Region::Range::Range(const QString& sRange)
  : Region::Element(),
      m_range()
{
    int delimiterPos = sRange.indexOf(':');
    if (delimiterPos == -1)
    {
        return;
    }

    //Region::filterSheetName(sRange);

    Region::Point ul(sRange.left(delimiterPos));
    Region::Point lr(sRange.mid(delimiterPos + 1));

    if (!ul.isValid() || !lr.isValid())
    {
      return;
    }
    m_range = QRect(ul.pos(), lr.pos());
}

Region::Range::~Range()
{
}

QString Region::Range::name(Sheet* originSheet) const
{
  QString name = "";
  if (m_sheet && m_sheet != originSheet)
  {
    name = m_sheet->sheetName() + '!';
  }
  return name + Cell::name(m_range.left(), m_range.top()) + ':' +
                Cell::name(m_range.right(), m_range.bottom() );
}

bool Region::Range::contains(const QPoint& point) const
{
  return m_range.contains(point);
}

bool Region::Range::contains(const QRect& range) const
{
  return m_range.contains(range.normalized());
}

} // namespace KSpread
