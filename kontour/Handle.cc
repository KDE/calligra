/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

#include "Handle.h"

#include <qpainter.h>
#include <qnamespace.h>

#include "kontour_global.h"
#include "GPage.h"

Handle::Handle(GPage *aGPage)
{
  mGPage = aGPage;
  mShow = false;
  mEmpty = true;
}

void Handle::show(bool flag)
{
  mShow = flag;
}

void Handle::empty(bool flag)
{
  mEmpty = flag;
}

void Handle::box(const KoRect &r)
{
  mBox = r;
  double x1 = r.left();
  double x3 = r.right();
  double x2 = (x1 + x3) / 2;
  double y1 = r.top();
  double y3 = r.bottom();
  double y2 = (y1 + y3) / 2;
  pos[0] = KoPoint(x1, y1);
  pos[1] = KoPoint(x2, y1);
  pos[2] = KoPoint(x3, y1);
  pos[3] = KoPoint(x3, y2);
  pos[4] = KoPoint(x3, y3);
  pos[5] = KoPoint(x2, y3);
  pos[6] = KoPoint(x1, y3);
  pos[7] = KoPoint(x1, y2);
  mRotCenter = r.center();
}

void Handle::rotCenter(const KoPoint &p)
{
  mRotCenter = p;
}

void Handle::draw(QPainter &p)
{
  if(!mShow)
    return;

  if(mEmpty)
    return;

  QBrush brush;
  p.save();
  p.setPen(Qt::black);

  p.drawPoint(qRound(mRotCenter.x()), qRound(mRotCenter.y()));
  p.drawEllipse(qRound(mRotCenter.x()) - 5, qRound(mRotCenter.y()) - 5, 11, 11);

  brush = QBrush(Qt::black);
  p.setBrush(brush);
  /* Top arrow */
  p.drawLine(static_cast<int>(pos[1].x()) - 5, static_cast<int>(pos[1].y()) - 5, static_cast<int>(pos[1].x()) + 4, static_cast<int>(pos[1].y()) - 5);
  drawArrow(p, static_cast<int>(pos[1].x()) - 5, static_cast<int>(pos[1].y()) - 5, Arrow_Left);
  drawArrow(p, static_cast<int>(pos[1].x()) + 4, static_cast<int>(pos[1].y()) - 5, Arrow_Right);

  /* Right arrow */
  p.drawLine(static_cast<int>(pos[3].x()) + 4, static_cast<int>(pos[3].y()) - 5, static_cast<int>(pos[3].x()) + 4, static_cast<int>(pos[3].y()) + 4);
  drawArrow(p, static_cast<int>(pos[3].x()) + 4, static_cast<int>(pos[3].y()) - 5, Arrow_Up);
  drawArrow(p, static_cast<int>(pos[3].x()) + 4, static_cast<int>(pos[3].y()) + 4, Arrow_Down);

  /* Bottom arrow */
  p.drawLine(static_cast<int>(pos[5].x()) - 5, static_cast<int>(pos[5].y()) + 4, static_cast<int>(pos[5].x()) + 4, static_cast<int>(pos[5].y()) + 4);
  drawArrow(p, static_cast<int>(pos[5].x()) - 5, static_cast<int>(pos[5].y()) + 4, Arrow_Left);
  drawArrow(p, static_cast<int>(pos[5].x()) + 4, static_cast<int>(pos[5].y()) + 4, Arrow_Right);

  /* Left arrow */
  p.drawLine(static_cast<int>(pos[7].x()) - 5, static_cast<int>(pos[7].y()) - 5, static_cast<int>(pos[7].x()) - 5, static_cast<int>(pos[7].y()) + 4);
  drawArrow(p, static_cast<int>(pos[7].x()) - 5, static_cast<int>(pos[7].y()) - 5, Arrow_Up);
  drawArrow(p, static_cast<int>(pos[7].x()) - 5, static_cast<int>(pos[7].y()) + 4, Arrow_Down);

  /* Left Top arrow */
  p.drawArc(static_cast<int>(pos[0].x()) - 5, static_cast<int>(pos[0].y()) - 5, 20, 20, 1440, 1440);
  drawArrow(p, static_cast<int>(pos[0].x()) + 5, static_cast<int>(pos[0].y()) - 5, Arrow_Right);
  drawArrow(p, static_cast<int>(pos[0].x()) - 5, static_cast<int>(pos[0].y()) + 5, Arrow_Down);

  /* Right Top arrow */
  p.drawArc(static_cast<int>(pos[2].x()) - 15, static_cast<int>(pos[2].y()) - 5, 20, 20, 0, 1440);
  drawArrow(p, static_cast<int>(pos[2].x()) - 5, static_cast<int>(pos[2].y()) - 5, Arrow_Left);
  drawArrow(p, static_cast<int>(pos[2].x()) + 4, static_cast<int>(pos[2].y()) + 5, Arrow_Down);

  /* Right Bottom arrow */
  p.drawArc(static_cast<int>(pos[4].x()) - 15, static_cast<int>(pos[4].y()) - 15, 20, 20, 4320, 1440);
  drawArrow(p, static_cast<int>(pos[4].x()) - 5, static_cast<int>(pos[4].y()) + 4, Arrow_Left);
  drawArrow(p, static_cast<int>(pos[4].x()) + 4, static_cast<int>(pos[4].y()) - 5, Arrow_Up);

  /* Left Bottom arrow */
  p.drawArc(static_cast<int>(pos[6].x()) - 5, static_cast<int>(pos[6].y()) - 15, 20, 20, 2880, 1440);
  drawArrow(p, static_cast<int>(pos[6].x()) + 5, static_cast<int>(pos[6].y()) + 4, Arrow_Right);
  drawArrow(p, static_cast<int>(pos[6].x()) - 5, static_cast<int>(pos[6].y()) - 5, Arrow_Up);

  brush = QBrush(Qt::blue);
  p.setBrush(brush);
  for(int i = 0; i < 8; i++)
  {
    p.drawRect(static_cast<int>(pos[i].x() - 3), static_cast<int>(pos[i].y() - 3), 6, 6);
    p.fillRect(static_cast<int>(pos[i].x() - 2), static_cast<int>(pos[i].y() - 2), 4, 4, brush);
  }
  p.restore();
}

int Handle::contains(const KoPoint &p)
{
  static int mask[] =
  {
    Kontour::HPosLeft | Kontour::HPosTop,
    Kontour::HPosTop,
    Kontour::HPosTop | Kontour::HPosRight,
    Kontour::HPosRight,
    Kontour::HPosRight | Kontour::HPosBottom,
    Kontour::HPosBottom,
    Kontour::HPosBottom | Kontour::HPosLeft,
    Kontour::HPosLeft
  };
  /* Check if one of the outer handles is selected */
  for(int i = 0; i < 8; i++)
  {
    KoRect r(pos[i].x() - 4, pos[i].y() - 4, 8, 8);
    if(r.contains(p))
      return mask[i];
  }
  /* Maybe the rotation center ? */
  if(mRotCenter.isNear(p, 5.0))
    return Kontour::HPosCenter;
  return 0;
}

void Handle::drawArrow(QPainter &p, int x, int y, ArrowDirection d)
{
  static QCOORD rpoints[] = { 0, -2, 6, 0, 0, 2};
  static QCOORD lpoints[] = { 0, -2, -6, 0, 0, 2};
  static QCOORD upoints[] = { -2, 0, 0, -6, 2, 0};
  static QCOORD dpoints[] = { -2, 0, 0, 6, 2, 0};

  QPointArray pt(3);
  switch(d)
  {
  case Arrow_Left:
    pt.putPoints(0, 3, lpoints);
    break;
  case Arrow_Right:
    pt.putPoints(0, 3, rpoints);
    break;
  case Arrow_Up:
    pt.putPoints(0, 3, upoints);
    break;
  case Arrow_Down:
    pt.putPoints(0, 3, dpoints);
    break;
  }
  pt.translate(x, y);
  p.drawPolygon(pt);
}
