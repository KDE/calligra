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

#include <DistributeCmd.h>
#include <klocale.h>
#include <GDocument.h>

DistributeCmd::DistributeCmd (GDocument* doc, HorizDistribution hdistrib,
                              VertDistribution vdistrib,
                              DistributionMode dmode) :
   ObjectManipCmd (doc, i18n("Distribute"))
{
  hDistrib = hdistrib;
  vDistrib = vdistrib;
  mode = dmode;
}

void DistributeCmd::execute () {
  Rect box;
  float xoff = 0, yoff = 0, xpos = 0, ypos = 0;
  GObject *firstObj, *lastObj;

  if (mode == DMode_AtPage)
    box = Rect (0, 0, document->getPaperWidth (),
                document->getPaperHeight ());
  else
    box = document->boundingBoxForSelection ();

  ObjectManipCmd::execute ();

  QList<GObject>& objs = document->getSelection ();

  firstObj = objs.first();
  lastObj = objs.last();
  QListIterator<GObject> it(objs);

  switch (hDistrib) {
  case HDistrib_Left:
    xoff = (box.width () - lastObj->boundingBox ().width ()) /
      (document->selectionCount () - 1);
    xpos = box.left ();
    break;
  case HDistrib_Center:
    xoff = (box.width () - firstObj->boundingBox ().width () / 2 -
            lastObj->boundingBox ().width () / 2) /
      (document->selectionCount () - 1);
    xpos = box.left () + firstObj->boundingBox ().width () / 2;
    break;
  case HDistrib_Distance:
    {
      float w = 0;
      for (; it.current(); ++it)
        w += (*it)->boundingBox ().width ();
      xoff = (box.width () - w) / (document->selectionCount () - 1);
      xpos = box.left ();
      break;
    }
  case HDistrib_Right:
    xoff = (box.width () - firstObj->boundingBox ().width ()) /
      (document->selectionCount () - 1);
    xpos = box.left () + firstObj->boundingBox ().width ();
    break;
  default:
    break;
  }

  switch (vDistrib) {
  case VDistrib_Top:
    yoff = (box.height () - lastObj->boundingBox ().height ()) /
      (document->selectionCount () - 1);
    ypos = box.top ();
    break;
  case VDistrib_Bottom:
    yoff = (box.height () - firstObj->boundingBox ().height ()) /
      (document->selectionCount () - 1);
    ypos = box.top () + firstObj->boundingBox ().height ();;
    break;
  case VDistrib_Center:
    yoff = (box.height () - firstObj->boundingBox ().height () / 2 -
            lastObj->boundingBox ().height () / 2) /
      (document->selectionCount () - 1);
    ypos = box.top () + firstObj->boundingBox ().height () / 2;
  case VDistrib_Distance:
    {
      float h = 0;
      for (; it.current(); ++it)
        h += (*it)->boundingBox ().height ();
      yoff = (box.height () - h) / (document->selectionCount () - 1);
      ypos = box.top ();
      break;
    }
  default:
    break;
  }

  for (; it.current(); ++it) {
    GObject *obj = *it;
    Rect obox = obj->boundingBox ();
    float dx = 0, dy = 0;

    switch (hDistrib) {
    case HDistrib_Left:
      dx = xpos - obox.left ();
      break;
    case HDistrib_Center:
      dx = xpos - (obox.left () + obox.width () / 2);
      break;
    case HDistrib_Right:
      dx = xpos - obox.right ();
      break;
    case HDistrib_Distance:
      dx = xpos - obox.left ();
    default:
      break;
    }

    switch (vDistrib) {
    case VDistrib_Top:
      dy = ypos - obox.top ();
      break;
    case VDistrib_Bottom:
      dy = ypos - obox.bottom ();
      break;
    case VDistrib_Center:
      dy = ypos - (obox.top () + obox.height () / 2);
      break;
    case VDistrib_Distance:
      dy = ypos - obox.top ();
      break;
    default:
      break;
    }

    QWMatrix matrix;
    matrix.translate (dx, dy);
    obj->transform (matrix, true);

    switch (hDistrib) {
    case HDistrib_Left:
    case HDistrib_Center:
    case HDistrib_Right:
      xpos += xoff;
      break;
    case HDistrib_Distance:
      xpos += obj->boundingBox ().width () + xoff;
      break;
    default:
      break;
    }
    switch (vDistrib) {
    case VDistrib_Top:
    case VDistrib_Bottom:
    case VDistrib_Center:
      ypos += yoff;
      break;
    case VDistrib_Distance:
      ypos += obj->boundingBox ().height () + yoff;
      break;
    default:
      break;
    }
  }
}

