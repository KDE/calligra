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

#include "Ruler.h"

#include <math.h>

#include <qpainter.h>
#include <qpixmap.h>

#include "units.h"
#include "GDocument.h"
#include "kontour_doc.h"

Ruler::Ruler(KontourDocument *aDoc, Orientation o, MeasurementUnit mu, QWidget *parent, const char *name):
QFrame(parent, name)
{
  mDoc = aDoc;
  mOrientation = o;
  mUnit = mu;
  
  zoom = mDoc->document()->zoomFactor();
  zeroPoint = 0;
  currentPosition = -1;
  buffer = 0L;
  
  setFrameStyle(Box | Raised);
  setLineWidth(1);
  setMidLineWidth(0);
  setMouseTracking(true);
  
  isMousePressed = false;

  if(mOrientation == Horizontal)
  {
    setFixedHeight(20);
    bg = new QPixmap(1, 20);
  }
  else
  {
    setFixedWidth(20);
    bg = new QPixmap(20, 1);
  }
  
  const char *nums[] =
  {
    "70 7 2 1",
    "  c Black",
    "X c None",
    "XX   XXXXXX XXXX   XXXX   XXXXXX XXX     XXXX  XXX     XXX   XXXX   XX",
    "X XXX XXXX  XXX XXX XX XXX XXXX  XXX XXXXXXX XXXXXXXXX XX XXX XX XXX X",
    "X XXX XXXXX XXXXXXX XXXXXX XXX X XXX XXXXXX XXXXXXXXX XXX XXX XX XXX X",
    "X XXX XXXXX XXXXX  XXXXX  XXX XX XXX    XXX    XXXXXX XXXX   XXXX    X",
    "X XXX XXXXX XXXX XXXXXXXXX XX     XXXXXX XX XXX XXXX XXXX XXX XXXXXX X",
    "X XXX XXXXX XXX XXXXXX XXX XXXXX XXXXXXX XX XXX XXXX XXXX XXX XXXXX XX",
    "XX   XXXXXX XXX     XXX   XXXXXX XXX    XXXX   XXXXX XXXXX   XXXX  XXX"
  };
  mNums = new QPixmap(nums);
}

Ruler::~Ruler()
{
  delete bg;
  delete buffer;
}

void Ruler::unit(MeasurementUnit mu)
{
  mUnit = mu;
  drawRuler();
  updatePointer(currentPosition, currentPosition);
  repaint();
}

void Ruler::zoomFactor(int xpos, int ypos)
{
  zoom = mDoc->document()->zoomFactor();
  if(mOrientation == Horizontal)
    zeroPoint = -xpos;
  else
    zeroPoint = -ypos;
  drawRuler();
  repaint();
}

void Ruler::updatePointer(int x, int y)
{
  if(!buffer)
    return;

  QRect r1, r2;
  if(mOrientation == Horizontal)
  {
    if(currentPosition != -1)
    {
      r1 = QRect(currentPosition, 0, 1, 20);
      bitBlt(buffer, currentPosition, 0, bg, 0, 0, 1, 20);
    }
    if(x != -1)
    {
      r2 = QRect(x, 0, 1, 20);
      bitBlt(bg, 0, 0, buffer, x, 0, 1, 20);
      currentPosition = x;
      drawMarker();
    }
  }
  else
  {
    if(currentPosition != -1)
    {
      r1 = QRect(0, currentPosition, 20, 1);
      bitBlt(buffer, 0, currentPosition, bg, 0, 0, 20, 1);
    }
    if(y != -1)
    {
      r2 = QRect(0, y, 20, 1);
      bitBlt(bg, 0, 0, buffer, 0, y, 20, 1);
      currentPosition = y;
      drawMarker();
    }
  }
  repaint(r1);
  repaint(r2);
}

void Ruler::updateOffset(int o)
{
  if(mOrientation == Horizontal)
    zeroPoint = -o;
  else
    zeroPoint = -o;
  drawRuler();
  repaint();
}

void Ruler::show()
{
  if(mOrientation == Horizontal)
    setFixedHeight(20);
  else
    setFixedWidth(20);
  QWidget::show();
}

void Ruler::hide()
{
  if(mOrientation == Horizontal)
    setFixedHeight(1);
  else
    setFixedWidth(1);
}

void Ruler::paintEvent(QPaintEvent *e)
{
  if(!buffer)
    return;

  const QRect &rect = e->rect();
  
//  kdDebug()<< "UPDATE x=" << rect.x() << " y=" << rect.y() << " w=" << rect.width() << " h=" << rect.height() << endl;

  if(mOrientation == Horizontal)
    bitBlt(this, rect.x(), rect.y(), buffer, rect.x(), rect.y(), rect.width(), rect.height());
  else
    bitBlt(this, rect.x(), rect.y(), buffer, rect.x(), rect.y(), rect.width(), rect.height());
  QFrame::paintEvent(e);
}

void Ruler::resizeEvent(QResizeEvent *e)
{
  recalculateSize(e);
}

void Ruler::mousePressEvent(QMouseEvent *e)
{
  if(!e)
    return;
  if (e->button()==Qt::LeftButton)
    isMousePressed = true;
  else if(e->button()==Qt::RightButton)
    emit rmbPressed();
}

void Ruler::mouseMoveEvent(QMouseEvent *me)
{
  if(mOrientation == Horizontal)
    updatePointer(me->x(), 0);
  else
    updatePointer(0, me->y());
  if(isMousePressed && mDoc->isReadWrite())
    emit drawHelpline(me->x() - 20, me->y() - 20, mOrientation == Horizontal);
}

void Ruler::mouseReleaseEvent(QMouseEvent *me)
{
  if(isMousePressed && mDoc->isReadWrite())
  {
    isMousePressed = false;
    emit drawHelpline(-1, -1, mOrientation == Horizontal);
    emit addHelpline(me->x() - 20, me->y() - 20, mOrientation == Horizontal);
  }
}

void Ruler::recalculateSize(QResizeEvent *)
{
  delete buffer;
  buffer = 0L;

  if(!isVisible())
    return;

  int w, h;
  if(mOrientation == Horizontal)
  {
    w = width();
    h = 20;
  }
  else {
    w = 20;
    h = height();
  }
  buffer = new QPixmap(w, h);
  drawRuler();
  updatePointer(currentPosition, currentPosition);
}

void Ruler::drawRuler()
{
  QPainter p;
  int start = 0;
  int pos = 0;
  double step = 0;
  bool s1 = false;
  bool s2 = false;
  bool s3 = false;

  if(!buffer)
    return;

  p.begin(buffer);
  p.setBackgroundColor(colorGroup().background());
  p.setPen(black);
  p.setFont(QFont("helvetica", 8));
  p.eraseRect(0, 0, buffer->width(), buffer->height());

  switch(mUnit)
  {
  case UnitPoint:
    s1 = cvtInchToPt(10.0) * zoom > 3.0;
    s2 = cvtInchToPt(50.0) * zoom > 3.0;
    s3 = cvtInchToPt(100.0) * zoom > 3.0;
    step = 30.0 / (100.0 * zoom);
    start = static_cast<int>((double)zeroPoint / zoom);
    break;
  case UnitInch:
    s1 = cvtInchToPt(0.1) * zoom > 3.0;
    s2 = cvtInchToPt(0.5) * zoom > 3.0;
    s3 = cvtInchToPt(1.0) * zoom > 3.0;
    step = 24.0 / (cvtInchToPt(1.0) * zoom);
    start = 10 * static_cast<int>(cvtPtToInch(zeroPoint) / zoom);
    break;
  case UnitCentimeter:
  case UnitMillimeter:
    s1 = cvtMmToPt(1.0) * zoom > 3.0;
    s2 = cvtMmToPt(5.0) * zoom > 3.0;
    s3 = cvtMmToPt(10.0) * zoom > 3.0;
    step = 30.0 / (cvtMmToPt(10.0) * zoom);
    start = static_cast<int>(cvtPtToMm(zeroPoint) / zoom);
    break;
  case UnitPica:
    s1 = cvtPicaToPt(1.0) * zoom > 3.0;
    s2 = cvtPicaToPt(5.0) * zoom > 3.0;
    s3 = cvtPicaToPt(10.0) * zoom > 3.0;
    step = 30.0 / (cvtPicaToPt(10.0) * zoom);
    start = static_cast<int>(cvtPtToPica(zeroPoint) / zoom);
    break;
  case UnitDidot:
    s1 = cvtDidotToPt(10.0) * zoom > 3.0;
    s2 = cvtDidotToPt(50.0) * zoom > 3.0;
    s3 = cvtDidotToPt(100.0) * zoom > 3.0;
    step = 30.0 / (cvtDidotToPt(100.0) * zoom);
    start = static_cast<int>(cvtPtToDidot(zeroPoint) / zoom);
    break;
  case UnitCicero:
    s1 = cvtCiceroToPt(1.0) * zoom > 3.0;
    s2 = cvtCiceroToPt(5.0) * zoom > 3.0;
    s3 = cvtCiceroToPt(10.0) * zoom > 3.0;
    step = 30.0 / (cvtCiceroToPt(10.0) * zoom);
    start = static_cast<int>(cvtPtToCicero(zeroPoint) / zoom);
    break;
  }
  
  if(fabs(step) < 0.9)
    step = 1;

   //workaround, will fix it, aleXXX
  step = qRound(step);
  if(mOrientation == Horizontal)
    switch(mUnit)
    {
    case UnitPoint:
    {
      do
      {
        pos = static_cast<int>(start * zoom - zeroPoint);
        if(s3 && start % 100 == 0)
          p.drawLine (pos, 10, pos, 20);
        if(s2 && start % 50 == 0)
          p.drawLine (pos, 13, pos, 20);
        if(s1 && start % 10 == 0)
          p.drawLine (pos, 15, pos, 20);
        if( start % qRound(step * 100) == 0 )
          drawNum(p, pos, 3, start, true);
        start++;
      }while(pos < buffer->width());
      break;
    }
    case UnitMillimeter:
    {
      do
      {
        pos = static_cast<int>(cvtMmToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(pos, 10, pos, 20);
        if(s2 && start % 5 == 0)
          p.drawLine(pos, 13, pos, 20);
        if(s1)
          p.drawLine(pos, 15, pos, 20);
        if(start % qRound(step * 10) == 0)
          drawNum(p, pos, 3, start, true);
        start++;
      }while(pos < buffer->width());
      break;
    }
    case UnitCentimeter:
    {
      do
      {
        pos = static_cast<int>(cvtMmToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(pos, 10, pos, 20);
        if(s2 && start % 5 == 0)
          p.drawLine(pos, 13, pos, 20);
        if(s1)
          p.drawLine(pos, 15, pos, 20);
        if(start % qRound(step * 10) == 0)
          drawNum(p, pos, 3, start / 10, true);
        start++;
      }while(pos < buffer->width());
      break;
    }
    case UnitDidot:
    {
      do
      {
        pos = static_cast<int>(cvtDidotToPt(start) * zoom - zeroPoint);
        if(s3 && start % 100 == 0)
          p.drawLine(pos, 10, pos, 20);
        if(s2 && start % 50 == 0)
          p.drawLine(pos, 13, pos, 20);
        if(s1 && start % 10 == 0)
          p.drawLine(pos, 15, pos, 20);
        if(start % qRound(step * 100) == 0)
          drawNum(p, pos, 3, start, true);
        start++;
      }while(pos < buffer->width());
      break;
    }
    case UnitCicero:
    {
      do
      {
        pos = static_cast<int>(cvtCiceroToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(pos, 10, pos, 20);
        if(s2 && start % 5 == 0)
          p.drawLine(pos, 13, pos, 20);
        if(s1)
          p.drawLine(pos, 15, pos, 20);
        if(start % qRound(step * 10) == 0)
          drawNum(p, pos, 3, start, true);
        start++;
      }while(pos < buffer->width());
      break;
    }
    case UnitPica:
    {
      do
      {
        pos = static_cast<int>(cvtPicaToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine (pos, 10, pos, 20);
        if(s2 && start % 5 == 0)
          p.drawLine (pos, 13, pos, 20);
        if(s1)
          p.drawLine(pos, 15, pos, 20);
        if(start % qRound(step * 10) == 0)
          drawNum(p, pos, 3, start, true);
        start++;
      }while(pos < buffer->width());
      break;
    }
    case UnitInch:
    {
      do
      {
        pos = static_cast<int>(cvtInchToPt(start / 10.0) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(pos, 10, pos, 20);
        if(s2 && start % 5 == 0)
          p.drawLine(pos, 13, pos, 20);
        if(s1)
          p.drawLine(pos, 15, pos, 20);
        if(start % qRound(step * 10) == 0)
          drawNum(p, pos, 3, start / 10, true);
        start++;
      }while(pos < buffer->width());
      break;
    }
    }
  else
    switch(mUnit)
    {
    case UnitPoint:
    {
      do
      {
        pos = static_cast<int>(start * zoom  - zeroPoint);
        if(s3 && start % 100 == 0)
          p.drawLine(10, pos, 20, pos);
        if(s2 && start % 50 == 0)
          p.drawLine(13, pos, 20, pos);
        if(s1 && start % 10 == 0)
          p.drawLine(15, pos, 20, pos);
        if(start % qRound(step * 100) == 0)
          drawNum(p, 2, pos, start, false);
        start++;
      }while(pos < buffer->height());
      break;
    }
    case UnitDidot:
    {
      do
      {
        pos = static_cast<int>(cvtDidotToPt(start) * zoom - zeroPoint);
        if(s3 && start % 100 == 0)
          p.drawLine(10, pos, 20, pos);
        if(s2 && start % 50 == 0)
          p.drawLine(13, pos, 20, pos);
        if(s1 && start % 10 == 0)
          p.drawLine(15, pos, 20, pos);
        if(start % qRound(step * 100) == 0)
          drawNum(p, 2, pos, start, false);
        start++;
      }while(pos < buffer->height());
      break;
    }
    case UnitMillimeter:
    {
      do
      {
        pos = static_cast<int>(cvtMmToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(10, pos, 20, pos);
        if(s2 && start % 5 == 0)
          p.drawLine(13, pos, 20, pos);
        if(s1)
          p.drawLine(15, pos, 20, pos);
        if(start % qRound(step * 10) == 0)
          drawNum(p, 2, pos, start, false);
        start++;
      }while(pos < buffer->height());
      break;
    }
    case UnitCentimeter:
    {
      do
      {
        pos = static_cast<int>(cvtMmToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(10, pos, 20, pos);
        if(s2 && start % 5 == 0)
          p.drawLine(13, pos, 20, pos);
        if(s1)
          p.drawLine(15, pos, 20, pos);
        if(start % qRound(step * 10) == 0)
          drawNum(p, 2, pos, start / 10, false);
        start++;
      }while(pos < buffer->height());
      break;
    }
    case UnitCicero:
    {
      do
      {
        pos = static_cast<int>(cvtCiceroToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(10, pos, 20, pos);
        if(s2 && start % 5 == 0)
          p.drawLine(13, pos, 20, pos);
        if(s1)
          p.drawLine(15, pos, 20, pos);
        if(start % qRound(step * 10) == 0)
          drawNum(p, 2, pos, start, false);
        start++;
      }while(pos < buffer->height());
      break;
    }
    case UnitPica:
    {
      do
      {
        pos = static_cast<int>(cvtPicaToPt(start) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(10, pos, 20, pos);
        if(s2 && start % 5 == 0)
          p.drawLine(13, pos, 20, pos);
        if(s1)
          p.drawLine(15, pos, 20, pos);
        if(start % qRound(step * 10) == 0)
          drawNum(p, 2, pos, start, false);
        start++;
      }while(pos < buffer->height());
      break;
    }
    case UnitInch:
      do
      {
        pos = static_cast<int>(cvtInchToPt(start/10.0) * zoom - zeroPoint);
        if(s3 && start % 10 == 0)
          p.drawLine(10, pos, 20, pos);
        if(s2 && start % 5 == 0)
          p.drawLine(13, pos, 20, pos);
        if(s1)
          p.drawLine(15, pos, 20, pos);
        if(start % qRound(step * 10) == 0)
          drawNum(p, 2, pos, start / 10, false);
        start++;
      }while(pos < buffer->height());
    }
  p.end();
  //TODO killu draws its rulers 6 (!!!!) times on startup..., aleXXX
}

void Ruler::drawMarker()
{
  QPainter p;
  p.begin(buffer);
  p.setPen(red);
  if(mOrientation == Horizontal)
    p.drawLine(currentPosition, 0, currentPosition, 19);
  else
    p.drawLine(0, currentPosition, 19, currentPosition);
  p.end();
}

void Ruler::drawNum(QPainter &p, int x, int y, int a, bool orient)
{
  QString num;
  if(a < 0)
    a = -a;
  num.setNum(a);
  int l = num.length();
  if(orient)
    x -= 4 * l;
  else
    y -= 4 * l;

  for(unsigned int k = 0; k < num.length(); k++)
  {
    int st = num.at(k).digitValue() * 7;
    p.drawPixmap(x, y, *mNums, st, 0, 7, 7);
    if(orient)
      x += 7;
    else
      y += 8;
  }
}

#include "Ruler.moc"
