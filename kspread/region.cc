/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
  QValueList<Element*> cells;
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
    int delimiterPos = (*it).find(':');
    if (delimiterPos > -1)
    {
      // range
      QString sRegion = *it;
      if (!sheet)
      {
        sheet = filterSheetName(sRegion);
      }
      Range* range = new Range(sRegion);
      range->setSheet(sheet);
      d->cells.append(range);
    }
    else
    {
      // single cell
      QString sRegion = *it;
      if (!sheet)
      {
        sheet = filterSheetName(sRegion);
      }
      Point* point = new Point(sRegion);
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
    kdError(36001) << "Region::Region(const QRect&): QRect is empty!" << endl;
    return;
  }
  add(rect, sheet);
}

Region::Region(const QPoint& point, Sheet* sheet)
{
  d = new Private();

  if (point.isNull())
  {
    kdError(36001) << "Region::Region(const QPoint&): QPoint is empty!" << endl;
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
      d->cells.append(new Point(*point));
    }
    else
    {
      Range* range = static_cast<Range*>(element);
      d->cells.append(new Range(*range));
    }
  }
}

Region::Region(int x, int y, Sheet* sheet)
{
  d = new Private();

  if (x<1 || y<1)
  {
    kdError(36001) << "Region::Region(int x, int y): Coordinates are invalid!" << endl;
    return;
  }
  add(QPoint(x,y), sheet);
}

Region::Region(int x, int y, int width, int height, Sheet* sheet)
{
  d = new Private();

  if (x<1 || y<1 || width<1 || height<1)
  {
    kdError(36001) << "Region::Region(int x, int y, int width, int height): Dimensions are invalid!" << endl;
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

void Region::add(const QPoint& point, Sheet* sheet)
{
//   kdDebug() << k_funcinfo << endl;
  if (point.x() < 1 || point.y() < 1)
  {
    return;
  }

  bool containsPoint = false;
  bool adjacentPoint = false;
  QRect neighbour;

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
      //return;
    }
    else
    {
      neighbour = element->rect().normalize();
      neighbour.setTopLeft(neighbour.topLeft() - QPoint(1,1));
      neighbour.setBottomRight(neighbour.bottomRight() + QPoint(1,1));
      if (neighbour.contains(point))
      {
        adjacentPoint = true; // TODO Stefan: Implement!
        break;
      }
    }
  }
  if ( !containsPoint )
  {
    Point* rpoint = new Point(point);
    rpoint->setSheet(sheet);
    d->cells.append(rpoint);
  }
}

void Region::add(const QRect& range, Sheet* sheet)
{
  if (range.width() == 0 || range.height() == 0)
  {
    return;
  }
  if (range.size() == QSize(1,1))
  {
    add(range.topLeft(), sheet);
    return;
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
      it = d->cells.remove(it);
      continue;
    }
    ++it;
  }
  if ( !containsRange )
  {
    Range* rrange = new Range(range);
    rrange->setSheet(sheet);
    d->cells.append(rrange);
    if (isValid())
    {
//       kdDebug() << name() << endl;
    }
  }
}

void Region::add(const Region& region)
{
  ConstIterator endOfList(region.d->cells.constEnd());
  for (ConstIterator it = region.d->cells.constBegin(); it != endOfList; ++it)
  {
    add((*it)->rect(), (*it)->sheet());
  }
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
    if (element->rect().normalize() == range.normalize())
    {
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

void Region::eor(const QPoint& point, Sheet* sheet)
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
    QRect fullRange = (*it)->rect().normalize();
    delete *it;
    it = cells().remove(it);

    // top range
    int left = fullRange.left();
    int top = fullRange.top();
    int width = fullRange.width();
    int height = y - top;
    insert(it, QRect(left, top, width, height), sheet);
    // left range
    left = fullRange.left();
    top = y;
    width = QMAX(0, x - left);
    height = 1;
    insert(it, QRect(left, top, width, height), sheet);
    // right range
    left = QMIN(x+1, fullRange.right());
    top = y;
    width = QMAX(0, fullRange.right() - x);
    height = 1;
    insert(it, QRect(left, top, width, height), sheet);
    // bottom range
    left = fullRange.left();
    top = y+1;
    width = fullRange.width();
    height = QMAX(0, fullRange.bottom() - y);
    insert(it, QRect(left, top, width, height), sheet);
  }

  if (!containsPoint)
  {
    add(point, sheet);
  }
}

void Region::insert(Region::Iterator pos, const QPoint& point, Sheet* sheet)
{
  if (point.x() < 1 || point.y() < 1)
  {
    return;
  }
  Point* rpoint = new Point(point);
  rpoint->setSheet(sheet);
  d->cells.insert(pos, 1, rpoint);
}

void Region::insert(Region::Iterator pos, const QRect& range, Sheet* sheet)
{
  if (range.width() == 0 || range.height() == 0)
  {
    return;
  }
  if (range.size() == QSize(1,1))
  {
    insert(pos, range.topLeft(), sheet);
    return;
  }
  Range* rrange = new Range(range);
  rrange->setSheet(sheet);
  d->cells.insert(pos, 1, rrange);
}

bool Region::isColumnAffected(uint col) const
{
  ConstIterator endOfList(d->cells.constEnd());
  for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it)
  {
    Element *element = *it;
    QRect normalizedRegion = element->rect().normalize();
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
    QRect normalizedRegion = element->rect().normalize();
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
    QRect region = element->rect().normalize();
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
    QRect region = element->rect().normalize();
    if ((row == 0 || ((int)row >= region.top() && (int)row <= region.bottom())) &&
         region.left() == 1 && region.right() == KS_colMax)
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
  for (Iterator it = d->cells.begin(); it != end; it = d->cells.remove(it))
  {
    delete *it;
  }
}

Region::ConstIterator Region::constBegin() const
{
  return d->cells.constBegin();
}

Region::ConstIterator Region::constEnd() const
{
  return d->cells.constEnd();
}

QValueList<Region::Element*>& Region::cells() const
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
      d->cells.append(new Point(*point));
    }
    else
    {
      Range* range = static_cast<Range*>(element);
      d->cells.append(new Range(*range));
    }
  }
}

Sheet* Region::filterSheetName(QString& sRegion)
{
  Sheet* sheet;
  int delimiterPos = sRegion.find( '!' );
  if (delimiterPos > -1)
  {
    QString sheetName = sRegion.left(delimiterPos);
    // remove the '!'
    sRegion = sRegion.right(sRegion.length() - delimiterPos - 1);
    sheet = d->view->doc()->map()->findSheet(sheetName);
    if (!sheet)
    {
      kdDebug() << "Sheet " << sheetName << " not found. Using active sheet!" << endl;
      sheet = d->view->activeSheet();
    }
  }
  return sheet;
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

Region::Point::Point()
  : Region::Element(),
    m_point(),
    m_columnFixed(false),
    m_rowFixed(false)
{
}

Region::Point::Point(const QPoint& point)
  : Region::Element(),
    m_point(point),
    m_columnFixed(false),
    m_rowFixed(false)
{
}

Region::Point::Point(const QString& sCell)
  : Region::Element(),
    m_point(),
    m_columnFixed(false),
    m_rowFixed(false)
{
    uint length = sCell.length();

    if (length == 0)
    {
        kdDebug(36001) << "Point::init: length = 0" << endl;
        return;
    }

    QString string = sCell;//Region::filterSheetName(sCell);

    uint p = 0;

    // Fixed ?
    if (string[0] == '$')
    {
        m_columnFixed = true;
        p++;
    }

    // Malformed ?
    if (p == length)
    {
        kdDebug(36001) << "Point::init: no point after '$' (string: '" << string.mid(p) << "'" << endl;
        return;
    }

    if (string[p] < 'A' || string[p] > 'Z')
    {
        if (string[p] < 'a' || string[p] > 'z')
        {
            kdDebug(36001) << "Point::init: wrong first character in point (string: '" << string.mid(p) << "'" << endl;
            return;
        }
    }
    //default is error
    int x = -1;
    //search for the first character != text
    int result = string.find( QRegExp("[^A-Za-z]+"), p );

    //get the colomn number for the character between actual position and the first non text charakter
    if ( result != -1 )
    {
        x = util_decodeColumnLabelText( string.mid( p, result - p ) ); // x is defined now
    }
    else  // If there isn't any, then this is not a point -> return
    {
        kdDebug(36001) << "Point::init: no number in string (string: '" << string.mid( p, result ) << "'" << endl;
        return;
    }
    p = result;

    //limit is KS_colMax
    if ( x > KS_colMax )
    {
        kdDebug(36001) << "Point::init: column value too high (col: " << x << ")" << endl;
        return;
    }

    // Malformed ?
    if (p == length)
    {
        kdDebug(36001) << "Point::init: p==length after cols" << endl;
        return;
    }

    if (string[p] == '$')
    {
        m_rowFixed = true;
        p++;
	// Malformed ?
        if ( p == length )
        {
            kdDebug(36001) << "Point::init: p==length after $ of row" << endl;
            return;
        }
    }

    uint p2 = p;
    while ( p < length )
    {
        if (!QChar(string[p++]).isDigit())
        {
            kdDebug(36001) << "Point::init: no number" << endl;
            return;
        }
    }

    bool ok;
    int y = string.mid( p2, p-p2 ).toInt( &ok );
    if ( !ok )
    {
        kdDebug(36001) << "Point::init: Invalid number (string: '" << string.mid( p2, p-p2 ) << "'" << endl;
        return;
    }
    if ( y > KS_rowMax )
    {
        kdDebug(36001) << "Point::init: row value too high (row: " << y << ")" << endl;
        return;
    }
    if ( y <= 0 )
    {
        kdDebug(36001) << "Point::init: y <= 0" << endl;
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

Region::Range::Range()
  : Region::Element(),
      m_range(),
      m_leftFixed(false),
      m_rightFixed(false),
      m_topFixed(false),
      m_bottomFixed(false)
{
}

Region::Range::Range(const QRect& rect)
  : Region::Element(),
      m_range(rect),
      m_leftFixed(false),
      m_rightFixed(false),
      m_topFixed(false),
      m_bottomFixed(false)
{
}

Region::Range::Range(const QString& sRange)
  : Region::Element(),
      m_range(),
      m_leftFixed(false),
      m_rightFixed(false),
      m_topFixed(false),
      m_bottomFixed(false)
{
    int delimiterPos = sRange.find(':');
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
    m_leftFixed = ul.columnFixed();
    m_rightFixed = lr.columnFixed();
    m_topFixed = ul.rowFixed();
    m_bottomFixed = lr.rowFixed();
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
  return m_range.normalize().contains(point);
}

bool Region::Range::contains(const QRect& range) const
{
  return m_range.normalize().contains(range.normalize());
}

} // namespace KSpread
