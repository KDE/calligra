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

#include <InsertPixmapCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include <GPixmap.h>

InsertPixmapCmd::InsertPixmapCmd (GDocument* doc, const QString &fname) :
 Command(i18n("Insert Pixmap"))
{
  document = doc;
  filename = fname;
  pixmap = 0L;
}

InsertPixmapCmd::~InsertPixmapCmd () {
  if (pixmap)
    pixmap->unref ();
}

void InsertPixmapCmd::execute () {
  if (pixmap)
    pixmap->unref ();

  pixmap = new GPixmap (document, filename);
  document->insertObject (pixmap);
}

void InsertPixmapCmd::unexecute () {
  if (pixmap)
    document->deleteObject (pixmap);
}

