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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <kprinter.h>
#include <qpainter.h>
#include <qglobal.h>
#include "GDocument.h"
#include "GPage.h"
#include "EPSExport.h"

EPSExport::EPSExport () {
}

EPSExport::~EPSExport () {
}

bool EPSExport::setup (GDocument *, const char* ) {
  return true;
}

bool EPSExport::exportToFile (GDocument* doc) {
  // compute bounding box
  Rect box = doc->activePage()->boundingBoxForAllObjects ();

  KPrinter printer;
  printer.setDocName (doc->fileName ());
  printer.setCreator ("KIllustrator");
  printer.setOutputFileName (outputFileName ());
  printer.setOutputToFile (true);
  printer.setFullPage (true);
  switch (doc->activePage()->pageLayout ().format) {
  case PG_DIN_A4:
    printer.setPageSize (KPrinter::A4);
    break;
  case PG_DIN_A5:
    printer.setPageSize (KPrinter::B5);
    break;
  case PG_US_LETTER:
    printer.setPageSize (KPrinter::Letter);
    break;
  case PG_US_LEGAL:
    printer.setPageSize (KPrinter::Legal);
    break;
  default:
    break;
  }
  printer.setOrientation (doc->activePage()->pageLayout().orientation == PG_PORTRAIT ?
                          KPrinter::Portrait : KPrinter::Landscape);

  QPainter paint;
  paint.begin (&printer);
#if 1
  // define the bounding box as clipping region
  paint.setClipRect (0, 0, box.width () + 2, box.height () + 2);
  // and move the objects to the origin
  paint.translate (-box.left () + 1, -box.top () + 1);
  // force update of cliping regions (only for gradient pixmaps)
#else
  paint.setClipRect (box.left (), box.top (),
                     box.width () + 2 + box.left (),
                     box.height () + 2 + box.top ());
#endif
  doc->activePage()->invalidateClipRegions ();
  doc->activePage()->drawContents (paint);
  doc->activePage()->invalidateClipRegions ();
  paint.end ();
  return true;
}
