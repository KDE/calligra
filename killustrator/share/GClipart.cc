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

#include "GClipart.h"
#include "GClipart.moc"
#include "qwmf.h"

#include <klocale.h>
#include <kapp.h>

GClipart::GClipart () {
  pic = 0L;
}

GClipart::GClipart (const list<XmlAttribute>& attribs) : GObject (attribs) {
  list<XmlAttribute>::const_iterator first = attribs.begin ();
	
  while (first != attribs.end ()) {
    const string& attr = (*first).name ();
    if (attr == "src") {
      QWinMetaFile wmf;
      
      url = (*first).stringValue ().c_str ();
      if (url.isLocalFile () && wmf.load (url.path ())) {
	QRect r = wmf.bbox ();
	
	width = (r.right () - r.left ()) * 72.0 / wmf.dpi ();
	height = (r.bottom () - r.top ()) * 72.0 / wmf.dpi ();
	pic = new QPicture ();
	wmf.paint (pic);
	calcBoundingBox ();
      }
    }
    first++;
  }
}

GClipart::GClipart (QWinMetaFile& wmf, const char* name) : url (name) {
  QRect r = wmf.bbox ();

  width = (r.right () - r.left ()) * 72.0 / wmf.dpi ();
  height = (r.bottom () - r.top ()) * 72.0 / wmf.dpi ();
  pic = new QPicture ();
  wmf.paint (pic);
  calcBoundingBox ();
}

GClipart::GClipart (const GClipart& obj) : GObject (obj) {
  url = obj.url;
  calcBoundingBox ();
}

const char* GClipart::typeName () {
  return i18n ("Clipart object");
}

void GClipart::draw (Painter& p, bool withBasePoints) {
  p.save ();
  QRect oldWin = p.window ();
  QRect vPort = p.viewport ();
  p.setViewport ((int) tmpMatrix.dx (), (int) tmpMatrix.dy (), 
		 (int) (width * tmpMatrix.m11 ()),
		 (int) (height * tmpMatrix.m22 ()));
  p.drawPicture (*pic);
  p.setWindow (oldWin);
  p.setViewport (vPort);
  p.restore ();
}

void GClipart::calcBoundingBox () {
  calcUntransformedBoundingBox (Coord (0, 0), Coord (width, 0),
				Coord (width, height), Coord (0, height));
}

GObject* GClipart::copy () {
  return new GClipart (*this);
}

void GClipart::writeToXml (XmlWriter& xml) {
  xml.startTag ("clipart", false);
  writePropertiesToXml (xml);
  xml.addAttribute ("src", (const char *) url.url ());
  xml.closeTag (true);
}
