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
#include "ToolController.h"

#include <klocale.h>
#include <kdebug.h>
#include <qpainter.h>
#include <qpen.h>
#include <CommandHistory.h>

#define KILLU_ZOOM_MAX 100
#define KILLU_ZOOM_MIN 0.05

ZoomTool::ZoomTool (CommandHistory* history)
:Tool (history)
{
  m_id=ToolZoom;
}

void ZoomTool::processEvent (QEvent* e, GDocument *, Canvas*)
{
   if (e->type () == QEvent::MouseButtonRelease)
   {
      processButtonReleaseEvent((QMouseEvent *) e);
      m_toolController->emitOperationDone (m_id);
   }
   else if (e->type () == QEvent::MouseMove)
   {
      processMouseMoveEvent((QMouseEvent *) e);
      m_toolController->emitOperationDone (m_id);
   }
}

void ZoomTool::activate (GDocument *_doc, Canvas *_canvas)
{
   state = S_Init;
   doc = _doc;
   canvas = _canvas;
   canvas->setCursor(Qt::arrowCursor);
   m_toolController->emitModeSelected (m_id,i18n ("Zoom in and out"));
}

void ZoomTool::processMouseMoveEvent (QMouseEvent* e)
{
   //kdDebug()<<"processMouseMoveEvent()"<<endl;
   if ((state == S_Init) && ((e->state () == Qt::LeftButton) ||(e->state()==Qt::MidButton)))
   {
      //kdDebug()<<"processMouseMoveEvent() entering rubberband mode"<<endl;
      selPoint[0].x(e->x());
      selPoint[0].y(e->y());
      selPoint[1].x(e->x());
      selPoint[1].y(e->y());
      state=S_Rubberband;
   }
   else if (state == S_Rubberband)
   {
      //kdDebug(1) << "S_Rubberband\n";
      selPoint[1].x(e->x());
      selPoint[1].y(e->y());
      canvas->repaint();
      QPainter painter;
      painter.save();
      QPen pen(Qt::blue, 1, Qt::DotLine);
      painter.begin(canvas);
      painter.translate(canvas->relativePaperArea().left(), canvas->relativePaperArea().top());
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
      kdDebug()<<"processButtonReleaseEvent() rubberband" <<endl;
      if (e->state()==Qt::MidButton)
         zoomOutRegion(selPoint[0].x(), selPoint[0].y(), selPoint[1].x(), selPoint[1].y());
      else
         zoomInRegion(selPoint[0].x(), selPoint[0].y(), selPoint[1].x(), selPoint[1].y());

      canvas->repaint ();
      state = S_Init;
   }
   else if(state == S_Init)
   {
      if (((e->button()== Qt::LeftButton) && (e->state()==Qt::NoButton))
          ||((e->button()==Qt::MidButton) && (e->state()==Qt::ShiftButton)))
         zoomIn (canvas);
      else if (((e->button()== Qt::MidButton) && (e->state()==Qt::NoButton))
               || ((e->button()==Qt::LeftButton) && (e->state()==Qt::ShiftButton)))
         zoomOut (canvas);
   }
}

void ZoomTool::zoomIn(Canvas* cnv)
{
   float z = cnv->getZoomFactor();
   z *= 1.25;
   if(z > KILLU_ZOOM_MAX)
      z = KILLU_ZOOM_MAX;
   cnv->setZoomFactor(z);
}

void ZoomTool::zoomOut (Canvas* cnv)
{
   float z = cnv->getZoomFactor();
   z *= 0.8;
   if(z < KILLU_ZOOM_MIN)
      z = KILLU_ZOOM_MIN;
   cnv->setZoomFactor(z);
}

void ZoomTool::zoomInRegion(int x1, int y1, int x2, int y2)
{

   int tmp;

   x1*=canvas->getZoomFactor();
   x2*=canvas->getZoomFactor();
   y1*=canvas->getZoomFactor();
   y2*=canvas->getZoomFactor();

   if (x2<x1)
   {
      tmp=x2;
      x2=x1;
      x1=tmp;
   };
   if (y2<y1)
   {
      tmp=y2;
      y2=y1;
      y1=tmp;
   };

   int dx=x2-x1;
   int dy=y2-y1;

   int cw = canvas->width();
   int ch = canvas->height();

   float zoomX(100000);
   if (dx!=0)
      zoomX=float(cw)/dx;

   float zoomY(100000);
   if (dy!=0)
      zoomY=float(ch)/dy;

   float zoom=zoomX;
   if (zoomY<zoom)
      zoom=zoomY;
   zoom*=canvas->getZoomFactor();

   if(zoom > KILLU_ZOOM_MAX)
      zoom = KILLU_ZOOM_MAX;
   else if(zoom < KILLU_ZOOM_MIN)
      zoom = KILLU_ZOOM_MIN;

   kdDebug()<<"ZoomTool::zoomInRegion() area ( "<<x1<<" | "<<y1<<" ) to ( "<<x2<<" | "<< y2<<" )"<<endl;

   canvas->setZoomFactor(zoom,(x2+x1)/2,(y2+y1)/2);
}

void ZoomTool::zoomOutRegion(int x1, int y1, int x2, int y2)
{
   int tmp;

   x1*=canvas->getZoomFactor();
   x2*=canvas->getZoomFactor();
   y1*=canvas->getZoomFactor();
   y2*=canvas->getZoomFactor();
   if (x2<x1)
   {
      tmp=x2;
      x2=x1;
      x1=tmp;
   };
   if (y2<y1)
   {
      tmp=y2;
      y2=y1;
      y1=tmp;
   };

   int dx=x2-x1;
   int dy=y2-y1;

   int cw = canvas->width();
   int ch = canvas->height();

   float zoomX=float(dx)/cw;

   float zoomY=float(dy)/ch;

   float zoom=zoomX;
   if (zoomY>zoom)
      zoom=zoomY;

   zoom*=canvas->getZoomFactor();

   if(zoom > KILLU_ZOOM_MAX)
      zoom = KILLU_ZOOM_MAX;
   else if(zoom < KILLU_ZOOM_MIN)
      zoom = KILLU_ZOOM_MIN;

   canvas->setZoomFactor(zoom,(x2+x1)/2,(y2+y1)/2);
   //canvas->setZoomFactor(zoom);
}

