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
#include <math.h>
#include "OvalTool.h"
#include "OvalTool.moc"
#include "GDocument.h"
#include "Canvas.h"
#include "Coord.h"
#include "CreateOvalCmd.h"
#include "CommandHistory.h"
#include "EllipseConfigDialog.h"
#include "units.h"
#include "PStateManager.h"
#include <qkeycode.h>
#include <kapp.h>
#include <klocale.h>
#include "version.h"
#include <kconfig.h>

OvalTool::OvalTool (CommandHistory *history) : Tool (history) {
  oval = NULL;
  KConfig* config = kapp->config ();
  QString oldgroup = config->group ();

  config->setGroup ("EllipseTool");
  useFixedCenter = config->readBoolEntry ("FixedCenter", false);
  config->setGroup (oldgroup);
}

void OvalTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == 
#if QT_VERSION >= 199
      QEvent::MouseButtonPress
#else
      Event_MouseButtonPress
#endif
      ) {
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    pos = Coord (xpos, ypos);
    bool flag = me->state () & ControlButton;
    oval = new GOval (flag);
    oval->setStartPoint (pos);
    oval->setEndPoint (pos);
    doc->insertObject (oval);
    emit modeSelected (flag ? i18n ("Create Circle") : 
		       i18n ("Create Ellipse"));
  }
  else if (e->type () == 
#if QT_VERSION >= 199
	   QEvent::MouseMove
#else
	   Event_MouseMove
#endif
	   ) {
    if (oval == NULL)
      return; 
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();

    canvas->snapPositionToGrid (xpos, ypos);

    if (useFixedCenter) {
      float dx = fabs (xpos - pos.x ()), 
	dy = fabs (ypos - pos.y ());
      Coord ps, pe;
      if (oval->isCircle ()) {
	float off = qRound ((dx > dy ? dx : dy) / 2.0);
	ps = Coord (pos.x () - off, pos.y () - off);
	pe = Coord (pos.x () + off, pos.y () + off);
      }
      else {
	ps = Coord (pos.x () - qRound (dx / 2.0), 
		    pos.y () - qRound (dy / 2.0));
	pe = Coord (pos.x () + qRound (dx / 2.0), 
		    pos.y () + qRound (dy / 2.0));
      }
      oval->setStartPoint (ps);
      oval->setEndPoint (pe);
    }
    else
      oval->setEndPoint (Coord (xpos, ypos));

    Rect r = oval->boundingBox ();
    MeasurementUnit unit = 
      PStateManager::instance ()->defaultMeasurementUnit ();
    const char *u = unitToString (unit);
    float xval, yval, wval, hval;
    xval = cvtPtToUnit (unit, r.x ());
    yval = cvtPtToUnit (unit, r.y ());
    wval = cvtPtToUnit (unit, r.width ());
    hval = cvtPtToUnit (unit, r.height ());
    
    sprintf (msgbuf, "%s [%.3f %s, %.3f %s, %.3f %s, %.3f %s]", 
	     oval->isCircle () ? I18N ("Create Circle") :
	     I18N ("Create Ellipse"), xval, u, yval, u, wval, u, hval, u);
    emit modeSelected (msgbuf);
  }
  else if (e->type () == 
#if QT_VERSION >= 199
	   QEvent::MouseButtonRelease
#else
	   Event_MouseButtonRelease
#endif
	   ) {
    if (oval == NULL)
      return;
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    if (useFixedCenter) {
    }
    else
      oval->setEndPoint (Coord (xpos, ypos));
    doc->unselectAllObjects ();

    if (! oval->isValid ()) 
      doc->deleteObject (oval);
    else {
      doc->setLastObject (oval);
      
      CreateOvalCmd *cmd = new CreateOvalCmd (doc, oval);
      history->addCommand (cmd);
    }
    oval = NULL;
  }
  else if (e->type () == 
#if QT_VERSION >= 199
	   QEvent::KeyPress
#else
	   Event_KeyPress
#endif
	   ) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == QT_ESCAPE)
      emit operationDone ();
  }

  return;
}

void OvalTool::activate (GDocument* , Canvas* /*canvas*/) {
  emit modeSelected (i18n ("Create Ellipse"));
}

void OvalTool::aroundFixedCenter (bool flag) {
  if (useFixedCenter != flag) {
    useFixedCenter = flag;

    KConfig* config = kapp->config ();
    QString oldgroup = config->group ();

    config->setGroup ("EllipseTool");
    config->writeEntry ("FixedCenter", useFixedCenter);
    config->setGroup (oldgroup);
  }
}

void OvalTool::configure () {
  EllipseConfigDialog::setupTool (this);
}


