/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_ruler.h"
#include <stdlib.h>
#include <math.h>

#define MARKER_WIDTH 1
#define MARKER_HEIGHT 20

#define RULER_SIZE 20

KivioRuler::KivioRuler(Orientation o, QWidget *parent, const char *name)
: QFrame(parent, name, WRepaintNoErase, WResizeNoErase)
{
  setBackgroundMode(NoBackground);
  setFrameStyle(Box | Sunken/*Raised*/);
  setLineWidth(1);
  setMidLineWidth(0);
  orientation = o;
  munit = UnitPoint;
  m_pZoom = 1.0;
  firstVisible = 0;
  buffer = 0L;
  currentPosition = -1;

  if (orientation == Horizontal) {
    setFixedHeight(RULER_SIZE);
    initMarker(MARKER_WIDTH, MARKER_HEIGHT);
  } else {
    setFixedWidth(RULER_SIZE);
    initMarker(MARKER_HEIGHT, MARKER_WIDTH);
  }

  const char* nums[] = {
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
  m_pNums = new QPixmap(nums);
}

KivioRuler::~KivioRuler()
{
  if (buffer)
    delete buffer;

  delete marker;
  delete m_pNums;
}

void KivioRuler::initMarker(int w, int h)
{
  QPainter p;
  marker = new QPixmap(w,h);
  p.begin(marker);
  p.setPen(blue);
  p.eraseRect(0, 0, w, h);
  p.drawLine(0,0,w-1,h-1);
  p.end();
}

void KivioRuler::recalculateSize(QResizeEvent*)
{
  if (buffer) {
    delete buffer;
    buffer = 0L;
  }

  int w, h;

  if (orientation == Horizontal) {
    w = width();
    h = RULER_SIZE;
  } else {
    w = RULER_SIZE;
    h = height();
  }
  buffer = new QPixmap(w, h);
  drawKivioRuler();
  updatePointer(currentPosition,currentPosition);
}

int KivioRuler::unit()
{
  return  munit;
}

void KivioRuler::setUnit(int u)
{
  munit = u;
  drawKivioRuler();
  updatePointer(currentPosition,currentPosition);
  repaint();
}

void KivioRuler::setZoom(float zoom)
{
  m_pZoom = zoom;
  recalculateSize(0L);
  drawKivioRuler();
  updatePointer(currentPosition, currentPosition);
  repaint();
}

void KivioRuler::updatePointer(int x, int y )
{
  if (! buffer)
    return;

  if (orientation == Horizontal) {
    if (currentPosition != -1) {
      repaint(QRect(currentPosition, 1, MARKER_WIDTH, MARKER_HEIGHT));
    }
    if (x != -1) {
      bitBlt(this, x, 1, marker, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
      currentPosition = x;
    }
  } else {
    if (currentPosition != -1) {
      repaint(QRect (1, currentPosition, MARKER_HEIGHT, MARKER_WIDTH));
    }
    if (y != -1) {
      bitBlt(this, 1, y, marker, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
      currentPosition = y;
    }
  }
}

void KivioRuler::updateVisibleArea( int xpos, int ypos )
{
  if (orientation == Horizontal)
    firstVisible = xpos;
  else
    firstVisible = ypos;

  drawKivioRuler();
  repaint();
  updatePointer(currentPosition, currentPosition);
}

void KivioRuler::paintEvent( QPaintEvent* e )
{
  if (!buffer)
    return;

  const QRect& rect = e->rect();

  if (orientation == Horizontal) {
      bitBlt(this, rect.topLeft(), buffer, rect);
  } else {
      bitBlt(this, rect.topLeft(), buffer, rect);
  }

  QFrame::paintEvent(e);
}

void KivioRuler::drawKivioRuler()
{
  QPainter p;
  QString buf;

  int st1 = 0;
  int st2 = 0;
  int st3 = 0;
  int st4 = 0;
  int stt = 0;

  if (!buffer)
    return;

  p.begin(buffer);
  p.setPen(QColor(0x70,0x70,0x70));
  p.setBackgroundColor(colorGroup().background());
  p.eraseRect(0, 0, buffer->width(), buffer->height());

  switch (munit) {
    case UnitPoint:
    case UnitMillimeter:
    case UnitDidot:
    case UnitCicero:
      st1 = 1;
      st2 = 5;
      st3 = 10;
      st4 = 25;
      stt = 5;
      break;
    case UnitCentimeter:
    case UnitPica:
    case UnitInch:
      st1 = 1;
      st2 = 2;
      st3 = 5;
      st4 = 10;
      stt = 1;
      break;
    default:
      break;
  }

  if (orientation == Horizontal) {
    int pos = 0;
    bool s1 = cvtUnitToPt(munit,st1)*m_pZoom > 3.0;
    bool s2 = cvtUnitToPt(munit,st2)*m_pZoom > 3.0;
    bool s3 = cvtUnitToPt(munit,st3)*m_pZoom > 3.0;
    bool s4 = cvtUnitToPt(munit,st4)*m_pZoom > 3.0;

    float cx = cvtPtToUnit(munit,7*4)/m_pZoom;
    int step = ((int)(cx/(float)stt)+1)*stt;
    int start = (int)(cvtPtToUnit(munit,firstVisible)/m_pZoom);

    do {
      pos = (int)(cvtUnitToPt(munit,start)*m_pZoom - firstVisible);

      if ( !s3 && s4 && start % st4 == 0 )
        p.drawLine(pos,RULER_SIZE-9,pos,RULER_SIZE);

      if ( s3 && start % st3 == 0 )
        p.drawLine(pos,RULER_SIZE-9,pos,RULER_SIZE);

      if ( s2 && start % st2 == 0 )
        p.drawLine(pos,RULER_SIZE-7,pos,RULER_SIZE);

      if ( s1 && start % st1 == 0 )
        p.drawLine(pos,RULER_SIZE-5,pos,RULER_SIZE);

      if ( start % step == 0 ) {
        buf.setNum(abs(start));
        drawNums(&p,pos,4,buf,true);
      }

      start++;
    } while (pos < buffer->width());

  } else {
    int pos = 0;
    bool s1 = cvtUnitToPt(munit,st1)*m_pZoom > 3.0;
    bool s2 = cvtUnitToPt(munit,st2)*m_pZoom > 3.0;
    bool s3 = cvtUnitToPt(munit,st3)*m_pZoom > 3.0;
    bool s4 = cvtUnitToPt(munit,st4)*m_pZoom > 3.0;

    float cx = cvtPtToUnit(munit,8*4)/m_pZoom;
    int step = ((int)(cx/(float)stt)+1)*stt;
    int start = (int)(cvtPtToUnit(munit,firstVisible)/m_pZoom);

    do {
      pos = (int)(cvtUnitToPt(munit,start)*m_pZoom - firstVisible);

      if ( !s3 && s4 && start % st4 == 0 )
        p.drawLine(RULER_SIZE-9,pos,RULER_SIZE,pos);

      if ( s3 && start % st3 == 0 )
        p.drawLine(RULER_SIZE-9,pos,RULER_SIZE,pos);

      if ( s2 && start % st2 == 0 )
        p.drawLine(RULER_SIZE-7,pos,RULER_SIZE,pos);

      if ( s1 && start % st1 == 0 )
        p.drawLine(RULER_SIZE-5,pos,RULER_SIZE,pos);

      if ( start % step == 0 ) {
        buf.setNum(abs(start));
        drawNums(&p,4,pos,buf,false);
      }

      start++;
    } while (pos < buffer->height());
  }
  p.end();
}

void KivioRuler::resizeEvent(QResizeEvent* e)
{
  recalculateSize(e);
}

void KivioRuler::show()
{
  if (orientation == Horizontal) {
    setFixedHeight(RULER_SIZE);
    initMarker(MARKER_WIDTH,MARKER_HEIGHT);
  } else {
    setFixedWidth(RULER_SIZE);
    initMarker(MARKER_HEIGHT,MARKER_WIDTH);
  }
  QWidget::show();
}

void KivioRuler::hide()
{
  if (orientation == Horizontal)
    setFixedHeight(1);
  else
    setFixedWidth(1);
}

void KivioRuler::drawNums(QPainter* p, int x, int y, QString& num,  bool orientationHoriz)
{
  if (orientationHoriz)
    x -= 7;
  else
    y -= 8;

  for ( uint k = 0; k < num.length(); k++ ) {
    int st = num.at(k).digitValue()*7;
    p->drawPixmap(x,y,*m_pNums,st,0,7,7);
    if (orientationHoriz)
      x += 7;
    else
      y += 8;
  }
}
#include "kivio_ruler.moc"
