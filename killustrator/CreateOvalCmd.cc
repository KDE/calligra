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

#include <CreateOvalCmd.h>

#include <klocale.h>

#include "GDocument.h"
#include "GOval.h"

CreateOvalCmd::CreateOvalCmd (GDocument* doc, GOval* oval) :
  Command(i18n("Create Oval"))
{
  document = doc;
  object = oval;
  object->ref ();
}

CreateOvalCmd::CreateOvalCmd (GDocument* doc, const Coord& p0,
                              const Coord& p1, bool flag) :
  Command(i18n("Create Oval"))
{
  document = doc;
  object = 0L;
  spos = p0;
  epos = p1;
  cflag = flag;
}

CreateOvalCmd::~CreateOvalCmd () {
  if (object)
    object->unref ();
}

void CreateOvalCmd::execute () {
  if (object == 0L) {
    // create oval
    object = new GOval (document, cflag);
    object->setStartPoint (spos);
    object->setEndPoint (epos);
    //    object->ref ();
  }
  document->insertObject (object);
}

void CreateOvalCmd::unexecute () {
  document->deleteObject (object);
}

