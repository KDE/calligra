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

#include <qdir.h>
#include <qfileinfo.h>

#include <iostream.h>

#include "ScriptDialog.h"
#include "ScriptDialog.moc"

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>

#include <qpushbt.h>
#include <qlayout.h>

ScriptDialog::ScriptDialog (QWidget* parent, const char* name) : 
    QDialog (parent, name, false) {
  QPushButton* button;

  setCaption (i18n ("Scripts"));

  QVBoxLayout *vl = new QVBoxLayout (this, 10);

  listBox = new QListBox (this);
  listBox->setFixedVisibleLines (10);
  vl->addWidget (listBox, 1);

  // a separator
  KSeparator* sep = new KSeparator (this);
  sep->setMaximumHeight (5);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (i18n ("Execute"));
  connect (button, SIGNAL (clicked ()), SLOT (executePressed ()));
  bbox->addStretch (1);
  button = bbox->addButton (i18n ("Help"));
  connect (button, SIGNAL (clicked ()), SLOT (helpPressed ()));
  bbox->setMaximumHeight (bbox->sizeHint ().height ());
  vl->addWidget (bbox);

  vl->activate ();
  adjustSize ();
 
  setMaximumWidth (300);
//  setMinimumSize (430, 400);
//  setMaximumSize (430, 400);
}

void ScriptDialog::setActiveDocument (GDocument* doc) {
}

void ScriptDialog::loadScripts () {
  listBox->clear ();
  lastSystemScript = -1;

  // load scripts from system directory
  QString systemScripts = kapp->kde_datadir () + "/killustrator/scripts";
  QDir systemDir (systemScripts.data (), "*.js", QDir::Name,
		  QDir::Files);
  if (systemDir.exists ()) {
    const QFileInfoList *systemList = systemDir.entryInfoList ();
    QFileInfoListIterator it (*systemList);
    QFileInfo *fileInfo;
    while ((fileInfo = it.current ())) {
      listBox->insertItem (fileInfo->fileName ().data ());
      ++it;
    }
    lastSystemScript = systemDir.count () - 1;
  }

  // load scripts from user's directory
  QString userScripts = QDir::homeDirPath () + "/.killustrator/scripts";
  QDir userDir (userScripts.data (), "*.js", QDir::Name,
		  QDir::Files);
  if (userDir.exists ()) {
    const QFileInfoList *userList = userDir.entryInfoList ();
    QFileInfoListIterator it (*userList);
    QFileInfo *fileInfo;
    while ((fileInfo = it.current ())) {
      listBox->insertItem (fileInfo->fileName ().data ());
      ++it;
    }
  }
}

void ScriptDialog::executePressed () {
  int item = listBox->currentItem ();
  if (item == -1)
    return;

  const char* script = listBox->text (item);
  if (item > lastSystemScript) {
    cout << "execute user script: " << script << endl;
  }
  else {
    cout << "execute system script: " << script << endl;
  }
}

void ScriptDialog::helpPressed () {
}
