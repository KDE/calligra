/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include <kdebug.h>

#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

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
  }

  View*  view;
  Sheet* sheet;
  QPoint anchor;
  QPoint cursor;
  QPoint marker;
};


/***************************************************************************
  class Selection
****************************************************************************/

Selection::Selection(View *view)
  : QObject(view), Region(1,1)
{
  d = new Private(view);
}

Selection::Selection(const Selection& selection)
  : QObject(selection.d->view), Region()
{
  kdDebug() << k_funcinfo << endl;
  d = new Private(selection.d->view);
}

Selection::~Selection()
{
  delete d;
}

void Selection::initialize(const QPoint& point, Sheet* sheet)
{
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
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = point;
  d->marker = topLeft;

  clear();
  add(topLeft, sheet);

  if (changedRegion == *this)
  {
    return;
  }
  changedRegion.add(topLeft, sheet);

  emit changed(changedRegion);
}

void Selection::initialize(const QRect& range, Sheet* sheet)
{
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
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint bottomRight(range.bottomRight());
  cell = d->view->activeSheet()->cellAt(bottomRight);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    bottomRight = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = topLeft;
  d->marker = bottomRight;

  clear();
  add(QRect(topLeft, bottomRight), sheet);

  if (changedRegion == *this)
  {
    return;
  }
  changedRegion.add(QRect(topLeft, bottomRight), sheet);

  emit changed(changedRegion);
}

void Selection::initialize(const Region& region, Sheet* sheet)
{
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

  // TODO Stefan: handle obscured cells correctly
  clear();
  add(region);

  QPoint topLeft(cells().last()->rect().normalize().topLeft());
  Cell* cell = d->view->activeSheet()->cellAt(topLeft);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint bottomRight(cells().last()->rect().normalize().bottomRight());
  cell = d->view->activeSheet()->cellAt(bottomRight);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    bottomRight = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = topLeft;
  d->marker = bottomRight;

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
  Sheet* sheet = cells().last()->sheet();
  if (sheet != d->view->activeSheet())
  {
    extend(point);
    return;
  }

  QPoint topLeft(point);
  Cell* cell = d->view->activeSheet()->cellAt(point);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  if (topLeft == d->marker)
  {
    return;
  }

  QRect area1 = cells().last()->rect().normalize();
  cells().remove(cells().last());
  add(extendToMergedAreas(QRect(d->anchor,topLeft)), sheet);

  QRect area2 = cells().last()->rect().normalize();
  Region changedRegion;

  bool newLeft   = area1.left() != area2.left();
  bool newTop    = area1.top() != area2.top();
  bool newRight  = area1.right() != area2.right();
  bool newBottom = area1.bottom() != area2.bottom();

  /* first, calculate some numbers that we'll use a few times */
  int farLeft = QMIN(area1.left(), area2.left());
  int innerLeft = QMAX(area1.left(), area2.left());

  int farTop = QMIN(area1.top(), area2.top());
  int innerTop = QMAX(area1.top(), area2.top());

  int farRight = QMAX(area1.right(), area2.right());
  int innerRight = QMIN(area1.right(), area2.right());

  int farBottom = QMAX(area1.bottom(), area2.bottom());
  int innerBottom = QMIN(area1.bottom(), area2.bottom());

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
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = point;
  d->marker = topLeft;

  add(topLeft, sheet);
  changedRegion.add(*this);

  emit changed(changedRegion);
}

void Selection::extend(const QRect& range, Sheet* sheet)
{
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
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    topLeft = QPoint(cell->column(), cell->row());
  }

  QPoint bottomRight(range.bottomRight());
  cell = d->view->activeSheet()->cellAt(bottomRight);
  if (cell->isObscured() && cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    bottomRight = QPoint(cell->column(), cell->row());
  }

  d->anchor = topLeft;
  d->cursor = topLeft;
  d->marker = bottomRight;

  add(extendToMergedAreas(QRect(topLeft, bottomRight)).normalize(), sheet);

  emit changed(*this);
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
  if ( isRowSelected() || isColumnSelected() )
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

QString Selection::name() const
{
  return Region::name(d->sheet);
}

void Selection::setSheet(Sheet* sheet)
{
  d->sheet = sheet;
}

Sheet* Selection::sheet() const
{
  return d->sheet;
}

void Selection::setActive(const QPoint& point)
{
  Element* element = 0;
  Iterator end = cells().end();
  for (Iterator it = cells().begin(); it != end; ++it)
  {
    QRect range = (*it)->rect();
    if (range.topLeft() == point || range.bottomRight() == point)
    {
      d->anchor = range.topLeft();
      d->cursor = range.bottomRight();
      d->marker = range.bottomRight();
      element = (*it);
      it = cells().remove(it);
    }
  }
  cells().append(element); // TODO Stefan: avoid the move to the end
}


QRect Selection::lastRange(bool extend) const
{
  QRect selection = QRect(d->anchor, d->marker).normalize();
  return extend ? extendToMergedAreas(selection) : selection;
}

QRect Selection::selection(bool extend) const
{
  QRect selection = QRect(d->anchor, d->marker).normalize();
  return extend ? extendToMergedAreas(selection) : selection;
}

QRect Selection::extendToMergedAreas(QRect area) const
{
  area = area.normalize();
  const Cell *cell = d->view->activeSheet()->cellAt(area.left(), area.top());

  if( Region::Range(area).isColumn() || Region::Range(area).isRow() )
  {
    return area;
  }
  else if ( !(cell->isObscured() && cell->isObscuringForced()) &&
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
      if( cell->isForceExtraCells())
      {
        right=QMAX(right,cell->mergedXCells()+x);
        bottom=QMAX(bottom,cell->mergedYCells()+y);
      }
      else if ( cell->isObscured() && cell->isObscuringForced() )
      {
        cell = cell->obscuringCells().first();
        left=QMIN(left,cell->column());
        top=QMIN(top,cell->row());
        bottom=QMAX(bottom,cell->row() + cell->mergedYCells());
        right=QMAX(right,cell->column() + cell->mergedXCells());
      }
    }

    area.setCoords(left,top,right,bottom);
  }
  return area;
}

#include "selection.moc"
