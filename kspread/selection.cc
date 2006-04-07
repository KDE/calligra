/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include <qregexp.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <kdebug.h>

#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_editors.h"
#include "kspread_sheet.h"
#include "kspread_view.h"
#include "kspread_util.h"

#include "selection.h"

// TODO Stefan: Substract points in selections
// TODO Stefan: KPart signal (kspread_events.h)

using namespace KSpread;

/***************************************************************************
  class Selection::Private
****************************************************************************/

class Selection::Private
{
public:
  Private(View *v)
  {
    view = v;
    sheet = 0;
    anchor = QPoint(1,1);
    cursor = QPoint(1,1);
    marker = QPoint(1,1);

    colors.push_back(Qt::red);
    colors.push_back(Qt::blue);
    colors.push_back(Qt::magenta);
    colors.push_back(Qt::darkRed);
    colors.push_back(Qt::darkGreen);
    colors.push_back(Qt::darkMagenta);
    colors.push_back(Qt::darkCyan);
    colors.push_back(Qt::darkYellow);

    multipleSelection = false;

    activeElement = Iterator();
    activeSubRegionStart = 0;
    activeSubRegionLength = 0;
  }

  View*  view;
  Sheet* sheet;
  QPoint anchor;
  QPoint cursor;
  QPoint marker;
  Q3ValueList<QColor> colors;

  bool multipleSelection : 1;

  Selection::Iterator activeElement;
  uint activeSubRegionStart;
  uint activeSubRegionLength;
};

/***************************************************************************
  class Selection
****************************************************************************/
namespace KSpread {

Selection::Selection(View *view)
  : QObject(view), Region(1,1)
{
  d = new Private(view);
  d->activeSubRegionStart = 0;
  d->activeSubRegionLength = 1;
}

Selection::Selection(const Selection& selection)
  : QObject(selection.d->view), Region()
{
/*  kDebug() << k_funcinfo << endl;*/
  d = new Private(selection.d->view);
  d->sheet = selection.d->sheet;
  d->activeSubRegionStart = 0;
  d->activeSubRegionLength = cells().count();
}

Selection::~Selection()
{
  delete d;
}

void Selection::initialize(const QPoint& point, Sheet* sheet)
{
    if (!util_isPointValid(point))
        return;

    if (!d->view->activeSheet())
	    return;

  if (!sheet)
  {
    if (d->sheet)
    {
      sheet = d->sheet;
    }
    else
    {
      sheet = d->view->activeSheet();
    }
  }

  Region changedRegion(*this);
  changedRegion.add(extendToMergedAreas(QRect(d->anchor,d->marker)));

  QPoint topLeft(point);
  Cell* cell = d->view->activeSheet()->cellAt(point);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = point;
  d->marker = topLeft;

  fixSubRegionDimension(); // TODO remove this sanity check
  Iterator it = cells().begin() += d->activeSubRegionStart + d->activeSubRegionLength;
  if (it != insert(it, topLeft, sheet/*, true*/))
  {
    // if the point was inserted
    clearSubRegion();
  }
  Element* element = *(cells().begin() += d->activeSubRegionStart);
  // we end up with one element in the subregion
  d->activeSubRegionLength = 1;
  if (element && element->type() == Element::Point)
  {
    Point* point = static_cast<Point*>(element);
    point->setColor(d->colors[cells().size() % d->colors.size()]);
  }
  else if (element && element->type() == Element::Range)
  {
    Range* range = static_cast<Range*>(element);
    range->setColor(d->colors[cells().size() % d->colors.size()]);
  }

  d->activeElement = cells().begin();

  if (changedRegion == *this)
  {
    emit changed(Region(topLeft, sheet));
    return;
  }
  changedRegion.add(topLeft, sheet);

  emit changed(changedRegion);
}

void Selection::initialize(const QRect& range, Sheet* sheet)
{
    if (!util_isRectValid(range) || ( range == QRect(0,0,1,1) ))
        return;

  if (!sheet)
  {
    if (d->sheet)
    {
      sheet = d->sheet;
    }
    else
    {
      sheet = d->view->activeSheet();
    }
  }

  Region changedRegion(*this);
  changedRegion.add(extendToMergedAreas(QRect(d->anchor,d->marker)));

  QPoint topLeft(range.topLeft());
  Cell* cell = d->view->activeSheet()->cellAt(topLeft);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint bottomRight(range.bottomRight());
  cell = d->view->activeSheet()->cellAt(bottomRight);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    bottomRight = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = bottomRight;
  d->marker = bottomRight;

  fixSubRegionDimension(); // TODO remove this sanity check
  Iterator it = cells().begin() += d->activeSubRegionStart + d->activeSubRegionLength;
  if (it != insert(it, QRect(topLeft, bottomRight), sheet/*, true*/))
  {
    // if the range was inserted
    clearSubRegion();
  }

  Element* element = *(cells().begin() += d->activeSubRegionStart);
  // we end up with one element in the subregion
  d->activeSubRegionLength = 1;
  if (element && element->type() == Element::Point)
  {
    Point* point = static_cast<Point*>(element);
    point->setColor(d->colors[cells().size() % d->colors.size()]);
  }
  else if (element && element->type() == Element::Range)
  {
    Range* range = static_cast<Range*>(element);
    range->setColor(d->colors[cells().size() % d->colors.size()]);
  }

  d->activeElement = cells().begin();

  if (changedRegion == *this)
  {
    return;
  }
  changedRegion.add(QRect(topLeft, bottomRight), sheet);

  emit changed(changedRegion);
}

void Selection::initialize(const Region& region, Sheet* sheet)
{
    if (!region.isValid())
        return;

  if (!sheet)
  {
    if (d->sheet)
    {
      sheet = d->sheet;
    }
    else
    {
      sheet = d->view->activeSheet();
    }
  }

  Region changedRegion(*this);
  changedRegion.add(extendToMergedAreas(QRect(d->anchor,d->marker)));

  // TODO Stefan: handle subregion insertion
  // TODO Stefan: handle obscured cells correctly
  clear();
  Element* element = add(region);
  if (element && element->type() == Element::Point)
  {
    Point* point = static_cast<Point*>(element);
    point->setColor(d->colors[cells().size() % d->colors.size()]);
  }
  else if (element && element->type() == Element::Range)
  {
    Range* range = static_cast<Range*>(element);
    range->setColor(d->colors[cells().size() % d->colors.size()]);
  }

  QPoint topLeft(cells().last()->rect().normalized().topLeft());
  Cell* cell = d->view->activeSheet()->cellAt(topLeft);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint bottomRight(cells().last()->rect().normalized().bottomRight());
  cell = d->view->activeSheet()->cellAt(bottomRight);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    bottomRight = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = topLeft;
  d->marker = bottomRight;

  d->activeElement = --cells().end();

  if (changedRegion == *this)
  {
    return;
  }

  emit changed(changedRegion);
}

void Selection::update()
{
  emit changed(*this);
}

void Selection::update(const QPoint& point)
{
  uint count = cells().count();

  if (cells().isEmpty())
  {
    add(point);
    d->activeSubRegionLength += cells().count() - count;
    return;
  }
  if (d->activeElement == cells().end())
  {
    // we're not empty, so this will not become again end()
    d->activeElement--;
  }

  Sheet* sheet = (*d->activeElement)->sheet();
  if (sheet != d->view->activeSheet())
  {
    extend(point);
    d->activeSubRegionLength += cells().count() - count;
    return;
  }

  QPoint topLeft(point);
  Cell* cell = d->view->activeSheet()->cellAt(point);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  if (topLeft == d->marker)
  {
    return;
  }

  QRect area1 = (*d->activeElement)->rect().normalized();
  QRect newRange = extendToMergedAreas(QRect(d->anchor, topLeft));

  Element* oldElement = *d->activeElement;
  // returns iterator to the next element or end
  Iterator it = cells().remove(d->activeElement);
  delete oldElement;
  // returns iterator to the new element (before 'it') or 'it'
  d->activeElement = insert(it, newRange, sheet, d->multipleSelection);
  d->activeSubRegionLength += cells().count() - count;

  // The call to insert() above can just return the iterator which has been
  // passed in. This may be cells.end(), if the old active element was the
  // iterator to the list's end (!= last element). So attempts to dereference
  // it will fail.
  if (d->activeElement == cells().end())
  {
    d->activeElement--;
  }

  QRect area2 = (*d->activeElement)->rect().normalized();
  Region changedRegion;

  bool newLeft   = area1.left() != area2.left();
  bool newTop    = area1.top() != area2.top();
  bool newRight  = area1.right() != area2.right();
  bool newBottom = area1.bottom() != area2.bottom();

  /* first, calculate some numbers that we'll use a few times */
  int farLeft = qMin(area1.left(), area2.left());
  int innerLeft = qMax(area1.left(), area2.left());

  int farTop = qMin(area1.top(), area2.top());
  int innerTop = qMax(area1.top(), area2.top());

  int farRight = qMax(area1.right(), area2.right());
  int innerRight = qMin(area1.right(), area2.right());

  int farBottom = qMax(area1.bottom(), area2.bottom());
  int innerBottom = qMin(area1.bottom(), area2.bottom());

  if (newLeft)
  {
    changedRegion.add(QRect(QPoint(farLeft, innerTop),
                      QPoint(innerLeft-1, innerBottom)));
    if (newTop)
    {
      changedRegion.add(QRect(QPoint(farLeft, farTop),
                        QPoint(innerLeft-1, innerTop-1)));
    }
    if (newBottom)
    {
      changedRegion.add(QRect(QPoint(farLeft, innerBottom+1),
                        QPoint(innerLeft-1, farBottom)));
    }
  }

  if (newTop)
  {
    changedRegion.add(QRect(QPoint(innerLeft, farTop),
                      QPoint(innerRight, innerTop-1)));
  }

  if (newRight)
  {
    changedRegion.add(QRect(QPoint(innerRight+1, innerTop),
                      QPoint(farRight, innerBottom)));
    if (newTop)
    {
      changedRegion.add(QRect(QPoint(innerRight+1, farTop),
                        QPoint(farRight, innerTop-1)));
    }
    if (newBottom)
    {
      changedRegion.add(QRect(QPoint(innerRight+1, innerBottom+1),
                        QPoint(farRight, farBottom)));
    }
  }

  if (newBottom)
  {
    changedRegion.add(QRect(QPoint(innerLeft, innerBottom+1),
                      QPoint(innerRight, farBottom)));
  }

  d->marker = topLeft;
  d->cursor = point;

  emit changed(changedRegion);
}

void Selection::extend(const QPoint& point, Sheet* sheet)
{
    if (!util_isPointValid(point))
        return;

  if (isEmpty())
  {
    initialize(point, sheet);
    return;
  }
  if (d->activeElement == cells().end())
  {
    // we're not empty, so this will not become again end()
    d->activeElement--;
  }

  if (!sheet)
  {
    if (d->sheet)
    {
      sheet = d->sheet;
    }
    else
    {
      sheet = d->view->activeSheet();
    }
  }

  Region changedRegion = Region(extendToMergedAreas(QRect(d->marker,d->marker)));

  QPoint topLeft(point);
  Cell* cell = d->view->activeSheet()->cellAt(point);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  uint count = cells().count();
  if (d->multipleSelection)
  {
    d->activeElement = insert(++d->activeElement, point, sheet, false);
  }
  else
  {
    eor(topLeft, sheet);
    d->activeElement = --cells().end();
  }
  d->anchor = (*d->activeElement)->rect().topLeft();
  d->cursor = (*d->activeElement)->rect().bottomRight();
  d->marker = d->cursor;

  d->activeSubRegionLength += cells().count() - count;

  changedRegion.add(topLeft, sheet);
  changedRegion.add(*this);

  emit changed(changedRegion);
}

void Selection::extend(const QRect& range, Sheet* sheet)
{
    //See comment in Selection::initialize(const QRect& range, Sheet* sheet)
    if (!util_isRectValid(range) || (range == QRect(0,0,1,1)))
        return;

  if (isEmpty())
  {
    initialize(range, sheet);
    return;
  }
  if (d->activeElement == cells().end())
  {
    // we're not empty, so this will not become again end()
    d->activeElement--;
  }

  if (!sheet)
  {
    if (d->sheet)
    {
      sheet = d->sheet;
    }
    else
    {
      sheet = d->view->activeSheet();
    }
  }

  QPoint topLeft(range.topLeft());
  Cell* cell = d->view->activeSheet()->cellAt(topLeft);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint bottomRight(range.bottomRight());
  cell = d->view->activeSheet()->cellAt(bottomRight);
  if (cell->isObscured() && cell->isPartOfMerged())
  {
    cell = cell->obscuringCells().first();
    bottomRight = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = topLeft;
  d->marker = bottomRight;

  uint count = cells().count();
  Element* element;
  if (d->multipleSelection)
  {
    d->activeElement = insert(++d->activeElement, extendToMergedAreas(QRect(topLeft, bottomRight)).normalized(), sheet, false);
    element = *d->activeElement;
  }
  else
  {
    element = add(extendToMergedAreas(QRect(topLeft, bottomRight)).normalized(), sheet);
    d->activeElement = --cells().end();
  }
  if (element && element->type() == Element::Point)
  {
    Point* point = static_cast<Point*>(element);
    point->setColor(d->colors[cells().size() % d->colors.size()]);
  }
  else if (element && element->type() == Element::Range)
  {
    Range* range = static_cast<Range*>(element);
    range->setColor(d->colors[cells().size() % d->colors.size()]);
  }

  d->activeSubRegionLength += cells().count() - count;

  emit changed(*this);
}

void Selection::extend(const Region& region)
{
    if (!region.isValid())
        return;

  uint count = cells().count();
  ConstIterator end(region.constEnd());
  for (ConstIterator it = region.constBegin(); it != end; ++it)
  {
    Element *element = *it;
    if (element && element->type() == Element::Point)
    {
      Point* point = static_cast<Point*>(element);
      extend(point->pos(), element->sheet());
    }
    else
    {
      extend(element->rect(), element->sheet());
    }
  }

  d->activeSubRegionLength += cells().count() - count;

  emit changed(*this);
}

Selection::Element* Selection::eor(const QPoint& point, Sheet* sheet)
{
  if (isSingular())
  {
    return Region::add(point, sheet);
  }
  return Region::eor(point, sheet);
}

const QPoint& Selection::anchor() const
{
  return d->anchor;
}

const QPoint& Selection::cursor() const
{
  return d->cursor;
}

const QPoint& Selection::marker() const
{
  return d->marker;
}

bool Selection::isSingular() const
{
  return Region::isSingular();
}

QRect Selection::selectionHandleArea() const
{
  int column, row;

  // complete rows/columns are selected, use the marker.
  if (isColumnOrRowSelected())
  {
    column = d->marker.x();
    row = d->marker.y();
  }
  else
  {
    column = lastRange().right();
    row = lastRange().bottom();
  }
  const Cell* cell = d->view->activeSheet()->cellAt(column, row);

  double xpos = d->view->activeSheet()->dblColumnPos( column );
  double ypos = d->view->activeSheet()->dblRowPos( row );
  double width = cell->dblWidth( column );
  double height = cell->dblHeight( row );

  QPoint rightBottom( d->view->doc()->zoomItX( xpos + width ),
                      d->view->doc()->zoomItY( ypos + height ) );

  QRect handle( ( rightBottom.x() - 2 ),
                  ( rightBottom.y() - 2 ),
                  ( 5 ),
                  ( 5 ) );
  return handle;
}

QString Selection::name(Sheet* sheet) const
{
  return Region::name(sheet ? sheet : d->sheet);
}

void Selection::setSheet(Sheet* sheet)
{
  d->sheet = sheet;
}

Sheet* Selection::sheet() const
{
  return d->sheet;
}

void Selection::setActiveElement(const QPoint& point)
{
  uint counter = 0;
  Iterator end = cells().end();
  for (Iterator it = cells().begin(); it != end; ++it)
  {
    QRect range = (*it)->rect();
    if (range.topLeft() == point || range.bottomRight() == point)
    {
      d->anchor = range.topLeft();
      d->cursor = range.bottomRight();
      d->marker = range.bottomRight();
      d->activeElement = it;
      d->activeSubRegionStart = counter;
      d->activeSubRegionLength = 1;
      if (d->view->canvasWidget()->editor())
      {
        d->view->canvasWidget()->editor()->setCursorToRange(counter);
      }
    }
    counter++;
  }
}

void Selection::setActiveElement(uint pos)
{
  if (pos >= cells().count())
  {
    kDebug() << "Selection::setActiveElement: position exceeds list" << endl;
    d->activeElement = cells().begin();
    return;
  }

  Iterator it = cells().begin() += pos;
  QRect range = (*it)->rect();
  d->anchor = range.topLeft();
  d->cursor = range.bottomRight();
  d->marker = range.bottomRight();
  d->activeElement = it;
}

Region::Element* Selection::activeElement() const
{
  return (d->activeElement == cells().end()) ? 0 : *d->activeElement;
}

void Selection::clear()
{
  d->activeSubRegionStart = 0;
  d->activeSubRegionLength = 0;
  Region::clear();
  d->activeElement = cells().begin();
}

void Selection::clearSubRegion()
{
  if (isEmpty())
  {
    return;
  }
//   kDebug() << *this << endl;
//   kDebug() << d->activeSubRegionStart << endl;
//   kDebug() << d->activeSubRegionLength << endl;

  Iterator it = cells().begin();
  Iterator end = it += d->activeSubRegionStart;
  end += d->activeSubRegionLength;
  while (it != end)
  {
/*    kDebug() << (*it)->name() << endl;*/
    delete *it;
    it = cells().remove(it);
  }
  d->activeSubRegionLength = 0;
  d->activeElement = it;
/*  kDebug() << "ENDE" << endl;*/
}

void Selection::fixSubRegionDimension()
{
  if (d->activeSubRegionStart > cells().count())
  {
    kDebug() << "Selection::fixSubRegionDimension: start position exceeds list" << endl;
    d->activeSubRegionStart = 0;
    d->activeSubRegionLength = cells().count();
    return;
  }
  if (d->activeSubRegionStart + d->activeSubRegionLength > cells().count())
  {
    kDebug() << "Selection::fixSubRegionDimension: length exceeds list" << endl;
    d->activeSubRegionLength = cells().count() - d->activeSubRegionStart;
    return;
  }
}

void Selection::setActiveSubRegion(uint start, uint length)
{
//   kDebug() << k_funcinfo << endl;
  d->activeSubRegionStart = start;
  d->activeSubRegionLength = length;
  fixSubRegionDimension();
  d->activeElement = cells().begin() += d->activeSubRegionStart;
}

QString Selection::activeSubRegionName() const
{
//   kDebug() << k_funcinfo << endl;
//   kDebug() << *this << endl;
//   kDebug() << "start = " << d->activeSubRegionStart << ", len = " << d->activeSubRegionLength << endl;

  QStringList names;
  Iterator it = cells().begin();
  it += d->activeSubRegionStart;
  Iterator end = it;
  end += d->activeSubRegionLength;
  while (it != end)
  {
    names += (*it++)->name(d->sheet);
  }
/*  kDebug() << "ENDE" << endl;*/
  return names.isEmpty() ? "" : names.join(";");
}

void Selection::setMultipleSelection(bool state)
{
  d->multipleSelection = state;
}

const Q3ValueList<QColor>& Selection::colors() const
{
  return d->colors;
}

QRect Selection::lastRange(bool extend) const
{
  QRect selection = QRect(d->anchor, d->marker).normalized();
  return extend ? extendToMergedAreas(selection) : selection;
}

QRect Selection::selection(bool extend) const
{
  QRect selection = QRect(d->anchor, d->marker).normalized();
  return extend ? extendToMergedAreas(selection) : selection;
}

QRect Selection::extendToMergedAreas(QRect area) const
{
  if (!d->view->activeSheet())
	  return area;

  area = area.normalized(); // TODO Stefan: avoid this
  const Cell *cell = d->view->activeSheet()->cellAt(area.left(), area.top());

  if( Region::Range(area).isColumn() || Region::Range(area).isRow() )
  {
    return area;
  }
  else if ( !(cell->isObscured() && cell->isPartOfMerged()) &&
              (cell->mergedXCells() + 1) >= area.width() &&
              (cell->mergedYCells() + 1) >= area.height())
  {
    /* if just a single cell is selected, we need to merge even when
    the obscuring isn't forced.  But only if this is the cell that
    is doing the obscuring -- we still want to be able to click on a cell
    that is being obscured.
    */
    area.setWidth(cell->mergedXCells() + 1);
    area.setHeight(cell->mergedYCells() + 1);
  }
  else
  {
    int top=area.top();
    int left=area.left();
    int bottom=area.bottom();
    int right=area.right();
    for ( int x = area.left(); x <= area.right(); x++ )
      for ( int y = area.top(); y <= area.bottom(); y++ )
    {
      cell = d->view->activeSheet()->cellAt( x, y );
      if( cell->doesMergeCells())
      {
        right=qMax(right,cell->mergedXCells()+x);
        bottom=qMax(bottom,cell->mergedYCells()+y);
      }
      else if ( cell->isObscured() && cell->isPartOfMerged() )
      {
        cell = cell->obscuringCells().first();
        left=qMin(left,cell->column());
        top=qMin(top,cell->row());
        bottom=qMax(bottom,cell->row() + cell->mergedYCells());
        right=qMax(right,cell->column() + cell->mergedXCells());
      }
    }

    area.setCoords(left,top,right,bottom);
  }
  return area;
}

Selection::Region::Point* Selection::createPoint(const QPoint& point) const
{
  return new Point(point);
}

Selection::Region::Point* Selection::createPoint(const QString& string) const
{
  return new Point(string);
}

Selection::Region::Point* Selection::createPoint(const Point& point) const
{
  return new Point(point);
}

Selection::Region::Range* Selection::createRange(const QRect& rect) const
{
  return new Range(rect);
}

Selection::Region::Range* Selection::createRange(const QString& string) const
{
  return new Range(string);
}

Selection::Region::Range* Selection::createRange(const Range& range) const
{
  return new Range(range);
}

/***************************************************************************
  class Point
****************************************************************************/

Selection::Point::Point(const QPoint& point)
  : Region::Point(point),
    m_color(Qt::black),
    m_columnFixed(false),
    m_rowFixed(false)
{
}

Selection::Point::Point(const QString& string)
  : Region::Point(string),
    m_color(Qt::black),
    m_columnFixed(false),
    m_rowFixed(false)
{
  if (!isValid())
  {
    return;
  }

  uint p = 0;
  // Fixed?
  if (string[p++] == '$')
  {
    m_columnFixed = true;
  }

  //search for the first character != text
  int result = string.find( QRegExp("[^A-Za-z]+"), p );
  if (string[result] == '$')
  {
    m_rowFixed = true;
  }
}

/***************************************************************************
  class Range
****************************************************************************/

Selection::Range::Range(const QRect& range)
  : Region::Range(range),
    m_color(Qt::black),
    m_leftFixed(false),
    m_rightFixed(false),
    m_topFixed(false),
    m_bottomFixed(false)
{
}

Selection::Range::Range(const QString& string)
  : Region::Range(string),
    m_color(Qt::black),
    m_leftFixed(false),
    m_rightFixed(false),
    m_topFixed(false),
    m_bottomFixed(false)
{
  if (!isValid())
  {
    return;
  }

  int delimiterPos = string.find(':');
  if (delimiterPos == -1)
  {
    return;
  }

  Selection::Point ul(string.left(delimiterPos));
  Selection::Point lr(string.mid(delimiterPos + 1));

  if (!ul.isValid() || !lr.isValid())
  {
    return;
  }
  m_leftFixed = ul.columnFixed();
  m_rightFixed = lr.columnFixed();
  m_topFixed = ul.rowFixed();
  m_bottomFixed = lr.rowFixed();
}

} // namespace KSpread
#include "selection.moc"
