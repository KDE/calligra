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

#include "DocumentInfo.h"
#include "DocumentInfo.moc"

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>

#include <qpushbt.h>
#include <qmlined.h>
#include <qlayout.h>

DocumentInfo::DocumentInfo (GDocument* doc, QWidget* parent, 
			    const char* name) : 
    QDialog (parent, name, true) {
  QPushButton* button;

  setCaption (i18n ("Document Info"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  QMultiLineEdit* textfield = new QMultiLineEdit (this);
  textfield->setReadOnly (true);
  vl->addWidget (textfield, 1);

  // a separator
  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (klocale->translate ("Done"));
  connect (button, SIGNAL (clicked ()), SLOT (accept ()));
  bbox->layout ();
  bbox->setMinimumSize (bbox->sizeHint ());

  vl->addWidget (bbox);

  vl->activate ();
  adjustSize ();
 
  setMinimumSize (430, 400);
  setMaximumSize (430, 400);

  QString s;
  doc->printInfo (s);
  textfield->setText ((const char *) s);
}

void DocumentInfo::showInfo (GDocument* doc) {
    DocumentInfo dialog (doc, 0L);
    dialog.exec ();
}

