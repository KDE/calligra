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

#include "koDash.h"

KoOutline::KoOutline()
{
  mColor = KoColor::black();
  mOpacity = 255;
  mWidth = 1.0;
  mDash = 0L;
  mJoin = JoinMiter;
  mCap = CapButt;
}

void KoOutline::color(const KoColor &c)
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

void KoOutline::dash(KoDash *d)
{
  mDash = d;
}

void KoOutline::join(Join j)
{
  mJoin = j;
}

void KoOutline::cap(Cap c)
{
  mCap = c;
}

KoOutline &KoOutline::operator=(const KoOutline &o)
{
  mColor = o.mColor;
  mOpacity = o.mOpacity;
  mWidth = o.mWidth;
  mDash = o.mDash;
  mJoin = o.mJoin;
  mCap = o.mCap;
  return *this;
}
