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

#include <string>
#include <set>

#include <stdio.h>
#include <fstream.h>
#include <unistd.h>
#include <stdlib.h>
#include <qprinter.h>
#include <qdatetime.h>
#include <qglobal.h>
#include "version.h"
#include "Painter.h"
#include "EPSExport.h"
#include "Canvas.h"

EPSExport::EPSExport () {
}

EPSExport::~EPSExport () {
}

bool EPSExport::setup (GDocument *doc, const char* fmt) {
  return true;
}

bool EPSExport::exportToFile (GDocument* doc) {
  ofstream epsStream (outputFileName ());
  if (!epsStream)
    return false;

  // compute bounding box
  Rect box = doc->boundingBoxForAllObjects ();

  // write header
  epsStream  << "%!PS-Adobe-2.0 EPSF-2.0\n"
	     << "%%Title: " << (const char *) doc->fileName () << "\n"
	     << "%%Creator: KIllustrator " << APP_VERSION << "\n"
	     << "%%CreationDate: " << QDateTime::currentDateTime ().toString ()
	     << '\n'
	     << "%%BoundingBox: " 
	     << qRound (box.left ()) << ' '
	     << qRound (doc->getPaperHeight () - box.bottom ()) << ' '
	     << qRound (box.right ()) << ' '
	     << qRound (doc->getPaperHeight () - box.top ()) << '\n'
	     << "%%EndComments"
	     << endl;

  // write prolog
  if (! GDocument::writePSProlog (epsStream))
    return false;

  epsStream << "/PaperWidth " << doc->getPaperWidth () << " def\n"
            << "/PaperHeight " << doc->getPaperHeight () << " def\n"
            << "InitTMatrix\n";

  set<string> reqFonts;
  if (doc->requiredFonts (reqFonts)) {
    set<string>::iterator i = reqFonts.begin ();
    for (; i != reqFonts.end (); i++) {
      const char* fontName = i->c_str ();
      epsStream << i->c_str () << " /_" << &fontName[1] << " TransFont\n";
    }
  }

  // write objects
  doc->writeToPS (epsStream);

  epsStream << "%%EOF" << endl;
  return true;
}
