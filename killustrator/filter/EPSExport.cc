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
#include <qprinter.h>
#include "Painter.h"
#include "EPSExport.h"

EPSExport::EPSExport () {
}

EPSExport::~EPSExport () {
}

bool EPSExport::setup (GDocument *doc, const char* fmt) {
  return true;
}

bool EPSExport::exportToFile (GDocument* doc) {
  const char* tmpname = tempnam (NULL, "killu");
  /*
   * Step 1: print the document to PostScript
   */

  // setup the printer
  QPrinter printer;
  printer.setOutputToFile (true);
  printer.setDocName ((const char *) doc->fileName ());
  printer.setCreator ("KIllustrator");
  printer.setOutputFileName (tmpname);

  // draw the objects
  Painter paint;
  paint.begin (&printer);
  QListIterator<GObject> it = doc->getObjects ();
  for (; it.current (); ++it) 
    it.current ()->draw (paint);
  paint.end ();

  /*
   * Step 2: convert the PostScript file to EPS using Ghostscript
   */

  QString cmd;
  cmd.sprintf ("%s %s %s", PS2EPS_CMD, tmpname, outputFileName ());
  int result = system (cmd);
  unlink (tmpname);
  return (result == 0);
}
