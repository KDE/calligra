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

#include "ToolGroup.h"
#include <qpushbt.h>

#include "ToolGroup.moc"

ToolGroup::ToolGroup () {
  current_id = 0;
}

void ToolGroup::insertButton (int id, ToolButton* button) {
  buttons.resize (id + 1);
  buttons.insert (id, button);

  connect (button, SIGNAL(clicked ()), this, SLOT(buttonPressed ()));
  connect (button, SIGNAL(rightButtonPressed ()), this,
	   SLOT(rightMouseButtonPressed ()));

  if (id == 0)
    buttons[0]->setOn (true);
}

void ToolGroup::rightMouseButtonPressed () {
  ToolButton* button = (ToolButton *) sender ();

  for (unsigned int i = 0; i < buttons.size (); i++) {
    if (buttons[i] == button) {
      emit toolConfigActivated (i);
    }
  }
}

void ToolGroup::selectTool (int id) {
  for (unsigned int i = 0; i < buttons.size (); i++) {
    if (i == (unsigned int) id) {
      buttons[i]->setOn (true);
      emit toolSelected (i);
    }
    else {
      ToolButton *bt = buttons[i];
      if (bt->isOn ())
	bt->setOn (false);
    }
  }
  current_id = id;
}

void ToolGroup::buttonPressed () {
  ToolButton* button = (ToolButton *) sender ();
  unsigned int i;

  for (i = 0; i < buttons.size (); i++) {
    if (buttons[i] == button) {
      if (i == current_id)
	button->setOn (true);
      else
	emit toolSelected (i);
    }
    else {
      ToolButton *bt = buttons[i];
      if (bt->isOn ())
	bt->setOn (false);
    }
  }
  current_id = i;
}
