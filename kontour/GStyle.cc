/* -*- C++ -*-

  $Id$
  
  This file is part of Kontour.
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#include "GStyle.h"

#include <qdom.h>

class GStylePrivate
{
public:
	KoColor ocolor;
	unsigned int lwidth;
	KoColor fcolor;
};

GStyle::GStyle() : d(new GStylePrivate)
{
  d->ocolor = KoColor::black();
  d->lwidth = 1;
  d->fcolor = KoColor::white();
}

GStyle::GStyle(const QDomElement &style)
{
}

GStyle::GStyle(GStyle &obj)
{
  d->ocolor = obj.d->ocolor;
  d->lwidth = obj.d->lwidth;
  d->fcolor = obj.d->fcolor;
}

GStyle::~GStyle()
{
	delete d;
}

QDomElement GStyle::writeToXml(QDomDocument &document)
{
  QDomElement style = document.createElement("style");
  return style;
}
  
void GStyle::outlineColor(const KoColor &c)
{
  d->ocolor = c;
}
  
const KoColor &GStyle::outlineColor() const
{
  return d->ocolor;
}

void GStyle::outlineWidth(unsigned int lwidth)
{
  d->lwidth = lwidth;
}

unsigned int GStyle::outlineWidth() const
{
  return d->lwidth;
}

void GStyle::fillColor(const KoColor &c)
{
  d->fcolor = c;
}

const KoColor &GStyle::fillColor() const
{
  return d->fcolor;
}

GStyle &GStyle::operator=(const GStyle &s)
{
  d->ocolor = s.d->ocolor;
  d->lwidth  = s.d->lwidth;
  d->fcolor = s.d->fcolor;
  return *this;
}
