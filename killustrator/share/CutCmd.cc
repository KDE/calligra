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

#include <CutCmd.h>

#include <qclipboard.h>
#include <qdom.h>
#include <kapp.h>
#include <klocale.h>

#include <GDocument.h>
#include <GObject.h>

CutCmd::CutCmd (GDocument* doc)
  : Command(i18n("Cut"))
{
  document = doc;
  objects.setAutoDelete(true);
  for(QListIterator<GObject> it(doc->getSelection()); it.current(); ++it) {
    MyPair *p=new MyPair;
    p->o = *it;
    p->o->ref ();
    // store the old position of the object
    p->pos = doc->findIndexOfObject(p->o);
    objects.append(p);
  }
}

CutCmd::~CutCmd () {
    for (MyPair *p=objects.first(); p!=0L;
         p=objects.next())
        p->o->unref ();
}

void CutCmd::execute () {

    QDomDocument docu("doc");
    QDomElement doc=docu.createElement("doc");
    doc.setAttribute ("mime", KILLUSTRATOR_MIMETYPE);
    docu.appendChild(doc);

    for (MyPair *p=objects.first(); p!=0L;
         p=objects.next()) {
        doc.appendChild(p->o->writeToXml(docu));
        document->deleteObject (p->o);
    }
    QApplication::clipboard()->setText(docu.toCString());
}

void CutCmd::unexecute () {
  QApplication::clipboard ()->clear ();
  document->unselectAllObjects ();

  for (MyPair *p=objects.first(); p!=0; p=objects.next()) {
    // insert the object at the old position
    p->o->ref ();
    document->insertObjectAtIndex (p->o, p->pos);
    document->selectObject (p->o);
  }
}

#include <CutCmd.moc>
