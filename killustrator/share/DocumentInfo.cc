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
#include <qlabel.h>
#include <qmlined.h>
#include <qlayout.h>

#include "GDocument.h"

DocumentInfo::DocumentInfo (GDocument* doc, QWidget* parent,
			    const char* name) :
  QDialog (parent, name, true) {
  QPushButton* button;
  gdoc = doc;

  setCaption (i18n ("Document Info"));

  QVBoxLayout *vl = new QVBoxLayout (this, 4);

  QMultiLineEdit* textfield = new QMultiLineEdit (this);
  textfield->setReadOnly (true);
  vl->addWidget (textfield, 1);
  
  QLabel *lbKeyWd   = new QLabel(i18n("Keywords:"),this);
  vl->addWidget(lbKeyWd);
  edKeyWd= new QLineEdit(this);
  vl->addWidget(edKeyWd);
  
  QLabel *lbComment = new QLabel(i18n("Comment:"),this);
  vl->addWidget(lbComment);
  edComnt= new QLineEdit(this);
  // copy comments from GDocument
  edComnt->setText("Comment text dummy");
  vl->addWidget(edComnt);
    
  // a separator
  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (i18n ("Ok"));
  connect (button, SIGNAL (clicked ()), this, SLOT (acceptClicked() ));
  button = bbox->addButton (i18n ("Cancel"));
  connect (button, SIGNAL (clicked ()), SLOT (reject ()));
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
  
  doc->getKeywords(s);
  s.replace( QRegExp("&quot;"),"\"" );
  s.replace( QRegExp("&amp;"), "&" );
  edKeyWd->setText((const char *)s);
  
  doc->getComment(s);
  s.replace( QRegExp("&quot;"),"\"" );
  s.replace( QRegExp("&amp;"), "&" );
  edComnt->setText((const char *)s);
}

void DocumentInfo::showInfo (GDocument* doc) {
    DocumentInfo dialog (doc, 0L);
    dialog.exec ();
}


void DocumentInfo::acceptClicked() {
  QString s;
  s = edComnt->text();
  s.replace( QRegExp("&"), "&amp;" );
  s.replace( QRegExp("\""),"&quot;" );
  gdoc->setComment ( s );
  s = edKeyWd->text();
  s.replace( QRegExp("&"), "&amp;" );  
  s.replace( QRegExp("\""),"&quot;" );
  gdoc->setKeywords( s );
  accept();
}
