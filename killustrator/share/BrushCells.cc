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
#include <qdrawutil.h>
#include <qcolor.h>
#include <iostream.h>

using namespace std;

#define CELL_WIDTH  50
#define CELL_HEIGHT 30

BrushCells::BrushCells (QWidget *parent, const QColor &color, const char *name) 
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
    p.setPen (QT_PRFX::black);
    QBrush brush (color, (QT_PRFX::BrushStyle) (i + 1));
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

void BrushCells::setColor(const QColor &color)
{
 brushPixmaps.clear();
 for (int i = 0; i < 14; i++) {
    QPixmap pix (CELL_WIDTH, CELL_HEIGHT);
    pix.fill (white);
    QPainter p;
    p.begin (&pix);
    p.setPen (QT_PRFX::black);
    QBrush brush (color, (QT_PRFX::BrushStyle) (i + 1));
    //      p.fillRect (0, 0, CELL_WIDTH - 1, CELL_HEIGHT - 1, brush);
    qDrawShadeRect (&p, 0, 0, CELL_WIDTH, CELL_HEIGHT, 
		    colorGroup (), true, 1, 1, &brush);
    p.end ();
    brushPixmaps.push_back (pix);
 }
}

int BrushCells::cellWidth (int) {
  return CELL_WIDTH;
}

int BrushCells::cellHeight (int) {
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

QT_PRFX::BrushStyle BrushCells::brushStyle () {
  return (QT_PRFX::BrushStyle) (currentBrush + 1);
}

void BrushCells::selectBrush (QT_PRFX::BrushStyle style) {
  currentBrush = (int) style - 1;
  repaint ();
}

#undef CELL_HEIGHT
#undef CELL_WIDTH
