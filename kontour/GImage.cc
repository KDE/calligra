/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2002 Igor Janssen (rm@kde.org)

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

#include "GImage.h"

#include <qdom.h>
#include <qimage.h>

#include <kurl.h>
#include <klocale.h>
#include <koPainter.h>
#include <kdebug.h>

GImage::GImage(const KURL &url):
GObject()
{
  mImage = new QImage();
  if(url.isLocalFile())
  {
    mImage->load(url.path());
/*    if (pix->isNull ()) {
      delete pix;
      pix = 0L;
      kdWarning(38000) << "pixmap is null !!!" << endl;
    }*/
  }
  calcBoundingBox();
}

GImage::GImage(const QDomElement &element):
GObject(element.namedItem("go").toElement())
{
/*
    url=element.attribute("src");
    if (url.isLocalFile ()) {
        pix = new QPixmap (url.path ());
        if (pix->isNull ()) {
            delete pix;
            pix = 0L;
        }
    }
    if (pix) {
        // use real pixmap dimension
        width = pix->width ();
        height = pix->height ();
    }
    calcBoundingBox ();*/
}

GImage::GImage(const GImage &obj):
GObject(obj)
{
/*  url = obj.url;
  if (obj.pix)
    pix = new QPixmap (*obj.pix);
  width = obj.width;
  height = obj.height;
  calcBoundingBox ();*/
}

GImage::~GImage()
{
  if(mImage)
    delete mImage;
}

GObject *GImage::copy() const
{
  return new GImage(*this);
}

QString GImage::typeName() const
{
  return i18n("Image");
}

QDomElement GImage::writeToXml(QDomDocument &document)
{
  QDomElement image = document.createElement("image");
  image.appendChild(GObject::writeToXml(document));
  return image;
}

void GImage::draw(KoPainter *p, int aXOffset, int aYOffset, bool withBasePoints, bool outline, bool withEditMarks)
{
  QWMatrix m;
  m = m.translate(aXOffset, aYOffset);
  m = tmpMatrix * m;
  p->drawImage(mImage, style()->fillOpacity(), m);
}

int GImage::getNeighbourPoint(const KoPoint &p)
{
  return -1;
}

void GImage::movePoint(int idx, double dx, double dy, bool /*ctrlPressed*/)
{
}

void GImage::removePoint(int idx, bool update)
{
}

bool GImage::contains(const KoPoint &p)
{
  double x1, y1, x2, y2;
  if(box.contains(p))
  {
    QPoint pp = iMatrix.map(QPoint(static_cast<int>(p.x()), static_cast<int>(p.y())));
    if(pp.x() <= mImage->width() && pp.x() >= 0 && pp.y() <= mImage->height() && pp.y() >= 0)
      return true;
  }
  return false;
}

bool GImage::findNearestPoint(const KoPoint &p, double max_dist, double &dist, int &pidx, bool all)
{
  return true;
}

void GImage::calcBoundingBox()
{
  box = calcUntransformedBoundingBox(KoPoint(0, 0), KoPoint(mImage->width(), 0), KoPoint(mImage->width(), mImage->height()), KoPoint(0, mImage->height()));
}

GPath *GImage::convertToPath() const
{
  return 0L;
}

bool GImage::isConvertible() const
{
  return false;
}

#include "GImage.moc"
