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

#include <BrushCells.h>

#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>

#define CELL_WIDTH  50
#define CELL_HEIGHT 30

BrushCells::BrushCells (QWidget *parent, const QColor &color, const char *name)
  : QTableView (parent, name) {

  setFrameStyle (QFrame::Panel | QFrame::Sunken);
  setNumRows(4);
  setNumCols(4);
  brushPixmaps.setAutoDelete(true);
  setColor(color);
  currentBrush = 0;
}

BrushCells::~BrushCells () {
}

void BrushCells::setColor(const QColor &color)
{
 brushPixmaps.clear();
 for (int i = 0; i < 14; i++) {
    QPixmap *pix=new QPixmap(CELL_WIDTH, CELL_HEIGHT);
    pix->fill(Qt::white);
    QPainter p;
    p.begin (pix);
    p.setPen(Qt::black);
    QBrush brush(color, (Qt::BrushStyle) (i + 1));
    qDrawShadeRect (&p, 0, 0, CELL_WIDTH, CELL_HEIGHT,
                    colorGroup(), true, 1, 1, &brush);
    p.end();
    brushPixmaps.append(pix);
 }
}

int BrushCells::cellWidth (int) {
  return CELL_WIDTH;
}

int BrushCells::cellHeight (int) {
  return CELL_HEIGHT;
}

void BrushCells::paintCell (QPainter *p, int row, int col) {
  int pos = row * 4 + col;
  if (pos < 14) {
    p->drawPixmap (0, 0, *brushPixmaps.at(pos));
    if (currentBrush == pos) {
      qDrawShadeRect (p, 0, 0, CELL_WIDTH - 1, CELL_HEIGHT - 1,
                      colorGroup (), false, 1, 1, 0L);
    }
  }
}

void BrushCells::mousePressEvent (QMouseEvent *event) {
  int row, col;

  row = findRow (event->y ());
  col = findCol (event->x ());

  if (row != -1 && col != -1) {
    int pos = row * 4 + col;
    if (pos < 14) {
      currentBrush = pos;
      repaint ();
    }
  }
}

Qt::BrushStyle BrushCells::brushStyle () {
  return (Qt::BrushStyle) (currentBrush + 1);
}

void BrushCells::selectBrush (Qt::BrushStyle style) {
  currentBrush = (int) style - 1;
  repaint ();
}

#undef CELL_HEIGHT
#undef CELL_WIDTH

#include <BrushCells.moc>
