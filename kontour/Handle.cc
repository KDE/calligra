/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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
  mMode = HMode_Default;
  mShow = true;
}

void Handle::show(bool flag)
{
  mShow = flag;
}

void Handle::mode(Mode m, bool propagate)
{
  if(mMode != m || propagate)
  {
    mMode = m;
    //m_parentDoc->activePage()->emitHandleChanged();
  }
}

void Handle::box(const KoRect &r)
{
  mBox = r;
  double x1 = r.left() - 4;
  double x3 = r.right() + 4;
  double x2 = (x1 + x3) / 2;
  double y1 = r.top() - 4;
  double y3 = r.bottom() + 4;
  double y2 = (y1 + y3) / 2;
  pos[0] = KoPoint(x1, y1);
  pos[1] = KoPoint(x2, y1);
  pos[2] = KoPoint(x3, y1);
  pos[3] = KoPoint(x3, y2);
  pos[4] = KoPoint(x3, y3);
  pos[5] = KoPoint(x2, y3);
  pos[6] = KoPoint(x1, y3);
  pos[7] = KoPoint(x1, y2);
  // TODO implement center()
//  mRotCenter = r.center();
}

void Handle::rotCenter(const KoPoint &p)
{
  mRotCenter = p;
//  m_parentDoc->activePage()->emitHandleChanged();
}

void Handle::draw(QPainter &p)
{
  if(!mShow)
    return;

  p.save();
  if(mMode == HMode_Default)
  {
    QBrush brush(Qt::blue);
    p.setBrush(brush);
    p.setPen(Qt::black);
    for(int i = 0; i < 8; i++)
    {
      p.drawRect(static_cast<int>(pos[i].x() - 3), static_cast<int>(pos[i].y() - 3), 6, 6);
      p.fillRect(static_cast<int>(pos[i].x() - 2), static_cast<int>(pos[i].y() - 2), 4, 4, brush);
    }
  }
  else
  {
    QBrush brush(Qt::black, Qt::SolidPattern);
    p.setPen (Qt::black);
    p.drawPoint(qRound(mRotCenter.x()), qRound(mRotCenter.y()));
    p.drawEllipse(qRound(mRotCenter.x()) - 5, qRound(mRotCenter.y()) - 5, 10, 10);
    p.setBrush(brush);
    p.drawLine(static_cast<int>(pos[1].x()) - 4, static_cast<int>(pos[1].y()), static_cast<int>(pos[1].x()) + 4, static_cast<int>(pos[1].y()));
    drawArrow(p, static_cast<int>(pos[1].x()) - 4, static_cast<int>(pos[1].y()), Arrow_Left);
    drawArrow(p, static_cast<int>(pos[1].x()) + 4, static_cast<int>(pos[1].y()), Arrow_Right);

    p.drawLine(static_cast<int>(pos[3].x()), static_cast<int>(pos[3].y()) - 4, static_cast<int>(pos[3].x()), static_cast<int>(pos[3].y()) + 4);
    drawArrow(p, static_cast<int>(pos[3].x()), static_cast<int>(pos[3].y()) - 4, Arrow_Up);
    drawArrow(p, static_cast<int>(pos[3].x()), static_cast<int>(pos[3].y()) + 4, Arrow_Down);

    p.drawLine(static_cast<int>(pos[5].x()) - 4, static_cast<int>(pos[5].y()), static_cast<int>(pos[5].x()) + 4, static_cast<int>(pos[5].y()));
    drawArrow(p, static_cast<int>(pos[5].x()) - 4, static_cast<int>(pos[5].y()), Arrow_Left);
    drawArrow(p, static_cast<int>(pos[5].x()) + 4, static_cast<int>(pos[5].y()), Arrow_Right);

    p.drawLine(static_cast<int>(pos[7].x()), static_cast<int>(pos[7].y()) - 4, static_cast<int>(pos[7].x()), static_cast<int>(pos[7].y()) + 4);
    drawArrow(p, static_cast<int>(pos[7].x()), static_cast<int>(pos[7].y()) - 4, Arrow_Up);
    drawArrow(p, static_cast<int>(pos[7].x()), static_cast<int>(pos[7].y()) + 4, Arrow_Down);

    p.drawArc(static_cast<int>(pos[0].x()), static_cast<int>(pos[0].y()), 10, 10, 1440, 1440);
    drawArrow(p, static_cast<int>(pos[0].x()) + 5, static_cast<int>(pos[0].y()), Arrow_Right);
    drawArrow(p, static_cast<int>(pos[0].x()), static_cast<int>(pos[0].y()) + 5, Arrow_Down);

    p.drawArc(static_cast<int>(pos[2].x()) - 10, static_cast<int>(pos[2].y()), 10, 10, 0, 1440);
    drawArrow(p, static_cast<int>(pos[2].x()) - 5, static_cast<int>(pos[2].y()), Arrow_Left);
    drawArrow(p, static_cast<int>(pos[2].x()), static_cast<int>(pos[2].y()) + 5, Arrow_Down);


    p.drawArc(static_cast<int>(pos[4].x()) - 10, static_cast<int>(pos[4].y()) - 10, 10, 10, 4320, 1440);
    drawArrow(p, static_cast<int>(pos[4].x()) - 5, static_cast<int>(pos[4].y()), Arrow_Left);
    drawArrow(p, static_cast<int>(pos[4].x()), static_cast<int>(pos[4].y()) - 5, Arrow_Up);

    p.drawArc(static_cast<int>(pos[6].x()), static_cast<int>(pos[6].y()) - 10, 10, 10, 2880, 1440);
    drawArrow(p, static_cast<int>(pos[6].x()) + 5, static_cast<int>(pos[6].y()), Arrow_Right);
    drawArrow(p, static_cast<int>(pos[6].x()), static_cast<int>(pos[6].y()) - 5, Arrow_Up);
  }
  p.restore();
}

int Handle::contains(const KoPoint &p)
{
  static int mask[] =
  {
    Kontour::HPos_Left | Kontour::HPos_Top,
    Kontour::HPos_Top,
    Kontour::HPos_Top | Kontour::HPos_Right,
    Kontour::HPos_Right,
    Kontour::HPos_Right | Kontour::HPos_Bottom,
    Kontour::HPos_Bottom,
    Kontour::HPos_Bottom | Kontour::HPos_Left,
    Kontour::HPos_Left
  };
  /* Check if one of the outer handles is selected */
  for(int i = 0; i < 8; i++)
  {
    KoRect r(pos[i].x() - 4, pos[i].y() - 4, 8, 8);
    if(r.contains(p))
      return mask[i];
  }
  /* Maybe the rotation center ? */
/*  if(mRotCenter.isNear(p, 5))
    return Kontour::HPos_Center;*/
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

