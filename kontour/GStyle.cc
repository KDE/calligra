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

GStyle::GStyle()
{
  ocolor = KoColor::black();
  fcolor = KoColor::white();
}

GStyle::GStyle(const QDomElement &style)
{

}

GStyle::GStyle(GStyle &obj)
{
  ocolor = obj.ocolor;
  fcolor = obj.fcolor;
}
  
QDomElement GStyle::writeToXml(QDomDocument &document)
{
  QDomElement style = document.createElement("style");
  return style;
}
  
void GStyle::outlineColor(const KoColor &c)
{
  ocolor = c;
}
  
void GStyle::fillColor(const KoColor &c)
{
  fcolor = c;
}

GStyle &GStyle::operator=(const GStyle &s)
{
  ocolor = s.ocolor;
  fcolor = s.fcolor;
  return *this;
}
