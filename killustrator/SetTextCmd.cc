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

#include <SetTextCmd.h>
#include <klocale.h>

#include <GDocument.h>
#include <GObject.h>
#include <GText.h>

SetTextCmd::SetTextCmd (GDocument* doc, GText* obj, const QString &s )
  : Command(i18n("Change Text")), text(s)
{
  document = doc;
  object = obj;
  object->ref ();
  state = 0L;
}

SetTextCmd::SetTextCmd (GDocument* doc, GText* obj, GOState* ostate)
  : Command(i18n("Change Text"))
{
  document = doc;
  object = obj;
  object->ref ();
  state = ostate;
  state->ref (); // the TextTool unreferences this state object !
  text = obj->getText ();
}

SetTextCmd::~SetTextCmd () {
  object->unref ();
  state->unref ();
}

void SetTextCmd::execute () {
  if (state)
    state->unref ();
  state = object->saveState ();
  object->setText (text);
}

void SetTextCmd::unexecute () {
  object->restoreState (state);
}

