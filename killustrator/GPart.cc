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
#include <kdebug.h>
#include <koQueryTrader.h>
#include <qdom.h>
#include <qpainter.h>
#include <qstring.h>
#include <qpixmap.h>

#include <KIllustrator_view.h>
#include <KIllustrator_doc.h>

GPart::GPart (GDocument* doc )
:GObject(doc)
{
  child = 0L;
}

GPart::GPart (GDocument* doc, KIllustratorChild *c)
:GObject(doc)
{
  child = c;
  initialGeom = child->geometry ();
  calcBoundingBox ();
}

GPart::GPart (GDocument* docu, KIllustratorDocument *doc, const QDomElement &element)
:GObject (docu, element.namedItem("gobject").toElement())
 {
    child = new KIllustratorChild(doc, 0, QRect(0,0,0,0));
    child->load(element);
    calcBoundingBox ();
 }

GPart::GPart (const GPart& obj)
: GObject (obj)
{
  calcBoundingBox ();
}

GPart::~GPart () {
}

QString GPart::typeName () const {
  return i18n ("Embedded Part");
}

void GPart::activate(KIllustratorView *view)
 {
  KoDocument* part = child->document();
  if ( !part )
    return;
  view->partManager()->addPart( part, false );
  view->partManager()->setActivePart( part, view );
 }

void GPart::deactivate()
 {

 }

void GPart::draw (QPainter& p, bool /*withBasePoints*/, bool outline)
 {
  p.save ();
  QRect r = child->geometry ();
  if (outline)
   {
    p.setWorldMatrix (tmpMatrix, true);
    p.setPen (black);
    p.drawRect (r.x (), r.y (), r.width (), r.height ());
   }
  else
   {
//    float s = p.worldMatrix ().m11 ();
//    QRect win = p.window ();
//    QRect vPort = p.viewport ();
//    QPicture *pic = child->draw (1.0, true);
//    p.setViewport (r.x () * s, r.y () * s, vPort.width (), vPort.height ());
    //p.drawPicture (*pic);
    QPixmap pic(r.width(), r.height());
    QPainter picp(&pic);
//    picp.begin();
    child->document()->paintEverything(picp, QRect(0, 0, r.width(), r.height()), false, 0);
//    picp.end();
    p.drawPixmap(r.x(), r.y(), pic);
//    p.drawRect (r);
//    p.setViewport (vPort);
//    p.setWindow (win);
   }
  p.restore ();
 }

void GPart::calcBoundingBox ()
{
   QRect r = tmpMatrix.map (initialGeom);

   if (r != oldGeom)
   {
      oldGeom = r;
      child->setGeometry (r);
   }
   updateBoundingBox (Coord (r.x (), r.y ()), Coord(r.right(), r.bottom()));
}

GObject* GPart::copy () {
  return new GPart (*this);
}

/*GObject* GPart::clone (const QDomElement &element) {
  return new GPart (0, element);
}*/

QDomElement GPart::writeToXml (QDomDocument &document) {

    QDomElement element=document.createElement("object");
    element.setAttribute ("url", child->url().url() );
    element.setAttribute ("mime", child->document()->nativeFormatMimeType ());
    QDomElement rect = document.createElement("rect");
    rect.setAttribute("x", oldGeom.x());
    rect.setAttribute("y", oldGeom.y());
    rect.setAttribute("w", oldGeom.width());
    rect.setAttribute("h", oldGeom.height());
    element.appendChild(rect);
    element.appendChild(GObject::writeToXml(document));
    return element;
}

#include <GPart.moc>
