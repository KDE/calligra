/* -*- C++ -*-

  $Id$
  
  This file is part of Kontour.
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

GStyle::GStyle()
{
  mStroked = true;
  mOutline = new KoOutline;
/*
  d->ftype = NoFill;
  d->fcolor = KoColor::white();
  d->fopacity = 100;
  d->pattern = Qt::SolidPattern;
*/
}

GStyle::GStyle(const QDomElement &style)
{
  mOutline = new KoOutline;
  mStroked = style.attribute("stroked").toInt();;
//  d->ocolor = ;
/*
  d->lwidth = style.attribute("width").toInt();
  d->oopacity = style.attribute("oopacity").toInt();
  d->join = (Qt::PenJoinStyle)style.attribute("join").toInt();
  d->cap = (Qt::PenCapStyle)style.attribute("cap").toInt();
  d->ftype = style.attribute("ftype").toInt();*/
//  d->fcolor = ;
//  d->fopacity = obj.d->fopacity;
//  d->pattern = obj.d->pattern;
}

GStyle::GStyle(GStyle &obj)
{
  mOutline = new KoOutline;
  mStroked = obj.mStroked;
/*  d->ocolor = obj.d->ocolor;
  d->lwidth = obj.d->lwidth;
  d->oopacity = obj.d->oopacity;
  d->join = obj.d->join;
  d->cap = obj.d->cap;
  d->ftype = obj.d->ftype;
  d->fcolor = obj.d->fcolor;*/
//  d->fopacity = obj.d->fopacity;
//  d->pattern = obj.d->pattern;
}

GStyle::~GStyle()
{
  delete mOutline;
}

KoOutline *GStyle::outline() const
{
  if(mStroked)
    return mOutline;
  else
    return 0L;
}

QDomElement GStyle::writeToXml(QDomDocument &document)
{
  QDomElement style = document.createElement("style");
/*  style.setAttribute("stroked", d->stroked);
  style.setAttribute("ocolor", d->ocolor.name());
  style.setAttribute("width", d->lwidth);
  style.setAttribute("oopacity", d->oopacity);
  style.setAttribute("join", d->join);
  style.setAttribute("cap", d->cap);
  style.setAttribute("ftype", d->ftype);
  style.setAttribute("fcolor", d->fcolor.name());
  style.setAttribute("pattern", d->pattern);*/
  return style;
}
  
const KoColor &GStyle::outlineColor() const
{
  return mOutline->color();
}

void GStyle::outlineColor(const KoColor &c)
{
  mOutline->color(c);
}

int GStyle::outlineOpacity() const
{
  return static_cast<int>(100.0 * static_cast<double>(mOutline->opacity()) / 255.0);
}

void GStyle::outlineOpacity(int o)
{
  mOutline->opacity(static_cast<int>(255.0 * static_cast<double>(o) / 100.0));
}

int GStyle::outlineWidth() const
{
  return static_cast<int>(mOutline->width());
}

void GStyle::outlineWidth(int w)
{
  mOutline->width(w);
}

KoOutline::Join GStyle::joinStyle() const
{
  return  mOutline->join();
}

void GStyle::joinStyle(KoOutline::Join join)
{
  mOutline->join(join);
}

KoOutline::Cap GStyle::capStyle() const
{
  return  mOutline->cap();;
}

void GStyle::capStyle(KoOutline::Cap cap)
{
  mOutline->cap(cap);
}

void GStyle::fillColor(const KoColor &)
{
}

const KoColor &GStyle::fillColor() const
{
  return KoColor::black();
}

Qt::BrushStyle GStyle::brushStyle() const
{
//  return d->pattern;
}

void GStyle::brushStyle(Qt::BrushStyle brushStyle)
{
//  d->pattern = brushStyle;
}

bool GStyle::stroked() const
{
//  return d->stroked;
}

void GStyle::stroked(bool stroked)
{
//  d->stroked = stroked;
}

int GStyle::filled() const
{
  return 0;
}

void GStyle::filled(int filled)
{
  //d->ftype = filled;
}

GStyle &GStyle::operator=(const GStyle &s)
{
  mStroked = s.mStroked;
  *mOutline = *s.mOutline;
/*  d->ftype = s.d->ftype;
  d->fcolor = s.d->fcolor;
  d->fopacity = s.d->fopacity;
  d->pattern = s.d->pattern;*/
  return *this;
}
