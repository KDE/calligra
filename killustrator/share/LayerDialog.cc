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

#include <iostream.h>

#include "LayerDialog.h"
#include "LayerDialog.moc"

#include "LayerView.h"

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <kiconloader.h>

#include <qpushbt.h>
#include <qlayout.h>
#include <qheader.h>

#include "GLayer.h"

LayerDialog::LayerDialog (QWidget* parent, const char* name) : 
    QDialog (parent, name, false) {
  QPushButton* button;

  document = 0L;
  setCaption (i18n ("Layers"));

  KIconLoader* loader = kapp->getIconLoader ();
  QVBoxLayout *vl = new QVBoxLayout (this, 10);

  layerView = new LayerView (this);
  vl->addWidget (layerView, 1);

#define BUTTON_WIDTH  30 
#define BUTTON_HEIGHT 20

  KButtonBox *bbox = new KButtonBox (this);
  bbox->addStretch (0.5);
  button = bbox->addButton ("Up");
  button->setPixmap (loader->loadIcon ("raiselayer.xpm"));
  button->setGeometry (0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
  connect (button, SIGNAL (clicked ()), SLOT (upPressed ()));
  button = bbox->addButton ("Down");
  button->setPixmap (loader->loadIcon ("lowerlayer.xpm"));
  button->setGeometry (0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
  connect (button, SIGNAL (clicked ()), SLOT (downPressed ()));
  button = bbox->addButton ("New");
  button->setPixmap (loader->loadIcon ("newlayer.xpm"));
  button->setGeometry (0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
  connect (button, SIGNAL (clicked ()), SLOT (newPressed ()));
  button = bbox->addButton ("Delete");
  button->setPixmap (loader->loadIcon ("deletelayer.xpm"));
  button->setGeometry (0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
  connect (button, SIGNAL (clicked ()), SLOT (deletePressed ()));
  bbox->addStretch (0.5);

  bbox->setFixedHeight (bbox->sizeHint ().height ());
  vl->addWidget (bbox);

  // a separator
  KSeparator* sep = new KSeparator (this);
  sep->setMaximumHeight (5);
  vl->addWidget (sep);

  // the standard buttons
  bbox = new KButtonBox (this);
  bbox->addStretch (1);
  button = bbox->addButton (i18n ("Close"));
  connect (button, SIGNAL (clicked ()), SLOT (reject ()));
  button = bbox->addButton (i18n ("Help"));
  connect (button, SIGNAL (clicked ()), SLOT (helpPressed ()));
  bbox->setFixedHeight (bbox->sizeHint ().height ());
  vl->addWidget (bbox);

  vl->activate ();
  adjustSize ();

  setMaximumWidth (300);
 }

void LayerDialog::manageDocument (GDocument* doc) {
  document = doc;
  layerView->setActiveDocument (doc);
}

void LayerDialog::upPressed () {
  document->raiseLayer (document->activeLayer ());
  layerView->setActiveDocument (document);
}

void LayerDialog::downPressed () {
  document->lowerLayer (document->activeLayer ());
  layerView->setActiveDocument (document);
}

void LayerDialog::newPressed () {
    document->addLayer ();
    // force update
    layerView->setActiveDocument (document);
}

void LayerDialog::deletePressed () {
  document->deleteLayer (document->activeLayer ());
  layerView->setActiveDocument (document);
}

void LayerDialog::helpPressed () {
}
