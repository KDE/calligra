/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
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

#ifndef __Handle_h__
#define __Handle_h__

#include <koPoint.h>
#include <koRect.h>

class KoPainter;

namespace Kontour
{
class GPage;

class Handle
{
public:
  Handle(GPage *aGPage);

  void show(bool flag = true);
  void empty(bool flag = true);

  void box(const KoRect &r);

  KoPoint rotCenter() const {return mRotCenter; }
  void rotCenter(const KoPoint &p);

  void draw(KoPainter *p, int aXOffset, int aYOffset, double zoom);
  int contains(const KoPoint &p);

private:
  enum ArrowDirection { Arrow_Left, Arrow_Right, Arrow_Up, Arrow_Down };
  void drawArrow(KoPainter *p, int x, int y, ArrowDirection d);

private:
  GPage *mGPage;
  KoPoint pos[8];
  KoPoint rpos[8];
  KoRect mBox;
  KoPoint mRotCenter;
  bool mShow;
  bool mEmpty;
};
};

using namespace Kontour;

#endif
