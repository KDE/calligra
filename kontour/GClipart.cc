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

#include <GClipart.h>
#include <qwmf.h>
#include <qdom.h>
#include "GDocument.h"

#include <klocale.h>

GClipart::GClipart (GDocument *doc)
:GObject(doc)
{

    pic = 0L;
}

GClipart::GClipart (GDocument *doc, const QDomElement &element)
: GObject (doc, element.namedItem("gobject").toElement())
{
    url=element.attribute("src");
    QWinMetaFile wmf;
    if (url.isLocalFile () && wmf.load (url.path ())) {
        QRect r = wmf.bbox ();
        width = (r.right () - r.left ()) * 72.0 / wmf.dpi ();
        height = (r.bottom () - r.top ()) * 72.0 / wmf.dpi ();
        pic = new QPicture ();
        wmf.paint (pic);
    }
    else
        // construct a malformed url
        url = KURL ();
    calcBoundingBox ();
}

GClipart::GClipart (GDocument *doc, const QString &name)
:GObject(doc)
,url (name)
{
    QWinMetaFile wmf;
    wmf.load(name);
    QRect r = wmf.bbox ();

    width = (r.right () - r.left ()) * 72.0 / wmf.dpi ();
    height = (r.bottom () - r.top ()) * 72.0 / wmf.dpi ();
    pic = new QPicture ();
    wmf.paint (pic);
    calcBoundingBox ();
}

GClipart::GClipart (const GClipart& obj)
:GObject (obj)
{
  url = obj.url;
  width = obj.width;
  height = obj.height;
  calcBoundingBox ();
}

QString GClipart::typeName () const {
  return i18n("Clipart object");
}

void GClipart::draw (QPainter& p, bool /*withBasePoints*/, bool outline, bool) {
  p.save ();
  if (outline) {
    p.setPen (black);
    p.drawRect (qRound (box.x ()), qRound (box.y ()), qRound (box.width ()), qRound (box.height ()));
  }
  else {
    if (! url.isMalformed ()) {
      p.setWorldMatrix (tmpMatrix, true);
      QWMatrix mx = p.worldMatrix ();
      QRect mr = mx.map (QRect (0, 0, qRound (width), qRound (height)));
      QRect oldWin = p.window ();
      QRect vPort = p.viewport ();
      p.setViewport (mr);
      p.setWorldMatrix (QWMatrix ());
      p.drawPicture (*pic);
      p.setWindow (oldWin);
      p.setViewport (vPort);
    }
    else {
      p.setPen (gray);
      p.fillRect (qRound (box.x ()), qRound (box.y ()), qRound (box.width ()), qRound (box.height ()),
                  gray);
    }
  }
  p.restore ();
}

void GClipart::calcBoundingBox () {
  calcUntransformedBoundingBox (Coord (0, 0), Coord (width, 0),
                                Coord (width, height), Coord (0, height));
}

GObject* GClipart::copy () {
  return new GClipart (*this);
}

/*GObject* GClipart::create (GDocument *doc, const QDomElement &element)
{
  return new GClipart (doc, element);
}*/

QDomElement GClipart::writeToXml (QDomDocument &document) {

    // FIXME (Werner): Make this store internal/external depening on the user's wish
    QDomElement element=document.createElement("clipart");
    element.setAttribute ("src", url.url ());
    element.appendChild(GObject::writeToXml(document));
    return element;
}

#include <GClipart.moc>
