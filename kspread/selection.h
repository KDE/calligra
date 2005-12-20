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

#ifndef KSPREAD_SELECTION
#define KSPREAD_SELECTION

#include <qobject.h>

#include <kdebug.h>

#include "region.h"

namespace KSpread
{
class View;

class Selection : public QObject, public Region
{
  Q_OBJECT
public:
  Selection(View*);
  Selection(const Selection&);
  ~Selection();

  /**
   * Sets the selection to the given point
   */
  void initialize(const QPoint&, Sheet* sheet = 0);
  /**
   * Sets the selection to the given range
   */
  void initialize(const QRect&, Sheet* sheet = 0);
  /**
   * Sets the selection to the given region
   */
  void initialize(const Region&, Sheet* sheet = 0);

  /**
   * Emits signal changed(const Region&)
   */
  void update();
  /**
   * Uses the anchor as starting point
   */
  void update(const QPoint&);

  /**
   * Extends the current selection with an additional point
   */
  void extend(const QPoint&, Sheet* sheet = 0);
  /**
   * Extends the current selection with an additional range
   */
  void extend(const QRect&, Sheet* sheet = 0);
  /**
   * Extends the current selection with an additional region
   */
  void extend(const Region&);

  /**
   * The anchor is the starting point of a range. For points marker and anchor are the same
   */
  const QPoint& anchor() const;
  /**
   * The cursor represents the cursor position. This needed for obscured cells
   */
  const QPoint& cursor() const;
  /**
   * The marker is the end point of a range. For points marker and anchor are the same
   */
  const QPoint& marker() const;

  /**
   * Checks wether the region consists only of one point
   */
  bool isSingular() const;
  /**
   * @return the area that the 'handle' of the selection is located in painting coordinates
   */
  QRect selectionHandleArea() const;

  /**
   * @return the name of the region (e.g. "A1:A2")
   */
  QString name(Sheet* originSheet = 0) const;

  void setSheet(Sheet* sheet);
  Sheet* sheet() const;

  void setActive(const QPoint&);

  QRect lastRange(bool extend = true) const;
  QRect extendToMergedAreas(QRect area) const;


  // TODO Stefan #2: replace this
  QRect selection(bool extend = true) const;

signals:
  void changed(const Region&);

protected:
  class Point;
  class Range;

private:
  class Private;
  Private *d;
};

// TODO Stefan: "virtual class Foo;" is not valid :(
//              find a way around it. maybe:
//              virtual class Point* Region::createPoint() { return new Region::Point; }
//              virtual class Point* Selection::createPoint() { return new Selection::Point; }
class Selection::Point : public Region::Point
{
public:

protected:
private:
  QColor m_color;
};

// TODO Stefan
class Selection::Range : public Region::Range
{
public:
  Range() : Region::Range() { kdDebug() << k_funcinfo << endl; }
protected:
private:
  QColor m_color;
};

} // namespace KSpread


/***************************************************************************
  kdDebug support
****************************************************************************/

inline kdbgstream operator<<( kdbgstream str, const KSpread::Selection& s )
{
  str << "Selection = " << s.name();
  return str;
}

#endif // KSPREAD_SELECTION
