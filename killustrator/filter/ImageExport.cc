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
#include <qpixmap.h>
#include <qimage.h>
#include <qglobal.h>
#include <qpainter.h>
#include <kimageio.h>
#include "ImageExport.h"
#include "GDocument.h"

#define RESOLUTION 72.0

ImageExport::ImageExport () {
// #ifdef HAVE_QIMGIO
//   qInitImageIO ();
// #endif
}

ImageExport::~ImageExport () {
}

bool ImageExport::setup (GDocument *, const char* fmt) {
  bool formatSupported = false;

  QStrList formats = QImageIO::outputFormats ();
  char* str = formats.first ();
  format = QString ();
  while (str) {
    if (strcasecmp (str, fmt) == 0) {
      format = fmt;
      format = format.upper ();
      formatSupported = true;
      break;
    }
    str = formats.next ();
  }
  return formatSupported;
}

bool ImageExport::exportToFile (GDocument* doc) {
  if (format.isNull ())
    return false;

  unsigned int w, h;
  w = qRound (doc->getPaperWidth () * RESOLUTION / 72.0);
  h = qRound (doc->getPaperHeight () * RESOLUTION / 72.0);

  // prepare a pixmap for drawing
  QPixmap *buffer = new QPixmap (w, h);
  if (buffer == 0L)
    return false;

  buffer->fill (Qt::white);
  QPainter p;
  p.begin (buffer);
  p.setBackgroundColor (Qt::white);
  p.eraseRect (0, 0, w, h);
  p.scale (RESOLUTION / 72.0, RESOLUTION / 72.0);

  // draw the objects
  doc->drawContents (p);

  p.end ();

  // compute the bounding box
  Rect box = doc->boundingBoxForAllObjects ();
  // and copy the affected area to the new pixmap
  QPixmap *pixmap = new QPixmap (qRound (box.width ()),
                                 qRound (box.height ()));
  if (pixmap == 0L)
    return false;
  bitBlt (pixmap, 0, 0, buffer, qRound (box.x ()), qRound (box.y ()),
          qRound (box.width ()), qRound (box.height ()));
  delete buffer;

  // now create an image
  QImage img  = pixmap->convertToImage ();
  img.setAlphaBuffer (true);
  delete pixmap;

  // and save the image in requested format
  return img.save(outputFileName (), format.latin1());
}
