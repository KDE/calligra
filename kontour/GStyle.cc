/* -*- C++ -*-

  $Id$
  
  This file is part of Kontour.
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#include <koFill.h>
#include <kdebug.h>

const int GStyle::NoFill = 0;
const int GStyle::ColorFill = 1;
const int GStyle::GradientFill = 2;
const int GStyle::PatternFill = 3;

GStyle::GStyle()
{
  mStroked = true;
  mOutline = new KoOutline;
  mOutlineOpacity = static_cast<int>(100.0 * static_cast<double>(mOutline->opacity()) / 255.0);
  mFilled = NoFill;
  mFill = new KoFill;
  mFillOpacity = static_cast<int>(100.0 * static_cast<double>(mFill->opacity()) / 255.0);
}

GStyle::GStyle(const QDomElement &style)
{
  mOutline = new KoOutline;
  mFill = new KoFill;
  mStroked = style.attribute("stroked").toInt();;
  mOutline->width(style.attribute("width").toDouble());
  mOutlineOpacity = style.attribute("oopacity").toInt();
  mOutline->color(KoColor(style.attribute("fcolor")));
  mOutline->opacity(static_cast<int>(255.0 * static_cast<double>(mOutlineOpacity) / 100.0));
  mOutline->join(static_cast<KoOutline::Join>(style.attribute("join").toInt()));
  mOutline->cap(static_cast<KoOutline::Cap>(style.attribute("cap").toInt()));
  mFilled = style.attribute("ftype").toInt();
  mFillOpacity = style.attribute("fopacity").toInt();
  mFill->color(KoColor(style.attribute("fcolor")));
  mFill->opacity(static_cast<int>(255.0 * static_cast<double>(mFillOpacity) / 100.0));
}

GStyle::GStyle(GStyle &obj)
{
  mStroked = obj.mStroked;
  mOutline = new KoOutline;
  mFilled = obj.mFilled;
  mFill = new KoFill;
}

GStyle::~GStyle()
{
  delete mOutline;
  delete mFill;
}

KoOutline *GStyle::outline() const
{
  if(mStroked)
    return mOutline;
  else
    return 0L;
}

KoFill *GStyle::fill() const
{
  if(mFilled != NoFill)
    return mFill;
  else
    return 0L;
}

QDomElement GStyle::writeToXml(QDomDocument &document)
{
  QDomElement style = document.createElement("style");
  style.setAttribute("stroked", mStroked);
  style.setAttribute("ocolor", mOutline->color().name());
  style.setAttribute("width", mOutline->width());
  style.setAttribute("oopacity", mOutlineOpacity);
  style.setAttribute("join", mOutline->join());
  style.setAttribute("cap", mOutline->cap());
  style.setAttribute("ftype", mFilled);
  style.setAttribute("fcolor", mFill->color().name());
  style.setAttribute("fopacity", mFillOpacity);
  return style;
}

bool GStyle::stroked() const
{
  return mStroked;
}

void GStyle::stroked(bool aStroked)
{
  mStroked = aStroked;
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
  return mOutlineOpacity;
}

void GStyle::outlineOpacity(int o)
{
  mOutlineOpacity = o;
  mOutline->opacity(static_cast<int>(255.0 * static_cast<double>(o) / 100.0));
}

double GStyle::outlineWidth() const
{
  return mOutline->width();
}

void GStyle::outlineWidth(double w)
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

int GStyle::filled() const
{
  return mFilled;
}

void GStyle::filled(int aFilled)
{
  mFilled = aFilled;
}

int GStyle::fillOpacity() const
{
  return mFillOpacity;
}

void GStyle::fillOpacity(int o)
{
  mFillOpacity = o;
  mFill->opacity(static_cast<int>(255.0 * static_cast<double>(o) / 100.0));
}

const KoColor &GStyle::fillColor() const
{
  return mFill->color();
}

void GStyle::fillColor(const KoColor &c)
{
  mFill->color(c);
}

GStyle &GStyle::operator=(const GStyle &s)
{
  mStroked = s.mStroked;
  *mOutline = *s.mOutline;
  mOutlineOpacity = s.mOutlineOpacity;
  mFilled = s.mFilled;
  *mFill = *s.mFill;
  mFillOpacity = s.mFillOpacity;
  return *this;
}
