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

#include <stdio.h>

#include <kapp.h>
#include <klocale.h>
#include "GLayer.h"
#include "GLayer.moc"

int GLayer::lastID = 1;

GLayer::GLayer (const char* text) : visibleFlag (true), printableFlag (true), 
  editableFlag (true), activeFlag (false) {
  if (text == NULL) {
    char buf[20];

    sprintf (buf, "%s #%d", i18n("Layer"), lastID++);
    ident = buf;
  }
}

GLayer::~GLayer () {
}

const char* GLayer::name () const {
  return ident.data ();
}

void GLayer::setName (const char* text) {
  ident = text;
}

void GLayer::setVisible (bool flag) {
  if (visibleFlag != flag) {
    visibleFlag = flag;
    emit propertyChanged ();
  }
}

void GLayer::setPrintable (bool flag) {
  if (printableFlag != flag) {
    printableFlag = flag;
    emit propertyChanged ();
  }
}

void GLayer::setEditable (bool flag) {
  if (editableFlag != flag) {
    editableFlag = flag;
    emit propertyChanged ();
  }
}

void GLayer::setActive (bool flag) {
  if (activeFlag != flag) {
    activeFlag = flag;
    emit propertyChanged ();
  }
}
