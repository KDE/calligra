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

#include <CopyCmd.h>

#include <qclipboard.h>
#include <qbuffer.h>
#include <qtextstream.h>
#include <qdragobject.h>
#include <qdom.h>
#include <kapp.h>
#include <klocale.h>

#include <GDocument.h>
#include <GObject.h>
#include "GPage.h"

CopyCmd::CopyCmd (GDocument* doc)
  : Command(i18n("Copy"))
{
  document = doc;
  for(QListIterator<GObject> it(doc->activePage()->getSelection()); it.current(); ++it) {
    GObject* o = *it;
    o->ref ();
    objects.append(o);
  }
}

CopyCmd::~CopyCmd () {
  for(GObject *o=objects.first();
       o!=0L; o=objects.next())
      o->unref ();
}

void CopyCmd::execute () {

    QDomDocument docu("killustrator");
    docu.appendChild( docu.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement doc=docu.createElement("killustrator");
    doc.setAttribute ("mime", KILLUSTRATOR_MIMETYPE);
    docu.appendChild(doc);
    QDomElement layer=docu.createElement("layer");
    doc.appendChild(layer);

    for (GObject *o=objects.first(); o!=0L;
         o=objects.next())
        layer.appendChild(o->writeToXml (docu));

    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    QTextStream stream( &buffer );
    stream.setEncoding( QTextStream::UnicodeUTF8 );
    stream << docu;
    buffer.close();

    QStoredDrag *drag = new QStoredDrag( "application/x-killustrator-snippet" );
    drag->setEncodedData( buffer.buffer() );

    QApplication::clipboard ()->setData( drag );
}

void CopyCmd::unexecute () {
  QApplication::clipboard ()->clear ();
}

