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
	KoColor          ocolor;
	unsigned int     lwidth;
	KoColor          fcolor;
	Qt::PenCapStyle  cap;
	Qt::PenJoinStyle join;
	Qt::BrushStyle   brushStyle;
	bool             stroked;
	bool             filled; // TODO : not necessary because of bstyle ?
};

GStyle::GStyle() : d(new GStylePrivate)
{
  d->ocolor     = KoColor::black();
  d->lwidth     = 1;
  d->cap        = Qt::RoundCap;
  d->join       = Qt::RoundJoin;
  d->brushStyle = Qt::SolidPattern;
  d->fcolor     = KoColor::white();
  d->stroked    = true;
  d->filled     = false;
}

GStyle::GStyle(const QDomElement &style)
{
}

GStyle::GStyle(GStyle &obj)
{
  d->ocolor     = obj.d->ocolor;
  d->lwidth     = obj.d->lwidth;
  d->cap        = obj.d->cap;
  d->join       = obj.d->join;
  d->brushStyle = obj.d->brushStyle;
  d->fcolor     = obj.d->fcolor;
  d->stroked    = obj.d->stroked;
  d->filled     = obj.d->filled;
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

Qt::PenJoinStyle GStyle::joinStyle() const
{
  return d->join;
}

void GStyle::joinStyle(Qt::PenJoinStyle join)
{
  d->join = join;
}

Qt::PenCapStyle GStyle::capStyle() const
{
  return d->cap;
}

void GStyle::capStyle(Qt::PenCapStyle cap)
{
  d->cap = cap;
}

Qt::BrushStyle GStyle::brushStyle() const
{
  return d->brushStyle;
}

void GStyle::brushStyle(Qt::BrushStyle brushStyle)
{
  d->brushStyle = brushStyle;
}

bool GStyle::stroked() const
{
  return d->stroked;
}

void GStyle::stroked(bool stroked)
{
  d->stroked = stroked;
}

bool GStyle::filled() const
{
  return d->filled;
}

void GStyle::filled(bool filled)
{
  d->filled = filled;
}

GStyle &GStyle::operator=(const GStyle &s)
{
  d->ocolor     = s.d->ocolor;
  d->lwidth     = s.d->lwidth;
  d->cap        = s.d->cap;
  d->join       = s.d->join;
  d->brushStyle = s.d->brushStyle;
  d->fcolor     = s.d->fcolor;
  d->stroked    = s.d->stroked;
  d->filled     = s.d->filled;
  return *this;
}
