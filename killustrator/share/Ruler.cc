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
      pos = qRound (currentPosition * zoom - MARKER_WIDTH / 2);
      r1 = QRect (pos - firstVisible, 1, MARKER_WIDTH, MARKER_HEIGHT);
      bitBlt (buffer, pos, 1, bg, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
    }
    if (x != -1) {
      pos = qRound (x * zoom - MARKER_WIDTH / 2);
      r2 = QRect (pos - firstVisible, 1, MARKER_WIDTH, MARKER_HEIGHT);
      bitBlt (buffer, pos, 1, marker, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
      currentPosition = x;
    }
  }
  else {
    if (currentPosition != -1) {
      pos = qRound (currentPosition * zoom - MARKER_HEIGHT / 2);
      r1 = QRect (1, pos - firstVisible, MARKER_HEIGHT, MARKER_WIDTH);
      bitBlt (buffer, 1, pos, bg, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
    }
    if (y != -1) {
      pos = qRound (y * zoom - MARKER_HEIGHT / 2);
      r2 = QRect (1, pos - firstVisible, MARKER_HEIGHT, MARKER_WIDTH);
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
  repaint ();
}

void Ruler::paintEvent (QPaintEvent *e) {
  if (! buffer)
    return;

  const QRect& rect = e->rect ();

  if (orientation == Horizontal)
    bitBlt (this, rect.x (), rect.y (), buffer, 
	    rect.x () + firstVisible, rect.y (), 
	    rect.width (), rect.height ());
  else
    bitBlt (this, rect.x (), rect.y (), buffer, 
	    rect.x (), rect.y () + firstVisible, 
	    rect.width (), rect.height ());
  QFrame::paintEvent (e);
}

void Ruler::drawRuler () {
  Painter p;
  char buf[10];
  int step = 0, step1 = 0, step2 = 0;

  if (! buffer)
    return;

  switch (munit) {
  case UnitPoint:
    step = (int) (10.0 * zoom);
    step1 = (int) (100.0 * zoom);
    step2 = (int) (50.0 * zoom);
    break;
  case UnitInch:
    step = (int) (1.0 * zoom);
    step1 = (int) (10.0 * zoom);
    step2 = (int) (5.0 * zoom);
    break;
  case UnitMillimeter:
    step = (int) (2.0 * zoom);
    step1 = (int) (20.0 * zoom);
    step2 = (int) (10.0 * zoom);
    break;
  }

  p.begin (buffer);
  p.setBackgroundColor (colorGroup ().background ());
  p.setPen (black);
  p.setFont (QFont ("times", 10));
  buffer->fill (backgroundColor ());
  p.eraseRect (0, 0, width (), height ());

#define MM_FACTOR 72.0 / 25.4
#define INCH_FACTOR 7.2
  
  if (orientation == Horizontal) {
    switch (munit) {
    case UnitPoint:
      {
	for (int i = 0; i < buffer->width (); i += step) {
	  if (i % step1 == 0) {
	    p.drawLine (i, 10, i, 30);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (i + 3, 18, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (i, 15, i, 30);
	  else
	    p.drawLine (i, 20, i, 30);
	}
	break;
      }
    case UnitMillimeter:
      {
	for (int i = 0; i < buffer->width (); i += step) {
	  int pos = qRound (i * MM_FACTOR) + 1;
	  if (i % step1 == 0) {
	    p.drawLine (pos, 10, pos, 30);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (pos + 3, 18, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (pos, 15, pos, 30);
	  else
	    p.drawLine (pos, 20, pos, 30);
	}
	break;
      }
    case UnitInch:
      {
	for (int i = 0; i < buffer->width (); i += step) {
	  int pos = qRound (i * INCH_FACTOR);
	  if (i % step1 == 0) {
	    p.drawLine (pos, 10, pos, 30);
	    sprintf (buf, "%d", (int) ((float) i / (zoom * 10)));
	    p.drawText (pos + 3, 18, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (pos, 15, pos, 30);
	  else
	    p.drawLine (pos, 20, pos, 30);
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
	for (int i = 0; i < buffer->height (); i += step) {
	  if (i % step1 == 0) {
	    p.drawLine (10, i, 30, i);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (10, i + 9, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (15, i, 30, i);
	  else
	    p.drawLine (20, i, 30, i);
	}
	break;
      }
    case UnitMillimeter:
      {
	for (int i = 0; i < buffer->height (); i += step) {
	  int pos = qRound (i * MM_FACTOR) + 1;
	  if (i % step1 == 0) {
	    p.drawLine (10, pos, 30, pos);
	    sprintf (buf, "%d", (int) (((float) i) / zoom));
	    p.drawText (10, pos + 9, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (15, pos, 30, pos);
	  else
	    p.drawLine (20, pos, 30, pos);
	}
	break;
      }
    case UnitInch:
      {
	for (int i = 0; i < buffer->height (); i += step) {
	  int pos = qRound (i * INCH_FACTOR);
	  if (i % step1 == 0) {
	    p.drawLine (10, pos, 30, pos);
	    sprintf (buf, "%d", (int) (((float) i) / (zoom * 10.0)));
	    p.drawText (10, pos + 9, buf);
	  }
	  else if (i % step2 == 0)
	    p.drawLine (15, pos, 30, pos);
	  else
	    p.drawLine (20, pos, 30, pos);
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
