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
#include "Ruler.moc"

#include <kapp.h>
#include <qpainter.h>
#include <qglobal.h>
#include "Painter.h"

#include <stdio.h>
#include <iostream.h>

#define MARKER_WIDTH 11
#define MARKER_HEIGHT 6

Ruler::Ruler (Orientation o, MeasurementUnit mu, QWidget *parent,
	      const char *name) : QFrame (parent, name) {
  setFrameStyle (Box | Raised);
  setLineWidth (1);
  setMidLineWidth (0);
  orientation = o;
  munit = mu;
  zoom = 1;
  firstVisible = 0;
  buffer = 0L;
  currentPosition = -1;

  if (orientation == Horizontal) {
    setFixedHeight (30);
    initMarker (MARKER_WIDTH, MARKER_HEIGHT);
  }
  else {
    setFixedWidth (30);
    initMarker (MARKER_HEIGHT, MARKER_WIDTH);
  }
}

void Ruler::initMarker (int w, int h) {
  Painter p;
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

void Ruler::recalculateSize (QResizeEvent *e) {
  if (buffer != 0L) {
    delete buffer;
    buffer = 0L;
  }

  if (! isVisible ())
    return;

  int w, h;
  
  int maxsize = (int)(1000.0 * zoom);

  if (orientation == Horizontal) {
    w = QMAX(width (), maxsize);
    h = 30;
  }
  else {
    w = 30;
    h = QMAX(height (), maxsize);
  }
  buffer = new QPixmap (w, h);
  drawRuler ();
  updatePointer (currentPosition, currentPosition);
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

void Ruler::setZoomFactor (float zf) {
  zoom = zf;
  recalculateSize (0L);
  drawRuler ();
  updatePointer (currentPosition, currentPosition);
  repaint ();
}

void Ruler::updatePointer (int x, int y) {
  if (! buffer)
    return;

  QRect r1, r2;
  int pos = 0;

  if (orientation == Horizontal) {
    if (currentPosition != -1) {
      pos = qRound (currentPosition * zoom  
		    - (firstVisible >= 0 ? 0 : firstVisible) 
		    - MARKER_WIDTH / 2);
      r1 = QRect (pos - (firstVisible >= 0 ? firstVisible : 0), 
		  1, MARKER_WIDTH, MARKER_HEIGHT);
      bitBlt (buffer, pos, 1, bg, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
    }
    if (x != -1) {
      pos = qRound (x * zoom - (firstVisible >= 0 ? 0 : firstVisible) 
		    - MARKER_WIDTH / 2);
      r2 = QRect (pos - (firstVisible >= 0 ? firstVisible : 0), 
		  1, MARKER_WIDTH, MARKER_HEIGHT);
      bitBlt (buffer, pos, 1, marker, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
      currentPosition = x;
    }
  }
  else {
    if (currentPosition != -1) {
      pos = qRound (currentPosition * zoom
		    - (firstVisible >= 0 ? 0 : firstVisible) 
		    - MARKER_HEIGHT / 2);
      r1 = QRect (1, pos - (firstVisible >= 0 ? firstVisible : 0), 
		  MARKER_HEIGHT, MARKER_WIDTH);
      bitBlt (buffer, 1, pos, bg, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
    }
    if (y != -1) {
      pos = qRound (y * zoom - (firstVisible >= 0 ? 0 : firstVisible) 
		    - MARKER_HEIGHT / 2);
      r2 = QRect (1, pos -  (firstVisible >= 0 ? firstVisible : 0), 
		  MARKER_HEIGHT, MARKER_WIDTH);
      bitBlt (buffer, 1, pos, marker, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
      currentPosition = y;
    }
  }
  repaint (r1.unite (r2));
}

void Ruler::updateVisibleArea (int xpos, int ypos) {
  if (orientation == Horizontal)
    firstVisible = -xpos;
  else
    firstVisible = -ypos;
  drawRuler ();
  repaint ();
}

void Ruler::paintEvent (QPaintEvent *e) {
  if (! buffer)
    return;

  const QRect& rect = e->rect ();

  if (orientation == Horizontal) {
    if (firstVisible >= 0)
      bitBlt (this, rect.x (), rect.y (), buffer, 
	      rect.x () + firstVisible, rect.y (), 
	      rect.width (), rect.height ());
    else
      bitBlt (this, rect.x (), rect.y (), buffer, 
	      rect.x (), rect.y (), 
	      rect.width (), rect.height ());
  }
  else {
    if (firstVisible >= 0)
      bitBlt (this, rect.x (), rect.y (), buffer, 
	      rect.x (), rect.y () + firstVisible, 
	      rect.width (), rect.height ());
    else
      bitBlt (this, rect.x (), rect.y (), buffer, 
	      rect.x (), rect.y (), 
	      rect.width (), rect.height ());
  }
  QFrame::paintEvent (e);
}

void Ruler::drawRuler () {
  Painter p;
  char buf[10];
  int step = 0, step1 = 0, step2 = 0, start = 0, ioff = 0;

  if (! buffer)
    return;

  switch (munit) {
  case UnitPoint:
    if (firstVisible < 0) {
      start = -((firstVisible / 10) * 10);
      ioff = -firstVisible % 10;
    }
    step = (int) (10.0 * zoom);
    step1 = (int) (100.0 * zoom);
    step2 = (int) (50.0 * zoom);
    break;
  case UnitInch:
    if (firstVisible < 0) {
      start = -((firstVisible / 10) * 10);
      ioff = -firstVisible % 10;
    }
    step = (int) (1.0 * zoom);
    if (step == 0) step = 1;
    step1 = (int) (10.0 * zoom);
    step2 = (int) (5.0 * zoom);
    break;
  case UnitMillimeter:
    if (firstVisible < 0) {
      start = -((firstVisible / 2) * 2);
      ioff = -firstVisible % 2;
    }
    step = (int) (2.0 * zoom);
    step1 = (int) (20.0 * zoom);
    step2 = (int) (10.0 * zoom);
    break;
  }

  p.begin (buffer);
  p.setBackgroundColor (colorGroup ().background ());
  p.setPen (black);
  p.setFont (QFont ("helvetica", 8));
  buffer->fill (backgroundColor ());
  p.eraseRect (0, 0, width (), height ());

#define MM_FACTOR 72.0 / 25.4
#define INCH_FACTOR 7.2

  /*
  start = -((firstVisible / 10) * 10);
  ioff = -firstVisible % 10;
  cout << "ioff with = " << ioff 
       << ", start = " << start 
       << "(" << firstVisible << ")" << endl;
  */
  if (orientation == Horizontal) {
    switch (munit) {
    case UnitPoint:
      {
	for (int i = -start; i < buffer->width (); i += step) {
	  int poff = i + ioff + start;
	  if (i % step1 == 0) {
	    p.drawLine (poff, 10, poff, 30);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (poff + 3, 18, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (poff, 15, poff, 30);
	  else
	    p.drawLine (poff, 20, poff, 30);
	}
	break;
      }
    case UnitMillimeter:
      {
	for (int i = -start; i < buffer->width (); i += step) {
	  int pos = qRound (i * MM_FACTOR) + 1;
	  int poff = pos + ioff + start;
	  if (i % step1 == 0) {
	    p.drawLine (poff, 10, poff, 30);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (poff + 3, 18, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (poff, 15, poff, 30);
	  else
	    p.drawLine (poff, 20, poff, 30);
	}
	break;
      }
    case UnitInch:
      {
	for (int i = -start; i < buffer->width (); i += step) {
	  int pos = qRound (i * INCH_FACTOR);
	  int poff = pos + ioff + start;
	  if (i % step1 == 0) {
	    p.drawLine (poff, 10, poff, 30);
	    sprintf (buf, "%d", (int) ((float) i / (zoom * 10)));
	    p.drawText (poff + 3, 18, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (poff, 15, poff, 30);
	  else
	    p.drawLine (poff, 20, poff, 30);
	}
	break;
      }
    default:
      break;
    }
  }
  else {
    switch (munit) {
    case UnitPoint:
      {
	for (int i = -start; i < buffer->height (); i += step) {
	  int poff = i + ioff + start;
	  if (i % step1 == 0) {
	    p.drawLine (10, poff, 30, poff);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (10, poff + 9, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (15, poff, 30, poff);
	  else
	    p.drawLine (20, poff, 30, poff);
	}
	break;
      }
    case UnitMillimeter:
      {
	for (int i = -start; i < buffer->height (); i += step) {
	  int pos = qRound (i * MM_FACTOR) + 1;
	  int poff = pos + ioff + start;
	  if (i % step1 == 0) {
	    p.drawLine (10, poff, 30, poff);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (10, poff + 9, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (15, poff, 30, poff);
	  else
	    p.drawLine (20, poff, 30, poff);
	}
	break;
      }
    case UnitInch:
      {
	for (int i = -start; i < buffer->height (); i += step) {
	  int pos = qRound (i * INCH_FACTOR);
	  int poff = pos + ioff + start;
	  if (i % step1 == 0) {
	    p.drawLine (10, poff, 30, poff);
	    sprintf (buf, "%d", (int) (((float) i) / (zoom * 10.0)));
	    p.drawText (10, poff + 9, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (15, poff, 30, poff);
	  else
	    p.drawLine (20, poff, 30, poff);
	}
	break;
      }
    default:
      break;
    }
  }
  p.end ();
}

void Ruler::resizeEvent (QResizeEvent *e) {
  recalculateSize (e);
}

void Ruler::show () {
  if (orientation == Horizontal) {
    setFixedHeight (30);
    initMarker (MARKER_WIDTH, MARKER_HEIGHT);
  }
  else {
    setFixedWidth (30);
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
