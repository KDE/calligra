/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 2002 Igor Jansen (rm@kde.org)

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

#include "DashList.h"

#include <klocale.h>
#include <koDash.h>

#include <qdom.h>

DashList::DashList()
{
  list.clear();
/*  GStyle *st = new GStyle();
  list.insert(i18n("Default"), st);
  mStyle = st;
  mCur = 0;
  mNum = 1;*/
}

/*QDomElement GStyleList::writeToXml(QDomDocument &document)
{
  QDomElement sl = document.createElement("stylelist");
  QDictIterator<GStyle> it(list);
  for(;it.current(); ++it)
  {
    GStyle *st = it;
    QDomElement style = st->writeToXml(document);
    style.setAttribute("id", it.currentKey());
    sl.appendChild(style);
  }
  return sl;
}*/
