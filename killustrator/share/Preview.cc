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
#include "version.h"

#define PREVIEW_HEIGHT 120
#define PREVIEW_WIDTH  270
#define PREVIEW_BORDER 5

#ifndef USE_QFD
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
      Rect bb = tmpDoc->boundingBoxForAllObjects ();
      
      float scale;
      float xscale = (float) (PREVIEW_WIDTH - 2*PREVIEW_BORDER) / (float) bb.width ();
      float yscale = (float) (PREVIEW_HEIGHT - 2*PREVIEW_BORDER) / (float) bb.height ();       
      if (xscale < yscale) {
          scale = xscale;
          pixmap = QPixmap (PREVIEW_WIDTH, bb.height () * scale + 2*PREVIEW_BORDER);
      }
      else {
          scale = yscale;
          pixmap = QPixmap (bb.width () * scale + 2*PREVIEW_BORDER, PREVIEW_HEIGHT);
      }
      float xtrans = - bb.left () + PREVIEW_BORDER;
      float ytrans = - bb.top () + PREVIEW_BORDER;      
      
      QPainter p;

      p.begin (&pixmap);
      p.setBackgroundColor (QT_PRFX::white);
      pixmap.fill (QT_PRFX::white);
      
      p.scale (scale, scale);
      p.translate (xtrans, ytrans);
      // what does this do?
      // p.eraseRect (0, 0, tmpDoc->getPaperWidth (),
	 //	   tmpDoc->getPaperHeight ());

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
      
      pixmap = QPixmap (PREVIEW_HEIGHT, PREVIEW_HEIGHT);
      QPainter p;
      p.begin (&pixmap);
      p.setBackgroundColor (QT_PRFX::white);
      pixmap.fill (QT_PRFX::white);
      QRect oldWin = p.window ();
      QRect vPort = p.viewport ();
      p.setViewport (0, 0, PREVIEW_HEIGHT, PREVIEW_HEIGHT);
      p.drawPicture (pic);
      p.setWindow (oldWin);
      p.setViewport (vPort);
      p.end ();
      
      res = true;
    }
  }
  return res;
}

/**
 * A preview handler for the KFilePreviewDialag that shows
 * a Pixmap object.
 */
bool pixmapPreviewHandler (const KFileInfo* fInfo, const QString fileName,
			QString&, QPixmap& pixmap) {
  bool res = false;

  if (fInfo->isFile ()) {
    QPixmap origPixmap;
    origPixmap.load ((const char *) fileName);
    
    if (!origPixmap.isNull ()) {
      float scale;
      float xscale = (float) PREVIEW_WIDTH / (float) origPixmap.width ();
      float yscale = (float) PREVIEW_HEIGHT / (float) origPixmap.height ();       
      if (xscale < yscale) {
        scale = xscale;
        pixmap = QPixmap (PREVIEW_WIDTH, origPixmap.height () * scale);
      }  
      else { 
        scale = yscale;
        pixmap = QPixmap (origPixmap.width () * scale, PREVIEW_HEIGHT);
      }
      
      QPainter p;
      p.begin (&pixmap);
      p.scale (scale, scale);
      p.drawPixmap(0, 0, origPixmap);
      p.end ();
    }  
      
    return ! pixmap.isNull ();
  }
  return res;
}
#endif
