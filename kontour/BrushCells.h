/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef BrushCells_h_
#define BrushCells_h_

#include <qtableview.h>
#include <qbrush.h>
#include <qlist.h>

class QColor;
class QPixmap;

class BrushCells : public QTableView {

    Q_OBJECT
public:
    BrushCells (QWidget *parent = 0L, const QColor &colour = Qt::black,
                const char *name = 0);
    ~BrushCells ();

    virtual QSize sizeHint() const { return QSize(200, 120); }
    virtual QSize minimumSizeHint() const { return QSize(200, 120); }
    virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); }

    Qt::BrushStyle brushStyle ();
    void selectBrush (Qt::BrushStyle style);
    void setColor(const QColor &);

protected:
    virtual int cellWidth (int col);
    virtual int cellHeight (int row);
    virtual void paintCell (QPainter *, int row, int col);

    virtual void mousePressEvent (QMouseEvent *event);

private:
    QList<QPixmap> brushPixmaps;
    int currentBrush;
};

#endif
