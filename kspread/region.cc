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

#include <qregexp.h>
#include <qstringlist.h>

#include <kdebug.h>

#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_global.h"
#include "kspread_map.h"
#include "kspread_sheet.h"
#include "kspread_util.h"
#include "kspread_view.h"

#include "region.h"

namespace KSpread
{

class Region::Private
{
public:
  Private()
  {
    view = 0;
  }

  View* view;
  QList<Element*> cells;
};


/***************************************************************************
  class Region
****************************************************************************/

Region::Region()
{
  d = new Private();
}

Region::Region(View* view, const QString& string, Sheet* sheet)
{
  d = new Private();
  d->view = view;

  if (string.isEmpty())
  {
    return;
  }
  QStringList substrings = QStringList::split(';', string);
  QStringList::ConstIterator end = substrings.constEnd();
  for (QStringList::ConstIterator it = substrings.constBegin(); it != end; ++it)
  {
    int delimiterPos = (*it).indexOf(':');
    if (delimiterPos > -1)
    {
      // range
      QString sRegion = *it;
      if (!sheet)
      {
        sheet = filterSheetName(sRegion);
      }

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
      QString sRegion = *it;
      if (!sheet)
      {
        sheet = filterSheetName(sRegion);
      }
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
  d->view = list.d->view;

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

  if (x<1 || y<1)
  {
    kError(36001) << "Region::Region(int x, int y): Coordinates are invalid!" << endl;
    return;
  }
  add(QPoint(x,y), sheet);
}

Region::Region(int x, int y, int width, int height, Sheet* sheet)
{
  d = new Private();

  if (x<1 || y<1 || width<1 || height<1)
  {
    kError(36001) << "Region::Region(int x, int y, int width, int height): Dimensions are invalid!" << endl;
    return;
  }
  add(QRect(x,y,width,height), sheet);
}


Region::~Region()
{
  d->cells.clear();
  delete d;
}

View* Region::view() const
{
  Q_ASSERT(d->view);
  return d->view;
}

void Region::setView(View* view)
{
  d->view = view;
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
//   kDebug() << k_funcinfo << endl;
  if (point.x() < 1 || point.y() < 1)
  {
    return 0;
  }
  return *insert(d->cells.end(), point, sheet, false);
}

Region::Element* Region::add(const QRect& range, Sheet* sheet)
{
  if (range.normalized().width() == 0 || range.normalized().height() == 0)
  {
    return 0;
  }
  if (range.size() == QSize(1,1))
  {
    return add(range.topLeft(), sheet);
  }
  return *insert(d->cells.end(), range, sheet, false);
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

void Region::sub(const QPoint& point)
{
  // TODO Stefan: Improve!
  Iterator endOfList(d->cells.end());
  for (Iterator it = d->cells.begin(); it != endOfList; ++it)
  {
    Element *element = *it;
    if (element->rect() == QRect(point,point))
    {
      delete element;
      d->cells.remove(element);
      break;
    }
  }
}

void Region::sub(const QRect& range)
{
  // TODO Stefan: Improve!
  Iterator endOfList(d->cells.end());
  for (Iterator it = d->cells.begin(); it != endOfList; ++it)
  {
    Element *element = *it;
    if (element->rect().normalized() == range.normalized())
    {
      delete element;
      d->cells.remove(element);
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

  Iterator it = cells().begin();
  Iterator endOfList = cells().end();
  while (it != endOfList)
  {
    if (!(*it)->contains(point))
    {
      ++it;
      continue;
    }
    containsPoint = true;
    int x = point.x();
    int y = point.y();
    QRect fullRange = (*it)->rect().normalized();
    delete *it;
    it = cells().erase(it);

    // top range
    int left = fullRange.left();
    int top = fullRange.top();
    int width = fullRange.width();
    int height = y - top;
    if (height > 0)
    {
      insert(it, QRect(left, top, width, height), sheet);
    }
    // left range
    left = fullRange.left();
    top = y;
    width = qMax(0, x - left);
    height = 1;
    if (width > 0)
    {
      insert(it, QRect(left, top, width, height), sheet);
    }
    // right range
    left = qMin(x+1, fullRange.right());
    top = y;
    width = qMax(0, fullRange.right() - x);
    height = 1;
    if (width > 0)
    {
      insert(it, QRect(left, top, width, height), sheet);
    }
    // bottom range
    left = fullRange.left();
    top = y+1;
    width = fullRange.width();
    height = qMax(0, fullRange.bottom() - y);
    if (height > 0)
    {
      insert(it, QRect(left, top, width, height), sheet);
    }
    return *it;
  }

  if (!containsPoint)
  {
    return add(point, sheet);
  }
  return 0;
}

Region::Iterator Region::insert(Region::Iterator pos, const QPoint& point, Sheet* sheet, bool multi)
{
  if (point.x() < 1 || point.y() < 1)
  {
    return pos;
  }

  bool containsPoint = false;
//   bool adjacentPoint = false;
//   QRect neighbour;

  // we don't have to check for occurences?
  if (multi)
  {
    Point* rpoint = createPoint(point);
    rpoint->setSheet(sheet);
    return d->cells.insert(pos, rpoint);
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
      neighbour = element->rect().normalized();
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
    return d->cells.insert(pos, rpoint);
  }
  return pos;
}

Region::Iterator Region::insert(Region::Iterator pos, const QRect& range, Sheet* sheet, bool multi)
{
  if (range.size() == QSize(1,1))
  {
    return insert(pos, range.topLeft(), sheet);
  }

  if (multi)
  {
    Range* rrange = createRange(range);
    rrange->setSheet(sheet);
    return d->cells.insert(pos, rrange);
  }

  bool containsRange = false;

  Iterator it( d->cells.begin() );
  Iterator endOfList( d->cells.end() );
  while ( it != endOfList )
  {
    if (sheet && sheet != (*it)->sheet())
    {
      ++it;
      continue;
    }
    if ((*it)->contains(range))
    {
      containsRange = true;
    }
    else if (range.contains((*it)->rect()))
    {
      delete *it;
      it = d->cells.erase(it);
      continue;
    }
    ++it;
  }
  if ( !containsRange )
  {
    Range* rrange = createRange(range);
    rrange->setSheet(sheet);
    return d->cells.insert(pos, rrange);
  }
  return pos;
}

bool Region::isColumnAffected(uint col) const
{
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    QRect normalizedRegion = element->rect().normalized();
    if ((int)col >= normalizedRegion.left() && (int)col <= normalizedRegion.right())
    {
      return true;
    }
  }
  return false;
}

bool Region::isRowAffected(uint row) const
{
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    QRect normalizedRegion = element->rect().normalized();
    if ((int)row >= normalizedRegion.top() && (int)row <= normalizedRegion.bottom())
    {
      return true;
    }
  }
  return false;
}

bool Region::isColumnSelected(uint col) const
{
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    QRect region = element->rect().normalized();
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
    QRect region = element->rect().normalized();
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
    QRect region = element->rect().normalized();
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
  Iterator end(d->cells.end());
  for (Iterator it = d->cells.begin(); it != end; it = d->cells.erase(it))
  {
    delete *it;
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
    QRect range = (*it)->rect().normalized();
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
  d->view = other.d->view;
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
    sheet = d->view->doc()->map()->findSheet(sheetName);
    if (!sheet)
    {
      kDebug() << "Sheet " << sheetName << " not found. Using active sheet!" << endl;
      sheet = d->view->activeSheet();
    }
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
      kDebug(36001) << "Region::Point::init: no number in string (string: '" << string.mid( p, result ) << "'" << endl;
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
    name = m_sheet->sheetName() + "!";
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
    name = m_sheet->sheetName() + "!";
  }
  return name + Cell::name(m_range.left(), m_range.top()) + ":" +
                Cell::name(m_range.right(), m_range.bottom() );
}

bool Region::Range::contains(const QPoint& point) const
{
  return m_range.normalized().contains(point);
}

bool Region::Range::contains(const QRect& range) const
{
  return m_range.normalized().contains(range.normalized());
}

} // namespace KSpread
