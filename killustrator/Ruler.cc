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

#include "Ruler.h"
#include "units.h"
#include "KIllustrator_doc.h"

#include <qpainter.h>
#include <qpixmap.h>
#include <qglobal.h>

#include <kdebug.h>

#include <Painter.h>

#include <stdio.h>
#include <string.h>
#include <math.h>  //fabs()

#include <iostream.h>

#define MARKER_WIDTH 11
#define MARKER_HEIGHT 6

#define RULER_SIZE 20

Ruler::Ruler (KIllustratorDocument *_doc, Orientation o, MeasurementUnit mu, QWidget *parent,
              const char *name) : QFrame (parent, name) {
  setFrameStyle (Box | Raised);
  setLineWidth (1);
  setMidLineWidth (0);
  orientation = o;
  munit = mu;
  doc = _doc;
  zoom = 1.0;
  zeroPoint = 0;
  buffer = 0L;
  currentPosition = -1;

  if (orientation == Horizontal) {
    setFixedHeight (RULER_SIZE);
    initMarker (MARKER_WIDTH, MARKER_HEIGHT);
  }
  else {
    setFixedWidth (RULER_SIZE);
    initMarker (MARKER_HEIGHT, MARKER_WIDTH);
  }
}

Ruler::~Ruler()
{
  delete marker;
  delete bg;
  delete buffer;
}

void Ruler::initMarker (int w, int h) {
  QPainter p;
  QPointArray pts (3);
  static QCOORD hpoints [] = { 0,0, MARKER_WIDTH-1,0,
                               (MARKER_WIDTH-1)/2,MARKER_HEIGHT-1 };
  static QCOORD vpoints [] = { 0,0, MARKER_HEIGHT-1,(MARKER_WIDTH-1)/2,
                               0,MARKER_WIDTH-1 };

  if (orientation == Horizontal)
    pts.putPoints (0, 3, hpoints);
  else
    pts.putPoints (0, 3, vpoints);

  marker = new QPixmap (w, h);
  p.begin (marker);
  p.setPen (black);
  p.setBrush (black);
  p.setBackgroundColor (colorGroup ().background ());
  p.eraseRect (0, 0, w, h);
  p.drawPolygon (pts);
  p.end ();

  bg = new QPixmap (w, h);
  p.begin (bg);
  p.setBackgroundColor (colorGroup ().background ());
  p.eraseRect (0, 0, w, h);
  p.end ();
}

void Ruler::recalculateSize (QResizeEvent *) {
   kdDebug(38000)<<"recalcSize() 1"<<endl;
  delete buffer;
  buffer = 0L;

  /*
  if (! isVisible ())
    return;
  */

  int w, h;

  int maxsize = (int)(1000.0 * zoom);

  kdDebug(38000)<<"recalcSize() max: "<<maxsize<<endl;
  if (orientation == Horizontal) {
    w = QMAX(width (), maxsize);
    h = RULER_SIZE;
  }
  else {
    w = RULER_SIZE;
    h = QMAX(height (), maxsize);
  }
  buffer = new QPixmap (w, h);
   kdDebug(38000)<<"recalcSize() new buffer created2"<<endl;
  drawRuler ();
   kdDebug(38000)<<"recalcSize() after drawRuler"<<endl;
  updatePointer (currentPosition, currentPosition);
   kdDebug(38000)<<"recalcSize() after updatePointer"<<endl;
}

MeasurementUnit Ruler::measurementUnit () const {
  return  munit;
}

void Ruler::setMeasurementUnit (MeasurementUnit mu) {
  munit = mu;
  drawRuler ();
  updatePointer (currentPosition, currentPosition);
  repaint ();
}

void Ruler::setZoomFactor (float zf, int xpos, int ypos)
{
   zoom = zf;
   if (orientation == Horizontal)
      zeroPoint = xpos;
   else
      zeroPoint = ypos;
   recalculateSize (0L);
   drawRuler ();
   updatePointer (currentPosition, currentPosition);
   repaint ();
}

void Ruler::updatePointer (int x, int y)
{
   if (! buffer)
      return;

   QRect r1, r2;
   int pos = 0;

   if (orientation == Horizontal)
   {
      //kdDebug(38000)<<"Ruler::updatePointer ( "<<x<<" | "<<y<<" ) zoom is "<<zoom<<endl;
      if (currentPosition != -1)
      {
         pos=currentPosition-MARKER_WIDTH/2-2;

         r1=QRect(pos,1,MARKER_WIDTH, MARKER_HEIGHT);
         bitBlt (buffer, pos, 1, bg, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
         //kdDebug(38000)<<"Ruler::updatePointer() old pos: "<<pos<<endl;
      }
      if (x != -1)
      {
         pos=x-MARKER_WIDTH/2-2;
         r2=QRect(pos,1,MARKER_WIDTH,MARKER_HEIGHT);

         bitBlt (bg, 0, 0, buffer, pos, 1, MARKER_WIDTH, MARKER_HEIGHT);
         bitBlt (buffer, pos, 1, marker, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
         currentPosition = x;
         //kdDebug(38000)<<"Ruler::updatePointer() new pos x: "<<pos<<" currentPos: "<<x<<endl;
      }
   }
   else
   {
      if (currentPosition != -1)
      {
         pos=currentPosition-MARKER_HEIGHT/2-2;
         r1 = QRect (1, pos, MARKER_HEIGHT, MARKER_WIDTH);
         bitBlt (buffer, 1, pos, bg, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
      }
      if (y != -1)
      {
         pos=y-MARKER_HEIGHT/2-2;
         r2 = QRect (1, pos,MARKER_HEIGHT, MARKER_WIDTH);
         bitBlt (bg, 0, 0, buffer, 1, pos, MARKER_HEIGHT, MARKER_WIDTH);
         bitBlt (buffer, 1, pos, marker, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
         currentPosition = y;
      }
   }
   repaint (r1.unite (r2));
}

void Ruler::updateVisibleArea (const QRect& area)
{
   if (orientation == Horizontal)
      zeroPoint = area.x();
   else
      zeroPoint = area.y();
   drawRuler ();
   repaint ();
}

void Ruler::paintEvent (QPaintEvent *e)
{
   if (! buffer)
      return;

   const QRect& rect = e->rect ();

   if (orientation == Horizontal)
   {
      bitBlt (this, rect.x (), rect.y (), buffer,
              rect.x (), rect.y (),
              rect.width (), rect.height ());
   }
   else
   {
      bitBlt (this, rect.x (), rect.y (), buffer,
              rect.x (), rect.y (),
              rect.width (), rect.height ());
   }
   QFrame::paintEvent (e);
}

void Ruler::drawRuler ()
{
   QPainter p;
   int start(0), pos(0);
   double step(0);
   bool s1(false), s2(false), s3(false);

   if (! buffer)
      return;

   p.begin (buffer);
   p.setBackgroundColor (colorGroup ().background ());
   p.setPen (black);
   p.setFont (QFont ("helvetica", 8));
   p.eraseRect (0, 0, buffer->width (), buffer->height ());

   switch (munit)
   {
   case UnitPoint:
      s1 = cvtInchToPt(10.0) * zoom > 3.0;
      s2 = cvtInchToPt(50.0) * zoom > 3.0;
      s3 = cvtInchToPt(100.0) * zoom > 3.0;
      step = 30.0 / (100.0 * zoom);
      start = (int)(zeroPoint / zoom);
      break;
   case UnitInch:
      s1 = cvtInchToPt(0.1) * zoom > 3.0;
      s2 = cvtInchToPt(0.5) * zoom > 3.0;
      s3 = cvtInchToPt(1.0) * zoom > 3.0;
      step = 24.0 / (cvtInchToPt(1.0) * zoom);
      start = 10*(int)(cvtPtToInch(zeroPoint) / zoom);
      break;
   case UnitCentimeter:
   case UnitMillimeter:
      s1 = cvtMmToPt(1.0) * zoom > 3.0;
      s2 = cvtMmToPt(5.0) * zoom > 3.0;
      s3 = cvtMmToPt(10.0) * zoom > 3.0;
      step = 30.0 / (cvtMmToPt(10.0) * zoom);
      //kdDebug(38000)<<" 30 / "<<cvtMmToPt(10.0)<<" * "<<zoom<<" = "<<step<<endl;
      start = (int)(cvtPtToMm(zeroPoint) / zoom);
      break;
   case UnitPica:
      s1 = cvtPicaToPt(1.0) * zoom > 3.0;
      s2 = cvtPicaToPt(5.0) * zoom > 3.0;
      s3 = cvtPicaToPt(10.0) * zoom > 3.0;
      step = 30.0 / (cvtPicaToPt(10.0) * zoom);
      start = (int)(cvtPtToPica(zeroPoint) / zoom);
      break;
   case UnitDidot:
      s1 = cvtDidotToPt(10.0) * zoom > 3.0;
      s2 = cvtDidotToPt(50.0) * zoom > 3.0;
      s3 = cvtDidotToPt(100.0) * zoom > 3.0;
      step = 30.0 / (cvtDidotToPt(100.0) * zoom);
      start = (int)(cvtPtToDidot(zeroPoint) / zoom);
      break;
   case UnitCicero:
      s1 = cvtCiceroToPt(1.0) * zoom > 3.0;
      s2 = cvtCiceroToPt(5.0) * zoom > 3.0;
      s3 = cvtCiceroToPt(10.0) * zoom > 3.0;
      step = 30.0 / (cvtCiceroToPt(10.0) * zoom);
      start = (int)(cvtPtToCicero(zeroPoint) / zoom);
      break;
   }
   if(fabs(step)<0.9)
      step = 1;

   //workaround, will fix it, aleXXX
   step=qRound(step);

   if (orientation == Horizontal)
   {
      switch (munit)
      {
      case UnitPoint:
         {
            do
            {
               pos = (int)(start * zoom - zeroPoint);
               if( s3 && start % 100 == 0 )
                  p.drawLine (pos, RULER_SIZE-10, pos, RULER_SIZE);
               if( s2 && start % 50 == 0 )
                  p.drawLine (pos, RULER_SIZE-7, pos, RULER_SIZE);
               if( s1 && start % 10 == 0 )
                  p.drawLine (pos, RULER_SIZE-5, pos, RULER_SIZE);
               if( start % qRound(step * 100) == 0 )
                  drawNum (p, pos, 8, start, true);
               start++;
            } while(pos < buffer->width());
            break;
         }
      case UnitMillimeter:
         {
            do
            {

               pos = (int)(cvtMmToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (pos, RULER_SIZE-10, pos, RULER_SIZE);
               if( s2 && start % 5 == 0 )
                  p.drawLine (pos, RULER_SIZE-7, pos, RULER_SIZE);
               if( s1 )
                  p.drawLine (pos, RULER_SIZE-5, pos, RULER_SIZE);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, pos, 8, start, true);
               start++;
            } while(pos < buffer->width());
            break;
         }
      case UnitCentimeter:
         {
            do
            {
               pos = (int)(cvtMmToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (pos, RULER_SIZE-10, pos, RULER_SIZE);
               if( s2 && start % 5 == 0 )
                  p.drawLine (pos, RULER_SIZE-7, pos, RULER_SIZE);
               if( s1 )
                  p.drawLine (pos, RULER_SIZE-5, pos, RULER_SIZE);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, pos, 8, start/10, true);
               start++;
            } while(pos < buffer->width());
            break;
      }
      case UnitDidot:
         {
            do
            {
               pos = (int)(cvtDidotToPt(start) * zoom - zeroPoint);
               if( s3 && start % 100 == 0 )
                  p.drawLine (pos, RULER_SIZE-10, pos, RULER_SIZE);
               if( s2 && start % 50 == 0 )
                  p.drawLine (pos, RULER_SIZE-7, pos, RULER_SIZE);
               if( s1 && start % 10 == 0 )
                  p.drawLine (pos, RULER_SIZE-5, pos, RULER_SIZE);
               if( start % qRound(step * 100) == 0 )
                  drawNum (p, pos, 8, start, true);
               start++;
            } while(pos < buffer->width());
            break;
         }
      case UnitCicero:
         {
            do
            {
               pos = (int)(cvtCiceroToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (pos, RULER_SIZE-10, pos, RULER_SIZE);
               if( s2 && start % 5 == 0 )
                  p.drawLine (pos, RULER_SIZE-7, pos, RULER_SIZE);
               if( s1 )
                  p.drawLine (pos, RULER_SIZE-5, pos, RULER_SIZE);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, pos, 8, start, true);
               start++;
            } while(pos < buffer->width());
            break;
         }
      case UnitPica:
         {
            do
            {
               pos = (int)(cvtPicaToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (pos, RULER_SIZE-10, pos, RULER_SIZE);
               if( s2 && start % 5 == 0 )
                  p.drawLine (pos, RULER_SIZE-7, pos, RULER_SIZE);
               if( s1 )
                  p.drawLine (pos, RULER_SIZE-5, pos, RULER_SIZE);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, pos, 8, start, true);
               start++;
            } while(pos < buffer->width());
            break;
         }
      case UnitInch:
         {
            do
            {
               pos = (int)(cvtInchToPt(start/10.0) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (pos, RULER_SIZE-10, pos, RULER_SIZE);
               if( s2 && start % 5 == 0 )
                  p.drawLine (pos, RULER_SIZE-7, pos, RULER_SIZE);
               if( s1 )
                  p.drawLine (pos, RULER_SIZE-5, pos, RULER_SIZE);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, pos, 8, start/10, true);
               start++;
            } while(pos < buffer->width());
            break;
         }
      default:
         break;
      }
   }
   else
   {
      switch (munit)
      {
      case UnitPoint:
         {
            do
            {
               pos = (int)(start * zoom  - zeroPoint);
               if( s3 && start % 100 == 0 )
                  p.drawLine (RULER_SIZE-10, pos, RULER_SIZE, pos);
               if( s2 && start % 50 == 0 )
                  p.drawLine (RULER_SIZE-7, pos, RULER_SIZE, pos);
               if( s1 && start % 10 == 0 )
                  p.drawLine (RULER_SIZE-5, pos, RULER_SIZE, pos);
               if( start % qRound(step * 100) == 0 )
                  drawNum (p, 2, pos, start, false);
               start++;
            } while(pos < buffer->height());
            break;
         }
      case UnitDidot:
         {
            do
            {
               pos = (int)(cvtDidotToPt(start) * zoom - zeroPoint);
               if( s3 && start % 100 == 0 )
                  p.drawLine (RULER_SIZE-10, pos, RULER_SIZE, pos);
               if( s2 && start % 50 == 0 )
                  p.drawLine (RULER_SIZE-7, pos, RULER_SIZE, pos);
               if( s1 && start % 10 == 0 )
                  p.drawLine (RULER_SIZE-5, pos, RULER_SIZE, pos);
               if( start % qRound(step * 100) == 0 )
                  drawNum (p, 2, pos, start, false);
               start++;
            } while(pos < buffer->height());
            break;
         }
      case UnitMillimeter:
         {
            do
            {
               pos = (int)(cvtMmToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (RULER_SIZE-10, pos, RULER_SIZE, pos);
               if( s2 && start % 5 == 0 )
               {
                  p.drawLine (RULER_SIZE-7, pos, RULER_SIZE, pos);
               }
               if( s1 )
                  p.drawLine (RULER_SIZE-5, pos, RULER_SIZE, pos);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, 2, pos, start, false);
               start++;
            } while(pos < buffer->height());
            break;
         }
      case UnitCentimeter:
         {
            do
            {
               pos = (int)(cvtMmToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (RULER_SIZE-10, pos, RULER_SIZE, pos);
               if( s2 && start % 5 == 0 )
                  p.drawLine (RULER_SIZE-7, pos, RULER_SIZE, pos);
               if( s1 )
                  p.drawLine (RULER_SIZE-5, pos, RULER_SIZE, pos);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, 2, pos, start/10, false);
               start++;
            } while(pos < buffer->height());
            break;
         }
      case UnitCicero:
         {
            do
            {
               pos = (int)(cvtCiceroToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (RULER_SIZE-10, pos, RULER_SIZE, pos);
               if( s2 && start % 5 == 0 )
                  p.drawLine (RULER_SIZE-7, pos, RULER_SIZE, pos);
               if( s1 )
                  p.drawLine (RULER_SIZE-5, pos, RULER_SIZE, pos);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, 2, pos, start, false);
               start++;
            } while(pos < buffer->height());
            break;
         }
      case UnitPica:
         {
            do
            {
               pos = (int)(cvtPicaToPt(start) * zoom - zeroPoint);
               if( s3 && start % 10 == 0 )
                  p.drawLine (RULER_SIZE-10, pos, RULER_SIZE, pos);
               if( s2 && start % 5 == 0 )
                  p.drawLine (RULER_SIZE-7, pos, RULER_SIZE, pos);
               if( s1 )
                  p.drawLine (RULER_SIZE-5, pos, RULER_SIZE, pos);
               if( start % qRound(step * 10) == 0 )
                  drawNum (p, 2, pos, start, false);
               start++;
            } while(pos < buffer->height());
            break;
         }
      case UnitInch:
         do
         {
            pos = (int)(cvtInchToPt(start/10.0) * zoom - zeroPoint);
            if( s3 && start % 10 == 0 )
               p.drawLine (RULER_SIZE-10, pos, RULER_SIZE, pos);
            if( s2 && start % 5 == 0 )
               p.drawLine (RULER_SIZE-7, pos, RULER_SIZE, pos);
            if( s1 )
               p.drawLine (RULER_SIZE-5, pos, RULER_SIZE, pos);
            if( start % qRound(step * 10) == 0 )
               drawNum (p, 2, pos, start/10, false);
            start++;
         } while(pos < buffer->height ());
         break;
      }
   }
   p.end ();
   //killu draws its rulers 6 (!!!!) times on startup..., aleXXX
/*   static int blah(0);
   kdDebug(38000)<<"Ruler::drawing ruler "<<blah<<endl;
   blah++;*/
}

void Ruler::resizeEvent (QResizeEvent *e) {
  recalculateSize (e);
}

void Ruler::show () {
  if (orientation == Horizontal) {
    setFixedHeight (RULER_SIZE);
    initMarker (MARKER_WIDTH, MARKER_HEIGHT);
  }
  else {
    setFixedWidth (RULER_SIZE);
    initMarker (MARKER_HEIGHT, MARKER_WIDTH);
  }
  QWidget::show ();
}

void Ruler::hide () {
  if (orientation == Horizontal)
    setFixedHeight (1);
  else
    setFixedWidth (1);
  QWidget::hide ();
}

void Ruler::mousePressEvent ( QMouseEvent * e)
{
   if (e==0) return;

   if (e->button()==Qt::LeftButton)
      isMousePressed = true;
   else if (e->button()==Qt::RightButton)
      emit rmbPressed();
}

void Ruler::mouseMoveEvent ( QMouseEvent * me)
{
   /* Implement the hooks so that a helpline can be drawn out of the ruler:
      - if the mouse is on the widget, draw a helpline
      - if it is outside remove the XOR'd helpline
      - if the mouse it over the page view, set the helpline
       (different place: update the helpline position in the status bar)*/
  if (isMousePressed && doc->isReadWrite())
    emit drawHelpline (me->x () +
         (orientation == Horizontal ? zeroPoint : 0) -
         RULER_SIZE,
         me->y () +
         (orientation == Vertical ? zeroPoint : 0) -
         RULER_SIZE,
         (orientation==Horizontal) ? true : false );
}

void Ruler::mouseReleaseEvent ( QMouseEvent * me)
{
  if (isMousePressed&& doc->isReadWrite())
  {
     isMousePressed = false;
     emit addHelpline (me->x () +
         (orientation == Horizontal ? zeroPoint : 0) -
         RULER_SIZE,
         me->y () +
         (orientation == Vertical ? zeroPoint : 0) -
         RULER_SIZE,
         (orientation==Horizontal) ? true : false );
  }
}

void Ruler::drawNum (QPainter &p, int x, int y, int a, bool orient)
 {
  char buf[10];
  sprintf(buf, "%d", (a>=0)? a : -a );
  int l = strlen(buf);
  if( orient )
   x -= 3*l;
  else
   y -= 2*l - 3;
  for(char *ch = buf; *ch; ch++)
   {
    p.drawText ( x, y, ch, 1);
    if( orient )
      x += 6;
    else
      y += 7;
   }
 }

#include <Ruler.moc>
