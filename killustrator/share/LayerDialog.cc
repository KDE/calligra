/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <LayerDialog.h>

#include <qpushbutton.h>
#include <qvbox.h>
#include <qhbox.h>

#include <klocale.h>
#include <kiconloader.h>

#include <LayerView.h>
#include <GDocument.h>
#include <GLayer.h>

LayerDialog::LayerDialog (QWidget* parent, const char* name) :
    KDialogBase(parent, name, false, i18n("Layers"), KDialogBase::Close,
                KDialogBase::Close, true) {

    document = 0L;

    QVBox *mb=makeVBoxMainWidget();
    layerView = new LayerView (mb);

    QHBox *box=new QHBox(mb);
    QPushButton *button = new QPushButton(box);
    button->setPixmap(UserIcon("raiselayer"));
    connect (button, SIGNAL (clicked ()), SLOT (upPressed ()));
    button = new QPushButton(box);
    button->setPixmap (UserIcon ("lowerlayer"));
    connect (button, SIGNAL (clicked ()), SLOT (downPressed ()));
    button = new QPushButton(box);
    button->setPixmap (UserIcon ("newlayer"));
    connect (button, SIGNAL (clicked ()), SLOT (newPressed ()));
    button = new QPushButton(box);
    button->setPixmap (UserIcon ("deletelayer"));
    connect (button, SIGNAL (clicked ()), SLOT (deletePressed ()));
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
  GLayer* layer = document->addLayer ();
  document->setActiveLayer (layer);
  // force update
  layerView->setActiveDocument (document);
}

void LayerDialog::deletePressed () {
  document->deleteLayer (document->activeLayer ());
  layerView->setActiveDocument (document);
}

#include <LayerDialog.moc>
