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

#ifndef __GStyleList_h__
#define __GStyleList_h__

#include <qdict.h>

class QDomDocument;
class QDomElement;
class QStringList;

namespace Kontour {

class GStyle;
class GStyleList
{
public:
  GStyleList();
    ~GStyleList();
  GStyleList(const QDomElement &sl);

  QDomElement writeToXml(QDomDocument &document);

  GStyle *style() const {return mStyle; }
  void style(QString aName);

  int currentNum() const {return mCur; }

  QStringList *stringList();

  void addStyle();
  void deleteStyle();

private:
  QDict<GStyle> list;

  int       mNum;
  int       mCur;
  GStyle   *mStyle;          //current style
};
};
using namespace Kontour;

#endif
