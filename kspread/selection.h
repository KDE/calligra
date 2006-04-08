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

#ifndef KSPREAD_SELECTION
#define KSPREAD_SELECTION

#include <qobject.h>
//Added by qt3to4:
#include <Q3ValueList>

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
   * @param point the point's location
   * @param sheet the sheet the point belongs to
   */
  virtual Element* eor(const QPoint& point, Sheet* sheet = 0);

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

  /**
   * Sets the selection's origin sheet.
   * @param sheet the sheet from which the selection starts
   */
  void setSheet(Sheet* sheet);
  /**
   * @return the selection's origin sheet
   */
  Sheet* sheet() const;

  /**
   * Sets the element, which has @p point as anchor, as active
   */
  void setActiveElement(const QPoint& point);
  /**
   * Sets the @p number 'th element as active
   */
  void setActiveElement(uint number);
  /**
   * @return the active element
   */
  Element* activeElement() const;
  /**
   * Sets the starting position and the length of a subregion in a multiple
   * selection
   */
  void setActiveSubRegion(uint start, uint length);
  /**
   *
   */
  QString activeSubRegionName() const;
  /**
   * Clears the elements of the subregion
   */
  void clearSubRegion();
  /**
   * fix subregion dimensions
   */
  void fixSubRegionDimension();
  /**
   * Deletes all elements of the region. The result is an empty region.
   */
  virtual void clear();


  /**
   * Sets, wether multiple selections are allowed.
   */
  void setMultipleSelection(bool state);

  QRect lastRange(bool extend = true) const;
  QRect extendToMergedAreas(QRect area) const;

  const QList<QColor>& colors() const;

  // TODO Stefan #2: replace this
  QRect selection(bool extend = true) const;

signals:
  void changed(const Region&);

protected:
  class Point;
  class Range;

  virtual Region::Point* createPoint(const QPoint&) const;
  virtual Region::Point* createPoint(const QString&) const;
  virtual Region::Point* createPoint(const Point&) const;
  virtual Region::Range* createRange(const QRect&) const;
  virtual Region::Range* createRange(const QString&) const;
  virtual Region::Range* createRange(const Range&) const;

private:
  class Private;
  Private *d;
};

/***************************************************************************
  class Selection::Point
****************************************************************************/

class Selection::Point : public Region::Point
{
public:
  Point(const QPoint& point);
  Point(const QString& string);

  void setColor(const QColor& color) { m_color = color; }
  virtual const QColor& color() const { return m_color; }

  bool columnFixed() const { return m_columnFixed; }
  bool rowFixed() const { return m_rowFixed; }

protected:

private:
  QColor m_color;
  bool m_columnFixed : 1;
  bool m_rowFixed    : 1;
};

/***************************************************************************
  class Selection::Range
****************************************************************************/

class Selection::Range : public Region::Range
{
public:
  Range(const QRect& rect);
  Range(const QString& string);

  virtual bool isColorizable() const { return true; }

  void setColor(const QColor& color) { m_color = color; }
  const QColor& color() const { return m_color; }

protected:

private:
  QColor m_color;
  bool m_leftFixed   : 1;
  bool m_rightFixed  : 1;
  bool m_topFixed    : 1;
  bool m_bottomFixed : 1;
};

} // namespace KSpread


/***************************************************************************
  kDebug support
****************************************************************************/

inline kdbgstream operator<<( kdbgstream str, const KSpread::Selection& s )
{
  str << "Selection = " << s.name();
  return str;
}

inline kndbgstream operator<<( kndbgstream str, const KSpread::Selection& )
{
  return str;
}

#endif // KSPREAD_SELECTION
