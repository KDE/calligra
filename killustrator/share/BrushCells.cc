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

#include "BrushCells.h"
#include "BrushCells.moc"

#include <qpainter.h>
#include <qdrawutl.h>
#include <iostream.h>

#define CELL_WIDTH  50
#define CELL_HEIGHT 30

BrushCells::BrushCells (QWidget *parent, const char *name)
  : QTableView (parent, name) {
  setFrameStyle (QFrame::Panel | QFrame::Sunken);
  setNumRows (3);
  setNumCols (5);
  setMinimumSize (5 * CELL_WIDTH, 3 * CELL_HEIGHT);
  for (int i = 0; i < 14; i++) {
    QPixmap pix (CELL_WIDTH, CELL_HEIGHT);
    pix.fill (white);
    QPainter p;
    p.begin (&pix);
    p.setPen (black);
    QBrush brush (black, (BrushStyle) (i + 1));
    //      p.fillRect (0, 0, CELL_WIDTH - 1, CELL_HEIGHT - 1, brush);
    qDrawShadeRect (&p, 0, 0, CELL_WIDTH, CELL_HEIGHT,
		    colorGroup (), true, 1, 1, &brush);
    p.end ();
    brushPixmaps.push_back (pix);
  }
  currentBrush = 0;
}

BrushCells::~BrushCells () {
}

int BrushCells::cellWidth (int col) {
  return CELL_WIDTH;
}

int BrushCells::cellHeight (int row) {
  return CELL_HEIGHT;
}

void BrushCells::paintCell (QPainter *p, int row, int col) {
  int pos = row * 5 + col;
  if (pos < 14) {
    p->drawPixmap (0, 0, brushPixmaps[pos]);
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
    int pos = row * 5 + col;
    if (pos < 14) {
      currentBrush = pos;
      repaint ();
    }
  }
}

Qt::BrushStyle BrushCells::brushStyle () {
  return (BrushStyle) (currentBrush + 1);
}

void BrushCells::selectBrush (Qt::BrushStyle style) {
  currentBrush = (int) style - 1;
  repaint ();
}
