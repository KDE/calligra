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

#include "ScriptConsole.h"
#include "ScriptConsole.moc"

#include "scripting/ScriptingEngine.h"

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>

#include <qpushbt.h>
#include <qmlined.h>
#include <qlayout.h>

ScriptConsole::ScriptConsole (QWidget* parent, const char* name) : 
    QDialog (parent, name, false) {
  QPushButton* button;

  setCaption (klocale->translate ("Scripting Console"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  editor = new ScriptEditor (this);
  vl->addWidget (editor, 1);

  // a separator
  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (klocale->translate ("Evaluate"));
  connect (button, SIGNAL (clicked ()), SLOT (evaluatePressed ()));
  button = bbox->addButton (klocale->translate ("Clear"));
  connect (button, SIGNAL (clicked ()), SLOT (clearPressed ()));
  button = bbox->addButton (klocale->translate ("Close"));
  connect (button, SIGNAL (clicked ()), SLOT (closePressed ()));
  bbox->addStretch (1);
  button = bbox->addButton (klocale->translate ("Help"));
  connect (button, SIGNAL (clicked ()), SLOT (helpPressed ()));
  bbox->layout ();
  bbox->setMinimumSize (bbox->sizeHint ());

  vl->addWidget (bbox);

  vl->activate ();
  adjustSize ();
 
  setMinimumSize (430, 400);
  setMaximumSize (430, 400);
}

void ScriptConsole::setActiveDocument (GDocument* doc) {
  ScriptingEngine* interp = ScriptingEngine::engine ();
  interp->setActiveDocument (doc);
}

void ScriptConsole::evaluatePressed () {
  QString code = editor->selection ();
  if (! code.isEmpty ()) {
    printf ("evaluate: \"%s\"\n", code.data ());
    ScriptingEngine* interp = ScriptingEngine::engine ();
    interp->evaluate (code.data ());
    printf ("result = %s\n", interp->result ());
    editor->append (interp->result ());
  }
}

void ScriptConsole::closePressed () {
  hide ();
}

void ScriptConsole::clearPressed () {
  editor->clear ();
}

void ScriptConsole::helpPressed () {
}
