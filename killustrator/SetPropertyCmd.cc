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

#include <SetPropertyCmd.h>
#include <klocale.h>

SetPropertyCmd::SetPropertyCmd (GDocument* doc,
                                const GObject::OutlineInfo& oinfo,
                                const GObject::FillInfo& finfo) :
  ObjectManipCmd (doc, i18n("Set property"))
{
  outline = oinfo;
  fill = finfo;
  tprops.mask = 0;
}

SetPropertyCmd::SetPropertyCmd (GObject* obj,
                                const GObject::OutlineInfo& oinfo,
                                const GObject::FillInfo& finfo) :
  ObjectManipCmd (obj, i18n("Set property"))
{
  outline = oinfo;
  fill = finfo;
  tprops.mask = 0;
}

SetPropertyCmd::SetPropertyCmd (GDocument* doc,
                                const GObject::OutlineInfo& oinfo,
                                const GObject::FillInfo& finfo,
                                const GText::TextInfo& tinfo) :
  ObjectManipCmd (doc, i18n("Set property"))
{
  outline = oinfo;
  fill = finfo;
  tprops = tinfo;
}

void SetPropertyCmd::execute ()
{
  // save the states
  ObjectManipCmd::execute ();

  for (unsigned int i = 0; i < objects.count (); i++)
  {
    objects[i]->setOutlineInfo (outline);
    objects[i]->setFillInfo (fill);
    if (tprops.mask && objects[i]->isA ("GText"))
    {
      GText* tobj = (GText *) objects[i];
      tobj->setTextInfo (tprops);
    }
  }
}

