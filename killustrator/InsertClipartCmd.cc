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

#include <InsertClipartCmd.h>

#include <qwmf.h>
#include <klocale.h>

#include "GClipart.h"
#include "GDocument.h"
#include "GPage.h"

InsertClipartCmd::InsertClipartCmd (GDocument* doc, const QString &fname) :
 Command(i18n("Insert Clipart"))
{
  document = doc;
  filename = fname;
  clipart = 0L;
}

InsertClipartCmd::~InsertClipartCmd () {
  if (clipart)
    clipart->unref ();
}

void InsertClipartCmd::execute () {
  QWinMetaFile wmf;
  if (clipart)
    clipart->unref ();

  if (wmf.load(filename)) {
    clipart = new GClipart (document, filename);
    document->activePage()->insertObject (clipart);
  }
}

void InsertClipartCmd::unexecute () {
  if (clipart)
    document->activePage()->deleteObject (clipart);
}

