/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#include "PasteCmd.h"

#include <qstring.h>
#include <qclipboard.h>
#include <qbuffer.h>
#include <qdom.h>

#include <klocale.h>
#include <kapplication.h>

#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"

PasteCmd::PasteCmd(GDocument *aGDoc):
Command(aGDoc, i18n("Paste"))
{
}

PasteCmd::~PasteCmd()
{
  for(GObject *o = objects.first(); o != 0L; o = objects.next())
    o->unref();
}

void PasteCmd::execute()
{
  for(GObject *o = objects.first(); o != 0L; o = objects.next())
    o->unref();
  objects.clear();

  QMimeSource *src = QApplication::clipboard()->data();

  if(src && src->provides("application/x-kontour-snippet"))
  {
    /* KIllustrator objects */
    QWMatrix m;
    m.translate(10, 10);

    QBuffer buffer(src->encodedData("application/x-kontour-snippet"));
    buffer.open( IO_ReadOnly );
    QDomDocument d;
    d.setContent(&buffer);
    buffer.close();
//    document()->activePage()->insertFromXml(d, objects);
    document()->activePage()->unselectAllObjects ();
    for(GObject *o = objects.first(); o != 0L; o = objects.next())
    {
      o->ref();
      o->transform(m);
      document()->activePage()->selectObject(o);
    }
  }
/*  else
  {
        // plain text
        GText *tobj = new GText (document);
        tobj->setText ( QApplication::clipboard()->text() );
        objects.append(tobj);
        document->activePage()->insertObject (tobj);
  }*/
}

void PasteCmd::unexecute()
{
  for(GObject *o = objects.first(); o != 0L; o = objects.next())
    document()->activePage()->deleteObject(o);
}
