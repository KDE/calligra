/* This file is part of the KDE project
  Copyright (c) 2002 Igor Janssen (rm@kde.org)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koOutline.h"

KoOutline::KoOutline()
{
  mColor = KoColor::white();
  mOpacity = 255;
  mWidth = 1.0;
  mDashOffset = 0.0;
  mJoin = JoinMiter;
  mCap = CapButt;
}

void KoOutline::color(KoColor c)
{
  mColor = c;
}

void KoOutline::opacity(int o)
{
  mOpacity = o;
}

void KoOutline::width(double w)
{
  mWidth = w;
}

void KoOutline::dashOffset(double d)
{
  mDashOffset = d;
}

void KoOutline::dashResize(int s)
{
  mDashes.resize(s);
}

void KoOutline::setDash(int n, double l)
{
  mDashes[n] = l;
}

void KoOutline::join(Join j)
{
  mJoin = j;
}

void KoOutline::cap(Cap c)
{
  mCap = c;
}
