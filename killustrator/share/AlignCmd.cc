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

#include <AlignCmd.h>
#include <GDocument.h>

#include <klocale.h>

AlignCmd::AlignCmd (GDocument* doc, HorizAlignment halign,
                    VertAlignment valign, bool centerToPage,
                    bool snapToGrid) : ObjectManipCmd (doc, i18n("Align")) {
  horizAlign = halign;
  vertAlign = valign;
  center = centerToPage;
  snap = snapToGrid;
}

void AlignCmd::execute () {
  GObject* alignObject; // the object for aligning the others
  Rect alignBox;
  float dx, dy;
  unsigned int i;

  unsigned int nobjs = document->selectionCount ();

  if (nobjs == 0)
    return;

  ObjectManipCmd::execute ();
  document->setAutoUpdate (false);
  if (nobjs > 1) {
    // alignment is possible only for two or more objects
    QListIterator<GObject> it(document->getSelection());
    alignObject = document->getSelection().last();
    alignBox=alignObject->boundingBox ();

    for (i = 0; it.current(); ++it, ++i) {
      GObject* obj = *it;
      if (obj == alignObject)
        continue;

      Rect objBox = obj->boundingBox ();

      switch (horizAlign) {
      case HAlign_Left:
        dx = alignBox.left () - objBox.left ();
        break;
      case HAlign_Center:
        {
          Coord aCoord = alignBox.center ();
          Coord oCoord = objBox.center ();
          dx = aCoord.x () - oCoord.x ();
          break;
        }
      case HAlign_Right:
        dx = alignBox.right () - objBox.right ();
        break;
      default:
        dx = 0;
        break;
      }

      switch (vertAlign) {
      case VAlign_Top:
        dy = alignBox.top () - objBox.top ();
        break;
      case VAlign_Center:
        {
          Coord aCoord = alignBox.center ();
          Coord oCoord = objBox.center ();
          dy = aCoord.y () - oCoord.y ();
          break;
        }
      case VAlign_Bottom:
        dy = alignBox.bottom () - objBox.bottom ();
        break;
      default:
        dy = 0;
        break;
      }

      QWMatrix matrix;
      matrix.translate (dx, dy);
      obj->transform (matrix, ! center);
    }
  }

  if (center) {
    // center the selection to the page
    Rect page (0, 0, document->getPaperWidth (), document->getPaperHeight ());
    Coord pcenter = page.center ();
    Coord bcenter = document->boundingBoxForSelection ().center ();
    QWMatrix matrix;
    matrix.translate (pcenter.x () - bcenter.x (),
                      pcenter.y () - bcenter.y ());
    for (QListIterator<GObject> it(document->getSelection()); it.current(); ++it)
        (*it)->transform (matrix, true);
  }
  document->setAutoUpdate (true);
}
