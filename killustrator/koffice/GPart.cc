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

#include <GPart.h>

#include <qdom.h>

#include <KIllustrator_view.h>
#include <KIllustrator_doc.h>

GPart::GPart () {
  child = 0L;
}

GPart::GPart (KIllustratorChild *c) {
  child = c;
  initialGeom = child->geometry ();
  calcBoundingBox ();
}

GPart::GPart (const QDomElement &element) :
    GObject (element.namedItem("gobject").toElement()) {

    int x = 0, y = 0, w = 0, h = 0;
    QString url, mime;

    x = element.attribute("x").toInt();
    y = element.attribute("y").toInt();
    w = element.attribute("width").toInt();
    h = element.attribute("height").toInt();
    url = element.attribute("url");
    mime = element.attribute("mime");
    initialGeom = QRect (x, y, w, h);

  // ####### Torben
  /* child = new KIllustratorChild ();
  child->setURL (url.c_str ());
  child->setMimeType (mime.c_str ());
  child->setGeometry (initialGeom);  */
    calcBoundingBox ();
}

GPart::GPart (const GPart& obj) : GObject (obj) {
  calcBoundingBox ();
}

GPart::~GPart () {
}

QString GPart::typeName () const {
  return i18n ("Embedded Part");
}

void GPart::draw (Painter&/*p*/, bool /*withBasePoints*/, bool /*outline*/) {
    // ####### Torben
    /**
  p.save ();
  QRect r = child->geometry ();
  if (outline) {
    p.setWorldMatrix (tmpMatrix, true);
    p.setPen (black);
    p.drawRect (r.x (), r.y (), r.width (), r.height ());
  }
  else {
    float s = p.worldMatrix ().m11 ();
    QRect win = p.window ();
    QRect vPort = p.viewport ();
    QPicture *pic = child->draw (1.0, true);
    p.setViewport (r.x () * s, r.y () * s, vPort.width (), vPort.height ());
    p.drawPicture (*pic);
    p.setViewport (vPort);
    p.setWindow (win);
  }
  p.restore (); */
}

void GPart::calcBoundingBox () {

    QRect r = tmpMatrix.map (initialGeom);
    if (r != oldGeom) {
        //cout << "UPDATE CHILD GEOMETRY !!!!!!!!!!!!" << endl;
        oldGeom = r;
        child->setGeometry (r);
        //cout << "new part geometry: " << r.x () << ", " << r.y ()
        //     << " - " << r.width () << ", " << r.height () << endl;
    }
    updateBoundingBox (Coord (r.x (), r.y ()),
                       Coord (r.right (), r.bottom ()));
}

GObject* GPart::copy () {
  return new GPart (*this);
}

GObject* GPart::clone (const QDomElement &element) {
  return new GPart (element);
}

QDomElement GPart::writeToXml (QDomDocument &document) {

    QDomElement element=document.createElement("object");
    element.setAttribute ("x", oldGeom.x ());
    element.setAttribute ("y", oldGeom.y ());
    element.setAttribute ("width", oldGeom.width ());
    element.setAttribute ("height", oldGeom.height ());
    element.setAttribute ("url", child->url().url() );
    element.setAttribute ("mime", child->document()->nativeFormatMimeType ());
    element.appendChild(GObject::writeToXml(document));
    return element;
}

#include <GPart.moc>
