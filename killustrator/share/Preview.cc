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

#include <fstream.h>
#include <qpicture.h>
#include "Preview.h"
#include "GDocument.h"
#include "qwmf.h"

/**
 * A preview handler for the KFilePreviewDialag that shows
 * a KIllustrator document.
 */
bool kilPreviewHandler (const KFileInfo* fInfo, const QString fileName,
			QString&, QPixmap& pixmap) {
  bool res = false;
  QString ext = fileName.right (3).lower ();

  if (fInfo->isFile () && (ext == "kil")) {
    // create a temporary document
    GDocument *tmpDoc = new GDocument ();

    // load it
    ifstream in ((const char *) fileName);
    if (in.fail ())
      return false;
    if (tmpDoc->readFromXml (in)) {
      float ratio = float (tmpDoc->getPaperHeight ()) / 
                    float (tmpDoc->getPaperWidth ());
      int width = 300;
      int height = (int) (ratio * float (width));
      float xscale = (float) width / (float) tmpDoc->getPaperWidth ();
      float yscale = (float) height / (float) tmpDoc->getPaperHeight ();

      pixmap = QPixmap (width, height);
      Painter p;

      p.begin (&pixmap);
      p.setBackgroundColor (white);
      pixmap.fill (white);

      p.scale (xscale, yscale);
      p.eraseRect (0, 0, tmpDoc->getPaperWidth (),
		   tmpDoc->getPaperHeight ());

      tmpDoc->drawContents (p, false);
      p.end ();

      res = true;
    }

    delete tmpDoc;
  }
  return res;
}

/**
 * A preview handler for the KFilePreviewDialag that shows
 * a WMF object.
 */
bool wmfPreviewHandler (const KFileInfo* fInfo, const QString fileName,
			QString&, QPixmap& pixmap) {
  bool res = false;
  QString ext = fileName.right (3).lower ();

  if (fInfo->isFile () && (ext == "wmf")) {
    QWinMetaFile wmf;

    if (wmf.load ((const char *) fileName)) {
      QPicture pic;
      wmf.paint (&pic);                                                     

      pixmap = QPixmap (200, 200);
      Painter p;

      p.begin (&pixmap);
      p.setBackgroundColor (white);
      pixmap.fill (white);

      QRect oldWin = p.window ();
      QRect vPort = p.viewport ();
      p.setViewport (0, 0, 200, 200);
      p.drawPicture (pic);
      p.setWindow (oldWin);
      p.setViewport (vPort);
      p.end ();

      res = true;
    }
  }
  return res;
}
