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
  bool             stroked;
  KoColor          ocolor;
  int              oopacity;
  unsigned int     lwidth;
  Qt::PenCapStyle  cap;
  Qt::PenJoinStyle join;
  int              ftype;
  KoColor          fcolor;
  int              fopacity;
  Qt::BrushStyle   pattern;
};

GStyle::GStyle()
{
  d = new GStylePrivate;
  d->stroked = true;
  d->ocolor = KoColor::black();
  d->lwidth = 1;
  d->oopacity = 100;
  d->join = Qt::RoundJoin;
  d->cap = Qt::RoundCap;
  d->ftype = NoFill;
  d->fcolor = KoColor::white();
  d->fopacity = 100;
  d->pattern = Qt::SolidPattern;
}

GStyle::GStyle(const QDomElement &style)
{
  d = new GStylePrivate;
}

GStyle::GStyle(GStyle &obj)
{
  d = new GStylePrivate;
  d->stroked = obj.d->stroked;
  d->ocolor = obj.d->ocolor;
  d->lwidth = obj.d->lwidth;
  d->oopacity = obj.d->oopacity;
  d->join = obj.d->join;
  d->cap = obj.d->cap;
  d->ftype = obj.d->ftype;
  d->fcolor = obj.d->fcolor;
  d->fopacity = obj.d->fopacity;
  d->pattern = obj.d->pattern;
}

GStyle::~GStyle()
{
  delete d;
}

QDomElement GStyle::writeToXml(QDomDocument &document)
{
  QDomElement style = document.createElement("style");
  style.setAttribute("stroked", d->stroked);
  style.setAttribute("ocolor", d->ocolor.name());
  style.setAttribute("width", d->lwidth);
  style.setAttribute("oopacity", d->oopacity);
  style.setAttribute("join", d->join);
  style.setAttribute("cap", d->cap);
  style.setAttribute("ftype", d->ftype);
  style.setAttribute("fcolor", d->fcolor.name());
  style.setAttribute("pattern", d->pattern);
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

int GStyle::outlineOpacity() const
{
  return d->oopacity;
}

void GStyle::outlineOpacity(int o)
{
  d->oopacity = o;
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
  return d->pattern;
}

void GStyle::brushStyle(Qt::BrushStyle brushStyle)
{
  d->pattern = brushStyle;
}

bool GStyle::stroked() const
{
  return d->stroked;
}

void GStyle::stroked(bool stroked)
{
  d->stroked = stroked;
}

int GStyle::filled() const
{
  return d->ftype;
}

void GStyle::filled(int filled)
{
  d->ftype = filled;
}

GStyle &GStyle::operator=(const GStyle &s)
{
  d->stroked = s.d->stroked;
  d->ocolor = s.d->ocolor;
  d->lwidth = s.d->lwidth;
  d->oopacity = s.d->oopacity;
  d->join = s.d->join;
  d->cap = s.d->cap;
  d->ftype = s.d->ftype;
  d->fcolor = s.d->fcolor;
  d->fopacity = s.d->fopacity;
  d->pattern = s.d->pattern;
  return *this;
}
