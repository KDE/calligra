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

#include <Handle.h>
#include <Painter.h>
#include <qpainter.h>
#include <qnamespace.h>
#include "GDocument.h"
#include "GPage.h"

Handle::Handle (GDocument* parent)
:m_parentDoc(parent)
,mode(HMode_Default)
,showIt(true)
{}

void Handle::show (bool flag) {
  showIt = flag;
}

void Handle::setBox (const Rect& r) {
  box = r;
  float x1 = r.left () - 5;
  float x3 = r.right () + 5;
  float x2 = (x1 + x3) / 2;
  float y1 = r.top () - 5;
  float y3 = r.bottom () + 5;
  float y2 = (y1 + y3) / 2;
  pos[0] = Coord (x1, y1);
  pos[1] = Coord (x2, y1);
  pos[2] = Coord (x3, y1);
  pos[3] = Coord (x3, y2);
  pos[4] = Coord (x3, y3);
  pos[5] = Coord (x2, y3);
  pos[6] = Coord (x1, y3);
  pos[7] = Coord (x1, y2);
  rcenter = r.center ();
}

void Handle::setRotCenter (const Coord& p) {
  rcenter = p;
  m_parentDoc->activePage()->emitHandleChanged();
}

void Handle::draw (QPainter& p) {
  if (! showIt)
    return;

  p.save ();
  if (mode == HMode_Default) {
     QBrush brush (Qt::black);
    p.setBrush (brush);
    for (int i = 0; i < 8; i++)
      p.fillRect ((int) pos[i].x () - 2, (int) pos[i].y () - 2, 4, 4, brush);
  }
  else {
     QBrush brush (Qt::black, Qt::SolidPattern);

     p.setPen (Qt::black);
    p.drawPoint (qRound(rcenter.x ()), qRound(rcenter.y ()));
    p.drawEllipse (qRound (rcenter.x ()) - 5, qRound (rcenter.y ()) - 5,
                   10, 10);

    p.setBrush (brush);
    p.drawLine (pos[1].x () - 4, pos[1].y (),
                pos[1].x () + 4, pos[1].y ());
    drawArrow (p, (int) pos[1].x () - 4, (int) pos[1].y (), Arrow_Left);
    drawArrow (p, (int) pos[1].x () + 4, (int) pos[1].y (), Arrow_Right);

    p.drawLine ((int) pos[3].x (), (int) pos[3].y () - 4,
                (int) pos[3].x (), (int) pos[3].y () + 4);
    drawArrow (p, (int) pos[3].x (), (int) pos[3].y () - 4, Arrow_Up);
    drawArrow (p, (int) pos[3].x (), (int) pos[3].y () + 4, Arrow_Down);

    p.drawLine ((int) pos[5].x () - 4, (int) pos[5].y (),
                (int) pos[5].x () + 4, (int) pos[5].y ());
    drawArrow (p, (int) pos[5].x () - 4, (int) pos[5].y (), Arrow_Left);
    drawArrow (p, (int) pos[5].x () + 4, (int) pos[5].y (), Arrow_Right);

    p.drawLine ((int) pos[7].x (), (int) pos[7].y () - 4,
                (int) pos[7].x (), (int) pos[7].y () + 4);
    drawArrow (p, (int) pos[7].x (), (int) pos[7].y () - 4, Arrow_Up);
    drawArrow (p, (int) pos[7].x (), (int) pos[7].y () + 4, Arrow_Down);

    p.drawArc ((int) pos[0].x (), (int) pos[0].y (),
               10, 10, 1440, 1440);
    drawArrow (p, (int) pos[0].x () + 5, (int) pos[0].y (), Arrow_Right);
    drawArrow (p, (int) pos[0].x (), (int) pos[0].y () + 5, Arrow_Down);

    p.drawArc ((int) pos[2].x () - 10, (int) pos[2].y (),
               10, 10, 0, 1440);
    drawArrow (p, (int) pos[2].x () - 5, (int) pos[2].y (), Arrow_Left);
    drawArrow (p, (int) pos[2].x (), (int) pos[2].y () + 5, Arrow_Down);


    p.drawArc ((int) pos[4].x () - 10, (int) pos[4].y () - 10,
               10, 10, 4320, 1440);
    drawArrow (p, (int) pos[4].x () - 5, (int) pos[4].y (), Arrow_Left);
    drawArrow (p, (int) pos[4].x (), (int) pos[4].y () - 5, Arrow_Up);

    p.drawArc ((int) pos[6].x (), (int) pos[6].y () - 10,
               10, 10, 2880, 1440);
    drawArrow (p, (int) pos[6].x () + 5, (int) pos[6].y (), Arrow_Right);
    drawArrow (p, (int) pos[6].x (), (int) pos[6].y () - 5, Arrow_Up);
  }
  p.restore ();
}

int Handle::contains (const Coord& p) {
  static int mask[] = {
    /*
    Handle_Left|Handle_Top, Handle_Top,
    Handle_Top|Handle_Right, Handle_Right,
    Handle_Right|Handle_Bottom, Handle_Bottom,
    Handle_Bottom|Handle_Left, Handle_Left
    */
     HPos_Left | HPos_Top,
     HPos_Top,
     HPos_Top | HPos_Right,
     HPos_Right,
     HPos_Right | HPos_Bottom,
     HPos_Bottom,
     HPos_Bottom | HPos_Left,
     HPos_Left
  };

  // Check if one of the outer handles is selected
  for (int i = 0; i < 8; i++) {
    Rect r (pos[i].x () - 4, pos[i].y () - 4, 8, 8);
    if (r.contains (p))
      return mask[i];
  }
  // Maybe the rotation center ?
  if (rcenter.isNear (p, 5))
    return HPos_Center;

  return 0;
}

void Handle::setMode (Handle::Mode m, bool propagate)
{
   if (mode != m || propagate)
   {
      mode = m;
      m_parentDoc->activePage()->emitHandleChanged();
   }
}

void Handle::drawArrow (QPainter& p, int x, int y, ArrowDirection d) {
  static QCOORD rpoints[] = { 0, -2, 6, 0, 0, 2};
  static QCOORD lpoints[] = { 0, -2, -6, 0, 0, 2};
  static QCOORD upoints[] = { -2, 0, 0, -6, 2, 0};
  static QCOORD dpoints[] = { -2, 0, 0, 6, 2, 0};

  QPointArray pt (3);
  switch (d) {
  case Arrow_Left:
    pt.putPoints (0, 3, lpoints);
    break;
  case Arrow_Right:
    pt.putPoints (0, 3, rpoints);
    break;
  case Arrow_Up:
    pt.putPoints (0, 3, upoints);
    break;
  case Arrow_Down:
    pt.putPoints (0, 3, dpoints);
    break;
  }
  pt.translate (x, y);
  p.drawPolygon (pt);
}

