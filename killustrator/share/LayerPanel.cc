/* -*- C++ -*-


  This file is part of KIllustrator.
  Copyright (C) 2000 Igor Janssen (rm@linux.ru.net)

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

#include "LayerPanel.h"

#include <qpushbutton.h>
#include <qlayout.h>

#include <klocale.h>
#include <kiconloader.h>

#include <LayerView.h>
#include <GDocument.h>
#include <GLayer.h>

LayerPanel::LayerPanel (QWidget* parent, const char* name) :
    QWidget(parent, name) {

    document = 0L;
    mGrid = new QGridLayout(this, 2, 4);
    layerView = new LayerView (this);
    mGrid->addMultiCellWidget( layerView, 1, 1, 0, 3 );
    
    btn_rl = new QPushButton(this);
    btn_rl->setPixmap(UserIcon("raiselayer"));
    connect (btn_rl, SIGNAL (clicked ()), SLOT (upPressed ()));
    mGrid->addWidget( btn_rl, 0, 0 );
    btn_ll = new QPushButton(this);
    btn_ll->setPixmap (UserIcon ("lowerlayer"));
    connect (btn_ll, SIGNAL (clicked ()), SLOT (downPressed ()));
    mGrid->addWidget( btn_ll, 0, 1 );
    btn_nl = new QPushButton(this);
    btn_nl->setPixmap (UserIcon ("newlayer"));
    connect (btn_nl, SIGNAL (clicked ()), SLOT (newPressed ()));
    mGrid->addWidget( btn_nl, 0, 2 );
    btn_dl = new QPushButton(this);
    btn_dl->setPixmap (UserIcon ("deletelayer"));
    connect (btn_dl, SIGNAL (clicked ()), SLOT (deletePressed ()));
    mGrid->addWidget( btn_dl, 0, 3 );
}

void LayerPanel::manageDocument (GDocument* doc) {
  document = doc;
  layerView->setActiveDocument (doc);
}

void LayerPanel::upPressed () {
  document->raiseLayer (document->activeLayer ());
  layerView->setActiveDocument (document);
}

void LayerPanel::downPressed () {
  document->lowerLayer (document->activeLayer ());
  layerView->setActiveDocument (document);
}

void LayerPanel::newPressed () {
  GLayer* layer = document->addLayer ();
  document->setActiveLayer (layer);
  // force update
  layerView->setActiveDocument (document);
}

void LayerPanel::deletePressed () {
  document->deleteLayer (document->activeLayer ());
  layerView->setActiveDocument (document);
}

#include <LayerPanel.moc>
