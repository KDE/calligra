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

#include <iostream.h>
#include <values.h>
#include "BezierTool.h"
#include "BezierTool.moc"
#include "GDocument.h"
#include "Canvas.h"
#include "Coord.h"
#include "GBezier.h"
#include "CreateBezierCmd.h"
#include "AddLineSegmentCmd.h"
#include "CommandHistory.h"
#include <qkeycode.h>
#include <kapp.h>
#include <klocale.h>

BezierTool::BezierTool (CommandHistory* history) : Tool (history) {
  curve = 0L;
  last = 0;
  newObj = true;
  oldNumOfPoints = 0;
}

void BezierTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == Event_MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    if (me->button () != LeftButton)
      return;

    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    if (curve == 0L) {
      newObj = true;
      addAtEnd = true;

      QList<GObject> olist;
      // look for existing bezier curves with an
      // end point near the mouse pointer
      if (doc->findContainingObjects (xpos, ypos, olist)) {
	QListIterator<GObject> it (olist);
	while (it.current ()) {
	  if (it.current ()->isA ("GBezier")) {
	    GBezier* obj = (GBezier *) it.current ();
	    if (((last = obj->getNeighbourPoint (Coord (xpos, ypos))) != -1)
		&& obj->isEndPoint (last) 
		&& (last == 1 || last == (int) obj->numOfPoints () - 2)) {
	      curve = obj;
	      addAtEnd = (last != 1);
	      newObj = false;
	      oldNumOfPoints = obj->numOfPoints ();
	      break;
	    }
	  }
	  ++it;
	}
      }
      
      if (curve == 0L) {
	curve = new GBezier ();
	// base point #1
	curve->addPoint (0, Coord (MAXFLOAT, MAXFLOAT));
	// first end point
	curve->addPoint (1, Coord (xpos, ypos));
	last = 2;
	doc->insertObject (curve);
	// base point #2
	curve->addPoint (last, Coord (xpos, ypos));
	curve->setWorkingSegment (0);
      }
      else {
	if (last == 1) {
	  last = 0;
	  curve->setWorkingSegment (0);
	}
	else {
	  // set current point to base point #1
	  last = curve->numOfPoints () - 1;
	  curve->setWorkingSegment (last / 3 - 1);
	}
      }
    }
    else {
      if (! addAtEnd) {
	if (last == 0) {
	  // add at beginning of curve
	  curve->addPoint (0, Coord (MAXFLOAT, MAXFLOAT), false);
	  curve->addPoint (0, Coord (xpos, ypos), false);
	  curve->addPoint (0, Coord (MAXFLOAT, MAXFLOAT));
	  last = 1;
	}
	else if (last == 1) 
	  last = 0;
	curve->setWorkingSegment (0);
      }
      else {
	// add at end of curve
	last = curve->numOfPoints ();
	if (! curve->isEndPoint (last - 1)) 
	  curve->initBasePoint (last - 3);
	
	if (last >= 3 && (last % 3 == 0))
	  // base point #2
	  curve->addPoint (last++, Coord (MAXFLOAT, MAXFLOAT), false);
	
	// next end point
	curve->addPoint (last, Coord (xpos, ypos));
	curve->setWorkingSegment (last / 3 - 1);
      }
    }
  }
  else if (e->type () == Event_MouseMove) {
    if (curve == 0L)
      return;

    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    curve->setPoint (last, Coord (xpos, ypos));
  }
  else if (e->type () == Event_MouseButtonRelease) {
    if (curve == 0L)
      return;
    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);
    
    curve->setPoint (last, Coord (xpos, ypos));
    if (me->button () == RightButton) {
      if ((addAtEnd && last >= 5 && (last % 3 == 2)) ||
	 (!addAtEnd && last == 0 && (curve->numOfPoints () % 3 == 0))) {
	doc->setLastObject (curve);
	curve->setWorkingSegment (-1);
	if (newObj) {
	  CreateBezierCmd *cmd = new CreateBezierCmd (doc, curve);
	  history->addCommand (cmd);
	}
	else {
	  QList<Coord> points;
	  points.setAutoDelete (true);

	  if (last == 0) {
	    for (int i = curve->numOfPoints () - oldNumOfPoints - 1; 
		 i >= 0; i--)
	      points.append (new Coord (curve->getPoint (i)));
	  }
	  else {
	    for (int i = oldNumOfPoints; i < (int) curve->numOfPoints (); i++)
	      points.append (new Coord (curve->getPoint (i)));
	    last = last - points.count () + 1;
	  }
	  AddLineSegmentCmd *cmd =  
	    new AddLineSegmentCmd (doc, curve, last, points);
	  history->addCommand (cmd);
	}
	curve = 0L; last = 0;
      }
    }
  }
  return;
}

void BezierTool::activate (GDocument* doc, Canvas*) {
  emit modeSelected (i18n ("Create Bezier Curve"));
}

void BezierTool::deactivate (GDocument*, Canvas*) {
  if (curve)
    curve->setWorkingSegment (-1);
  curve = 0L;
  last = 0;
}
