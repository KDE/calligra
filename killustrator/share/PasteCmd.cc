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

#include <PasteCmd.h>

#include <qstring.h>
#include <qclipboard.h>
#include <qbuffer.h>
#include <qdom.h>

#include <klocale.h>
#include <kapp.h>

#include <GDocument.h>
#include <GObject.h>
#include <GText.h>

PasteCmd::PasteCmd (GDocument* doc)
  : Command(i18n("Paste"))
{
  document = doc;
}

PasteCmd::~PasteCmd () {
    for (GObject *o=objects.first(); o!=0L; o=objects.next())
        o->unref();
}

void PasteCmd::execute () {

    for (GObject *o=objects.first(); o!=0L; o=objects.next())
        o->unref ();
    objects.clear ();

    QMimeSource *src = QApplication::clipboard()->data();

    if ( src && src->provides( "application/x-killustrator-snippet" ) )
    {
        // KIllustrator objects
        QWMatrix m;
        m.translate(10, 10);

        QBuffer buffer( src->encodedData( "application/x-killustrator-snippet" ) );
        buffer.open( IO_ReadOnly );
        QDomDocument d;
        d.setContent(&buffer);
        buffer.close();
        document->insertFromXml (d, objects);
        document->unselectAllObjects ();
        for (GObject *o=objects.first(); o!=0L; o=objects.next()) {
            o->ref ();
            o->transform (m, true);
            document->selectObject(o);
        }
    }
    else {
        // plain text
        GText *tobj = new GText ();
        tobj->setText ( QApplication::clipboard()->text() );
        objects.append(tobj);
        document->insertObject (tobj);
    }
}

void PasteCmd::unexecute () {
    for (GObject *o=objects.first(); o!=0L; o=objects.next())
        document->deleteObject(o);
}

#include <PasteCmd.moc>
