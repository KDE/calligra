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

#include "ZoomTool.h"
#include "Coord.h"
#include "Canvas.h"

#include <klocale.h>
#include <kdebug.h>
#include <qpainter.h>
#include <qpen.h>
#include <CommandHistory.h>

ZoomTool::ZoomTool (CommandHistory* history) : Tool (history)
 {
  zoomFactors.append(0.5);
  zoomFactors.append(1.0);
  zoomFactors.append(1.5);
  zoomFactors.append(2.0);
  zoomFactors.append(4.0);
  zoomFactors.append(6.0);
  zoomFactors.append(8.0);
  zoomFactors.append(10.0);
 }

void ZoomTool::processEvent (QEvent* e, GDocument *_doc, Canvas* _canvas)
 {
//  doc = _doc;
//  canvas = _canvas;
  if (e->type () == QEvent::MouseButtonRelease)
   {
    processButtonReleaseEvent((QMouseEvent *) e);
    emit operationDone ();
   }
  else
   if (e->type () == QEvent::MouseButtonPress)
    {
     processButtonPressEvent((QMouseEvent *) e);
     emit operationDone ();
    }
   else
    if (e->type () == QEvent::MouseMove)
     {
      processMouseMoveEvent((QMouseEvent *) e);
      emit operationDone ();
     }
 }

void ZoomTool::activate (GDocument *_doc, Canvas *_canvas)
 {
  state = S_Init;
  doc = _doc;
  canvas = _canvas;
  emit modeSelected (i18n ("Zoom In"));
 }

void ZoomTool::processButtonPressEvent (QMouseEvent* e)
 {
  /************
   * S_Init
   */
  if (state == S_Init)
   {
    state = S_Rubberband;
    selPoint[0].x(e->x());
    selPoint[0].y(e->y());
    selPoint[1].x(e->x());
    selPoint[1].y(e->y());
   }
 }
 
void ZoomTool::processMouseMoveEvent (QMouseEvent* e)
 {
  if (state == S_Rubberband)
   {
    kdDebug(1) << "S_Rubberband\n";
    selPoint[1].x(e->x());
    selPoint[1].y(e->y());
    canvas->repaint();
    QPainter painter;
    painter.save();
    QPen pen(blue, 1, DotLine);
    painter.begin(canvas);
    painter.translate(canvas->xOffset(), canvas->yOffset());
    painter.setPen(pen);
    float sfactor = canvas->scaleFactor();
    painter.scale(sfactor, sfactor);
    Rect selRect(selPoint[0], selPoint[1]);
    painter.drawRect((int) selRect.x (), (int) selRect.y (),
                      (int) selRect.width (), (int) selRect.height ());
    painter.restore();
    painter.end();
    return;
   }
 }
 
void ZoomTool::processButtonReleaseEvent (QMouseEvent* e)
 {
  if (state == S_Rubberband)
   {
    zoomRegion(selPoint[0].x(), selPoint[0].y(), selPoint[1].x(), selPoint[1].y());
    canvas->repaint ();
    state = S_Init;
    return;
   }
  if(state == S_Init)
   {
    if (e->button () == LeftButton)
     zoomIn (e->x (), e->y ());
    else
     if (e->button () == RightButton)
      zoomOut (canvas);
    return;
   }
 }

void ZoomTool::zoomIn (int x, int y) {
  for (QValueList<float>::Iterator i=zoomFactors.begin(); i!=zoomFactors.end(); ++i) {
    if (*i == canvas->getZoomFactor()) {
        if(*i!=zoomFactors.last()) {
             ++i;
            canvas->setZoomFactor(*i);
//            scrollview->center(x, y);
            break;
        }
    }
  }
}

void ZoomTool::zoomIn(Canvas* cnv)
 {
  float z = cnv->getZoomFactor();
  z *= 1.25;
  if(z > 10.0)
   z = 10.0;
  cnv->setZoomFactor(z);
 }

void ZoomTool::zoomOut (Canvas* cnv)
 {
  float z = cnv->getZoomFactor();
  z *= 0.8;
  if(z < 0.1)
   z = 0.1;
  cnv->setZoomFactor(z);
 }

void ZoomTool::zoomRegion(int x1, int y1, int x2, int y2)
 {
/*  if (x1 == x2 || y1 == y2)
   {
    zoomIn(canvas);
    return;
   }
  
  if(x1 > x2)
   {
    int s = x1;
    x1 = x2;
    x2 = s;
   }
  
  if(y1 > y2)
   {
    int s = y1;
    y1 = y2;
    y2 = s;
   }

  int x = x2 - x1;
  int y = y2 - y1;

  int cw = canvas->viewport()->width();
  int ch = canvas->viewport()->height();
  float cz = canvas->getZoomFactor();
  float zw = (float)cw/(float)x;
  float zh = (float)ch/(float)y;
  float z = QMIN(zw,zh)*cz;
  
  if(z > 10.0)
   z = 10.0;
  canvas->setZoomFactor(z);
  x = canvas->x() + x1 + x/2;
  y = canvas->y() + y1 + y/2;
  x = x * QMIN(zw,zh);
  y = y * QMIN(zw,zh);
  canvas->scrollView()->center(x,y);*/
 }

int ZoomTool::insertZoomFactor (float z) {
  QValueList<float>::Iterator i;
  int pos = 0;
  for (i = zoomFactors.begin (); i != zoomFactors.end (); ++i, pos++) {
    if (*i == z) {
      canvas->setZoomFactor (z);
      return pos;
    }
    else if (*i > z) {
      // insert at position i
      zoomFactors.insert (i, z);
      canvas->setZoomFactor (z);
      return pos;
    }
  }
  zoomFactors.append(z);
  return zoomFactors.count();
}

#include <ZoomTool.moc>
